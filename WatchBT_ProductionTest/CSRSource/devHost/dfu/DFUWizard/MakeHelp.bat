@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by DFUWIZARD.HPJ. >"hlp\DFUWizard.hm"
echo. >>"hlp\DFUWizard.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\DFUWizard.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\DFUWizard.hm"
echo. >>"hlp\DFUWizard.hm"
echo // Prompts (IDP_*) >>"hlp\DFUWizard.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\DFUWizard.hm"
echo. >>"hlp\DFUWizard.hm"
echo // Resources (IDR_*) >>"hlp\DFUWizard.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\DFUWizard.hm"
echo. >>"hlp\DFUWizard.hm"
echo // Dialogs (IDD_*) >>"hlp\DFUWizard.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\DFUWizard.hm"
echo. >>"hlp\DFUWizard.hm"
echo // Frame Controls (IDW_*) >>"hlp\DFUWizard.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\DFUWizard.hm"
REM -- Make help for Project DFUWIZARD


echo Building Win32 Help files
start /wait hcw /C /E /M "hlp\DFUWizard.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\DFUWizard.hlp" goto :Error
if not exist "hlp\DFUWizard.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\DFUWizard.hlp" Debug
if exist Debug\nul copy "hlp\DFUWizard.cnt" Debug
if exist Release\nul copy "hlp\DFUWizard.hlp" Release
if exist Release\nul copy "hlp\DFUWizard.cnt" Release
echo.
goto :done

:Error
echo hlp\DFUWizard.hpj(1) : error: Problem encountered creating help file

:done
echo.
