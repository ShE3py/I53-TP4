SHELL = /bin/bash
CC = gcc

SRC = src
OBJS = af.o afd.o afn.o compregex.o misc.o stack.o set.o vstack.o
OUT = out

CFLAGS = -Wall -g -I$(SRC)
LFLAGS = -L$(OUT) -laf -lm

mkdirs = $(OUT)/grass

all: $(mkdirs) test mydot mygrep

$(mkdirs):
	mkdir -p $(OUT)/gv/
	mkdir -p $(OUT)/png/
	touch $@

test: $(SRC)/test.c $(OUT)/libaf.a
	$(CC) $< $(CFLAGS) -o $@ $(LFLAGS)

mydot: $(SRC)/mydot.c $(OUT)/libaf.a
	$(CC) $< $(CFLAGS) -o $@ $(LFLAGS)

mygrep: $(SRC)/mygrep.c $(OUT)/libaf.a
	$(CC) $< $(CFLAGS) -o $@ $(LFLAGS)

$(OUT)/libaf.a: $(addprefix $(OUT)/,$(OBJS))
	ar rcs $@ $^

$(OUT)/%.o: $(SRC)/%.c $(SRC)/%.h
	$(CC) -c $< $(CFLAGS) -o $@

$(OUT)/%.o: $(SRC)/util/%.c $(SRC)/util/%.h
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -rf $(OUT)
	rm -f test mydot mygrep
