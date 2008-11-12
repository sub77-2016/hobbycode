#include <wx/wxprec.h>
 
#ifndef WX_PRECOMP
#	include <wx/wx.h>
#endif
 
#include "MainFrame.h"

// MainApp is the class for our application, it just acts
// as a container for the window or frame in MainFrame.
class MainApp: public wxApp
{
public:
  virtual bool OnInit();
};

// Implements MyApp& GetApp()
DECLARE_APP(MainApp)

IMPLEMENT_APP(MainApp) // Initializes the MainApp class...
 
// .. and tells our program to run it
bool MainApp::OnInit()
{
  // Create an instance of our frame, or window
  MainFrame *MainWin = new MainFrame(_("Edit"), wxPoint(1, 1), wxSize(300, 200));
  MainWin->Show(TRUE); // show the window
  SetTopWindow(MainWin); // and finally, set it as the main window
  return TRUE;
}
