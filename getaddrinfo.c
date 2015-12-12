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
#include <errno.h>
#include "map.h" // https://github.com/rxi/map

/* Has the hosts file map been initialized? */
static int initialized = 0;
static map_t(struct in_addr) hostsMap;
/* Initialize hosts file map */
static int initializeHostsMap()
{
  int lineNo;
  char *buf;
  size_t bufSize;
  const char* hostsFilename;
  FILE* hostsFile;
  struct in_addr addr;
  if (initialized)
    return 0;
  hostsFilename = getenv("FAKE_HOSTS_FILE");
  if (!hostsFilename)
  {
    fprintf(stderr, "error: no FAKE_HOSTS_FILE environment variable\n");
    exit(1);
  }
  hostsFile = fopen(hostsFilename, "r");
  if (!hostsFile)
  {
    fprintf(stderr, "error: FAKE_HOSTS_FILE: open(\"%s\") %d: %s\n",
      hostsFilename, errno, strerror(errno));
    exit(1);
  }

  map_init(&hostsMap);
  buf = NULL;
  bufSize = 0;
  lineNo = 0;
  while (getline(&buf, &bufSize, hostsFile) > 0) {
    char *c, *caddr;
    //printf("line: %s\n", buf);
    c = strchr(buf, ':');
    if (!c)
    {
      fprintf(stderr, "error: FAKE_HOSTS_FILE: %s:%d: expected colon\n",
        hostsFilename, lineNo);
      exit(1);
    }
    *c = '\0';
    caddr = c+1;
    c = strchr(caddr, '\n');
    if (!c)
    {
      fprintf(stderr, "error: FAKE_HOSTS_FILE: %s:%d: expected newline\n",
        hostsFilename, lineNo);
      exit(1);
    }
    *c = '\0';
    if (!inet_aton(caddr, &addr))
    {
      fprintf(stderr, "error: FAKE_HOSTS_FILE: %s:%d: expected ip4 addr\n",
        hostsFilename, lineNo);
      exit(1);
    }
    map_set(&hostsMap, buf, addr);
    lineNo++;
  }
  free(buf);
  initialized = 1;
  //printf("initialized hosts map\n");
  //fprintf(stderr, "initialized hosts map\n");
  return 0;
}

/* Functions to be preloaded */
int (*oldgetaddrinfo)(const char *, const char *, const struct addrinfo *, struct addrinfo **)=NULL;
struct hostent* (*oldgethostbyname)(const char *)=NULL;

struct hostent *he=NULL;

struct hostent *gethostbyname(const char *name){
  struct in_addr *fakeAddr;
	struct in_addr **addr_list;
	int i;

  //printf("Resolving %s by hooked gethostbyname()\n", name);

  if (!initialized)
    initializeHostsMap();

  // hdon sez: any reason to resolve this symbol dynamically every time?
	oldgethostbyname = dlsym(RTLD_NEXT, "gethostbyname");

  fakeAddr = map_get(&hostsMap, name);
	if (fakeAddr)
  {
    //printf("  resolved name with FAKE_HOSTS_FILE\n");
    // hdon sez: any reason to resolve this hostname every time?
		he = oldgethostbyname("test.com"); /* dummy resolve to fill data structures, must always resolve */
		assert(he != NULL);
		addr_list = (struct in_addr **)he->h_addr_list;
		for( i = 0; addr_list[i] != NULL; i++) {
      *addr_list[i] = *fakeAddr;
		}
	}else{
    //printf("  resolving name with system gethostbyname()\n");
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
  struct in_addr *fakeAddr;
	struct addrinfo *rptr;

  if (!initialized)
    initializeHostsMap();

  // hdon sez: any reason to resolve this symbol dynamically every time?
	oldgetaddrinfo = dlsym(RTLD_NEXT, "getaddrinfo");

  fakeAddr = map_get(&hostsMap, name);
	if (fakeAddr)
  {
		//printf("Resolving %s by hooked getaddrinfo()\n", name);
    // hdon sez: any reason to resolve this hostname every time?
		result = oldgetaddrinfo("localhost", service, hints, res); /* dummy resolve to fill data structures */
		assert(result == 0);
		for( rptr = *res; rptr != NULL; rptr = rptr->ai_next ) {
      /* Only IPv4 is supported, currently */
			if (rptr->ai_family == AF_INET) {
        ((struct sockaddr_in*)rptr->ai_addr)->sin_addr = *fakeAddr;
			}
		}
	} else{
		//printf("Resolving %s by system getaddrinfo()\n", name);
		result = oldgetaddrinfo(name, service, hints, res);
	}
	return result;
}

