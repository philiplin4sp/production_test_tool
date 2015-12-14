:: Set path to locate nvscmd app
set nvscmdtoolspath="c:\ADK2.0\Tools\bin\"
:: Delete previously created .txt files
IF EXIST "serialFlash.txt" DEL "serialFlash.txt"
IF EXIST "verifyFlashResult.txt" DEL "verifyFlashResult.txt"
:: change the path and earse the flash
call %nvscmdtoolspath%\nvscmd erase 
::Write into the serial flash and direct the output to serialFlash.txt
call %nvscmdtoolspath%\nvscmd burn ..\SWRelease\headset.ptn all >serialFlash.txt   
call %nvscmdtoolspath%\nvscmd verify ..\SWRelease\headset.ptn >verifyFlashResult.txt