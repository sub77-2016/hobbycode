# Set the appropriate options to match your platform

CXX = g++
CXXFLAGS = -O2 -Wall -I./ `fltk-config --cxxflags` -DUSE_GRAPHICS -DTEST_LB_
LIBS = `fltk-config --ldflags` -lmgl

APP_NAME = lbmain 

OBJS = lb_main.o lb_d2q9_mix.o lb_d2q9.o lb_kernel.o lb_viewer.o mgl_fltk.o 

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
