@echo off

rem set SDK_HOME="C:\Program Files (x86)\Windows Kits\8.1\bin"
set SDK_HOME="C:\Program Files (x86)\Windows Kits\10\bin"

set SIGNTOOL=%SDK_HOME%\x64\SignTool.exe

set NAME="Open Source Developer, Peter O'Donoghue"
set DESCRIPTION="Digital Lab Board Software"
set TIMESTAMP_URL=http://time.certum.pl/
rem set TIMESTAMP_URL=http://timestamp.verisign.com/scripts/timstamp.dll

set SIGN_COMMAND=%SIGNTOOL% sign /n %NAME% /t %TIMESTAMP_URL% /d %DESCRIPTION% /fd sha1 /v

echo Signing files

for %%f in (%~dp0\*.msi) do echo %SIGN_COMMAND% %%f
for %%f in (%~dp0\*.msi) do %SIGN_COMMAND% %%f

rem pause
