#!/bin/sh
g++ main.cpp intrepid-gl.a `sdl-config --cflags --libs` -lSDL_ttf -lGL -lz

