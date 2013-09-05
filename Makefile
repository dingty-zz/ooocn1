################################################################################
# Makefile
#
# Description: This file contains the make rules for 15-441 Project 1
#
#
# Authors:
#		Hongchao Deng (Timber) <hongchad@andrew.cmu.edu>
#
################################################################################

IDIR =include
ODIR=src/obj
SRCDIR=src

CC=gcc
LIBS=
CFLAGS=-I$(IDIR) -Wall -Werror


_HEADERS =
HEADERS = $(patsubst %,$(IDIR)/%,$(_HEADERS))

_OBJ = server.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


lisod: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

$(ODIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)


.PHONY: clean

clean:
	@rm -f lisod $(ODIR)/* $(SRCDIR)/*~ $(IDIR)/*~

