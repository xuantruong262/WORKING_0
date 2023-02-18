@echo off
chcp 437
if exist ..\results (
    rmdir /S /Q ..\results
)
mkdir ..\results

set TMPAT=TM_path_%TIMING_MODE%_%PRODUCT%

setlocal enabledelayedexpansion
set tmnumber=0
set tmnodiff=0
set pass=0
set passmatch=0
set nopassfail=0
set fail=0
set tmfail=0
set passunmatch=0
set difflog=0
for /f "usebackq tokens=1,2,3,4,5 delims=/" %%a in (..\!PAT!_!MODEL!\!TMPAT!.txt) do (
    set /a tmnumber = !tmnumber! + 1
    if "%%b" == "" (
        set tmpath=..\
        set tmname=%%~na
    )else if "%%c" == "" (
        set tmpath=%%a
        set tmname=%%~nb
    )else if "%%d" == "" (
        set tmpath=%%a\%%b
        set tmname=%%~nc
    ) else if "%%e" == "" (
        set tmpath=%%a\%%b\%%c
        set tmname=%%~nd
    ) else (
        set tmpath=%%a\%%b\%%c\%%d
        set tmname=%%~ne
    )
    set tmpath=!tmpath:*:=!

    REM ### added by nhuvo 20190424 to skip comment in TM_path_*
    echo.!tmname! | findstr /C:"#" 1>nul       
    if errorlevel 1 (
    REM ###
        findstr /M /R "^In file:.*" ..\log\!tmpath!\!tmname!.log > check.log
        set /P check=< check.log
        if "!check!" == "..\log\!tmpath!\!tmname!.log" (
            findstr /V /R "^In file:.*" ..\log\!tmpath!\!tmname!.log > temp.log
        ) else (
            copy ..\log\!tmpath!\!tmname!.log temp.log 
        )

        findstr /M /R "^In file:.*" check_result\expected_log_!OSCI_USK!_!PAT!_!MODEL!_!PRODUCT!_!TIMING_MODE!\!tmpath!\!tmname!.log > expected_check.log

        rem findstr /M /R "^In file:.*" check_result\expected_log_!OSCI_USK!_!PAT!_!MODEL!_!TIMING_MODE!\!tmpath!\!tmname!.log > expected_check.log
        set /P expected_check=< expected_check.log

        if "!expected_check!" == "check_result\expected_log_!OSCI_USK!_!PAT!_!MODEL!_!PRODUCT!_!TIMING_MODE!\!tmpath!\!tmname!.log" (
            findstr /V /R "^In file:.*" check_result\expected_log_!OSCI_USK!_!PAT!_!MODEL!_!PRODUCT!_!TIMING_MODE!\!tmpath!\!tmname!.log > expected_temp.log
        ) else (
            copy check_result\expected_log_!OSCI_USK!_!PAT!_!MODEL!_!PRODUCT!_!TIMING_MODE!\!tmpath!\!tmname!.log expected_temp.log
        )

        fc temp.log expected_temp.log >> ..\results\all_results.txt
        fc temp.log expected_temp.log > diff.log
        findstr /M "differences encountered" diff.log > findstr_temp.log
        set /P cmp=<findstr_temp.log
        findstr /M "scheape3\scheapcompile\models_rvc" diff.log > findstr_addr.log
        set /P check_addr=< findstr_addr.log
        if "!cmp!" == "diff.log" (
            set /a tmnodiff=1
        ) else (
            if "!check_addr!" == "diff.log" (
                set /a tmnodiff=1
            ) else (
                set /a tmnodiff=0
            )
        )
        set cmp=""
        set check_addr=""

        findstr /M "PASS" ..\log\!tmpath!\!tmname!.log > findstr.log
        set /P match=< findstr.log
        if "!match!" == "..\log\!tmpath!\!tmname!.log" (
            if !tmnodiff! == 1 (
                set /a passmatch = !passmatch! + 1
                set /a pass = !pass! + 1
            ) else (
                echo !tmpath!\!tmname! : UNMATCH WITH EXPECTED LOG. >> ..\results\all_results.txt
                set /a passunmatch = !passunmatch! + 1
                set /a fail = !fail! + 1
            )
        ) else (
            findstr /M "FAIL" ..\log\!tmpath!\!tmname!.log > findfailstr.log
            set /P failmatch=< findfailstr.log
            if "!failmatch!" == "..\log\!tmpath!\!tmname!.log" (
                echo !tmpath!\!tmname! : FAIL. >> ..\results\all_results.txt
                set /a fail = !fail! + 1
                set /a tmfail = !tmfail! + 1
            ) else (
                if !tmnodiff! == 1 (
                    set /a nopassfail = !nopassfail! + 1
                    set /a pass = !pass! + 1
                ) else (
                    echo !tmpath!\!tmname! : UNMATCH WITH EXPECTED LOG. >> ..\results\all_results.txt
                    set /a fail = !fail! + 1
                    set /a difflog = !difflog! + 1
                )
            )
        )
        del check.log temp.log expected_check.log expected_temp.log findstr.log findstr_temp.log diff.log findstr_addr.log findfailstr.log
     REM ### added by nhuvo 20190424 to skip comment in TM_path_*
     ) else (
        set /a tmnumber = !tmnumber! - 1
     )
     REM ###
)
echo ---------------------------SUMMARY--------------------------- >> ..\results\all_results.txt
echo - Req: !MODEL!_UT_Windows_results_!OSCI_USK!_!PAT!_!TIMING_MODE! >> ..\results\all_results.txt
echo - NUMBER OF TMS: !tmnumber! >> ..\results\all_results.txt
echo - NUMBER OF PASSED TMS: !pass! (match with expected log) >> ..\results\all_results.txt
echo - NUMBER OF FAILED TMS: !fail! (No match with expected log) >> ..\results\all_results.txt
echo ------------------------------------------------------------- >> ..\results\all_results.txt
type ..\results\all_results.txt
endlocal   
