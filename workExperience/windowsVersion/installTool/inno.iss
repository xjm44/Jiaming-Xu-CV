; Ref: https://jrsoftware.org/ishelp/

#define MyAppName "mainApp"
#define MyAppVersion "3.6.25.725001"
#define MyAppPublisher "company"
#define MyAppURL "https://www.company.com"
#define MyAppExeName "mainApp.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{FB13317B-0714-48A4-BD88-B0154261CF84}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\mainApp
;DisableProgramGroupPage=yes
DefaultGroupName=MAINAPP
LicenseFile=license.txt
;UsePreviousAppDir=yes
;UpdateUninstallLogAppName=no
;CreateUninstallRegKey=no
;InfoBeforeFile=info.txt
UninstallDisplayIcon={app}\bin\{#MyAppExeName}

; Use per-user installation (No Admin privilege required)
PrivilegesRequired=admin
OutputDir=.
OutputBaseFilename=mainApp_setup
Compression=lzma
SolidCompression=yes
WizardStyle=modern
; Only 64-bit binaries are shi pped for now
ArchitecturesAllowed=x64

; Branding & cosmetic
SetupIconFile=ico\icon.ico
WizardSmallImageFile=ico\title_2.bmp
WizardImageAlphaFormat=premultiplied

WizardImageFile=ico\title.bmp
DisableWelcomePage=no
WizardImageStretch=no


[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
; Credits: github.com/kira-96/Inno-Setup-Chinese-Simplified-Translation
;Name: "chinesesimplified"; MessagesFile: "compiler:Languages\ChineseSimplified.isl"

[Messages]
; override default welcome page message
english.WelcomeLabel2=Welcome %nInstallation of [name/ver] shall begin.%n%nIt is recommended that you close all other applications and disable any anti virus before continuing.


[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: checkedonce

[Files]
Source: "E:\mainApp\*"; DestDir: "{app}"; Excludes: "unins*"; Flags: ignoreversion recursesubdirs
; Install the icon file
Source: "ico\icon.ico"; DestDir: "{app}"

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\bin\{#MyAppExeName}"; IconFilename: "{app}\icon.ico"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\bin\{#MyAppExeName}"; IconFilename: "{app}\icon.ico"; Tasks: desktopicon

[Run]
Filename: "{app}\bin\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: files; Name: "{app}\bin\PocoDynamic.dll"
Type: files; Name: "{app}\Lib\params.yaml"


[Registry]
Root: HKLM; Subkey: "Software\MCS"; ValueType: string; ValueName: "MCSVersion"; ValueData: "1.0."; Flags: uninsdeletevalue
 
[Code]
function InitializeSetup():boolean;
var
  MykeynotExist:boolean;
  ResultCode: Integer;
  uicmd: String;
begin
  MykeynotExist:= true;
  if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{B7F653CF-1BE5-4F40-BA4A-E3BBC6869116}', 'UninstallString', uicmd) then
  begin
  MyKeynotExist:= false;
  MsgBox('Detected {#MyAppName} in this computer'#10#10'You need to unistall it before intall {#MyAppName}',mbInformation,MB_OK)
  Exec(ExpandConstant('{pf}\InstallShield Installation Information\{{FB13317B-0714-48A4-BD88-B0154261CF84}\Setup.exe'), '', '', SW_Show, ewNoWait, ResultCode);
  end;
  Result:= MykeynotExist
  if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{FB13317B-0714-48A4-BD88-B0154261CF84}_is1', 'UninstallString', uicmd) then
  begin
  MyKeynotExist:= false;
  Exec(RemoveQuotes(uicmd), '', '', SW_SHOW, ewWaitUntilTerminated, ResultCode);
  end;
  Result:= MykeynotExist
end;


// check the installation path
function IsEnglishStr(file: String): Boolean;
var
  value: Integer;
  len: Integer;
  i: Integer;
begin
  Result := true;
  len := length(file);
for i := 1 to len do
  begin
  // convert to ascii
  value := ord(file[i]);
  // check if is chi characters
  if (value < 45) or (value > 122) then
    begin
      Result := false;
    end;
  end;
end;

// next button - event
function NextButtonClick(CurPageID:Integer):Boolean;
begin
  Result := True;
  if (CurPageID = wpSelectDir) then
    begin
    if(IsEnglishStr(WizardDirValue) = false) or (Pos(' ', WizardDirValue) > 0)then
      begin
      Result := False;
      MsgBox('Install folder is invalid! Only English Characters and Numbers are allowed and cannot contain spaces.', mbError, MB_OK);
      end;
    end;
end;



