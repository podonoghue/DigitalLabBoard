@echo off

set STORE=My

rem set SDK_HOME="C:\Program Files (x86)\Windows Kits\8.1\bin"
set SDK_HOME="C:\Program Files (x86)\Windows Kits\10\bin"

set INF_2_CAT=%SDK_HOME%\x86\Inf2Cat.exe
set SIGNTOOL=%SDK_HOME%\x64\SignTool.exe

set NAME="Open Source Developer, Peter O'Donoghue"
set DESCRIPTION="Digital Lab Board Driver"
set TIMESTAMP_URL=http://time.certum.pl/
rem set TIMESTAMP_URL=http://timestamp.verisign.com/scripts/timstamp.dll

set SIGN_COMMAND=%SIGNTOOL% sign /n %NAME% /t %TIMESTAMP_URL% /d %DESCRIPTION% /fd sha1 /v

echo Creating CAT files
%INF_2_CAT% /driver:%~dp0\Drivers /os:XP_X86,XP_X64,7_X86,7_X64,Vista_X86,Vista_X64

echo Creating XP CAT files
%INF_2_CAT% /driver:%~dp0\DriversXP /os:XP_X86,XP_X64,7_X86,7_X64,Vista_X86,Vista_X64

echo Signing files
rem %SIGN_COMMAND% %~dp0\Drivers\*.cat %~dp0\DriversXP\*.cat
rem pause
