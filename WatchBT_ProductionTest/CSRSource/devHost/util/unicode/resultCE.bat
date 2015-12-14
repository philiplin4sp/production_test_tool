@REM ****************************************************************************
@REM NAME
@REM 	resultCE.bat  -  copy output files to the "result" directory
@REM 
@REM DESCRIPTION
@REM	Copy the output files for this project to the "result" directory. This
@REM	batch file should be called with two parameters:
@REM		result <outdir> <resultdir>
@REM
@REM	<outdir> is the CE output directory, i.e. $(OutDir).
@REM
@REM	<resultdir> is the corresponding sub-directory under "result", usually
@REM	either "debug" or "release".
@REM 
@REM MODIFICATION HISTORY
@REM 	1.1	13:dec:01	ckl	Created, based on at's stuff.
@REM 	1.2	20:dec:01	ckl	Comments changed.
@REM 
@REM CVS IDENTIFIER
@REM 	$Id: //depot/bc/bluesuite_2_4/devHost/util/unicode/resultCE.bat#1 $
@REM 	
@REM ****************************************************************************

@REM set the copy command to overwrite
set COPYCMD=/Y

@REM Path to the root of the build tree
set TOP=..\..\..

@REM Individual output directories
set RESULT=%TOP%\result
set INCLUDEBASE=%RESULT%\include
set INCLUDE=%INCLUDEBASE%\unicode
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
xcopy /d ichar.h %INCLUDE% > nul 
xcopy /d main.h %INCLUDE% > nul 
if exist %1\ichar.lib xcopy /d %1\ichar.lib %LIB% > nul 
if exist %1\ichar4.lib xcopy /d %1\ichar4.lib %LIB% > nul 
