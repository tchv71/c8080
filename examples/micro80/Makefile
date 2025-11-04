platform=$(shell uname -o)
static=-static
ext=
asm=
ifeq ($(platform),Darwin)
	static=
	ext=.macos
	asm=-A../../include/sjasmplus.macos
endif

all: micro80.bin

.PHONY: clean
clean:
	rm micro80.bin micro80.lst micro80.asm

micro80.bin: micro80.c Makefile
	../../c8080${ext} ${asm} -m micro80.c