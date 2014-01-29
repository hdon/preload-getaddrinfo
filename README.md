preload-getaddrinfo
===================

A simple `LD_PRELOAD` module for GNU/Linux, intercepting `getaddrinfo()` and `gethostbyname()` glibc functions

Originally was coded for complicated debugging task, but adapted 
for some practical use, e.g. resolving .i2p/.onion links to your proxy server.

Use case
--------
You want to access i2p/tor links transparently, but no NOT want:
- modify your DNS server
- modify your browser proxy settings/domains inclusion, etc.

In this case getaddrinfo() preloader like this will help.

You should:
- Change hardcoded `PROXY_IP` define in getaddrinfo.c to your proxy ip
- Setup your proxy server (like privoxy) to listen on 80 port, using iptables/privoxy config
- Setup your proxy server to use i2p/tor
- preload gettaddrinfo.so to your browser instance, like:
````
$ LD_PRELOAD=./getaddrinfo.so chromium-browser
````

And suddenly you will be able transparently access i2p/onion sites directly by typing them into browser.
 
Also, you could extend it to some pretty sneak userland rootkit, ~~Hi, NSA~~, but better not do it.

Getting it
----------
Just clone this repo somewhere locally.

Requirements
------------
Standart GNU/Linux gcc toolchain, git, and your favourite editor ;)

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

Example usage (assuming your privoxy ip is 192.168.1.1)
-------------------------------------------------------

- set up privoxy (/etc/privoxy/config) like:

````
accept-intercepted-requests 1
listen-address  0.0.0.0:8118

#i2p http proxy
forward .i2p 127.0.0.1:4444 

#tor socks proxy
forward-socks4a .onion localhost:9050 .
````

- set up iptables like:

````
-A PREROUTING -d 192.168.1.1/32 -p tcp -m tcp --dport 80 -j REDIRECT --to-ports 8118
-A OUTPUT -d 192.168.1.1/32 -p tcp -m tcp --dport 80 -j REDIRECT --to-ports 8118
````

- launch browser like:

````
LD_PRELOAD=./getaddrinfo.so chromium-browser
````

Limitations
------------
- will not work on suid/sgid binaries (unless your .so will be suid/sgid too)
- will not work on statically linked binaries (no need for shared functions)



BUGS
------------
There are some, for sure...


FAQ
---
`Q:` Does you need `root` or similar administrative privileges to use it?

`A:` No.

`Q:` How do you make chromium understand custom TLD, like .ip2

`A:` Enter address with `http://` prefix, like `http://eepnews.i2p/`, instead of just `eepnews.ip2`

