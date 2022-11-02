SHELL = /bin/bash
CC = gcc

SRC = src
OBJS = af.o afd.o afn.o util/misc.o util/stack.o util/set.o
OUT = out

CFLAGS = -Wall -g -I$(SRC)
LFLAGS = -lm

mkdirs = $(OUT)/grass

all: $(mkdirs) test.exe

$(mkdirs):
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
