@REM ****************************************************************************
@REM NAME
@REM 	result.bat  -  copy output files to the "result" directory
@REM 
@REM DESCRIPTION
@REM	Copy the output files for this project to the "result" directory. This
@REM	batch file should be called with two parameters:
@REM		result <outdir> <resultdir>
@REM
@REM	<outdir> is the Visual Studio output directory, i.e. $(OutDir).
@REM
@REM	<resultdir> is the corresponding sub-directory under "result", usually
@REM	either "debug" or "release".
@REM 
@REM MODIFICATION HISTORY
@REM 	1.1	23:mar:01	at	Created.
@REM	1.2	04:apr:01	at	Intermediate directories created to
@REM					work on Windows 98.
@REM	1.3 25:oct:02	doc added copy to MakeDFUCab directory
@REM	1.4 06:dec:02	doc removed copy to MakeDFUCab directory
@REM 
@REM CVS IDENTIFIER
@REM 	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/resultCE.bat#1 $
@REM 	
@REM ****************************************************************************

@REM set the copy command to overwrite
set COPYCMD=/Y

@REM Path to the root of the build tree
set TOP=..\..\..

@REM Individual output directories
set RESULT=%TOP%\result
set INCLUDEBASE=%RESULT%\include
set INCLUDE=%INCLUDEBASE%\dfu
set BINBASE=%RESULT%\bin
set BIN=%BINBASE%\%2
set LIBBASE=%RESULT%\lib
set LIB=%LIBBASE%\%2

@REM Ensure that the directories exist
if not exist %RESULT% mkdir %RESULT%
if not exist %INCLUDEBASE% mkdir %INCLUDEBASE%
if not exist %INCLUDE% mkdir %INCLUDE%
if not exist %BINBASE% mkdir %BINBASE%
if not exist %BIN% mkdir %BIN%
if not exist %LIBBASE% mkdir %LIBBASE%
if not exist %LIB% mkdir %LIB%

@REM Copy the output files
xcopy /d DFUEngine.h %INCLUDE% > nul 
xcopy /d %1\DFUEngine.dll %BIN% > nul 
xcopy /d %1\DFUEngine.lib %LIB% > nul 
