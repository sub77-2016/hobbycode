;--------------------------------
;Include Modern UI

  !include "MUI.nsh"

;--------------------------------
;General

  ;Name and file
  Name "Avimator" 
  OutFile "Avimator-0.4.1.exe"
  
  ;Default installation folder
  InstallDir "$PROGRAMFILES\Avimator"

  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\Avimator" ""

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING
 
;--------------------------------
;Pages

  !insertmacro MUI_PAGE_COMPONENTS 
  !insertmacro MUI_PAGE_DIRECTORY 
  !insertmacro MUI_PAGE_INSTFILES 

  !insertmacro MUI_UNPAGE_CONFIRM 
  !insertmacro MUI_UNPAGE_INSTFILES 
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;-------------------------------- 
;Installer Sections 
    
Section "Avimator" SecAvimator
 
;Add files
  SetOutPath "$INSTDIR"
  
  File "Avimator.exe"
  File "glu32.dll"
  File "Avimator.ico"
 
  SetOutPath "$INSTDIR\data"
  file "data\*.*"
  
  
;create desktop shortcut
;  CreateShortCut "$DESKTOP\Avimator.lnk" "$INSTDIR\Avimator.exe" "" "$INSTDIR\Avimator.ico"
 
;create start-menu items
  CreateDirectory "$SMPROGRAMS\Avimator"
  CreateShortCut "$SMPROGRAMS\Avimator\Uninstall.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0
;  CreateShortCut "$SMPROGRAMS\Avimator\Avimator.lnk" "$INSTDIR\Avimator.exe" "" "$INSTDIR\Avimator.exe" 0
  CreateShortCut "$SMPROGRAMS\Avimator\Avimator.lnk" "$INSTDIR\Avimator.exe" "" "$INSTDIR\Avimator.ico"
 
;write uninstall information to the registry
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Avimator" "DisplayName" "Avimator (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Avimator" "UninstallString" "$INSTDIR\Uninstall.exe"
 
  ;Store installation folder
  WriteRegStr HKCU "Software\Avimator" "" $INSTDIR

  WriteUninstaller "$INSTDIR\Uninstall.exe"
 
SectionEnd

Section "Second Life Animations" SecAnim

  SetOutPath "$INSTDIR\animations"
  File "animations\*.bvh"
  File "animations\*.txt"

SectionEnd
 
;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecAvimator ${LANG_ENGLISH} "Install Avimator (Required)"
  LangString DESC_SecAnim ${LANG_ENGLISH} "Install Second Life Animations (Optional)"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecAnim} $(DESC_SecAnim)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecAvimator} $(DESC_SecAvimator)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------    
;Uninstaller Section  
Section "Uninstall"
   
;Delete Files 
  RMDir /r "$INSTDIR\*.*"    
 
;Remove the installation directory
  RMDir "$INSTDIR"
  
;Delete Start Menu Shortcuts
  Delete "$DESKTOP\Avimator.lnk"
  Delete "$SMPROGRAMS\Avimator\*.*"
  RmDir  "$SMPROGRAMS\Avimator"
  
;Delete Uninstaller And Unistall Registry Entries
  DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Avimator"
  DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Avimator"  
  
  DeleteRegKey /ifempty HKCU "Software\Avimator"

SectionEnd
 
 