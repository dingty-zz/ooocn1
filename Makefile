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


_HEADERS = netservice.h selectpool.h logger.h clientsocket.h linkedlist.h
HEADERS = $(patsubst %,$(IDIR)/%,$(_HEADERS))

_OBJ = main.o netservice.o selectpool.o logger.o clientsocket.o linkedlist.o\
	   pool_internal.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


lisod: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

$(ODIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)


.PHONY: clean

clean:
	@rm -f lisod \
		$(ODIR)/* $(SRCDIR)/*~ $(IDIR)/*~ $(SRCDIR)/*.orig $(IDIR)/*.orig \
		lockfile logfile

