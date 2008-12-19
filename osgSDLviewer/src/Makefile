# Set the appropriate options to match your platform

CXX = g++
CXXFLAGS = -g -O2 -Wall `sdl-config --cflags`
LIBS = -lm -lGL -lGLU `sdl-config --libs`

APP_NAME = SDLviewer

OBJS = main.o SDLviewer.o

$(APP_NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(APP_NAME) $(OBJS) $(LIBS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf *.o
	rm -rf *~

.PHONY: clean
