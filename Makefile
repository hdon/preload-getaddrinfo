# simple makefile for preload-geettaddrinfo project
# v0.3

.PHONY : testaddrinfo getaddrinfo

clean: rm testaddrinfo getaddrinfo.so

all: testaddrinfo getaddrinfo test

testaddrinfo:
	gcc -o testaddrinfo testaddrinfo.c

getaddrinfo:
	gcc -Wl,--no-as-needed -rdynamic -Wall -fPIC -shared -ldl -o getaddrinfo.so getaddrinfo.c

test: getaddrinfo.so testaddrinfo
	LD_PRELOAD=./getaddrinfo.so ./testaddrinfo delfi.lt
	LD_PRELOAD=./getaddrinfo.so ./testaddrinfo delfi.lv
	LD_PRELOAD=./getaddrinfo.so ./testaddrinfo delfi.i2p

