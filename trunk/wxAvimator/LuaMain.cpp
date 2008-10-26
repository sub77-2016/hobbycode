/*
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Copyright (C) 2006 by Vinay Pulim.
 * All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>

#include "MainApplication.h"
#include "AnimationView.h"
#include "LuaScriptEngine.h"

char execPath[256]="";

#ifdef MACOSX
#define SLASH '/'
#else
#define SLASH '\\'
#endif

class MyApp: public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();
};
 
 
IMPLEMENT_APP(MyApp)
 
  
bool MyApp::OnInit()
{
  	strcpy(execPath, ".");

	new MainApplication(wxT("MainApplication - Lua"), wxPoint(300,300),
				     wxSize(800, 600)); // Create an instance of our frame, or window
	MainApplication *mainApp = MainApplication::getInstancePtr();
	mainApp->Show(TRUE); // show the window
  	//SetTopWindow(mainApp);// and finally, set it as the main window

LuaScriptEngine *eng = new LuaScriptEngine();
eng->loadEngine();
eng->runScript("./test.lua");

/*
   // test code
    wxFrame *frame = new wxFrame((wxFrame *)NULL, -1,  wxT("Hello GL World"), wxPoint(300,300), wxSize(480,320));
    new AnimationView(frame); 
    frame->Show(TRUE);
*/

  Animation *anim = new Animation("./animations/avatar_express_sad.bvh");
  mainApp->animVwr->setAnimation(anim);
 for (int i=0; i<1000; i++) {
   mainApp->animVwr->render();
   mainApp->animVwr->getAnimation()->stepForward();
 }
  
    return TRUE;
}

int MyApp::OnExit()
{
 return wxApp::OnExit();
}

