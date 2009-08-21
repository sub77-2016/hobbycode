#!/bin/sh
# for a PC version
swig -lua -DLB_ENABLE_RGB lb-lua.i
# for parallel version
swig -lua -DLB_ENABLE_RGB -DLB_ENABLE_MPI -o lb-lua_wrap-mpi.c lb-lua.i
