/*
 *  main.cpp
 *
 */

#include "SDLviewer.h"

using namespace SDLGL;

int main(int argc, char *argv[])
{
    //SDLviewer viewer;
    SDLviewer viewer(640,480);
	
	viewer.init();
	viewer.startRendering();
    viewer.cleanup();
    
    return 0;
}
