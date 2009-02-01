# Compiler flags
CXX = g++
CXXFLAGS  =  -g -Wall -I $(HOME)/include 
LIBS = -lm -L $(HOME)/lib -L /usr/X11R6/lib -lgraph -lX11

APP_NAME = main

OBJS = prog2.o 

$(APP_NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(APP_NAME) $(OBJS) $(LIBS)
	rm -rf *.o

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf *.o
	rm -rf *~
	rm -f $(APP_NAME)

.PHONY: clean

