#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "MainApplication.h"
#include "LuaScriptEditor.h"

#include "app.xpm"
#include "new.xpm"
#include "fileopen.xpm"
#include "filesave.xpm"
#include "exefile.xpm"
#include "delete.xpm"
 
BEGIN_EVENT_TABLE ( MainApplication, wxFrame )
  EVT_MENU(appMENU_New, MainApplication::NewFile)
  EVT_MENU(appMENU_Open, MainApplication::OpenFile)
  EVT_MENU(appMENU_Close, MainApplication::CloseFile)
  EVT_MENU(appMENU_Save, MainApplication::SaveFile)
  EVT_MENU(appMENU_SaveAs, MainApplication::SaveFileAs)
  EVT_MENU(appMENU_Quit, MainApplication::Quit)
  EVT_MENU(appMENU_Play, MainApplication::OnPlay)
  EVT_MENU(appMENU_Editor, MainApplication::OnOpenEditor)
  EVT_MENU(appMENU_About, MainApplication::OnAbout)
END_EVENT_TABLE()

/*
 * Set our instance pointer to NULL - this will ensure the
 * singleton accessor works as expected.
 */
MainApplication* MainApplication::m_instance = NULL;
 
MainApplication::MainApplication(const wxString& title,
				 const wxPoint& pos, const wxSize& size) :
 wxFrame((wxFrame *) NULL, -1, title, pos, size)
{
  // Singleton 
  m_instance = static_cast<MainApplication*>(this);

  // set the frame icon
  SetIcon(wxICON(sample));

#if wxUSE_MENUS

  MainMenu = new wxMenuBar();
  // The "File" item should be in the help menu
  wxMenu *FileMenu = new wxMenu();
  // The "Debug" 
  wxMenu *scriptMenu = new wxMenu;
  // The "About" item should be in the help menu
  wxMenu *helpMenu = new wxMenu;
 
  FileMenu->Append(appMENU_New, wxT("&New...\tControlN"),
		   wxT("Create a new file"));
  FileMenu->Append(appMENU_Open, wxT("&Open..."),
		   wxT("Open an existing file"));
  FileMenu->Append(appMENU_Save, wxT("&Save"),
		   wxT("Save the current document"));
  FileMenu->Append(appMENU_SaveAs, wxT("Save &As"),
		   wxT("Save the current document under a new file name"));
  FileMenu->AppendSeparator();
  FileMenu->Append(appMENU_Close, wxT("&Close"),
		   wxT("Close the current document"));
  FileMenu->AppendSeparator();
  FileMenu->Append(appMENU_Quit, wxT("&Quit"),
		   wxT("Quit the editor"));

  scriptMenu->Append(appMENU_Editor, wxT("Open Editor..."),
                     wxT("Clear Output Logs"));

  helpMenu->Append(appMENU_About, wxT("&About...\tF1"),
                     wxT("Show about dialog"));
 
  MainMenu->Append(FileMenu, wxT("File"));
  MainMenu->Append(scriptMenu, wxT("Script"));
  MainMenu->Append(helpMenu, wxT("&Help"));

  SetMenuBar(MainMenu);

#endif // wxUSE_MENUS

  wxToolBar* toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
				     wxTB_HORIZONTAL|wxNO_BORDER);
  wxBitmap bmpNew(new_xpm);
  wxBitmap bmpOpen(fileopen_xpm);
  wxBitmap bmpSave(filesave_xpm);
  wxBitmap bmpPlay(exefile_xpm);
  wxBitmap bmpClear(delete_xpm);

  toolBar->AddTool(appMENU_New, bmpNew, wxT("New"));
  toolBar->AddTool(appMENU_Open, bmpOpen, wxT("Open"));
  toolBar->AddTool(appMENU_Save, bmpSave, wxT("Save"));
  toolBar->AddSeparator();
  toolBar->AddTool(appMENU_Play, bmpPlay, wxT("Play"));
  toolBar->AddTool(appMENU_Editor, bmpClear, wxT("Clear"));
  toolBar->Realize();
  SetToolBar(toolBar);

  // initialize splitter
  splitter = new wxSplitterWindow(this, wxID_ANY, 
				  wxDefaultPosition, wxDefaultSize, wxSP_3D);

 // initialize main animation viewer
  new AnimationView(this);
  animVwr = AnimationView::getInstancePtr();
  Animation *anim = new Animation("./animations/avatar_female_walk.bvh");
  animVwr->setAnimation(anim);

  //splitter->Initialize(MainEditBox);
  // Unncomment this to prevent unsplitting
  //splitter->SetMinimumPaneSize(20);

#if wxUSE_STATUSBAR

  // create a status bar just for fun (by default with 1 pane only)
  CreateStatusBar(2);
  SetStatusText(_T("wxAvimator!"));

#endif // wxUSE_STATUSBAR
}
 
void MainApplication::NewFile(wxCommandEvent& WXUNUSED(event))
{

/*
  // Clear the edit box
  MainEditBox->Clear();
  // reset the path of our current open file
  CurrentDocPath = wxT("");
  // Set the Title to reflect the file open
  SetTitle(_("Edit - untitled *"));
*/
}
 
void MainApplication::OpenFile(wxCommandEvent& WXUNUSED(event))
{
/*
  wxFileDialog *OpenDialog = new wxFileDialog(this, _("Choose a file to open"), wxEmptyString, wxEmptyString,
					      _("Lua Script Files (*.lua)|*.lua|All Files (*.*)|*.*"),
					      wxFD_OPEN, wxDefaultPosition);
 
  // Creates a "open file" dialog with 4 file types
  if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "cancel"
    {
      CurrentDocPath = OpenDialog->GetPath();
 
      // Sets our current document to the file the user selected
      MainEditBox->LoadFile(CurrentDocPath); //Opens that file
      // Set the Title to reflect the  file open
      //SetTitle(wxString("Edit - ") << OpenDialog->GetFilename());
    }  
*/
}
 
void MainApplication::CloseFile(wxCommandEvent& WXUNUSED(event))
{
/*
  // Clear the Text Box
  MainEditBox->Clear();
  // Reset the current File being edited
  CurrentDocPath = wxT("");
  // Set the Title to reflect the file open
  SetTitle(_("Edit - untitled *"));
*/
}

void MainApplication::SaveFile(wxCommandEvent& event)
{
  /*
  if (CurrentDocPath.empty())
    {
      SaveFileAs(event);
      return;
    }

  // Save to the already-set path for the document
  if (MainEditBox->IsModified())
    MainEditBox->SaveFile(CurrentDocPath);
*/
}
 
void MainApplication::SaveFileAs(wxCommandEvent& WXUNUSED(event))
{
/*
  wxFileDialog *SaveDialog = new wxFileDialog(this, _("Save File As _?"), wxEmptyString, wxEmptyString,
					      _("Lua Script Files (*.lua)|*.lua|All Files (*.*)|*.*"),
					      wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
  
  // Creates a Save Dialog with 4 file types
  if (SaveDialog->ShowModal() == wxID_OK) // If the user clicked "OK"
    {
      CurrentDocPath = SaveDialog->GetPath();
      // set the path of our current document to the file the user chose to save under
      MainEditBox->SaveFile(CurrentDocPath); // Save the file to the selected path
      // Set the Title to reflect the file open
      //SetTitle(wxString("Edit - ") << SaveDialog->GetFilename());  // The "About" item should be in the help menu
    }
*/
}
 
void MainApplication::Quit(wxCommandEvent& WXUNUSED(event))
{

  Close(TRUE); // Tells the OS to quit running this process
}

/*
void MainApplication::OnSplitHorizontal(wxCommandEvent& WXUNUSED(event))
{
  if ( splitter->IsSplit() )
    splitter->Unsplit();
  
  MainEditBox->Show(true);
  OutputBox->Show(true);
  splitter->SplitHorizontally(  MainEditBox, OutputBox  );
}
*/

void MainApplication::OnPlay(wxCommandEvent& WXUNUSED(event))
{
  for (int i=0; i<1000; i++) {
	animVwr->render();
      animVwr->getAnimation()->stepForward();
  }
}

void MainApplication::OnOpenEditor(wxCommandEvent& WXUNUSED(event))
{
  LuaScriptEditor *ed = new LuaScriptEditor();
  ed->Show(true);
}

void MainApplication::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox(wxString::Format(_T("Welcome to %s!\n")
				_T("\n")
				_T("This is the minimal wxWidgets sample\n")
				_T("running under %s."),
				wxVERSION_STRING,
				wxGetOsDescription().c_str()
				),
	       _T("About wxWidgets minimal sample"),
	       wxOK | wxICON_INFORMATION,
	       this);
}


