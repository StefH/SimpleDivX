[Setup]
AppName=SimpleDivX
AppVerName=SimpleDivX
AppVersion=1.4
AppCopyright=Copyright (C) 1999-2006 CyberDemonII (c)
AppPublisherURL=http://www.simpledivx.org

DefaultDirName={pf}\SimpleDivX
DisableProgramGroupPage=yes
DefaultGroupName=SimpleDivX
OutputBaseFilename=SimpleDivX_1.40.25_Update
; DirExistsWarning=yes
; ^ since no icons will be created in "{group}", we don't need the wizard
;   to ask for a group name.
UninstallDisplayIcon={app}\SimpleDivX.exe
Compression=lzma/ultra
SolidCompression=yes

[Code]
procedure MyAfterInstall();
begin
  RegWriteStringValue(HKEY_CURRENT_USER, 'Software\SimpleDivX\Settings',
    'InstallDir', ExpandConstant('{app}'));
end;

[Files]
;Source: "../SimpleDivX-src/SimpleDivX/SimpleDivX.exe.manifest"; DestDir: "{app}"
Source: "../SimpleDivX-src/SimpleDivX/languages\*.lang"; DestDir: "{app}\Languages"
Source: "../SimpleDivX-src/SimpleDivX/release/*.dll"; DestDir: "{app}"
Source: "../SimpleDivX-src/SimpleDivX/release/*.exe"; DestDir: "{app}"

;Source: "Tools/System DLL/*.dll"; DestDir: "{app}"

; readme
Source: "ReadMe.txt"; DestDir: "{app}"; Flags: isreadme ; AfterInstall: MyAfterInstall

; [virtualdub]
;Source: "Tools\VirtualDub\txt\*.*"; DestDir: "{app}\txt\VirtualDub"
;Source: "Tools\VirtualDub\bin\*.dll"; DestDir: "{app}\Tools"
;Source: "Tools\VirtualDub\bin\*.exe"; DestDir: "{app}\Tools"

;Source: "Tools\mplayer\bin\*.exe"; DestDir: "{app}\Tools"
;Source: "Tools\mplayer\bin\*.dll"; DestDir: "{app}\Tools"
;Source: "Tools\mplayer\bin\mplayer\*"; DestDir: "{app}\Tools\mplayer"
;Source: "Tools\mplayer\txt\*.txt"; DestDir: "{app}\txt\mplayer"

; VobSub
;Source: "Tools\VobSub\bin\*.dll"; DestDir: "{app}\Tools"

;Source: "SimpleDivX Web site.url"; DestDir: "{app}"

;Source: "Guide\SimpleDivX_Guide\*.*"; DestDir: "{app}\Guide"
;Source: "Guide\SimpleDivX_Guide\images\en\*.*"; DestDir: "{app}\Guide\images\en"
;Source: "Guide\SimpleDivX_Guide\images\it\*.*"; DestDir: "{app}\Guide\images\it"
;Source: "Guide\SimpleDivX_Guide\StyleSheet\*.*"; DestDir: "{app}\Guide\StyleSheet"

[Icons]
;Name: "{group}\SimpleDivX"; Filename: "{app}\SimpleDivX.exe"; WorkingDir: "{app}"
;Name: "{group}\SimpleDivX Web Site"; Filename: "{app}\SimpleDivX Web Site.url"
;Name: "{group}\Readme"; Filename: "{app}\ReadMe.txt"
;Name: "{group}\Guide\Brazilian Portuguese"; Filename: "{app}\Guide\Brazilian Portuguese Guide.htm"
;Name: "{group}\Guide\Czech"; Filename: "{app}\Guide\Czech Guide.htm"
;Name: "{group}\Guide\English"; Filename: "{app}\Guide\English Guide.htm"
;Name: "{group}\Guide\French"; Filename: "{app}\Guide\French Guide.htm"
;Name: "{group}\Guide\German"; Filename: "{app}\Guide\German Guide.htm"
;Name: "{group}\Guide\Italian"; Filename: "{app}\Guide\Italian Guide.htm"
;Name: "{group}\Guide\Polish"; Filename: "{app}\Guide\Polish Guide.htm"
;Name: "{group}\Guide\Portugal Portuguese"; Filename: "{app}\Guide\Portugal Portuguese Guide.htm"
;Name: "{group}\Guide\Spanish"; Filename: "{app}\Guide\Spanish Guide.htm"

[Run]
Filename: "{app}\SimpleDivX.exe"; Description: "Launch application"; Flags: postinstall nowait skipifsilent

[Registry]
Root: HKCU; Subkey: "Software\SimpleDivX\Settings"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\SimpleDivX"; Flags: uninsdeletekey
