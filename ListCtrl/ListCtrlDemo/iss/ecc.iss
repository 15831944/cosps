; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define AppExeName "ecc"
#define AppExeFullName "ecc.exe"
#define AppName "Easy Code Counter"
#define AppVersion GetFileVersion("..\Release\ecc.exe")
#define AppCopyRight GetFileCopyright("..\Release\ecc.exe")
#define AppOrgName "www.easy-code-counter.com"
#define AppWebSite "http://www.easy-code-counter.com/"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppName={#AppName}
AppVersion={#AppVersion}
AppVerName={#AppName} {#AppVersion}
AppPublisher={#AppOrgName}
AppPublisherURL={#AppWebSite}
AppSupportURL={#AppWebSite}
AppUpdatesURL={#AppWebSite}
DefaultDirName={pf}\{#AppName}
DefaultGroupName={#AppName}
DisableProgramGroupPage=yes
OutputDir=output
OutputBaseFilename=Easy-Code-Counter-Setup-{#AppVersion}
VersionInfoVersion={#AppVersion}
VersionInfoCopyright={#AppCopyRight}

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}";Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "..\Release\{#AppExeFullName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Release\{#AppExeFullName}.manifest"; DestDir: "{app}"; Flags: ignoreversion
Source: "source\gtb.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "source\log4cplus.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "source\msvcp60.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "source\mfc42.dll"; DestDir: "{sys}"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "source\msvcrt.dll"; DestDir: "{sys}"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "..\dat\*"; DestDir: "{app}\dat"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\help\*"; DestDir: "{app}\help"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{#AppName}"; Filename: "{app}\{#AppExeFullName}"
Name: "{group}\Help"; Filename: "{app}\help\help_en.chm"
Name: "{group}\{cm:UninstallProgram,{#AppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#AppName}"; Filename: "{app}\{#AppExeFullName}"; Tasks: desktopiconName: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#AppName}"; Filename: "{app}\{#AppExeFullName}"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\{#AppExeFullName}"; Description: "{cm:LaunchProgram,{#AppName}}"; Flags: nowait postinstall skipifsilent

[Registry]
Root: HKLM; Subkey: "Software\{#AppName}"; Flags: uninsdeletekeyifempty
Root: HKLM; Subkey: "Software\{#AppName}\Common"; Flags: uninsdeletekeyifempty
Root: HKLM; Subkey: "Software\{#AppName}\Common"; ValueType: string; ValueName: "Path"; ValueData: "{app}"; Flags: uninsdeletekeyifempty uninsdeletevalue

[UninstallDelete]
Type: files; Name: "{app}\dat\user\*.log"
Type: files; Name: "{app}\dat\user\filter_tree.xml"
Type: files; Name: "{app}\dat\user\lang_grammar.xml"
Type: files; Name: "{app}\dat\user\preferences.ini"
Type: dirifempty; Name: "{app}\dat"
Type: dirifempty; Name: "{app}"
Type: files; Name: "{win}\{#AppExeName}.INI"


[UninstallRun]
