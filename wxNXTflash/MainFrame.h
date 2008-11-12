#ifndef _MainFrame_h_
#define _MainFrame_h_

#include <wx/string.h>
#include <wx/menu.h>
#include <wx/textctrl.h>

// MainFrame is the class for our window, it
// contains the window and all objects in it.
class MainFrame: public wxFrame
{
public:
  MainFrame( const wxString &title, const wxPoint &pos, const wxSize &size );
 
  void Quit( wxCommandEvent& event );
  void NewFile( wxCommandEvent& event );
  void OpenFile( wxCommandEvent& event );
  void SaveFile( wxCommandEvent& event );
  void SaveFileAs( wxCommandEvent& event );
  void CloseFile( wxCommandEvent& event );
 
  wxTextCtrl *MainEditBox;
  wxMenuBar *MainMenu;
  // The Path to the file we have open
  wxString CurrentDocPath;
 
  DECLARE_EVENT_TABLE()
};

#endif

