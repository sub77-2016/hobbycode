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
ai2d: Game 
	$(AR) rvs $(INTREPID_OBJ) *.o
	@echo intrepid compiled

# **************************

Game: BaseGameEntity.o EntityManager.o
	@echo common compiled
	
BaseGameEntity.o: Game/BaseGameEntity.cpp
	$(CPP) $(FLAGS) -c $<

EntityManager.o: Game/EntityManager.cpp
	$(CPP) $(FLAGS) -c $<


