%: %.c
	gcc -Og -g3 -W -Wall -Wstrict-prototypes -Wmissing-prototypes -o $@ $<
