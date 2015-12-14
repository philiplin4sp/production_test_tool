:: Set path to locate spiUnlock app
set spiUnlocktoolspath="C:\Program Files (x86)\CSR\BlueSuite 2.5.0"
:: change the path and unlock SPI
call %spiUnlocktoolspath%\spiunlock.exe unlock ..\SWRelease\customerkey.txt
::erase CSR flash and previous restore_result.txt stored
IF EXIST "restore_result.txt" DEL "restore_result.txt"
call %spiUnlocktoolspath%\BlueFlashCmd.exe erase 
call %spiUnlocktoolspath%\BlueFlashCmd.exe ..\SWRelease\bin.xpv >restore_result.txt
