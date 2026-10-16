@echo off
setlocal
cd /d %~dp0
cl /nologo /O2 /W3 test_encoding.c /link /SUBSYSTEM:CONSOLE user32.lib gdi32.lib
if errorlevel 1 exit /b 1
del *.obj >nul 2>nul
copy /y "..\..\source\Release\FVPKernel.dll" . >nul
copy /y "..\..\source\Release\FVPLoaderGui.exe" . >nul
