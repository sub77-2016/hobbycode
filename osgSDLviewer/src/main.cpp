/*
 *  main.cpp
 *
 */

#include "SDLviewer.h"

using namespace SDLGL;

int main(int argc, char *argv[])
{
    SDLviewer viewer;
	
	viewer.InitApp();
	viewer.EventLoop();
    viewer.Cleanup();
    
    return 0;
}
