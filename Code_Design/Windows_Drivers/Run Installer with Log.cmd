@echo off
cls
set VERSION=1_1_0
set MSI_FILE=Digital_Lab_Board_Drivers_%VERSION%_Win_x64

msiexec /i %MSI_FILE% /l*v usbdmInstall.log
