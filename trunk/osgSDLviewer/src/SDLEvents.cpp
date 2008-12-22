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

#include "SDLEvents.h"


namespace SDLGL { 
	
	// Constructors
	SDLEvents::SDLEvents()
	{
		mDone = false;
		mMinimized = false;
		
		initializeTimer();
	}

	// Destructor
	SDLEvents::~SDLEvents(void)
	{

	}
	
	bool SDLEvents::isDone(void)
	{
		return mDone;
	}
	
	bool SDLEvents::initializeTimer(void)
	{
    	mTimer = SDL_AddTimer(30, timerLoop, this);
    	
    	return true;
	}

	Uint32 SDLEvents::timerLoop(Uint32 interval, void* param)
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
	
	void SDLEvents::handleEvents(SDL_Event& event)
	{
		switch(event.type) 
        {
        	case SDL_USEREVENT:
           	//handleUserEvents(&event);
            	break;
                
           	case SDL_KEYDOWN:
             	// Quit when user presses Esc key.
              	switch(event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						mDone = true;
						break;

					default:
						OnKeyDown( 
							event.key.keysym.sym,
							event.motion.x, 
							event.motion.y, 
							event.motion.xrel, 
							event.motion.yrel);
						break;
				}
                break;
                
			case SDL_KEYUP:
				OnKeyUp( event.key.keysym.sym );
				break;
  
			case SDL_MOUSEMOTION:
				OnMouseMoved(
				event.button.button, 
				event.motion.x, 
				event.motion.y, 
				event.motion.xrel, 
				event.motion.yrel);
				break;
 
			case SDL_MOUSEBUTTONUP:
				OnMouseButtonUp(
					event.button.button, 
					event.motion.x, 
					event.motion.y, 
					event.motion.xrel, 
					event.motion.yrel);
				break;
 
			case SDL_MOUSEBUTTONDOWN:
				OnMouseButtonDown(
					event.button.button, 
					event.motion.x, 
					event.motion.y, 
					event.motion.xrel, 
					event.motion.yrel);
				break;
 
			case SDL_ACTIVEEVENT:
				if ( event.active.state & SDL_APPACTIVE ) {
					if ( event.active.gain ) 
					{
						mMinimized = false;
						OnWindowActive();
					} 
					else 
					{
						mMinimized = true;
						OnWindowInactive();
					}
				}
				break;
                	
			case SDL_VIDEORESIZE:
				{
					resize();
					/*
    				if ( !(mScreen = SDL_SetVideoMode(event.resize.w, event.resize.h, 
    													mBpp, mFlags)) )
   					{
   						mWidth = mScreen->w;
   						mHeight = mScreen->h;
   						resize();
   					}
   					*/
				}
				break;
            
            case SDL_QUIT:
                mDone = true;
                break;
                
            default:
                break;
        }   // End switch              
	}

	/*
	void SDLEvents::handleUserEvents(SDL_Event* event)
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
	*/

	void SDLEvents::redraw(void)
	{

	}
	
	void SDLEvents::resize(void)
	{

	}
	
	// Handle Key pressed
	void SDLEvents::OnKeyDown(const int& iKeyEnum,
								  const int& iX, 
					 			  const int& iY, 
					 			  const int& iRelX, 
					 			  const int& iRelY)
	{
		// Handle Keybord Input
		osg::UChar8	key;    
		
    	switch (iKeyEnum)
    	{
    		case SDLK_LEFT:
	  			// Left arrow released
	  			key = 't';
	  			break;
			case SDLK_RIGHT:
	  			// Right arrow released
	  			key = 'u';
	  			break;
			case SDLK_UP:
	  			// Up arrow released
	  			key = 'y';
	  			break;
			case SDLK_DOWN:
	  			// Down arrow released
	  			key = 'h';
	  			break;
	  			
	  		case SDLK_q:
	  			// Left arrow released
	  			key = 't';
	  			break;	  			
			case SDLK_e:
	  			// Right arrow released
	  			key = 'u';
	  			break;	  			
    		case SDLK_a:
      			// Left arrow pressed
      			key = 'g';
      			break;
    		case SDLK_d:
     			// Right arrow pressed
     			key = 'j';
      			break;
    		case SDLK_w:
      			// Up arrow pressed
      			key = 'y';
      			break;
    		case SDLK_s:
      			// Down arrow pressed
      			key = 'h';
      			break;
    	}
    	
    	//passs the event to the manager object
		//mMgr->key(key, iX, iY);
	} 
 
 	// Handle Key released
	void SDLEvents::OnKeyUp(const int& iKeyEnum)
	{
		switch (iKeyEnum)
		{
			case SDLK_LEFT:
	  		// Left arrow released
	  		break;
			case SDLK_RIGHT:
	  		// Right arrow released
	  		break;
			case SDLK_UP:
	  		// Up arrow released
	  		break;
			case SDLK_DOWN:
	  		// Down arrow released
	  		break;
		}
		
		//mMgr->idle();
	}
 
	void SDLEvents::OnMouseMoved(const int& iButton, 
			   const int& iX, 
			   const int& iY, 
			   const int& iRelX, 
			   const int& iRelY)
	{
		// Handle mouse movement
 
		// iX and iY are absolute screen positions
		// iRelX and iRelY are screen position relative to last detected mouse movement
		
		//mMgr->mouseMove(iX, iY);
	}
 
 	void SDLEvents::OnMouseButtonDown(const int& iButton, 
				const int& iX, 
				const int& iY, 
				const int& iRelX, 
				const int& iRelY)
	{
		// Handle mouse button pressed
		//osg::UInt32 button;
		// translate the button value from qt to OpenSG
		switch (iButton)
		{
    		case SDL_BUTTON_LEFT: 
    			//button = osg::SimpleSceneManager::MouseLeft;    
    			break;
    			
    		case SDL_BUTTON_MIDDLE:   
    			//button = osg::SimpleSceneManager::MouseMiddle; 
    			break;
    			
    		case SDL_BUTTON_RIGHT: 
    			//button = osg::SimpleSceneManager::MouseRight; 
    			break;
    			
    		default:
    			return;
		}
	
		//passs the event to the manager object
		//mMgr->mouseButtonPress(button, iX, iY);		
	}
	
	void SDLEvents::OnMouseButtonUp(const int& iButton, 
			      const int& iX, 
			      const int& iY, 
			      const int& iRelX, 
			      const int& iRelY)
	{
		// Handle mouse button released
		//osg::UInt32 button;
		// translate the button value from qt to OpenSG
		switch (iButton)
		{
    		case SDL_BUTTON_LEFT: 
    			//button = osg::SimpleSceneManager::MouseLeft;    
    			break;
    			
    		case SDL_BUTTON_MIDDLE:   
    			//button = osg::SimpleSceneManager::MouseMiddle; 
    			break;
    			
    		case SDL_BUTTON_RIGHT: 
    			//button = osg::SimpleSceneManager::MouseRight; 
    			break;
    			
    		default:
    			return;
		}
	
		//passs the event to the manager object
		//mMgr->mouseButtonRelease(button, iX, iY);		
	}
 
	void SDLEvents::OnWindowInactive()
	{
		// Pause game
	}
 
	void SDLEvents::OnWindowActive()
	{
		// Un-pause game
	}	

}
