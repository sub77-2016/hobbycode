/*
 *  main.cpp
 *
 */

#include "SDLviewer.h"

int main(int argc, char *argv[])
{
    GameApp theGame;
	
	theGame.InitApp();
	theGame.EventLoop();
    theGame.Cleanup();
    
    return 0;
}
