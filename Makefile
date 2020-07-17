CC := gcc
CFLAGS := -O2 -Wall -Werror -Wno-error=unused-result -pedantic -std=gnu99
INSTALL := install -Dm755

.PHONY: clean install

hummingbird: hummingbird.c
	$(CC) $(CFLAGS) hummingbird.c -o hummingbird

install: hummingbird
	mkdir -p "$(DESTDIR)/usr/lib/hummingbird"
	dd count=1 bs=512 if=/dev/urandom of="$(DESTDIR)/usr/lib/hummingbird/random.seed"

	$(INSTALL) hummingbird "$(DESTDIR)/usr/bin/hummingbird"
	$(INSTALL) bin/shutdown "$(DESTDIR)/usr/bin/shutdown"
	$(INSTALL) bin/reboot "$(DESTDIR)/usr/bin/reboot"

clean:
	rm hummingbird
