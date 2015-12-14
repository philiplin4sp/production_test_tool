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
@REM	1.1 25:oct:02	doc created based on at's stuff
@REM	1.2 28:nov:02	doc	Added call to Cab creation tool
@REM	1.3 03:dec:02	doc	Fixed call to cab creation tool
@REM	1.4	04:dec:02	doc	Removed call to Cab creation tool
@REM	1.5	06:dec:02	doc	Removed copy to MakeDFUCab directory
@REM 
@REM CVS IDENTIFIER
@REM 	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUWizard2CE/resultCE.bat#1 $
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
xcopy /d %1\DFUWizardCE.exe %BIN% > nul 

