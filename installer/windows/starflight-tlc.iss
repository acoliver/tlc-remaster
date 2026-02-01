; Inno Setup Script for Starflight: The Lost Colony
; https://jrsoftware.org/isinfo.php

#define MyAppName "Starflight The Lost Colony"
#define MyAppVersion GetEnv('VERSION')
#if MyAppVersion == ""
#define MyAppVersion "1.0.0"
#endif
#define MyAppPublisher "Starflight TLC Team"
#define MyAppURL "https://github.com/acoliver/tlc-remaster"
#define MyAppExeName "starflighttlc.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
AppId={{8F3C4A2E-7B5D-4E1F-9A6C-3D8E5F7A2B1C}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}/issues
AppUpdatesURL={#MyAppURL}/releases
DefaultDirName={autopf}\Starflight TLC
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
DisableProgramGroupPage=yes

; Output settings
OutputDir=..\..\
OutputBaseFilename=starflight-tlc-{#MyAppVersion}-windows-x64-setup
; Compression
Compression=lzma2/ultra64
SolidCompression=yes
; Modern installer look
WizardStyle=modern
; Require admin for Program Files install, but allow per-user
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
; Windows version requirement
MinVersion=10.0
; 64-bit only
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
; Icon
SetupIconFile={#SourcePath}\..\..\package\starflight-tlc\starflighttlc.ico
; Uninstall info
UninstallDisplayIcon={app}\{#MyAppExeName}
UninstallDisplayName={#MyAppName}

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; Main executable
Source: "..\..\package\starflight-tlc\starflighttlc.exe"; DestDir: "{app}"; Flags: ignoreversion

; App icon
Source: "..\..\package\starflight-tlc\starflighttlc.ico"; DestDir: "{app}"; Flags: ignoreversion

; Game data
Source: "..\..\package\starflight-tlc\data\*"; DestDir: "{app}\data"; Flags: ignoreversion recursesubdirs createallsubdirs

; DLLs
Source: "..\..\package\starflight-tlc\*.dll"; DestDir: "{app}"; Flags: ignoreversion

; Create saves directory
Source: "..\..\package\starflight-tlc\saves\*"; DestDir: "{app}\saves"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist

[Dirs]
; Ensure saves directory exists and is writable
Name: "{app}\saves"; Permissions: users-modify

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"; IconFilename: "{app}\starflighttlc.ico"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon; WorkingDir: "{app}"; IconFilename: "{app}\starflighttlc.ico"

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent; WorkingDir: "{app}"

