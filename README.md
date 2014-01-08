preload-getaddrinfo
===================

A simple `LD_PRELOAD` module for GNU/Linux, intercepting `getaddrinfo()` and `gethostbyname()` glibc functions

Originally was coded for complicated debugging task, but adapted 
for some practical use, e.g. resolving .i2p domains to your proxy server.

Use case
--------
You want to access @i2p links transparently, but no NOT want:
- modify your DNS server
- modify your browser proxy settings/domains inclusion, etc.

In this case getaddrinfo() preloader like this will help.

You should:
- Change hardcoded PROXY_IP #define in getaddrinfo.c to your proxy ip
- Setup your proxy server (like www.privoxy.org) listen on 80 port, using iptables, or privoxy configuration
- Setup your proxy server to use i2p
- preload gettaddrinfo.so to your browser instance you want, like:

````
$ LD_PRELOAD=./getaddrinfo.so chromium-browser
````

And suddenly you will be able transparently access @i2p sites by directly typing them into browser.
 
Also, you could extend it to some pretty sneak userland rootkit, ~~Hi, NSA~~, but better not do it.

Getting it
-------
Just clone this repo somewhere locally.

Building
--------
Get into this repo dir, usually by `cd preload-getaddrinfo`, or very similar, and do:
````
make getaddrinfo
````

Also, there is simple test program provided, `testaddrinfo.c`, and you can build it like

````
make testaddrinfo
````

You can automate both building and testing, by:
````
make test
````

Requirements
------------
Standart GNU/Linux gcc toolchain, git, and your favourite editor ;)


FAQ
---
`Q:` Does you need `root` or similar administrative privileges to use it?

`A:` No. 
