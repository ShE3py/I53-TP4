SHELL = /bin/bash
CC = gcc
CFLAGS = -Wall -g
LFLAGS = -lm
OUT = out
OBJS = afd.o afn.o

test.exe: $(addprefix $(OUT)/,af.o $(OBJS))
	$(CC) $^ $(CFLAGS) -o $@ $(LFLAGS)

$(OUT)/%.o: src/%.c src/%.h
	$(CC) -c $< $(CFLAGS) -o $@

$(OUT)/%.o: src/%.c
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -f $(OUT)/*
	rm -f *.exe
