#!/bin/sh
g++ `sdl-config --cflags` -I../../intrepid_0-2_alpha  -c main.cpp
g++ `sdl-config --libs` -o main main.o -L../../intrepid_0-2_alpha/src/ -lintrepid-gl -lSDL_ttf -lGL -lz

