# Created by Waipot Ngamsaad (waipot@ngamsaad.name)

# Compiler configuration
CXX = g++
CXXFLAGS = -g -O2 -Wall -I../../include -I/usr/local/include/sphinx3 -I/usr/local/include/sphinxbase
LIBS = -L../../src -L/usr/local/lib -ls3decoder -lsphinxad -lsphinxbase -lm  

all: main_livedecode.o main_livepretend.o
	$(CXX) $(CXXFLAGS) -o main_livedecode main_livedecode.o $(LIBS)
	$(CXX) $(CXXFLAGS) -o main_livepretend main_livepretend.o $(LIBS)

main_livedecode.o: main_livedecode.c
	$(CXX) $(CXXFLAGS) -c $<

main_livepretend.o: main_livepretend.c
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf *.o
	rm -rf *~
	rm -rf *.raw
	rm -f main_livedecode main_livepretend

.PHONY: clean

# Dependencies tracking:
-include ./*.d
