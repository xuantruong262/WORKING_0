@echo off
if exist ..\All_simulation_results_Windows (rd /S /Q ..\All_simulation_results_Windows)
mkdir ..\All_simulation_results_Windows

REM ****** User settings ******
set Model_List=icache_e2_fcc2
set Product_List=all_product
set Pat_List=pat_1,pat_2
set Time_Mode=lt,at


:: ****** OSCI ******
call "setup_osci.bat"
::call "compile.bat"
call :run_and_check_loop

:: ****** USK ******
call "setup_usk.bat"
::call "compile.bat"
call :run_and_check_loop

exit /b

:run_and_check_loop
SETLOCAL EnableDelayedExpansion

FOR %%P IN (%Model_List%) DO (
    set MODEL=%%P
    REM Pat 1, Pap 2, product list
    FOR %%J IN (%Pat_List%) DO (
        set PAT=%%J
        IF EXIST ..\!PAT!_!MODEL! (
            FOR %%G IN (%Product_List%) DO (
                set PRODUCT=%%G
                FOR %%H IN (%Time_Mode%) DO (
                    set TIMING_MODE=%%H
                    IF EXIST ..\!PAT!_!MODEL!\TM_path_!TIMING_MODE!_!PRODUCT!.txt (
                        echo !MODEL! !PAT! !TIMING_MODE!
                        call "run.bat"
                        call "check_results.bat"
                        mkdir ..\All_simulation_results_Windows\log_!OSCI_USK!_!PAT!_!MODEL!_!PRODUCT!_!TIMING_MODE!
                        mkdir ..\All_simulation_results_Windows\result_!OSCI_USK!_!PAT!_!MODEL!_!PRODUCT!_!TIMING_MODE!
                        xcopy /E ..\log\* ..\All_simulation_results_Windows\log_!OSCI_USK!_!PAT!_!MODEL!_!PRODUCT!_!TIMING_MODE!\.
                        xcopy /E ..\results\* ..\All_simulation_results_Windows\result_!OSCI_USK!_!PAT!_!MODEL!_!PRODUCT!_!TIMING_MODE!\.
                    )
                )
            )
        )
    )
)
ENDLOCAL
exit /b
