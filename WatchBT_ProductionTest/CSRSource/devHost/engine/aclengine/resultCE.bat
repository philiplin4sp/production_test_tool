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
@REM 	1.1	02:apr:01	ckl	Created, based on at's stuff.
@REM	1.2	04:apr:01	at	Intermediate directories created to
@REM					work on Windows 98.
@REM 
@REM CVS IDENTIFIER
@REM 	$Id: //depot/bc/main/devHost/HostStack/btcli/resultCE.bat#2 $
@REM 	
@REM ****************************************************************************

@REM set the copy command to overwrite
set COPYCMD=/Y

@REM Path to the root of the build tree
set TOP=..\..\..

@REM Individual output directories
set RESULT=%TOP%\result
set LIBBASE=%RESULT%\lib
set LIB=%LIBBASE%\%2

@REM Ensure that the directories exist
if not exist %RESULT% mkdir %RESULT%
if not exist %LIBBASE% mkdir %LIBBASE%
if not exist %LIB% mkdir %LIB%

@REM Copy the output files
xcopy /d %1\aclengine.lib %LIB% > nul 
