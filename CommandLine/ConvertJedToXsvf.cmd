@echo off

set XILINX=C:\Apps\Xilinx\14.7\ISE_DS\settings64.bat C:\Apps\Xilinx\14.7\ISE_DS\ISE\bin\nt64

rem Get file name and append ".jed" if needed
set IMPACT=%XILINX%\impact.exe
set JED_FILE=%~f1
IF "%JED_FILE%"=="" (set JED_FILE=cpld_tester)
set XSVF_FILE=%~n1
IF "%XSVF_FILE%"=="" (set XSVF_FILE=%JED_FILE%)
IF "%~x1"=="" (set JED_FILE=%JED_FILE%.jed)
set SCRATCH_FILE=%TMP%\impact_batch_cmd.tmp"
rem echo JED_FILE  = %JED_FILE%
rem echo XSVF_FILE = %XSVF_FILE%
rem pause

if not exist "%JED_FILE%" (
   echo "%JED_FILE%" does not exit
   exit
)

rem Create Impact command file
echo setMode -bs                                     >  %SCRATCH_FILE%
echo addDevice -p 1 -file "%JED_FILE%"               >> %SCRATCH_FILE%
echo setCable -port svf -file "%XSVF_FILE%"          >> %SCRATCH_FILE%
echo readidcode -p 1                                 >> %SCRATCH_FILE%
echo program -e -v -p 1                              >> %SCRATCH_FILE%
echo closeCable                                      >> %SCRATCH_FILE%
echo quit                                            >> %SCRATCH_FILE%

rem Run command
%IMPACT% -batch %SCRATCH_FILE%

pause