/* preload-getaddrinfo.c */
/* copyright (C) by paulius@chroot.lt 2014 */
/* v0.3 */
#define _GNU_SOURCE
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/* Functions to be preloaded */
int (*oldgetaddrinfo)(const char *, const char *, const struct addrinfo *, struct addrinfo **)=NULL;
struct hostent* (*oldgethostbyname)(const char *)=NULL;

struct hostent *he=NULL;

/* Internal check of end of domainname */
int check_i2p(const char* name){
  return 1;
}

struct hostent *gethostbyname(const char *name){
	struct in_addr **addr_list;
	int i;
	oldgethostbyname = dlsym(RTLD_NEXT, "gethostbyname");

	if( check_i2p(name) ){
		printf("Resolving %s by hooked gethostbyname()\n", name);
		he = oldgethostbyname("test.com"); /* dummy resolve to fill data structures, must always resolve */
		assert(he != NULL);
		addr_list = (struct in_addr **)he->h_addr_list;
		for( i = 0; addr_list[i] != NULL; i++) {
			inet_aton(getenv("FAKE_DEV_HOST"), addr_list[i]);
		}
	}else{
		printf("Resolving %s by system gethostbyname()\n", name);
		he = oldgethostbyname(name);
		if (he == NULL){ 
			herror("gethostbyname");
			return NULL;
		}
	}
	return he;
}

int getaddrinfo(const char *name, const char *service, const struct addrinfo *hints, struct addrinfo **res){
	int result;
	struct addrinfo *rptr;

	oldgetaddrinfo = dlsym(RTLD_NEXT, "getaddrinfo");

	if ( check_i2p(name) ){
		printf("Resolving %s by hooked getaddrinfo()\n", name);
		result = oldgetaddrinfo("test.com", service, hints, res); /* dummy resolve to fill data structures */
		assert(result == 0);
		for( rptr = *res; rptr != NULL; rptr = rptr->ai_next ) {
        	void *ipaddr;
			if (rptr->ai_family == AF_INET) {
				struct sockaddr_in *ipv4 = (struct sockaddr_in *)rptr->ai_addr;
				ipaddr = &(ipv4->sin_addr);
				inet_pton(AF_INET, getenv("FAKE_DEV_HOST"), ipaddr);
			}
		}
	} else{
		printf("Resolving %s by system getaddrinfo()\n", name);
		result = oldgetaddrinfo(name, service, hints, res);
	}
	return result;
}

