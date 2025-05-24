!include "MUI2.nsh"

!define APPNAME "CubeCoach"
!define DESCRIPTION "Rubik's Cube Trainer"
!define VERSION "0.1.0"
!define LICENSE_FILE "resources\\LICENSE.txt"  ; Use single backslashes for NSIS

Name "${APPNAME}"
OutFile "CubeCoachInstaller.exe"
InstallDir "$PROGRAMFILES64\\${APPNAME}"
RequestExecutionLevel admin

# Pages
!insertmacro MUI_PAGE_LICENSE "${LICENSE_FILE}"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

# Languages (optional)
!insertmacro MUI_LANGUAGE "English"

Section "Install"

    # Install binaries to bin folder
    SetOutPath "$INSTDIR\\bin"
    File /r "bin\\*.*"

    # Install resources folder
    SetOutPath "$INSTDIR\\resources"
    File /r "resources\\*.*"

    # Create Start Menu folder and shortcut
    CreateDirectory "$SMPROGRAMS\\${APPNAME}"
    CreateShortCut "$SMPROGRAMS\\${APPNAME}\\${APPNAME}.lnk" "$INSTDIR\\bin\\CubeCoach.exe"

SectionEnd
