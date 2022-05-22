CC       ?= aarch64-apple-darwin-clang
STRIP    ?= aarch64-apple-darwin-strip
LDID     ?= ldid
CFLAGS   ?= -arch arm64 -isysroot $(TARGET_SYSROOT) -miphoneos-version-min=13.0
GINSTALL ?= install
PREFIX   ?= /usr

SRC := $(wildcard *.c)

all: getent

copy-headers:
	cp -af $(MACOSX_SYSROOT)/usr/include/net include/
	cp -af $(MACOSX_SYSROOT)/usr/include/netinet include/

getent: $(SRC) ent.xml copy-headers
	$(CC) $(CFLAGS) -o $@ -Iinclude $(SRC)
	$(STRIP) $@
	$(LDID) -S$(word 3,$^) $@

install: getent getent.1
	$(GINSTALL) -Dm755 $< -t $(DESTDIR)$(PREFIX)/bin
	$(GINSTALL) -Dm644 $(word 2,$^) -t $(DESTDIR)$(PREFIX)/share/man/man1

clean:
	rm -f getent

.PHONY: all clean copy-headers
