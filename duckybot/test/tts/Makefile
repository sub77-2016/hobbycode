# Created by Waipot Ngamsaad (waipot@ngamsaad.name)

# Compiler configuration
CXX = g++
CXXFLAGS = -Wall  -I../../include -I/usr/include/festival -I/usr/include/estools
LIBS = -L../src -lFestival -lestools -lestbase -leststring -lesd -lncurses -lm  

# Application variables
APP = test_TTS

OBJS = main.o tts.o

$(APP): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf *.o
	rm -rf *~
	rm -rf *.wave
	rm -f $(APP)

.PHONY: clean

# Dependencies tracking:
-include ./*.d
