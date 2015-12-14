Building HCISource Source Code
==============================

The following .sln project files are available for Visual Studio 2005 to build the appropriate targets:

*********************************************************************************************************
*
* NB: The projects use a variable "HOSTBUILD_RESULT" to indicate the path to the output directory. In
* order to set this variable you must open the .bat file for a .sln
*
* e.g. To build btcli, double click on the file "run_vs_btcli.bat" in Windows Explorer
*
*********************************************************************************************************

CSRSource\devHost\Bluetest3.sln
CSRSource\devHost\btcli.sln to build Btcli
CSRSource\devHost\btclictrl.sln, a GUI to launch Btcli
CSRSource\devHost\DFUBabel.sln 
CSRSource\devHost\DFUEngine.sln
CSRSource\devHost\DFUWizard.sln
CSRSource\devHost\PSCli.sln
CSRSource\devHost\PSTool.sln
CSRSource\devHost\USBPurger.sln is a utility for removing references to USB drivers for CSR chips

Note:

  Some solutions use projects for which the source has not been included. The pre-built dlls are in 
  "CSRSource\results\bin\debug" or "CSRSource\results\bin\release", as appropriate.

btcli can also be built for CE or Windows Mobile 5.0 using the appropriate configuration in Visual Studio

(The Win32 version must be built first because the build relies on some .h files being copied 
and this is only done in the Win32 configuration)

