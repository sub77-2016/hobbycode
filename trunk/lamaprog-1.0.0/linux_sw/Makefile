all: prog

CC=gcc
CFLAGS=-O3
PERL=/usr/bin/perl

prog : prog.o hexfile.o com.o
	$(CC) $(CFLAGS) -Wall -o prog prog.o hexfile.o com.o

prog.o: prog.c
	$(CC) $(CFLAGS) -c -Wall prog.c

hexfile.o : hexfile.c
	$(CC) $(CFLAGS) -c -Wall hexfile.c

com.o : com.c
	$(CC) $(CFLAGS) -c -Wall com.c 

po:
	xgettext -j --no-location -a prog.c -o prog.po
#	msgfmt -f prog.po -o cs_CZ/LC_MESSAGES/prog.mo
	msgfmt -f prog.po -o prog.mo

clear:
	rm -f *.o *~

install:
	$(PERL) install.pl