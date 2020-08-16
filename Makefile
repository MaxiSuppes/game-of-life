CC = gcc
CFLAGS = -g -std=c99 -Wall -Werror
CFLAGS += -Wbad-function-cast
CFLAGS += -Wtype-limits -Wshadow -Wno-unused-variable
CFLAGS += -Wpointer-arith -Wunreachable-code -Wformat=2
CFLAGS += -pedantic

FILE=glider
STATES=40
ROWS=10
COLS=10

run: conway
	./$< $(STATES) $(ROWS) $(COLS) $(FILE)
	ffmpeg -start_number 1 -i "$(FILE)_%03d.pbm" "$(FILE).mov"
	rm -f *.pbm
conway: main.c neighbors.S
	$(CC) $(CFLAGS) *.c *.S -DUSE_MIPS -o $@ || $(CC) $(CFLAGS) *.c -o $@

clean:
	rm -f conway *.pbm *.mov *.o
