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


namespace SDLGL { 
	
	class SDLviewer
	{   
    public:
        // Constructor and destructor
        SDLviewer (void);
        virtual ~SDLviewer(void);

        // Initialization functions
        void InitApp(void);
        void InitializeSDL(void);
        void InstallTimer(void);
        static Uint32 GameLoopTimer(Uint32 interval, void* param);
        
        // Cleanup functions
        void Cleanup(void);
        
        // Event-related functions
        void EventLoop(void);
        void HandleUserEvents(SDL_Event* event);
        
        // Game related functions
        void GameLoop(void);
        void RenderFrame(void);
        
     protected:
        SDL_TimerID mTimer;
        bool mDone;        
	};

	typedef SDLviewer* SDLviewerPtr;
	typedef SDLviewer** SDLviewerHandle;

	const int RUN_GAME_LOOP = 1;
}

#endif

