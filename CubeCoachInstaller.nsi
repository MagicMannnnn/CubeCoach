!include "MUI2.nsh"

# Installer information
Name "CubeCoach"
OutFile "dist\CubeCoachInstaller.exe"
InstallDir "$PROGRAMFILES64\CubeCoach"
RequestExecutionLevel admin

# Pages
!insertmacro MUI_PAGE_LICENSE "LICENSE.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

# Uninstaller pages
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Language
!insertmacro MUI_LANGUAGE "English"



# Files to install
Section "Install"
  SetOutPath "$INSTDIR\bin"
  File "out\build\x64-release\CubeCoach.exe"

  SetOutPath "$DOCUMENTS\CubeCoach\resources"
  File /r "resources\*.*"

  # Create a start menu shortcut
  CreateDirectory "$SMPROGRAMS\\CubeCoach"
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  CreateShortCut "$SMPROGRAMS\\CubeCoach\\CubeCoach.lnk" "$INSTDIR\\bin\\CubeCoach.exe"

SectionEnd

# Uninstall section
Section "Uninstall"
  Delete "$INSTDIR\bin\CubeCoach.exe"
  Delete "$DESKTOP\CubeCoach.lnk"
  Delete "$SMPROGRAMS\CubeCoach\CubeCoach.lnk"
  RMDir /r "$INSTDIR\resources"
  RMDir "$SMPROGRAMS\CubeCoach"
  RMDir /r "$INSTDIR"
SectionEnd
