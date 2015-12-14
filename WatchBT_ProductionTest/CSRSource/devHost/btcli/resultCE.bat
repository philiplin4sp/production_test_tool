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
@REM 	$Id: //depot/bc/bluesuite_2_4/devHost/btcli/resultCE.bat#1 $
@REM 	
@REM ****************************************************************************

@REM set the copy command to overwrite
set COPYCMD=/Y

@REM Path to the root of the build tree
set TOP=..\..\..

@REM Individual output directories
set RESULT=%TOP%\result
set BINBASE=%RESULT%\bin
set BIN=%BINBASE%\%2

@REM Ensure that the directories exist
if not exist %RESULT% mkdir %RESULT%
if not exist %BINBASE% mkdir %BINBASE%
if not exist %BIN% mkdir %BIN%

@REM Copy the output files
xcopy /d %1\btcli.exe %BIN% > nul 
