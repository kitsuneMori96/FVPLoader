@echo off
setlocal
cd /d %~dp0
rem usage: run.bat 936
echo [Encoding]> FVPLoader.ini
echo CodePage=%1>> FVPLoader.ini
if exist test_encoding.log del test_encoding.log
if exist FVPLOG.ini del FVPLOG.ini
FVPLoaderGui.exe test_encoding.exe
ping -n 4 127.0.0.1 >nul
type test_encoding.log
