# Set the appropriate options to match your platform

CXX = g++
CXXFLAGS = -O2 -Wall -I $(HOME)/include -DUSE_GRAPHICS -DTEST_LB_
LIBS = -lm -L $(HOME)/lib -L /usr/X11R6/lib -lgraph -lX11 -lpng -lz

APP_NAME = lbmain 

OBJS = main.o lb2dmix.o lbrgb.o lbcolormap.o lbd2q9.o lbkernel.o 

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
