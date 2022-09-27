IDIR=../include
ODIR=../obj
LDIR=../lib

CC=gcc
CFLAGS=-I$(IDIR)

LIBS=-lfl

_DEPS = node.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = lex.yy.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

run:
	make flex
	make scanner
	./scanner test.c

flex: lexical.l
	flex lexical.l

scanner: $(OBJ)
	$(CC) -o $@ $^ $(LIBS) $(CFLAGS)

bison: syntax.y
	bison -d syntax.y
	# mv ./syntax.tab.h ../include/syntax.tab.h

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ scanner