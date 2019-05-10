CC	= gcc

all: build

build: src/init.c
	$(CC) src/signals.c src/init.c -o hummingbird.o

install: all
	install -Dm755 "hummingbird.o" "$(DESTDIR)/usr/bin/hummingbird"
	install -Dm755 "shutdown.sh" "$(DESTDIR)/usr/bin/shutdown"
	install -Dm755 "reboot.sh" "$(DESTDIR)/usr/bin/reboot"
