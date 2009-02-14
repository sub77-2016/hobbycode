#!/bin/sh

g++ -c mathgl.cpp -I../
g++ -c ../mgl_fltk.cpp -I../ `fltk-config --cxxflags`
g++ -o mathgl mathgl.o mgl_fltk.o -lmgl `fltk-config --ldflags`
