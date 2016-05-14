[Setup]
AppName=SimpleDivX
AppVerName=SimpleDivX
AppVersion=1.5
AppCopyright=Copyright (C) 1999-2016 Stef Heyenrath (c)
AppPublisherURL=http://www.simpledivx.org

DefaultDirName={pf}\SimpleDivX
DisableProgramGroupPage=yes
DefaultGroupName=SimpleDivX
OutputBaseFilename=SimpleDivX_1.5.0
; DirExistsWarning=yes
; ^ since no icons will be created in "{group}", we don't need the wizard
;   to ask for a group name.
UninstallDisplayIcon={app}\SimpleDivX.exe
Compression=lzma2/fast
SolidCompression=yes

[Code]
procedure MyAfterInstall();
begin
  RegWriteStringValue(HKEY_CURRENT_USER, 'Software\SimpleDivX\Settings',
    'InstallDir', ExpandConstant('{app}'));
end;

[Files]
Source: "../SimpleDivX/SimpleDivX.exe.manifest"; DestDir: "{app}"
Source: "../SimpleDivX/languages\*.lang"; DestDir: "{app}\Languages"
Source: "../SimpleDivX/release/*.dll"; DestDir: "{app}"
Source: "../SimpleDivX/release/*.exe"; DestDir: "{app}"
Source: "../SimpleDivX/ReadMe.txt"; DestDir: "{app}"

Source: "Tools/System DLL/*.dll"; DestDir: "{app}"

;virtualdub
Source: "Tools\VirtualDub\VirtualDubMod_1.5.10.3_b2550\*.exe"; DestDir: "{app}\Tools"
Source: "Tools\VirtualDub\VirtualDubMod_1.5.10.3_b2550\*.dll"; DestDir: "{app}\Tools"
Source: "Tools\VirtualDub\VirtualDubMod_1.5.10.3_b2550\*.ini"; DestDir: "{app}\Tools"

;mencoder
Source: "Tools\mplayer\MPlayer-generic-r37853+g6d7f566\*.exe"; DestDir: "{app}\Tools"
Source: "Tools\mplayer\MPlayer-generic-r37853+g6d7f566\*.dll"; DestDir: "{app}\Tools"
Source: "Tools\mplayer\MPlayer-generic-r37853+g6d7f566\mplayer\*"; DestDir: "{app}\Tools\mplayer"

; VobSub
Source: "Tools\VobSub\bin\*.dll"; DestDir: "{app}\Tools"

Source: "SimpleDivX Web site.url"; DestDir: "{app}" ; AfterInstall: MyAfterInstall

Source: "Guide\SimpleDivX_Guide\*.*"; DestDir: "{app}\Guide"
Source: "Guide\SimpleDivX_Guide\images\en\*.*"; DestDir: "{app}\Guide\images\en"
Source: "Guide\SimpleDivX_Guide\images\it\*.*"; DestDir: "{app}\Guide\images\it"
Source: "Guide\SimpleDivX_Guide\images\ru\*.*"; DestDir: "{app}\Guide\images\ru"
Source: "Guide\SimpleDivX_Guide\StyleSheet\*.*"; DestDir: "{app}\Guide\StyleSheet"

[Icons]
Name: "{group}\SimpleDivX"; Filename: "{app}\SimpleDivX.exe"; WorkingDir: "{app}"
Name: "{group}\SimpleDivX Web Site"; Filename: "{app}\SimpleDivX Web Site.url"
Name: "{group}\Readme"; Filename: "{app}\ReadMe.txt"
Name: "{group}\Guide\Brazilian Portuguese"; Filename: "{app}\Guide\Brazilian Portuguese Guide.htm"
Name: "{group}\Guide\Czech"; Filename: "{app}\Guide\Czech Guide.htm"
Name: "{group}\Guide\English"; Filename: "{app}\Guide\English Guide.htm"
Name: "{group}\Guide\French"; Filename: "{app}\Guide\French Guide.htm"
Name: "{group}\Guide\German"; Filename: "{app}\Guide\German Guide.htm"
Name: "{group}\Guide\Italian"; Filename: "{app}\Guide\Italian Guide.htm"
Name: "{group}\Guide\Russian"; Filename: "{app}\Guide\Russian Guide.htm"
Name: "{group}\Guide\Polish"; Filename: "{app}\Guide\Polish Guide.htm"
Name: "{group}\Guide\Portugal Portuguese"; Filename: "{app}\Guide\Portugal Portuguese Guide.htm"
Name: "{group}\Guide\Spanish"; Filename: "{app}\Guide\Spanish Guide.htm"

[Run]
Filename: "{app}\SimpleDivX.exe"; Description: "Launch application"; Flags: postinstall nowait skipifsilent

[Registry]
Root: HKCU; Subkey: "Software\SimpleDivX\Settings"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\SimpleDivX"; Flags: uninsdeletekey
