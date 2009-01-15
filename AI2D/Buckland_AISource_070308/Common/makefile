FLAGS=-I./
CC=gcc
CPP=g++
AR=ar
REM=rm

INTREPID_OBJ=libAI2D.a

ZLIB=-lz
SDL=`sdl-config --cflags --libs`
SDL_TTF=-lSDL_ttf
SDL_GL=-lGL

# **************************

all: ai2d 
	$(REM) *.o

# **************************

#ai2d: 2D Debug FSM fuzzy Game Goals Graph Messaging Time Triggers
ai2d: 2D 
	$(AR) rvs $(INTREPID_OBJ) *.o
	@echo intrepid compiled

# **************************

2D: Vector2D.o
	@echo common compiled
	
Vector2D.o: 2D/Vector2D.cpp
	$(CPP) $(FLAGS) -c $<

