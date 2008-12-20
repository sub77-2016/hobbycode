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

#ifndef _SDLviewer_h_
#define _SDLviewer_h_

#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGPassiveWindow.h>
#include <OpenSG/OSGSimpleSceneManager.h>

namespace SDLGL { 
	
	class SDLviewer
	{   
    public:
        // Constructors and destructor
        SDLviewer (void);
        SDLviewer(const unsigned int w, const unsigned int h);
        
        virtual ~SDLviewer(void);

        // Initialization functions
        bool init(void);
        
      	// Main Render Loop functions
        void startRendering(void);
        
     	// Cleanup functions
        void cleanup(void);
        
        // Run the Renderer
        virtual void run(void);
        
        // Get Scene Manager
        osg::SimpleSceneManager* getSceneManager(void);
        
        
   	protected:
   		bool initializeSceneMgr(void);
   		
        bool initializeSDL(void);
        
        bool installTimer(void);
        
        static Uint32 timerLoop(Uint32 interval, void* param);  
                   
        void handleUserEvents(SDL_Event* event);  
        
        bool renderOneFrame(void);
        
        virtual void createScene(void);
        
        virtual void draw(void);
        
        SDL_TimerID mTimer;
        
        bool mDone, mFullScr;     
        
        unsigned int mWidth, mHeight;  
        
      	osg::SimpleSceneManager *mMgr;
    	// no GLUT window this time, but a passive one
    	osg::PassiveWindowPtr mPwin;
    	
    	// Root Scene node
    	osg::NodePtr mScene;
         
	};

	typedef SDLviewer* SDLviewerPtr;
	typedef SDLviewer** SDLviewerHandle;

	const int RUN_GAME_LOOP = 1;
}

#endif

