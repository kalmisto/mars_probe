CFLAGS=-W -ggdb -O2 `sdl-config --cflags`
CC=gcc

.PHONY: default

default: mars_probe

mars_probe: mars_probe.c
	$(CC) $(CFLAGS) mars_probe.c -o peli `sdl-config --libs` -lm
