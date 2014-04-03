[Setup]
AppName=Dream vs. Dream
AppVersion=0.2
DefaultDirName={pf}\Dream vs. Dream
DefaultGroupName=Dream vs. Dream
UninstallDisplayIcon={app}\DvD.exe
Compression=lzma2
SolidCompression=yes
OutputDir=.
; "ArchitecturesInstallIn64BitMode=x64" requests that the install be
; done in "64-bit mode" on x64, meaning it should use the native
; 64-bit Program Files directory and the 64-bit view of the registry.
; On all other architectures it will install in "32-bit mode".
ArchitecturesInstallIn64BitMode=x64
; Note: We don't set ProcessorsAllowed because we want this
; installation to run on all architectures (including Itanium,
; since it's capable of running 32-bit code too).

[Files]
; Install DvD-x64.exe if running in 64-bit mode (x64; see above), DvD-x86.exe otherwise.

; Place all x64 files here
Source: "x64\DvD.exe"; DestDir: "{app}"; Check: Is64BitInstallMode
Source: "x64\*.dll";   DestDir: "{app}"; Check: Is64BitInstallMode
; Place all x86 files here, first one should be marked 'solidbreak'
Source: "x86\DvD.exe"; DestDir: "{app}"; Check: not Is64BitInstallMode; Flags: solidbreak
Source: "x86\*.dll";   DestDir: "{app}"; Check: Is64BitInstallMode
; Place all common files here, first one should be marked 'solidbreak'
Source: "x86\data\*";      DestDir: "{app}\data"; Flags: recursesubdirs solidbreak

[Icons]
Name: "{group}\Dream vs. Dream"; Filename: "{app}\DvD.exe"
