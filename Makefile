CC := gcc
<<<<<<< HEAD
CFLAGS := -O2 -Wall -Werror -Wno-unused-variable -pedantic -std=gnu99
=======
CFLAGS := -O2 -Wall -Werror -Wno-error=unused-result -pedantic -std=c99
>>>>>>> c562c32f974992a9ec6b5da6810f547b3fd8d9a0
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
