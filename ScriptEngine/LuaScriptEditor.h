#ifndef _LuaScriptEditor_h_ // Make sure to only declare these classes once
#define _LuaScriptEditor_h_

#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/toolbar.h>
#include <wx/textctrl.h>
#include <wx/splitter.h>
#include <wx/stc/stc.h>

#include "LuaScriptEngine.h"
 
class LuaScriptEditor: public wxFrame // LuaScriptEditor is the class for our window,
{ // It contains the window and all objects in it
 public:
  LuaScriptEditor( const wxString& title, const wxPoint& pos, const wxSize& size );

  // Main Editor Box
  wxTextCtrl *MainEditBox;
  // Manu Bar
  wxMenuBar *MainMenu;
  // The Path to the file we have open
  wxString CurrentDocPath;
  // Splitter for output
  wxSplitterWindow* splitter;
  // Script Engine
  LuaScriptEngine *lua;
  // Show output
  wxTextCtrl *OutputBox;

  // File Menu Events
  void Quit(wxCommandEvent& event);
  void NewFile(wxCommandEvent& event);
  void OpenFile(wxCommandEvent& event);
  void SaveFile(wxCommandEvent& event);
  void SaveFileAs(wxCommandEvent& event);
  void CloseFile(wxCommandEvent& event);

  // Debug Events
  void OnLuaRun(wxCommandEvent& event);

  // Help Events
  void OnAbout(wxCommandEvent& event);

  // Output windows
  void OnSplitHorizontal(wxCommandEvent& event);
  void OnUnsplit(wxCommandEvent& event);
 
  DECLARE_EVENT_TABLE()
};
 
enum
  {
    TEXT_Main = wxID_HIGHEST + 1, // declares an id which will be used to call our button
    MENU_New,
    MENU_Open,
    MENU_Close,
    MENU_Save,
    MENU_SaveAs,
    MENU_Quit,
    MENU_Run,
    MENU_Unsplit,
    MENU_About
  };
 
#endif 

