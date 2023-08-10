.PHONY: all clean

SDL2_CFLAGS := $(shell sdl2-config --cflags)
SDL2_LIBS := $(shell sdl2-config --libs)
CFLAGS := -Og -g3 -W -Wall -Wshadow -Wstrict-prototypes -Wmissing-prototypes -fno-common
NAME := parser

all: parser tester

parser: parser.c Makefile
	gcc $(CFLAGS) $(SDL2_CFLAGS) -o $@ $< $(SDL2_LIBS) -lm

tester: tester.c Makefile linked_list.h
	gcc $(CFLAGS) -o $@ $< -lm

clean:
	rm -f parser tester *~

mem_$(NAME): Makefile parser.c
	gcc $(CFLAGS) $(SDL2_CFLAGS) -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -o $@ parser.c -lm $(SDL2_LIBS)

und_$(NAME): Makefile parser.c
	gcc $(CFLAGS) $(SDL2_CFLAGS) -fsanitize=undefined -o $@ parser.c -lm $(SDL2_LIBS)
