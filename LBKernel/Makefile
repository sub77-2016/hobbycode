# Set the appropriate options to match your platform

CXX = g++
CXXFLAGS = -O2 -Wall `fltk-config --cxxflags` -DUSE_GRAPHICS -DTEST_LB_
LIBS = `fltk-config --ldflags` -lmgl-fltk 

APP_NAME = lbmain 

OBJS = main.o lb2dmix.o lbd2q9.o lbkernel.o lbviewer.o

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
