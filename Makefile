CC       ?= aarch64-apple-darwin-clang
STRIP    ?= aarch64-apple-darwin-strip
LDID     ?= ldid
CFLAGS   ?= -arch arm64  -isysroot /home/cameron/Documents/SDK/iPhoneOS14.3.sdk -miphoneos-version-min=13.0
GINSTALL ?= install
PREFIX   ?= /usr
DESTDIR  ?=

SRC := getent.c getutxent.c

all: getent

install: install-getent

copy-headers:
	cp -af $(MACOSX_SYSROOT)/usr/include/netinet include/
	cp -af $(MACOSX_SYSROOT)/usr/include/net include/

getent: $(SRC) ent.xml copy-headers
	$(CC) $(CFLAGS) -o getent -Iinclude $(SRC)
	$(STRIP) getent
	$(LDID) -Sent.xml getent

install-getent: getent getent.1
	$(GINSTALL) -Dm755 getent $(DESTDIR)/$(PREFIX)/bin/getent
	$(GINSTALL) -Dm644 getent.1 $(DESTDIR)/$(PREFIX)/share/man/man1/getent.1

clean:
	rm -f getent

.PHONY: all install install-getent clean copy-headers
