echo off

cls
set VERSIONn=2.0.0
set APPNAME="EEE20001 Lab Software"
set MSI_FILE=EEE20001-Lab-Software-2020-01

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
