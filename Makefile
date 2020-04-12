CC	= gcc

all: build

build: src/init.c
	$(CC) -std=c99 src/signals.c src/init.c -o hummingbird.o

install: all seed_install
	install -Dm755 "hummingbird.o" "$(DESTDIR)/usr/bin/hummingbird"
	install -Dm755 "shutdown.sh" "$(DESTDIR)/usr/bin/shutdown"
	install -Dm755 "reboot.sh" "$(DESTDIR)/usr/bin/reboot"

seed_install:
	mkdir -p "$(DESTDIR)/usr/lib/hummingbird"
	dd count=1 bs=512 if=/dev/random of="$(DESTDIR)/usr/lib/hummingbird/random.seed"
