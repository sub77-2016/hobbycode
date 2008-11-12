# Set the appropriate options to match your platform

CXX = g++
CXXFLAGS = -g -O2 -Wall -DLINUX `wx-config --cxxflags`
LIBS = -lm `wx-config --libs`

APP_NAME = wxNXTflash

OBJS = Main.o MainFrame.o

$(APP_NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(APP_NAME) $(OBJS) $(LIBS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf *.o
	rm -rf *~

.PHONY: clean
