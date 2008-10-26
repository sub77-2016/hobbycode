#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "LuaScriptEditor.h"

#include "app.xpm"
#include "new.xpm"
#include "fileopen.xpm"
#include "filesave.xpm"
#include "exefile.xpm"
#include "delete.xpm"
 
BEGIN_EVENT_TABLE ( LuaScriptEditor, wxFrame )
  EVT_MENU(MENU_New, LuaScriptEditor::NewFile)
  EVT_MENU(MENU_Open, LuaScriptEditor::OpenFile)
  EVT_MENU(MENU_Close, LuaScriptEditor::CloseFile)
  EVT_MENU(MENU_Save, LuaScriptEditor::SaveFile)
  EVT_MENU(MENU_SaveAs, LuaScriptEditor::SaveFileAs)
  EVT_MENU(MENU_Quit, LuaScriptEditor::Quit)
  EVT_MENU(MENU_Run, LuaScriptEditor::OnLuaRun)
  EVT_MENU(MENU_Unsplit, LuaScriptEditor::OnUnsplit)
  EVT_MENU(MENU_About, LuaScriptEditor::OnAbout)
END_EVENT_TABLE()
 
LuaScriptEditor::LuaScriptEditor(const wxString& title,
				 const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *) NULL, -1, title, pos, size)
{
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
 
  FileMenu->Append(MENU_New, wxT("&New...\tControlN"),
		   wxT("Create a new file"));
  FileMenu->Append(MENU_Open, wxT("&Open..."),
		   wxT("Open an existing file"));
  FileMenu->Append(MENU_Save, wxT("&Save"),
		   wxT("Save the current document"));
  FileMenu->Append(MENU_SaveAs, wxT("Save &As"),
		   wxT("Save the current document under a new file name"));
  FileMenu->AppendSeparator();
  FileMenu->Append(MENU_Close, wxT("&Close"),
		   wxT("Close the current document"));
  FileMenu->AppendSeparator();
  FileMenu->Append(MENU_Quit, wxT("&Quit"),
		   wxT("Quit the editor"));

  scriptMenu->Append(MENU_Run, wxT("Run...\tF7"),
                     wxT("Execute Lua Scripts"));

  scriptMenu->Append(MENU_Unsplit, wxT("&Clear"),
                     wxT("Clear Output Logs"));

  helpMenu->Append(MENU_About, wxT("&About...\tF1"),
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
  wxBitmap bmpRun(exefile_xpm);
  wxBitmap bmpClear(delete_xpm);
  toolBar->AddTool(MENU_New, bmpNew, wxT("New"));
  toolBar->AddTool(MENU_Open, bmpOpen, wxT("Open"));
  toolBar->AddTool(MENU_Save, bmpSave, wxT("Save"));
  toolBar->AddTool(MENU_Run, bmpRun, wxT("Run"));
  toolBar->AddTool(MENU_Unsplit, bmpClear, wxT("Clear"));
  toolBar->Realize();
  SetToolBar(toolBar);

  // initialize splitter
  /*splitter = new wxSplitterWindow(this, wxID_ANY, wxPoint(0, 0), 
    wxSize(400, 400), wxSP_3D);*/
  splitter = new wxSplitterWindow(this, wxID_ANY, 
				  wxDefaultPosition, wxDefaultSize, wxSP_3D);
 
  MainEditBox = new wxTextCtrl(splitter, TEXT_Main, wxT("--print('Start Lua...')\n"), 
			       wxDefaultPosition, wxDefaultSize,
			       wxTE_MULTILINE | wxTE_RICH , wxDefaultValidator, wxTextCtrlNameStr);

  OutputBox = new wxTextCtrl(splitter, wxID_ANY, wxT("Output Logs:"), 
			       wxDefaultPosition, wxDefaultSize,
			       wxTE_MULTILINE | wxTE_RICH , wxDefaultValidator, wxTextCtrlNameStr);

  OutputBox->SetEditable(false);
  OutputBox->Show(false);

  splitter->Initialize(MainEditBox);
  // Unncomment this to prevent unsplitting
  splitter->SetMinimumPaneSize(20);

  // initilize Script Engine (Singleton)
  lua = LuaScriptEngineStonT::getInstancePtr();
  lua->loadEngine();

#if wxUSE_STATUSBAR

  // create a status bar just for fun (by default with 1 pane only)
  CreateStatusBar(2);
  SetStatusText(_T("Welcome to LuaEditor!"));

#endif // wxUSE_STATUSBAR
}
 
void LuaScriptEditor::NewFile(wxCommandEvent& WXUNUSED(event))
{
  // Clear the edit box
  MainEditBox->Clear();
  // reset the path of our current open file
  CurrentDocPath = wxT("");
  // Set the Title to reflect the file open
  SetTitle(_("Edit - untitled *"));
}
 
void LuaScriptEditor::OpenFile(wxCommandEvent& WXUNUSED(event))
{
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
}
 
void LuaScriptEditor::CloseFile(wxCommandEvent& WXUNUSED(event))
{
  // Clear the Text Box
  MainEditBox->Clear();
  // Reset the current File being edited
  CurrentDocPath = wxT("");
  // Set the Title to reflect the file open
  SetTitle(_("Edit - untitled *"));
}
 
void LuaScriptEditor::SaveFile(wxCommandEvent& WXUNUSED(event))
{
  /*
  if (CurrentDocPath.empty())
    {
      SaveFileAs(&WXUNUSED(event));
      return;
    }
  */
  // Save to the already-set path for the document
  if (MainEditBox->IsModified())
    MainEditBox->SaveFile(CurrentDocPath);
}
 
void LuaScriptEditor::SaveFileAs(wxCommandEvent& WXUNUSED(event))
{
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
}
 
void LuaScriptEditor::Quit(wxCommandEvent& WXUNUSED(event))
{
  lua->unloadEngine();

  Close(TRUE); // Tells the OS to quit running this process
}

void LuaScriptEditor::OnLuaRun(wxCommandEvent& WXUNUSED(event))
{
  wxString code = MainEditBox->GetValue();
  char msg[256];
  
  // run lua script
  //int error = lua->runString((char *)code.char_str());
  int error = lua->loadBuffer((char *)code.char_str(), &msg[0]);
  //wxMessageBox(wxString::Format(_T("Result, %d"),result));
  //wxMessageBox(code);

  if ( splitter->IsSplit() )
    splitter->Unsplit();
  MainEditBox->Show(true);
  OutputBox->Show(true);
  splitter->SplitHorizontally(  MainEditBox, OutputBox  ); 
 
  // Show Output
  OutputBox->Clear();

  if (!error)
    OutputBox->AppendText(wxString::Format(_T("Script Running Success!\n")));
  else
    OutputBox->AppendText(wxString::Format(_T("%s"), msg));
}

void LuaScriptEditor::OnSplitHorizontal(wxCommandEvent& WXUNUSED(event))
{
  if ( splitter->IsSplit() )
    splitter->Unsplit();
  
  MainEditBox->Show(true);
  OutputBox->Show(true);
  splitter->SplitHorizontally(  MainEditBox, OutputBox  );
}

void LuaScriptEditor::OnUnsplit(wxCommandEvent& WXUNUSED(event))
{
  if ( splitter->IsSplit() )
    splitter->Unsplit();
}

void LuaScriptEditor::OnAbout(wxCommandEvent& WXUNUSED(event))
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
