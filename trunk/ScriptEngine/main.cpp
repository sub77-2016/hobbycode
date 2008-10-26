#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "LuaScriptEditor.h"

class MainApp: public wxApp // MainApp is the class for our application
{ // MainApp just acts as a container for the window,
 public: // or frame in LuaScriptEditor
  virtual bool OnInit();
};
 
IMPLEMENT_APP(MainApp) // Initializes the MainApp class and tells our program
// to run it
bool MainApp::OnInit()
{
  // call the base class initialization method, currently it only parses a
  // few common command-line options but it could be do more in the future
  if ( !wxApp::OnInit() )
    return false;

  LuaScriptEditor *luaScriptEditor = new LuaScriptEditor(wxT("wxLuaEditor"), wxPoint(300,300),
				     wxSize(480, 320)); // Create an instance of our frame, or window
  luaScriptEditor->Show(TRUE); // show the window
  SetTopWindow(luaScriptEditor);// and finally, set it as the main window
  
  return TRUE;
}

