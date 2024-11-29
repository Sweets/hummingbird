CC ?= gcc
CFLAGS ?= -O2 -Wall -Werror -Wno-error=unused-result -pedantic -std=gnu99
INSTALL ?= install -Dm755
PREFIX ?= /usr
BINDIR ?= $(PREFIX)/bin
LIBDIR ?= $(PREFIX)/lib

.PHONY: clean install

hummingbird: src/signal.c src/shutdown.c src/init.c src/hummingbird.c
	$(CC) $(CFLAGS) src/signal.c src/shutdown.c src/init.c src/hummingbird.c -o hummingbird

install: hummingbird
	mkdir -p "$(DESTDIR)/usr/lib/hummingbird"
	dd count=1 bs=512 if=/dev/urandom of="$(DESTDIR)/usr/lib/hummingbird/random.seed"

	$(INSTALL) hummingbird "$(DESTDIR)$(BINDIR)/hummingbird"
	$(INSTALL) bin/shutdown "$(DESTDIR)$(BINDIR)/shutdown"
	$(INSTALL) bin/reboot "$(DESTDIR)$(BINDIR)/reboot"
	
	$(INSTALL) usr/lib/hummingbird/fs "$(DESTDIR)$(LIBDIR)/hummingbird/fs"
	$(INSTALL) usr/lib/hummingbird/tty "$(DESTDIR)$(LIBDIR)/hummingbird/tty"
	$(INSTALL) usr/lib/hummingbird/shutdown "$(DESTDIR)$(LIBDIR)/hummingbird/shutdown"

clean:
	rm hummingbird
