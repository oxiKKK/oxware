@echo off

:: create folder
if not exist ".\deploy" mkdir ".\deploy\" /Y
if not exist ".\deploy" mkdir ".\deploy\oxware\" /Y

:: exes
xcopy ".\build\oxware.exe" ".\deploy\oxware\" /Y
xcopy ".\build\cheater.dll" ".\deploy\oxware\" /Y
xcopy ".\build\mmapper.dll" ".\deploy\oxware\" /Y
xcopy ".\build\oxui.dll" ".\deploy\oxware\" /Y
xcopy ".\build\util.dll" ".\deploy\oxware\" /Y

:: deps
xcopy ".\dependencies\runtime\msvcp140.dll" ".\deploy\oxware\" /Y
xcopy ".\dependencies\runtime\vcruntime140.dll" ".\deploy\oxware\" /Y
xcopy ".\dependencies\uninstall.bat" ".\deploy\oxware\" /Y
xcopy ".\dependencies\README.txt" ".\deploy\oxware\" /Y