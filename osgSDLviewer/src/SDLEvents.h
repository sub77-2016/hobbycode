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

#ifndef _SDLEvents_h_
#define _SDLEvents_h_

#include <SDL.h>
//#include <SDL_opengl.h>

//#include <GL/gl.h>
//#include <GL/glu.h>

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGPassiveWindow.h>
#include <OpenSG/OSGPerspectiveCamera.h>
#include <OpenSG/OSGSimpleSceneManager.h>

namespace SDLGL { 
	
	class SDLEvents
	{   
    public:
        // Constructor and destructor
        SDLEvents();        
        virtual ~SDLEvents(void);
        
        bool isDone(void);

        
   	protected:
        
        virtual void redraw(void) = 0;

        virtual void resize(void) = 0;
        
        /// Window Handle
        /** OVERLOADED - Window is active again. **/
		virtual void OnWindowActive();
 
		/** OVERLOADED - Window is inactive. **/
		virtual void OnWindowInactive();
		
		/** OVERLOADED - Keyboard key has been pressed.
		@param iKeyEnum The key number.
		**/
		virtual void OnKeyDown(const int& iKeyEnum,
							   	  const int& iX, 
					 			  const int& iY, 
					 			  const int& iRelX, 
					 			  const int& iRelY);
 
 
		/** OVERLOADED - Keyboard key has been released.
		@param iKeyEnum The key number.
		**/
		virtual void OnKeyUp(const int& iKeyEnum);
 
		/** OVERLOADED - The mouse has been moved.
		@param iButton	Specifies if a mouse button is pressed.
		@param iX	The mouse position on the X-axis in pixels.
		@param iY	The mouse position on the Y-axis in pixels.
		@param iRelX	The mouse position on the X-axis relative to the last position, in pixels.
		@param iRelY	The mouse position on the Y-axis relative to the last position, in pixels.
 
		@bug The iButton variable is always NULL.
		**/
		virtual void OnMouseMoved(const int& iButton,
					   			  const int& iX, 
					 			  const int& iY, 
					 			  const int& iRelX, 
					 			  const int& iRelY);
 
		/** OVERLOADED - A mouse button has been released.
		@param iButton	Specifies if a mouse button is pressed.
		@param iX	The mouse position on the X-axis in pixels.
		@param iY	The mouse position on the Y-axis in pixels.
		@param iRelX	The mouse position on the X-axis relative to the last position, in pixels.
		@param iRelY	The mouse position on the Y-axis relative to the last position, in pixels.
		**/
 
		virtual void OnMouseButtonUp(const int& iButton, 
					 				 const int& iX, 
					 				 const int& iY, 
					 				 const int& iRelX, 
					 				 const int& iRelY);
 
		/** OVERLOADED - A mouse button has been pressed.
		@param iButton	Specifies if a mouse button is pressed.
		@param iX	The mouse position on the X-axis in pixels.
		@param iY	The mouse position on the Y-axis in pixels.
		@param iRelX	The mouse position on the X-axis relative to the last position, in pixels.
		@param iRelY	The mouse position on the Y-axis relative to the last position, in pixels.
		**/
		virtual void OnMouseButtonDown(const int& iButton, 
					 				   const int& iX, 
					 				   const int& iY, 
					 				   const int& iRelX, 
					 				   const int& iRelY);
    	
    private:
        
        bool initializeTimer(void);
        
        static Uint32 timerLoop(Uint32 interval, void* param);  
                   
        //void handleUserEvents(SDL_Event* event); 
                
       	void handleEvents(SDL_Event& event);  
       	
        SDL_TimerID mTimer;
        
        bool mDone;
        
        bool mMinimized;
     
	};

	//typedef SDLEvents* SDLEventsPtr;
	//typedef SDLEvents** SDLEventsHandle;

	const int RUN_GAME_LOOP = 1;
}

#endif

