# Uncomment the appropriate line to match your platform:

ROOT = /usr
CC = gcc
CFLAGS = -Wall
LIBS = -lm

APP_NAME = flash51

OBJS = main.o programmer.o hexfiles.o udelay.o lpt.o isp.o

$(APP_NAME): $(OBJS)
	$(CC) $(CFLAGS) -O3 -o $(APP_NAME) $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $<

.cpp.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o
	rm -rf *~

.PHONY: clean
