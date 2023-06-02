@echo off

set OXWARE_APPDATA=%appdata%\oxware
set OXWARE_REGISTRY="HKEY_CURRENT_USER\SOFTWARE\oxware"

::
:: appdata
::
echo Deleting appdata folder inside %OXWARE_APPDATA%...

if exist %OXWARE_APPDATA% rmdir %OXWARE_APPDATA% /S /Q

::
:: registry
::
echo Deleting registry inside %OXWARE_REGISTRY%...

:: see if exists
reg query %OXWARE_REGISTRY% >nul 
if %errorlevel% equ 0 (
  reg delete %OXWARE_REGISTRY% /f
) 

echo Done! Now you can delete the cheat folder.
pause