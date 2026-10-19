[Setup]
AppId={{0EC24C16-A89F-4453-ABC1-08EE38CFBB86}
AppName=PickleJuicer
AppVersion=1.0.0
AppPublisher=JUICER

DefaultDirName={commoncf64}\VST3
UsePreviousAppDir=no
DisableDirPage=no
AppendDefaultDirName=no
DirExistsWarning=no

UninstallFilesDir={autopf}\JUICER\PickleJuicer
DisableProgramGroupPage=yes

PrivilegesRequired=admin
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible

OutputDir=Output
OutputBaseFilename=PickleJuicer-Setup-1.0.0
Compression=lzma2
SolidCompression=yes
WizardStyle=modern

[Files]
Source: "..\build\HelloVST_artefacts\Release\VST3\PickleJuicer.vst3\*"; \
    DestDir: "{app}\PickleJuicer.vst3"; \
    Flags: ignoreversion recursesubdirs createallsubdirs