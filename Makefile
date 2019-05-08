CC	= gcc

all: build

build: src/init.c
	$(CC) src/signals.c src/init.c -o hummingbird.o

install: all
	install -Dm755 "hummingbird.o" "/usr/bin/hummingbird"
	install -Dm755 "shutdown.sh" "/usr/bin/shutdown"
	install -Dm755 "reboot.sh" "/usr/bin/reboot"
