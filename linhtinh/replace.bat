@echo off
set config_file=%~1
for /f "delims=" %%a in (!config_file!) do (
  set line=%%a
  echo !line:%%%%TM_NAME=%~2! >> %~3
)
