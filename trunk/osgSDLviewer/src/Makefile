# Set the appropriate options to match your platform

CXX = g++
CXXFLAGS = -g -O2 -Wall `sdl-config --cflags` `osg-config --cflags --dbg Base System`
LIBS = -lm -lGL -lGLU `sdl-config --libs` `osg-config --libs --dbg Base System` -lopal-ode 

APP_NAME = osgSDLViewer

OBJS = test.o SimulationEngine.o SDLViewer.o PhysicalEntity.o

$(APP_NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(APP_NAME) $(OBJS) $(LIBS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf *.o
	rm -rf *~
	rm -f $(APP_NAME)

.PHONY: clean
