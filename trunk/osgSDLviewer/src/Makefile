# Set the appropriate options to match your platform

CXX = g++
CXXFLAGS = -g -O2 -Wall `sdl-config --cflags` `osg-config --cflags --dbg Base System`
LIBS = -lm -lGL -lGLU `sdl-config --libs` `osg-config --libs --dbg Base System`

APP_NAME = SDLViewer

OBJS = test.o SDLViewer.o

$(APP_NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(APP_NAME) $(OBJS) $(LIBS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf *.o
	rm -rf *~
	rm -f $(APP_NAME)

.PHONY: clean
