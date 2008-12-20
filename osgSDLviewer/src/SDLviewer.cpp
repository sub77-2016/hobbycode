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

#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>

#include "SDLviewer.h"


namespace SDLGL { 
	
	// Constructors
	SDLviewer::SDLviewer(void)
	{
    	mDone = false;
    	mFlags = SDL_OPENGL | SDL_RESIZABLE | SDL_FULLSCREEN;
    	//drawContext = NULL;
	}
	
	SDLviewer::SDLviewer(const unsigned int w, const unsigned int h)
	{
    	mDone = false;
    	mWidth = w;
    	mHeight = h;
    	mFlags = SDL_OPENGL | SDL_RESIZABLE;
    	//drawContext = NULL;
	}

	// Destructor
	SDLviewer::~SDLviewer(void)
	{
		
	}

	// Initialization functions
	bool SDLviewer::init(void)
	{
    	if ( !initializeSDL() )
    		return false;
    		
    	if ( !installTimer() )
    		return false;  
    	
    	return true;  
	}

	bool SDLviewer::initializeSDL(void)
	{
    	int error;
    	SDL_Surface* drawContext;
    
    	// init SDL
    	if ( (error = SDL_Init(SDL_INIT_EVERYTHING) < 0) )
    	{
    		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
    		exit(1);
    	}
    	atexit(SDL_Quit);
    	
    	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
    	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    
    	// Create a double-buffered draw context
    	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    	
    	if ( !(drawContext = SDL_SetVideoMode(mWidth, mHeight, 0, mFlags)) )
   		{
   			std::cerr<<"Unable to set "<<mWidth<<"x"<<mHeight<<" video: %s\n"<< SDL_GetError()<<std::endl;
   			exit(1);
   		}
   		
   		SDL_EnableUNICODE(1);
    
    	gluOrtho2D(0.0, 4.0, 0.0, 3.0);    
    	
    	return true;
	}

	bool SDLviewer::installTimer(void)
	{
    	mTimer = SDL_AddTimer(30, timerLoop, this);
    	
    	return true;
	}

	Uint32 SDLviewer::timerLoop(Uint32 interval, void* param)
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
	void SDLviewer::cleanup(void)
	{
    	SDL_bool success;
    	success = SDL_RemoveTimer(mTimer);
    
    	SDL_Quit();
	}

	// Main Render Loop functions
	bool SDLviewer::startRendering(void)
	{
    	SDL_Event event;
    
    	// Infinite loop
    	mDone = false;
    	
    	while((!mDone) && (SDL_WaitEvent(&event))) 
    	{
        	switch(event.type) 
        	{
            	case SDL_USEREVENT:
                	handleUserEvents(&event);
                	break;
                
            	case SDL_KEYDOWN:
                	// Quit when user presses Esc key.
                	//if 
                	mDone = true;
                	break;
            
            	case SDL_QUIT:
                	mDone = true;
                	break;
                
            	default:
                	break;
        	}   // End switch            
    	}   // End while 
    	
    	return true;       
	}

	void SDLviewer::handleUserEvents(SDL_Event* event)
	{
    	switch (event->user.code) 
    	{
        	case RUN_GAME_LOOP:
        		renderOneFrame();
            	break;
            
        	default:
            	break;
    	}
	}

	bool SDLviewer::renderOneFrame(void) 
	{
    	glClear(GL_COLOR_BUFFER_BIT);
    	
    	// Update Scene
    	draw();

    	// Swap Buffers
    	SDL_GL_SwapBuffers();
    	
    	return true;
	}
	
	void SDLviewer::draw()
	{
		glColor3f(0.7, 0.5, 0.8);
    	glRectf(1.0, 1.0, 3.0, 2.0);
	}

}
