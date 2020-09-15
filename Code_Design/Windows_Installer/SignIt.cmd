@echo off

set SIGNTOOL="C:\Program Files (x86)\Windows Kits\10\bin\x64\SignTool.exe"
set NAME="Open Source Developer, Peter O'Donoghue"
set TIMESTAMP_URL=http://time.certum.pl/

set SIGN_COMMAND=%SIGNTOOL% sign /d "CPLD Programmer" /n %NAME% /t %TIMESTAMP_URL% /fd sha1 /v
echo Signing files

for %%f in (%~dp0\*.msi) do echo %SIGN_COMMAND% %%f
for %%f in (%~dp0\*.msi) do %SIGN_COMMAND% %%f

rem pause