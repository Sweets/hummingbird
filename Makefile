CC	= gcc

all: build

build: init.c
	$(CC) src/signals.c src/init.c -o hummingbird.o

install: all
	install -Dm755 "hummingbird.o" "/usr/bin/hummingbird"
