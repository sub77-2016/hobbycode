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

#include "SDLViewer.h"


namespace SDLGL { 
	
	// Constructors
	SDLViewer::SDLViewer(const unsigned int w, const unsigned int h, VideoMode video)
	{
		mScreen = NULL;
		
		mMinimized = false;
    	mDone = false;
    	
    	mScrMode = video;
    	
    	mWidth = w;
    	mHeight = h;  	
    	mBpp = 0;

    	mMgr = NULL;
    	//mPwin = 0;
    	//mScene = 0;
    	//mCamera = 0;
	}

	// Destructor
	SDLViewer::~SDLViewer(void)
	{
		if (mMgr)
			delete mMgr;
			
		//if (mPwin)
			//delete mPwin;
			
		//if (mScene)
			//delete mScene;
	}

	// Initialization functions
	bool SDLViewer::init(void)
	{			
    	if ( !initializeSDL() )
    		return false;
    		
    	if ( !initializeTimer() )
    		return false;  
    		
		if ( !initializeSceneMgr() )
			return false;
    	
    	return true;  
	}

	bool SDLViewer::initializeSDL(void)
	{
    	//int width, height, bpp;
    	
    	/* Drawing Context */
    	//SDL_Surface* screen = NULL;
    	
    	/* Information about the current video settings. */
    	const SDL_VideoInfo* info = NULL;
    	
    	//Uint32 flags;
    	
    
    	// init SDL
    	if ( SDL_Init(SDL_INIT_EVERYTHING) < 0 )
    	{
    		std::cerr<< "Unable to init SDL: %s\n"<< SDL_GetError()<< std::endl;
    		exit(1);
    	}
    	atexit(SDL_Quit);
    	
    	// Let's get some video information.
    	if ( !(info = SDL_GetVideoInfo()) )
    	{
    		std::cerr<< "Video query failed: %s\n"<< SDL_GetError()<< std::endl;
    		exit(1);
    	}    	
    	
   		//SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    	//SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
    	//SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    	//SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    
    	// Create a double-buffered draw context
    	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    	
    	mBpp = info->vfmt->BitsPerPixel;
    	//width = (int)mWidth;
    	//height = (int)mHeight;
    	
    	mFlags = SDL_OPENGL | SDL_RESIZABLE;
    	    	
    	if ( isFullScreen() )
    		mFlags = mFlags | SDL_FULLSCREEN;
    	
    	if ( !(mScreen = SDL_SetVideoMode(mWidth, mHeight, mBpp, mFlags)) )
   		{
   			std::cerr<< "Unable to set "<< mWidth<<"x"<< mHeight<< " video: %s\n"<< SDL_GetError()<< std::endl;
   			exit(1);
   		}
   		// Make sure we have the correct size
   		mWidth = mScreen->w;
   		mHeight = mScreen->h;
   		
   		// Set window title
   		SDL_WM_SetCaption("SDL OpenGL Viewer", "OpenGL");
   		   		
   		SDL_EnableUNICODE(1);
    	
    	return true;
	}

	bool SDLViewer::initializeTimer(void)
	{
    	mTimer = SDL_AddTimer(30, timerLoop, this);
    	
    	return true;
	}

	Uint32 SDLViewer::timerLoop(Uint32 interval, void* param)
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
	
	bool SDLViewer::initializeSceneMgr(void)
	{
		//osg::PassiveWindowPtr mPwin = osg::PassiveWindow::create();
		mPwin = osg::PassiveWindow::create();
		    	
		mMgr = new osg::SimpleSceneManager;		
    	mMgr->setWindow(mPwin); 
    	
    	//osg::NodePtr camBeacon = osg::Node::create();
    	
    	//osg::beginEditCP(camBeacon);
    		//camBeacon>addChild(osg::makeTorus(.5, 2, 16, 16));
    	//osg::endEditCP(camBeacon);
    	
    	// Setup camera
    	//mCamera = osg::PerspectiveCamera::create();    	

    	//osg::beginEditCP(mCamera);
        	//mCamera->setBeacon( camBeacon );
        	//mCamera->setFov( osg::deg2rad( 60 ) );
        	//mCamera->setNear( 0.5 );
        	//mCamera->setFar( 8000 );
    	//osg::endEditCP(mCamera);    	
    	
    	// Set up viewport 
    	//osg::ViewportPtr viewport = osg::Viewport::create();
    	//osg::ViewportPtr viewport;
    	//viewport = mMgr->getWindow()->getPort(0);
    	
    	//osg::beginEditCP(viewport);
			//viewport->setCamera(mCamera);
			//viewport->setBackground(leftBkg);
			//viewport->setRoot(mScene);
			//viewport->setSize(0,0,0.5,1);
		//osg::endEditCP(viewport);   
		
		//mPwin->getPort(0) = viewport;		
     	
		mPwin->init();  
		
		mScene = osg::Node::create();	

		//osg::beginEditCP(mScene);
			//mScene->setCore(osg::Group::create());
			//mScene->addChild(camBeacon);			
		//osg::endEditCP(mScene);			
			    	
    	// Set up Default Scene 	
    	createScene();       	

		mMgr->setRoot(mScene);
    	//mMgr->setNavigationMode(osg::Navigator::WALK);
    	mMgr->resize(mWidth, mHeight); 
   		
    	mMgr->showAll(); 
    	
    	return true;
	}

	// Cleanup functions
	void SDLViewer::cleanup(void)
	{
    	SDL_bool success;
    	success = SDL_RemoveTimer(mTimer);
    
    	SDL_Quit();
	}

	// Main Render Loop functions
	void SDLViewer::startRendering(void)
	{
    	SDL_Event event;
    
    	// Infinite loop
    	mDone = false;
    	
    	while((!mDone) && (SDL_WaitEvent(&event))) 
    	{
    		// Pump message in all registered windows
    		SDL_PumpEvents();
    		
    		handleEvents(event);
    		
    		if ( !renderOneFrame() )
    			return;
    	}
	}
	
	void SDLViewer::handleEvents(SDL_Event& event)
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
    				if ( !(mScreen = SDL_SetVideoMode(event.resize.w, event.resize.h, 
    													mBpp, mFlags)) )
   					{
   						mWidth = mScreen->w;
   						mHeight = mScreen->h;
   						resize();
   					}
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
	void SDLViewer::handleUserEvents(SDL_Event* event)
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

	bool SDLViewer::renderOneFrame(void) 
	{
		// Lock surface if needed
		if ( SDL_MUSTLOCK( mScreen ) )
			if ( SDL_LockSurface( mScreen ) < 0 )
				return false;
					
    	//Clear the color and depth buffers.
    	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    	 	
    	// Update Scene
    	redraw();
    	
    	// Unlock if needed
		if ( SDL_MUSTLOCK( mScreen ) ) 
			SDL_UnlockSurface( mScreen );    	

    	// Swap Buffers
    	SDL_GL_SwapBuffers();
    	
    	return true;
	}
	
	void SDLViewer::run(void)
	{
		if ( !init() )
			return;
			
		startRendering();
		
		// clean up
		cleanup();
	}
	
	void SDLViewer::createScene(void)
	{
		//gluOrtho2D(0.0, 4.0, 0.0, 3.0);   

    	mScene = osg::makeTorus(.5, 2, 16, 16);
    	//mMgr->setRoot(mScene);
    	//mMgr->showAll();   
	} 
	
	void SDLViewer::redraw(void)
	{
		mMgr->redraw();
		
		//glColor3f(0.7, 0.5, 0.8);
    	//glRectf(1.0, 1.0, 3.0, 2.0);  	
	}
	
	void SDLViewer::resize(void)
	{
		mMgr->resize(mWidth, mHeight);	
		//mPwin->init();	
	}
	
	osg::SimpleSceneManager* SDLViewer::getSceneManager(void)
	{
		return mMgr;	
	}
	
	bool SDLViewer::isFullScreen(void)
	{
		return (mScrMode == FULLSCREEN);	
	}
	
	bool SDLViewer::isMinimized(void)
	{
		return mMinimized;	
	}
	
	// Handle Key pressed
	void SDLViewer::OnKeyDown(const int& iKeyEnum,
								  const int& iX, 
					 			  const int& iY, 
					 			  const int& iRelX, 
					 			  const int& iRelY)
	{
		// Handle Keybord Input
		osg::UChar8	key = ' ';    
		
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
		mMgr->key(key, iX, iY);
	} 
 
 	// Handle Key released
	void SDLViewer::OnKeyUp(const int& iKeyEnum)
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
 
	void SDLViewer::OnMouseMoved(const int& iButton, 
			   const int& iX, 
			   const int& iY, 
			   const int& iRelX, 
			   const int& iRelY)
	{
		// Handle mouse movement
 
		// iX and iY are absolute screen positions
		// iRelX and iRelY are screen position relative to last detected mouse movement
		
		mMgr->mouseMove(iX, iY);
	}
 
 	void SDLViewer::OnMouseButtonDown(const int& iButton, 
				const int& iX, 
				const int& iY, 
				const int& iRelX, 
				const int& iRelY)
	{
		// Handle mouse button pressed
		osg::UInt32 button;
		// translate the button value from qt to OpenSG
		switch (iButton)
		{
    		case SDL_BUTTON_LEFT: 
    			button = osg::SimpleSceneManager::MouseLeft;    
    			break;
    			
    		case SDL_BUTTON_MIDDLE:   
    			button = osg::SimpleSceneManager::MouseMiddle; 
    			break;
    			
    		case SDL_BUTTON_RIGHT: 
    			button = osg::SimpleSceneManager::MouseRight; 
    			break;
    			
    		default:
    			return;
		}
	
		//passs the event to the manager object
		mMgr->mouseButtonPress(button, iX, iY);		
	}
	
	void SDLViewer::OnMouseButtonUp(const int& iButton, 
			      const int& iX, 
			      const int& iY, 
			      const int& iRelX, 
			      const int& iRelY)
	{
		// Handle mouse button released
		osg::UInt32 button;
		// translate the button value from qt to OpenSG
		switch (iButton)
		{
    		case SDL_BUTTON_LEFT: 
    			button = osg::SimpleSceneManager::MouseLeft;    
    			break;
    			
    		case SDL_BUTTON_MIDDLE:   
    			button = osg::SimpleSceneManager::MouseMiddle; 
    			break;
    			
    		case SDL_BUTTON_RIGHT: 
    			button = osg::SimpleSceneManager::MouseRight; 
    			break;
    			
    		default:
    			return;
		}
	
		//passs the event to the manager object
		mMgr->mouseButtonRelease(button, iX, iY);		
	}
 
	void SDLViewer::OnWindowInactive()
	{
		// Pause game
	}
 
	void SDLViewer::OnWindowActive()
	{
		// Un-pause game
	}	

}
