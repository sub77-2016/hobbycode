/*
 * Created by Waipot Ngamsaad (ngamsaad.waipot@gmail.com)
 * Dated 20-12-2008

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <GL/gl.h>
#include <GL/glu.h>

#include "SDLviewer.h"


namespace SDLGL { 
	
	// Constructor
	SDLviewer::SDLviewer(void)
	{
    	mDone = false;
    	//drawContext = NULL;
	}

	// Destructor
	SDLviewer::~SDLviewer(void)
	{
		
	}

	// Initialization functions
	void SDLviewer::InitApp(void)
	{
    	InitializeSDL();
    	InstallTimer();    
	}

	void SDLviewer::InitializeSDL(void)
	{
    	int error;
    	SDL_Surface* drawContext;
    
    	error = SDL_Init(SDL_INIT_EVERYTHING);
    
    	// Create a double-buffered draw context
    	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
                        
    	Uint32 flags;
    	flags = SDL_OPENGL;// | SDL_FULLSCREEN;
    	drawContext = SDL_SetVideoMode(640, 480, 0, flags);
    
    	gluOrtho2D(0.0, 4.0, 0.0, 3.0);
    
	}

	void SDLviewer::InstallTimer(void)
	{
    	mTimer = SDL_AddTimer(30, GameLoopTimer, this);
	}

	Uint32 SDLviewer::GameLoopTimer(Uint32 interval, void* param)
	{
    	// Create a user event to call the game loop.
    	SDL_Event event;
    
    	event.type = SDL_USEREVENT;
    	event.user.code = RUN_GAME_LOOP;
    	event.user.data1 = 0;
    	event.user.data2 = 0;
    
    	SDL_PushEvent(&event);
    
    	return interval;
	}

	// Cleanup functions
	void SDLviewer::Cleanup(void)
	{
    	SDL_bool success;
    	success = SDL_RemoveTimer(mTimer);
    
    	SDL_Quit();
	}

	// Event-related functions
	void SDLviewer::EventLoop(void)
	{
    	SDL_Event event;
    
    	while((!mDone) && (SDL_WaitEvent(&event))) 
    	{
        	switch(event.type) 
        	{
            	case SDL_USEREVENT:
                	HandleUserEvents(&event);
                	break;
                
            	case SDL_KEYDOWN:
                	// Quit when user presses a key.
                	mDone = true;
                	break;
            
            	case SDL_QUIT:
                	mDone = true;
                	break;
                
            	default:
                	break;
        	}   // End switch
            
    	}   // End while
        
	}

	void SDLviewer::HandleUserEvents(SDL_Event* event)
	{
    	switch (event->user.code) 
    	{
        	case RUN_GAME_LOOP:
            	GameLoop();
            	break;
            
        	default:
            	break;
    	}
	}

	// Game related functions
	void SDLviewer::GameLoop(void)
	{
    	RenderFrame();    
	}

	void SDLviewer::RenderFrame(void) 
	{
    	glClear(GL_COLOR_BUFFER_BIT);
    	glColor3f(0.7, 0.5, 0.8);
    	glRectf(1.0, 1.0, 3.0, 2.0);
    	SDL_GL_SwapBuffers();
	}

}
