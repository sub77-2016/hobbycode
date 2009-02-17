# Set the appropriate options to match your platform

CXX = g++
CXXFLAGS = -O2 -Wall -I ../ -I $(HOME)/include `fltk-config --cxxflags` -DUSE_GRAPHICS -DTEST_LB_
LIBS = -L ../ -L $(HOME)/lib `fltk-config --ldflags` -lmgl -lgraph -ltiny_lb

APP_NAME = lb_d2q9_mix 

OBJS = lb_d2q9_mix_main.o

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
