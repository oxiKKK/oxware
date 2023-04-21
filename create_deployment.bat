:: create folder
if not exist ".\deploy" mkdir ".\deploy\" /Y
if not exist ".\deploy" mkdir ".\deploy\oxware\" /Y

:: exes
xcopy ".\build\oxware.exe" ".\deploy\oxware\" /Y
xcopy ".\build\cheater.dll" ".\deploy\oxware\" /Y
xcopy ".\build\mmapper.dll" ".\deploy\oxware\" /Y
xcopy ".\build\oxui.dll" ".\deploy\oxware\" /Y
xcopy ".\build\util.dll" ".\deploy\oxware\" /Y

:: runtime deps
xcopy ".\runtime_deps\msvcp140.dll" ".\deploy\oxware\" /Y
xcopy ".\runtime_deps\vcruntime140.dll" ".\deploy\oxware\" /Y