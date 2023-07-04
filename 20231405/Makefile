SDL2_CFLAGS := $(shell sdl2-config --cflags)
SDL2_LIBS := $(shell sdl2-config --libs)

parser: Makefile parser.c
	gcc -Og -g3 -W -Wall -Wshadow -Wstrict-prototypes -Wmissing-prototypes -fno-common $(SDL2_CFLAGS) -o $@ parser.c $(SDL2_LIBS)