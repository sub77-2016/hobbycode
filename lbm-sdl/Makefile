# Set the appropriate options to match your platform

CXX = g++
CXXFLAGS = -g -O2 -Wall `sdl-config --cflags` 
LIBS = -lm -lGL -lGLU `sdl-config --libs` 

APP_NAME = lbm-sdl

OBJS = main.o lbgk2d.o 

$(APP_NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(APP_NAME) $(OBJS) $(LIBS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf *.o
	rm -rf *~
	rm -f $(APP_NAME)

.PHONY: clean
