call "copy_src.bat"
if "%OSCI_USK%" == "osci" (
  msbuild       /t:build ^
                /m:3 ^
                /fl1 ^
                /flp1:verbosity=detailed;logfile=osci_All.log ^
                /fl2 ^
                /flp2:verbosity=diagnostic;logfile=osci_JustErrors.log;errorsonly ^
                /fl3 ^
                /flp3:verbosity=diagnostic;logfile=osci_JustWarnings.log;warningsonly ^
                ..\tb\SCHEAPG4\scheapCompile\build\TOPV02\msvc140\scheapG4.sln ^
                /p:RunCodeAnalysis=false ^
                /p:Configuration=Release ^
                /p:Platform=x64
) else (
  msbuild       /t:build ^
                /m:3 ^
                /fl1 ^
                /flp1:verbosity=detailed;logfile=usk_All.log ^
                /fl2 ^
                /flp2:verbosity=diagnostic;logfile=usk_JustErrors.log;errorsonly ^
                /fl3 ^
                /flp3:verbosity=diagnostic;logfile=usk_JustWarnings.log;warningsonly ^
                ..\tb\SCHEAPG4\scheapCompile\build\TOPV02\msvc140\scheapG4.sln ^
                /p:Configuration=Release_usk ^
                /p:Platform=x64
)