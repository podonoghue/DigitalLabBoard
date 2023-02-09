@echo off
cls
set VERSIONn=2.2.0
set APPNAME="EEE20001 Lab Software"
set MSI_FILE=EEE20001-Lab-Software-2022-02

set SOURCE_BIN_DIR="..\PackageFiles\bin\x86_64-win-gnu"
set BUILD_BIN_DIR="bin"

set WIX_DIR="C:\Program Files (x86)\WiX Toolset v3.11\bin"

set HEAT=%WIX_DIR%\heat.exe
set HEAT_OPTIONS=-srd -ke -gg -sfrag -template fragment -sw5150 -platform 
set HEAT_DIR=%BUILD_BIN_DIR%

set CANDLE=%WIX_DIR%\candle.exe
set CANDLE_OPTIONS=-arch x64 
set CANDLE_FILES=eee20001_software.wxs

set LIGHT=%WIX_DIR%\light.exe
set LIGHT_OPTIONS=-ext WixUIExtension -ext WixUtilExtension -sw0204
set LIGHT_DIRS=-b %BUILD_BIN_DIR% -b .
set LIGHT_FILES=eee20001_software.wixobj

set WIX_BUILD_DIR=wixBuildDir

del *.wixobj
del *.wixpdb

if exist %MSI_FILE%.msi del %MSI_FILE%.msi

if exist %BUILD_BIN_DIR% rmdir /S /Q %BUILD_BIN_DIR%
mkdir %BUILD_BIN_DIR%

echo copy /V %SOURCE_BIN_DIR% %BUILD_BIN_DIR%
copy /V %SOURCE_BIN_DIR% %BUILD_BIN_DIR%
move  %BUILD_BIN_DIR%\*.exe .

if exist %WIX_BUILD_DIR% rmdir /S /Q %WIX_BUILD_DIR%
mkdir %WIX_BUILD_DIR%

echo "%HEAT% dir %SOURCE_BIN_DIR%  %HEAT_OPTIONS% -cg Cg.eee20001_software -dr INSTALLDIR -out %WIX_BUILD_DIR%\bin.wxs"
%HEAT% dir %HEAT_DIR%  %HEAT_OPTIONS% -cg Cg.eee20001_software -dr INSTALLDIR -out %WIX_BUILD_DIR%\bin.wxs
echo "%CANDLE% %CANDLE_OPTIONS% -dProductVersion=%VERSIONn% -dPrimaryAppName=%APPNAME% -o %WIX_BUILD_DIR%\ *.wxs %WIX_BUILD_DIR%\*.wxs"
%CANDLE% %CANDLE_OPTIONS% -dProductVersion=%VERSIONn% -dPrimaryAppName=%APPNAME% -o %WIX_BUILD_DIR%\ *.wxs %WIX_BUILD_DIR%\*.wxs
echo "%LIGHT% %LIGHT_OPTIONS% %LIGHT_DIRS% -o %MSI_FILE% %WIX_BUILD_DIR%\*.wixobj"
%LIGHT% %LIGHT_OPTIONS% %LIGHT_DIRS% -o %MSI_FILE% %WIX_BUILD_DIR%\*.wixobj

rem Moved to Linux as doesn't work in VM
rem cmd /c .\SignIt.cmd

:finish
pause