;NSIS Modern User Interface
;Welcome/Finish Page Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI.nsh"

;--------------------------------
;General

  ;Name and file
  Name "Brutal Chess"
  OutFile "brutalchess.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\Brutal Chess"

  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\Brutal Chess" ""

  BrandingText " "

  RequestExecutionLevel admin

;--------------------------------
;Variables

  Var MUI_TEMP
  Var STARTMENU_FOLDER
  Var WIN_VERSION


;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "gpl.txt"
  !insertmacro MUI_PAGE_DIRECTORY

  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Brutal Chess" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
  !insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER


  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "Dummy Section" SecDummy

  SetOutPath "$INSTDIR\bin"

  File "Release\brutalchess.exe"
  File "C:\WINDOWS\System32\SDL.dll"
  File "C:\WINDOWS\System32\SDL_image.dll"
  File "C:\WINDOWS\System32\jpeg.dll"
  File "C:\WINDOWS\System32\libpng12.dll"
  File "C:\WINDOWS\System32\libtiff.dll"
  File "C:\WINDOWS\System32\zlib1.dll"
  File "C:\WINDOWS\System32\freetype6.dll"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Brutal Chess.lnk" "$INSTDIR\bin\brutalchess.exe"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_END

  
  ;Make sure the installer works on Win98+
  ;Check for NT based system
  ReadRegStr $WIN_VERSION HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion

  ;Windows XP, 2003, and Vista are only versions with SxS support 
  StrCmp $WIN_VERSION '5.1' lbl_SxS_yes ;XP
  StrCmp $WIN_VERSION '5.2' lbl_SxS_yes ;2003
  StrCmp $WIN_VERSION '6.0' lbl_SxS_yes ;Vista

  lbl_SxS_no:
  SetOutPath "$INSTDIR\bin\"
  Goto lbl_cpy_dll

  lbl_SxS_yes:
  SetOutPath "$INSTDIR\bin\Microsoft.VC80.CRT"
  File "C:\Program Files\Microsoft Visual Studio 8\VC\redist\x86\Microsoft.VC80.CRT\Microsoft.VC80.CRT.manifest"

  ;Copy the dlls to the system specific directory already set
  lbl_cpy_dll:
  File "C:\Program Files\Microsoft Visual Studio 8\VC\redist\x86\Microsoft.VC80.CRT\msvcm80.dll"
  File "C:\Program Files\Microsoft Visual Studio 8\VC\redist\x86\Microsoft.VC80.CRT\msvcp80.dll"
  File "C:\Program Files\Microsoft Visual Studio 8\VC\redist\x86\Microsoft.VC80.CRT\msvcr80.dll"


  ;Setup all the Brutal Chess Resources
  SetOutPath "$INSTDIR\models"

  File "models\pawn.obj"
  File "models\rook.obj"
  File "models\bishop.obj"
  File "models\king.obj"
  File "models\knight.obj"
  File "models\queen.obj"

  SetOutPath "$INSTDIR\art"
  File "art\brutalchesslogo.png"
  File "art\marblehugeblack.png"
  File "art\marblehugewhite.png"

  SetOutPath "$INSTDIR\fonts"
  File "fonts\ZEROES__.TTF"

  ;Store installation folder
  WriteRegStr HKCU "Software\Brutal Chess" "" $INSTDIR

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Brutal Chess" \
                 "DisplayName" "Brutal Chess"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Brutal Chess" \
                 "UninstallString" "$INSTDIR\uninstall.exe"

  SetOutPath "$INSTDIR"

  ;Install all the docs
  File /oname=AUTHORS.txt "AUTHORS"
  File /oname=ChangeLog.txt "ChangeLog"
  File /oname=LICENSE.txt "COPYING"
  File /oname=NEWS.txt "NEWS"
  File /oname=README.txt "README"

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

Section "Quick Launch icon" SecQuick
  SetOutPath "$INSTDIR\bin"
  StrCmp $QUICKLAUNCH $TEMP +2
    CreateShortcut "$QUICKLAUNCH\Brutal Chess.lnk" "$INSTDIR\bin\brutalchess.exe"
SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecDummy ${LANG_ENGLISH} "The core program"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;Delete the application and all of our dlls
  Delete "$INSTDIR\bin\brutalchess.exe"
  Delete "$INSTDIR\bin\stdout.txt"
  Delete "$INSTDIR\bin\stderr.txt"
  Delete "$INSTDIR\bin\SDL.dll"
  Delete "$INSTDIR\bin\SDL_image.dll"
  Delete "$INSTDIR\bin\jpeg.dll"
  Delete "$INSTDIR\bin\libpng12.dll"
  Delete "$INSTDIR\bin\libtiff.dll"
  Delete "$INSTDIR\bin\zlib1.dll"
  Delete "$INSTDIR\bin\freetype6.dll"
  Delete "$INSTDIR\bin\msvcm80.dll"
  Delete "$INSTDIR\bin\msvcp80.dll"
  Delete "$INSTDIR\bin\msvcr80.dll"

  Delete "$INSTDIR\bin\Microsoft.VC80.CRT\Microsoft.VC80.CRT.manifest"
  Delete "$INSTDIR\bin\Microsoft.VC80.CRT\msvcm80.dll"
  Delete "$INSTDIR\bin\Microsoft.VC80.CRT\msvcp80.dll"
  Delete "$INSTDIR\bin\Microsoft.VC80.CRT\msvcr80.dll"

  Delete "$INSTDIR\models\pawn.obj"
  Delete "$INSTDIR\models\rook.obj"
  Delete "$INSTDIR\models\bishop.obj"
  Delete "$INSTDIR\models\king.obj"
  Delete "$INSTDIR\models\knight.obj"
  Delete "$INSTDIR\models\queen.obj"

  Delete "$INSTDIR\art\brutalchesslogo.png"
  Delete "$INSTDIR\art\marblehugeblack.png"
  Delete "$INSTDIR\art\marblehugewhite.png"

  Delete "$INSTDIR\fonts\ZEROES__.TTF"

  Delete "$INSTDIR\AUTHORS.txt"
  Delete "$INSTDIR\ChangeLog.txt"
  Delete "$INSTDIR\LICENSE.txt"
  Delete "$INSTDIR\NEWS.txt"
  Delete "$INSTDIR\README.txt"

  Delete "$INSTDIR\Uninstall.exe"

  RMDir "$INSTDIR\bin\Microsoft.VC80.CRT"
  RMDir "$INSTDIR\bin"
  RMDir "$INSTDIR\art"
  RMDir "$INSTDIR\fonts"
  RMDir "$INSTDIR\models"
  RMDir "$INSTDIR"


  !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP

  Delete "$SMPROGRAMS\$MUI_TEMP\Brutal Chess.lnk"    
  Delete "$SMPROGRAMS\$MUI_TEMP\Uninstall.lnk"
  Delete "$QUICKLAUNCH\Brutal Chess.lnk"
  
  ;Delete empty start menu parent diretories
  StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"
 
  startMenuDeleteLoop:
	ClearErrors
    RMDir $MUI_TEMP
    GetFullPathName $MUI_TEMP "$MUI_TEMP\.."
    
    IfErrors startMenuDeleteLoopDone
  
    StrCmp $MUI_TEMP $SMPROGRAMS startMenuDeleteLoopDone startMenuDeleteLoop
  startMenuDeleteLoopDone:

  DeleteRegKey /ifempty HKCU "Software\Brutal Chess"

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Brutal Chess"

SectionEnd
