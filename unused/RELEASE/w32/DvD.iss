[Setup]
AppName=Dream vs. Dream
AppVersion=0.2
DefaultDirName={pf}\Dream vs. Dream
DefaultGroupName=Dream vs. Dream
UninstallDisplayIcon={app}\DvD.exe
Compression=lzma2
SolidCompression=yes
OutputDir=..
ArchitecturesInstallIn64BitMode=x64
AllowNoIcons=yes

[Files]
; x64
Source: "x64\DvD.exe"; DestDir: "{app}"; Check: Is64BitInstallMode
Source: "x64\*.dll";   DestDir: "{app}"; Check: Is64BitInstallMode

; x86
Source: "x86\DvD.exe"; DestDir: "{app}"; Check: not Is64BitInstallMode; Flags: solidbreak
Source: "x86\*.dll";   DestDir: "{app}"; Check: not Is64BitInstallMode

; Common
Source: "x86\data\*";      DestDir: "{app}\data"; Flags: recursesubdirs solidbreak

[Icons]
Name: "{group}\Dream vs. Dream"; Filename: "{app}\DvD.exe"
Name: "{group}\Uninstall"; Filename: "{uninstallexe}"
