# Minimal LBM demonstration program by Nils Thuerey 
# See http://www.ntoken.com and http://www.matthiasmueller.info/realtimephysics/ for further info
# The code is released under the GPL, see LICENSE.txt for details

EXE=lbm
SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))

# default without OpenMP
#LDFLAGS    =   
#CFLAGS     = -c -O3  

# uncomment the following lines (instead of the 2 above) for OpenMP 
# support with GCC:
CXX=g++-4.2
LDFLAGS    = -fopenmp -lgomp 
CFLAGS     = -c -fopenmp -O3 

default: $(EXE)

$(EXE): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $(EXE) $(OBJECTS)

%.o : %.cpp
	$(CXX) $(CFLAGS) -o $@ $<

clean:
	 rm -f *.o $(EXE) *~

