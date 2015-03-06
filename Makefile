# simple makefile for preload-geettaddrinfo project
# v0.4

all : testaddrinfo getaddrinfo

.PHONY : testaddrinfo getaddrinfo

clean:
	rm -f testaddrinfo getaddrinfo.so

testaddrinfo:
	gcc -o testaddrinfo testaddrinfo.c

getaddrinfo:
	gcc -Wl,--no-as-needed -rdynamic -Wall -fPIC -shared -ldl -o getaddrinfo.so getaddrinfo.c

test: getaddrinfo.so testaddrinfo
	LD_PRELOAD=./getaddrinfo.so ./testaddrinfo delfi.lt
	LD_PRELOAD=./getaddrinfo.so ./testaddrinfo delfi.lv
	LD_PRELOAD=./getaddrinfo.so ./testaddrinfo delfi.i2p
	LD_PRELOAD=./getaddrinfo.so ./testaddrinfo asdf.onion

