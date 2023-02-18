:: Windows batch file for SC-HEAP E3
@echo off

set __script_dir=%~dp0
if "%__script_dir:~-1%"=="\" set __script_dir=%__script_dir:~0,-1%
echo "execute batch file %~dpnx0"

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\vsvars32.bat"
set OSCI_USK=usk
set PAT=pat_2
set MODEL=icache_e2_fcc2
set TIMING_MODE=lt
set PRODUCT=all_product

set SCHEAP_HOME=%__script_dir%\..\tb\SCHEAPG4
set SCHEAP_INC_DIR=%__script_dir%\..\tb\SCHEAPG4\scheapCompile

set CFOREST_DIR=%SCHEAP_HOME%\scheapCompile\models\iss\cforestg4x
set SCHEAP_PYTHON_HOME=C:\Python27
REM Renesas network
REM set RLM_LICENSE=5053@licedu3
REM B2B
set RLM_LICENSE=52293@172.29.138.155

:: OSCI
set OSCI_HOME=C:\lib\lib\systemc-2.3.1a_MOD_vc140_64bit
set BOOST_OSCI_ROOT=%OSCI_HOME%\src\sysc\packages
set OSCI_INC_DIR=%OSCI_HOME%\src
set OSCI_LIB_DIR=%OSCI_HOME%\msvc140\SystemC\x64
set OSCI_TLM_DIR=%OSCI_HOME%\src

:: USK
set OSCAR_HOME=C:\Users\truong.vu-xuan\Documents\VLAB Works\VLAB 2.6.2 (vc140)\VLAB 2.6.2 (vc140)
set BOOST_USK_ROOT=%OSCAR_HOME%\include
set USK_INC_DIR=%OSCAR_HOME%\include
set USK_LIB_DIR=%OSCAR_HOME%\lib
set USK_TLM_DIR=%OSCAR_HOME%\include

:: common (for USK)
set SYSTEMC_INCPATH=%USK_INC_DIR%
set SYSTEMC_LIBPATH=%USK_LIB_DIR%
set TLM_INC_DIR=%USK_TLM_DIR%
set PATH=%PATH%;%SCHEAP_HOME%\scheapCompile\lib;%SCHEAP_HOME%\scheapCompile\lib\oscar-win-vc140-x64;%SYSTEMC_LIBPATH%;%SCHEAP_HOME%\..\bin_astc
