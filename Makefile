SHELL = /bin/bash
CC = gcc

SRC = src
OBJS = fsa/dfa.o fsa/nfa.o fsa/compat.o util/set.o util/stack.o util/misc.o
OUT = out

CFLAGS = -Wall -g -I$(SRC)
LFLAGS = -lm

mkdirs = $(OUT)/grass

all: $(mkdirs) test.exe

$(mkdirs):
	mkdir -p $(OUT)/fsa/
	mkdir -p $(OUT)/util/
	touch $@

test.exe: $(addprefix $(OUT)/,$(OBJS) main.o)
	$(CC) $^ $(CFLAGS) -o $@ $(LFLAGS)

$(OUT)/%.o: $(SRC)/%.c $(SRC)/%.h
	$(CC) -c $< $(CFLAGS) -o $@

$(OUT)/%.o: $(SRC)/%.c
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -rf $(OUT)
	rm -f resources/*.png
	rm -f *.exe
