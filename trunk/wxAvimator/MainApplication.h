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
 *
 */

#ifndef _MainApplication_h
#define _MainApplication_h

#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/toolbar.h>
#include <wx/textctrl.h>
#include <wx/splitter.h>
#include <wx/stc/stc.h>

#include "AnimationView.h"
 
class MainApplication: public wxFrame // LuaScriptEditor is the class for our window,
{ // It contains the window and all objects in it
 public:
  /**
   * Singleton accessor to this class.
   */
  static MainApplication* getInstancePtr(){ return m_instance; };

  MainApplication( const wxString& title, const wxPoint& pos, const wxSize& size );

  // Main Animation Viewer
 AnimationView *animVwr;
  // Manu Bar
  wxMenuBar *MainMenu;
  // The Path to the file we have open
  wxString CurrentDocPath;
  // Splitter for output
  wxSplitterWindow* splitter;

  // File Menu Events
  void Quit(wxCommandEvent& event);
  void NewFile(wxCommandEvent& event);
  void OpenFile(wxCommandEvent& event);
  void SaveFile(wxCommandEvent& event);
  void SaveFileAs(wxCommandEvent& event);
  void CloseFile(wxCommandEvent& event);

  // Help Events
  void OnAbout(wxCommandEvent& event);

  // Output windows
  //void OnSplitHorizontal(wxCommandEvent& event);
  void OnPlay(wxCommandEvent& event);
  void OnOpenEditor(wxCommandEvent& event);

protected:
  DECLARE_EVENT_TABLE()

  /**
   * The one and only CIntroScreen object.
   * @seealso getInstance()
   */
  static MainApplication *m_instance;
};
 
enum
  {
    appTEXT_Main = wxID_HIGHEST + 1, // declares an id which will be used to call our button
    appMENU_New,
    appMENU_Open,
    appMENU_Close,
    appMENU_Save,
    appMENU_SaveAs,
    appMENU_Quit,
    appMENU_Play,
    appMENU_Editor,
    appMENU_About
  };
 
#endif 

