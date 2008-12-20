/*
 *  main.cpp
 *
 */

#include "SDLviewer.h"

using namespace SDLGL;

int main(int argc, char *argv[])
{
    SDLviewer viewer;
	
	viewer.init();
	viewer.startRendering();
    viewer.cleanup();
    
    return 0;
}
