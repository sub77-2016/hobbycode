FLAGS=
CC=gcc
CPP=g++
AR=ar
REM=rm

INTREPID_OBJ=libAI2D.a
PACKER_OBJ=packer

ZLIB=-lz
SDL=`sdl-config --cflags --libs`
SDL_TTF=-lSDL_ttf
SDL_GL=-lGL

# **************************

all: ai2d 
	$(REM) *.o

# **************************

ai2d: 2D Debug FSM fuzzy Game Goals Graph Messaging Time Triggers
	$(AR) rvs $(INTREPID_OBJ) *.o
	@echo intrepid compiled

# **************************

2D: Vector2d.o
	@echo common compiled
	
ag.o: Vector2d.cpp
	$(CPP) $(FLAGS) -c common/ag.cpp

# **************************

object: object.o phys_object.o
	@echo objects compiled

object.o: object/object.cpp
	$(CPP) $(FLAGS) -c object/object.cpp
phys_object.o: object/phys_object.cpp
	$(CPP) $(FLAGS) -c object/phys_object.cpp

# **************************

scene: scenes.o physics.o
	@echo scenes compiled

scenes.o: scenes/scenes.cpp
	$(CPP) $(FLAGS) -c scenes/scenes.cpp
physics.o: scenes/physics.cpp
	$(CPP) $(FLAGS) -c scenes/physics.cpp

# **************************

video: sprites.o surfaces.o opengl.o
	@echo video compiled

sprites.o: video/sprites.cpp
	$(CPP) $(FLAGS) -c video/sprites.cpp
surfaces.o: video/surfaces.cpp
	$(CPP) $(FLAGS) -c video/surfaces.cpp
opengl.o: video/opengl.cpp
	$(CPP) $(FLAGS) -c video/opengl.cpp

# **************************

gui: menu.o gui.o checkbox.o radiobutton.o textbox.o button.o
	@echo GUI compiled

menu.o: gui/menu.cpp
	$(CPP) $(FLAGS) -c gui/menu.cpp
gui.o: gui/gui.cpp
	$(CPP) $(FLAGS) -c gui/gui.cpp
checkbox.o: gui/checkbox.cpp
	$(CPP) $(FLAGS) -c gui/checkbox.cpp
radiobutton.o: gui/radiobutton.cpp
	$(CPP) $(FLAGS) -c gui/radiobutton.cpp
textbox.o: gui/textbox.cpp
	$(CPP) $(FLAGS) -c gui/textbox.cpp
button.o: gui/button.cpp
	$(CPP) $(FLAGS) -c gui/button.cpp

# **************************

pack: pack.o
	@echo pack compiled

pack.o: pack/pack.c
	$(CC) $(FLAGS) -c pack/pack.c

# **************************

packer: pack packer.o
	$(CC) $(FLAGS) -o $(PACKER_OBJ) pack.o packer.o $(SDL) $(SDL_TTF) $(ZLIB)
	$(REM) packer.o
	@echo packer compiled

packer.o: pack/packer.c
	$(CC) $(FLAGS) -c pack/packer.c
	
