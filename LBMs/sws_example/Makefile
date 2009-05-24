# Minimal shallow water demonstration program by Nils Thuerey 
# See http://www.ntoken.com and http://www.matthiasmueller.info/realtimephysics/ for further info
# The code is released under the GPL, see LICENSE.txt for details

EXE=sws
SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))

LDFLAGS    =  
CFLAGS     = -c -O3  

#LDFLAGS    = -c -g 
#CFLAGS     = -O1 -g 

# uncomment the following lines (instead of the 2 above) for OpenMP support:
#CXX=g++-4.2
#LDFLAGS    = -c -fopenmp -lgomp 
#CFLAGS     = -fopenmp -O3 

default: $(EXE)

$(EXE): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $(EXE) $(OBJECTS)

%.o : %.cpp
	$(CXX) $(CFLAGS) -o $@ $<

clean:
	 rm -f *.o $(EXE) *~

