@echo off
set TMPAT=TM_path_%TIMING_MODE%_%PRODUCT%
set SC_FS=tp02_01_001
set SC_PS=tp02_02_001
set SC_NS=tp02_03_001
set SC_US=tp02_04_001
set SC_MS=tp02_05_001
set SC_SEC=tp02_06_001

if "%OSCI_USK%" == "osci" (
  set USK=
  set EXE=
) else (
  set USK=_usk
  set EXE=-usk
)
IF EXIST ..\log (
    RMDIR /S /Q ..\log
    MKDIR ..\log
) ELSE (
    MKDIR ..\log
)
IF NOT EXIST %SCHEAP_HOME%\scheapCompile\build\TOPV02\msvc140\Release%USK%\sim%EXE%.exe (
  echo There is no sim%EXE%.exe! Please run the compilation step!
) ELSE (
    setlocal enabledelayedexpansion
    set SIML_EXE=!SCHEAP_HOME!\scheapCompile\build\TOPV02\msvc140\Release%USK%\sim%EXE%.exe
    for /f "usebackq tokens=1,2,3,4,5 delims=/" %%a in (..\!PAT!_!MODEL!\!TMPAT!.txt) do (
        if "%%b" == "" (
            set tmpath=..\
            set tmname=%%~na
            set pathfile=.
        ) else if "%%c" == "" (
            set tmpath=%%a
            set tmname=%%~nb
            set pathfile=..\..
        ) else if "%%d" == "" (
            set tmpath=%%a\%%b
            set tmname=%%~nc
            set pathfile=..\..\..
        ) else if "%%e" == "" (
            set tmpath=%%a\%%b\%%c
            set tmname=%%~nd
            set pathfile=..\..\..\..
        ) else (
            set tmpath=%%a\%%b\%%c\%%d
            set tmname=%%~ne
            set pathfile=..\..\..\..\..
        )
        echo.!tmpath!\!tmname!
        if not exist ..\log\!tmpath! (
            mkdir ..\log\!tmpath!
        )
        cd ..\sim_!OSCI_USK!_!PAT!_!MODEL!_!PRODUCT!_!TIMING_MODE!\!tmpath!
        set SCRIPT_PY=!tmname!.py
        IF NOT EXIST !SCRIPT_PY! (
            IF NOT EXIST !tmname!.py (
                copy %SCHEAP_HOME%\..\..\scripts_linux\gen_sim\default_pythonfile_!MODEL!.py !tmname!.py
            )
            set SCRIPT_PY=!tmname!.py
        )
        set HEAP_CFG=!tmname!.cfg
        IF NOT EXIST !HEAP_CFG! (
            IF EXIST !tmname!.cfg (
                ERASE !tmname!.cfg
            )
            call !pathfile!\scripts_windows\replace.bat %SCHEAP_HOME%\..\..\scripts_linux\gen_sim\configfile_!TIMING_MODE!.skl !tmname! !tmname!.cfg
            set HEAP_CFG=!tmname!.cfg 
        )
        REM Added by NganTran Aug-30-2013: add time resolution
        if "!tmname!" == "!SC_FS!" (
            !SIML_EXE! -config !HEAP_CFG! -py_scr !SCRIPT_PY! -sctime SC_FS > !pathfile!\log\!tmpath!\!tmname!.log
        ) else if "!tmname!" == "!SC_PS!" (
            !SIML_EXE! -config !HEAP_CFG! -py_scr !SCRIPT_PY! -sctime SC_PS > !pathfile!\log\!tmpath!\!tmname!.log
        ) else if "!tmname!" == "!SC_NS!" (
            !SIML_EXE! -config !HEAP_CFG! -py_scr !SCRIPT_PY! -sctime SC_NS > !pathfile!\log\!tmpath!\!tmname!.log
        ) else if "!tmname!" == "!SC_US!" (
            !SIML_EXE! -config !HEAP_CFG! -py_scr !SCRIPT_PY! -sctime SC_US > !pathfile!\log\!tmpath!\!tmname!.log
        ) else if "!tmname!" == "!SC_MS!" (
            !SIML_EXE! -config !HEAP_CFG! -py_scr !SCRIPT_PY! -sctime SC_MS > !pathfile!\log\!tmpath!\!tmname!.log
        ) else if "!tmname!" == "!SC_SEC!" (
            !SIML_EXE! -config !HEAP_CFG! -py_scr !SCRIPT_PY! -sctime SC_SEC > !pathfile!\log\!tmpath!\!tmname!.log
        ) else (
            echo !SIML_EXE!
            echo !HEAP_CFG!
            echo !SCRIPT_PY!
			!SIML_EXE! -config !HEAP_CFG! -py_scr !SCRIPT_PY! > !tmname!.log
			move !tmname!.log !pathfile!\log\!tmpath!
        )
        cd !pathfile!\scripts_windows  
    )
    endlocal
)
