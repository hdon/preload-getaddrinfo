/* testaddrinfo.c */
/* copyright (C) by paulius@chroot.lt 2014 */
/* v0.2 */
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char *argv[]){
	struct addrinfo hints, *res, *rptr;
	struct hostent *he;
	struct in_addr **addr_list;
	char ipaddr_str[INET_ADDRSTRLEN];
	int i;

    if (argc != 2) {
        printf("Usage: %s <address>\n", argv[0]);
        return 1;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

	if ( (getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
		printf("Error resolving %s\n", argv[1]);
		return 1;
	}   

	for( rptr = res; rptr != NULL; rptr = rptr->ai_next ) {
        void *ipaddr;
		if (rptr->ai_family == AF_INET) {
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)rptr->ai_addr;
			ipaddr = &(ipv4->sin_addr);
		}

		inet_ntop(rptr->ai_family, ipaddr, ipaddr_str, sizeof ipaddr_str);
		printf("Resolving by getaddrinfo() %s to %s\n", argv[1], ipaddr_str);

	}
    freeaddrinfo(res);

	if ((he = gethostbyname(argv[1])) == NULL) {  
		herror("gethostbyname");
        return 1;
    }

	addr_list = (struct in_addr **)he->h_addr_list;
	for( i = 0; addr_list[i] != NULL; i++) {
		printf("Resolving by gethostbyname() %s to %s\n", argv[1], inet_ntoa(*addr_list[i]) );
	}

    return 0;
}

