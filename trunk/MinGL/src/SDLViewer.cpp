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

#include <SDL_opengl.h>

#include "SDLViewer.h"


namespace MINGL { 
	
    template<class SDLViewer> SDLViewer* Singleton<SDLViewer>::ms_Singleton = 0;   
    /* 
    SDLViewer* SDLViewer::getInstancePtr(void)
    {
        return ms_Singleton;
    }
    SDLViewer& SDLViewer::getInstance(void)
    {
        assert( ms_Singleton );  return ( *ms_Singleton );
    }
	*/
	// Constructors
	SDLViewer::SDLViewer(const unsigned int w, const unsigned int h, VideoMode mode)
	{
		mScreen = NULL;
		
		mMinimized = false;
    	mDone = false;
    	mTimer = 0;
    	
    	mScrMode = mode;
    	
    	mWidth = w;
    	mHeight = h;  	
    	mBpp = 0;

    	mCamera = NULL;
	}

	// Destructor
	SDLViewer::~SDLViewer(void)
	{
		//if (mScreen)
			//delete mScreen;
			
		if (mCamera)
			delete mCamera;
	}

	// Initialization functions
	bool SDLViewer::init(void)
	{			
    	if ( false == initializeSDL() )
    		return false;

		if ( false == initializeGL() )
			return false;
    	
    	return true;  
	}

	bool SDLViewer::initializeSDL(void)
	{
    	/* Information about the current video settings. */
    	const SDL_VideoInfo* info = NULL;
    
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
    		
    	if ( info->hw_available == 1 )
			mFlags = mFlags | SDL_HWSURFACE;
    	
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
   		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, 
   							SDL_DEFAULT_REPEAT_INTERVAL);
   							
   		mLastTick = SDL_GetTicks();
    	
    	return true;
	}
	
	bool SDLViewer::initializeGL(void)
	{	
		mCamera = new Camera();
		
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glPixelStorei( GL_PACK_ALIGNMENT, 1 );
  
		glDepthFunc( GL_LEQUAL );
		glEnable( GL_DEPTH_TEST );

		glEnable( GL_NORMALIZE );
		
		// switch off default light
		GLfloat nul[4]={0,0,0,0};
		glLightfv(GL_LIGHT0, GL_DIFFUSE, nul);
		glLightfv(GL_LIGHT0, GL_SPECULAR, nul);
		
		//glClearColor(0.9, 0.9, 0.8, 1.0);
		//glColor3f(0.0, 0.0, 0.0);
		//glPointSize(4.0);
		
		// avoid divide by zero
		if (mHeight == 0)
     		mHeight = 1;

		glViewport(0, 0, (GLint)mWidth, (GLint)mHeight);
		
		//GLfloat h = (GLfloat)mWidth/(GLfloat)mHeight;
		
		//glMatrixMode(GL_PROJECTION);
		//glLoadIdentity();
		
		//gluPerspective(40.0, h, 1, 5000.0);
		//gluOrtho2D(0.0, 4.0, 0.0, 3.0);
		//gluPerspective(90.0, h, 0.01, 1000.0);
		setProjection();
		mCamera->setModelView();
		
		//glMatrixMode(GL_MODELVIEW);		
		//glLoadIdentity();
						
		//glEnable(GL_TEXTURE_2D);	// Turn on texture mapping
		//glEnable(GL_DEPTH_TEST);

		//glCullFace(GL_BACK); // don't draw the back of faces
		//glEnable(GL_CULL_FACE);
		//glEnable(GL_NORMALIZE);
		//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0); //don't light the back of faces
		//glEnable(GL_LIGHTING);

		//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		//creating light source
		//lightPtr = new LightObject(0, -30, 10, 30);

		//float matAmbientAndDiffuse[4] = {.5, .5, .5, 1};
		//float matSpecular[4] = {1, 1, 1, 1};
		//float matShininess = 60;

		//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbientAndDiffuse);
		//glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
		//glMaterialf(GL_FRONT, GL_SHININESS, matShininess);

		//GLfloat ambient[] =  {0.1f, 0.1f, 0.1f, 1.0f};
		//GLfloat diffuse[] =  {.8f, .8f, 0.8f, 1.0f};
		//GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
		//GLfloat light_position[] = {20.0f, 20.0f, 30.0f, 0.0f};

		//glEnable(GL_LIGHTING);
		//glEnable(GL_LIGHT0);
		//glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		//glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
		//glLightfv(GL_LIGHT0, GL_POSITION, light_position);

		//make display lists here
		//humanPtr->MakeDisplayList();
			    	
    	// Set up Default Scene 	
    	createScene();    	
    	
    	return true;
	}
	
	void SDLViewer::setProjection(void)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		
  		gluPerspective(60.0, (GLfloat)mWidth/(GLfloat)mHeight, 0.1, 1000.0);
  		
  		//glMatrixMode(GL_MODELVIEW);		
		//glLoadIdentity();
	}

	// Cleanup functions
	void SDLViewer::cleanUp(void)
	{
    	SDL_bool success;
    	success = SDL_RemoveTimer(mTimer);
    
    	SDL_Quit();
	}

	Uint32 SDLViewer::timeElapsed(void)
	{
	 	Uint32 now = SDL_GetTicks();
        return (now - mLastTick);
    }
     
	// Main Render Loop functions
	void SDLViewer::startRendering(void)
	{
    	SDL_Event event;
    
    	// Infinite loop
    	mDone = false;
    	
    	//while( (!mDone) && (SDL_WaitEvent(&event)) ) 
    	while( !mDone )
    	{
    		// Pump message in all registered windows
    		//SDL_PumpEvents();
    		
    		//idle();
    		
    		while( SDL_PollEvent(&event) )
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
            	break;
                
           	case SDL_KEYDOWN:
             	// Quit when user presses Esc key.
              	switch(event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						mDone = true;
						break;

					default:
						OnKeyPress( 
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
				OnMouseMove(
				event.button.button, 
				event.motion.x, 
				event.motion.y, 
				event.motion.xrel, 
				event.motion.yrel);
				break;
 
			case SDL_MOUSEBUTTONUP:
				OnButtonUp(
					event.button.button, 
					event.motion.x, 
					event.motion.y, 
					event.motion.xrel, 
					event.motion.yrel);
				break;
 
			case SDL_MOUSEBUTTONDOWN:
				OnButtonDown(
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
					resize(event.resize.w, event.resize.h);
				}
				break;
            
            case SDL_QUIT:
                mDone = true;
                break;
                
            default:
                break;
        }   // End switch              
	}

	bool SDLViewer::renderOneFrame(void) 
	{
		// Lock surface if needed
		if ( SDL_MUSTLOCK( mScreen ) )
			if ( SDL_LockSurface( mScreen ) < 0 )
				return false;
				
		//Clear the color and depth buffers.
    	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    	 	
    	// Frame started
    	if (!frameStarted())
    		return false;    	
    	
    	// Update Scene
    	redraw();
    	
    	// Frame ended
    	if (!frameEnded())
    		return false;
    		    	
    	// Unlock if needed
		if ( SDL_MUSTLOCK( mScreen ) ) 
			SDL_UnlockSurface( mScreen );    	

    	// Swap Buffers
    	glFlush();
    	SDL_GL_SwapBuffers();
    	
    	return true;
	}
	
	void SDLViewer::run(void)
	{
		if ( !init() )
			return;
			
		startRendering();
		
		// clean up
		cleanUp();
	}
	
	void SDLViewer::createScene(void)
	{
		//gluLookAt(0.0, 1.0, 6.0,
           //0.0, 0.0, 0.0,
           //0.0, 1.0, 0.0);
           
		//glMatrixMode(GL_MODELVIEW);
  		//glLoadIdentity();
		//gluOrtho2D(0.0, 4.0, 0.0, 3.0);      
	} 
	
	void SDLViewer::redraw(void)
	{	           	
  		//glColor3f(0.7, 0.5, 0.8);
    	//glRectf(1.0, 1.0, 3.0, 2.0);  
    	
    	glBegin(GL_TRIANGLES);
  			glColor3f(1.0, 0.0, 0.0);
  			glVertex3f(2.0, 2.5, -1.0);
  			glColor3f(0.0, 1.0, 0.0);
  			glVertex3f(-3.5, -2.5, -1.0);
  			glColor3f(0.0, 0.0, 1.0);
  			glVertex3f(2.0, -4.0, 0.0);
		glEnd();
		
		glBegin(GL_POLYGON);
  			glColor3f(1.0, 1.0, 1.0);
  			glVertex3f(-1.0, 2.0, 0.0);
  			glColor3f(1.0, 1.0, 0.0);
  			glVertex3f(-3.0, -0.5, 0.0);
  			glColor3f(0.0, 1.0, 1.0);
  			glVertex3f(-1.5, -3.0, 0.0);
  			glColor3f(0.0, 0.0, 0.0);
  			glVertex3f(1.0, -2.0, 0.0);
  			glColor3f(1.0, 0.0, 1.0);
  			glVertex3f(1.0, 1.0, 0.0);
		glEnd();  	
		
		mCamera->setModelView();    			
	}
	
	void SDLViewer::resize(int w, int h)
	{
		if ( !(mScreen = SDL_SetVideoMode(w, h, mBpp, mFlags)) )
		{
			std::cerr<< "Unable to resize "<< w<<"x"<< h<< " video: %s\n"<< SDL_GetError()<< std::endl;
   			exit(1);
		}
		else
   		{
   			mWidth = mScreen->w;
   			mHeight = mScreen->h;
   			
   			// avoid divide by zero
			if (mHeight == 0)
     			mHeight = 1;
   			
   			glViewport(0, 0, (GLint)mWidth, (GLint)mHeight);
   			
   			//GLfloat h = (GLfloat)mWidth/(GLfloat)mHeight;

  			//glMatrixMode(GL_PROJECTION);
  			//glLoadIdentity();
  			
  			//glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);
  			//gluPerspective(40.0, h, 1, 5000.0);
  			//gluPerspective(90.0, h, 1.0, 100.0);
  			setProjection();
  			mCamera->setModelView();
  			
  			//glMatrixMode(GL_MODELVIEW);
  			//glLoadIdentity();
  			
  			//gluLookAt(0.0, 1.0, 6.0,
           			//0.0, 0.0, 0.0,
          			//0.0, 1.0, 0.0);
   		}
	}
	
	void SDLViewer::idle(void)
	{
		
	}
	
	bool SDLViewer::isFullScreen(void)
	{
		return (mScrMode == FULLSCREEN);	
	}
	
	bool SDLViewer::toggleFullScreen(void)
	{
		if ( SDL_WM_ToggleFullScreen(mScreen) )
		{
			if (mScrMode == WINDOWED)
				mScrMode = FULLSCREEN;
			else if (mScrMode == FULLSCREEN)
				mScrMode = WINDOWED;				
		}
		else
		{
			std::cerr<< "Unable to toggle full screen : %s\n"<< SDL_GetError()<< std::endl;
			return false;
		}
			
		return true; 
	}
	
	bool SDLViewer::isMinimized(void)
	{
		return mMinimized;	
	}
	
	// Handle Key pressed
	void SDLViewer::OnKeyPress(const int& iKeyEnum,
								  const int& iX, 
					 			  const int& iY, 
					 			  const int& iRelX, 
					 			  const int& iRelY)
	{
		// Handle Keybord Input	
    	switch (iKeyEnum)
    	{
    		case SDLK_LEFT:
	  			// Left arrow released
	  			mCamera->rotate(0, -5);
	  			break;
			case SDLK_RIGHT:
	  			// Right arrow released
	  			mCamera->rotate(0, +5);
	  			break;
			case SDLK_UP:
	  			// Up arrow released
	  			mCamera->pan(0, 0, -5);
	  			break;
			case SDLK_DOWN:
	  			// Down arrow released
	  			mCamera->pan(0, 0, +5);
	  			break;
	  				  			
    		case SDLK_a:
      			// Left arrow pressed
      			mCamera->rotate(0, -5);
      			break;
    		case SDLK_d:
     			// Right arrow pressed
     			mCamera->rotate(0, +5);
      			break;
    		case SDLK_w:
      			// Up arrow pressed
      			mCamera->pan(0, 0, -5);
      			break;
    		case SDLK_s:
      			// Down arrow pressed
      			mCamera->pan(0, 0, +5);
      			break;

			case SDLK_q:
	  			// Left arrow released
	  			break;	  			
			case SDLK_e:
	  			// Right arrow released
				break;
				
			case SDLK_F11:
	  			toggleFullScreen();
				break;
    	}
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
	}
 
	void SDLViewer::OnMouseMove(const int& iButton, 
			   const int& iX, 
			   const int& iY, 
			   const int& iRelX, 
			   const int& iRelY)
	{
		// Handle mouse movement 
		// iX and iY are absolute screen positions
		// iRelX and iRelY are screen position relative to last detected mouse movement		
	}
 
 	void SDLViewer::OnButtonDown(const int& iButton, 
				const int& iX, 
				const int& iY, 
				const int& iRelX, 
				const int& iRelY)
	{
		// Handle mouse button pressed
		switch (iButton)
		{
    		case SDL_BUTTON_LEFT:   
    			break;
    			
    		case SDL_BUTTON_MIDDLE:   
    			break;
    			
    		case SDL_BUTTON_RIGHT: 
    			break;
    			
    		default:
    			return;
		}	
	}
	
	void SDLViewer::OnButtonUp(const int& iButton, 
			      const int& iX, 
			      const int& iY, 
			      const int& iRelX, 
			      const int& iRelY)
	{
		// Handle mouse button released
		switch (iButton)
		{
    		case SDL_BUTTON_LEFT:   
    			break;
    			
    		case SDL_BUTTON_MIDDLE:   
    			break;
    			
    		case SDL_BUTTON_RIGHT: 
    			break;
    			
    		default:
    			return;
		}		
	}
 
	void SDLViewer::OnWindowInactive()
	{
		// pause 
	}
 
	void SDLViewer::OnWindowActive()
	{
		// un-pause 
	}	

}
