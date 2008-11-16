# Compiler flags
CC = gcc
CFLAGS  =  -std=c99 -O3 -Wall -I $(HOME)/include 
LDFLAGS =  -std=c99 -O3 -Wall -L $(HOME)/lib -L /usr/X11R6/lib 
# For debugging
#CFLAGS  =  -std=c99 -g -Wall -I $(HOME)/include 
#LDFLAGS =  -std=c99 -g -Wall -L $(HOME)/lib -L /usr/X11R6/lib 

LIBS = -lm -lgraph -lX11

% : %.c

% : %.o
	$(CC) $(LDFLAGS) $< $(LIBS) -o $@

%.o : %.c
	$(CC) -c $(CFLAGS)$<  -o $@

clean:
	rm -rf *~

.PHONY: clean

