CSR UNICODE SUPPORT

The files in this directory are intended to aid the creation of
applications with Unicode support, primarily under Win32.


MICROSOFT LAYER FOR UNICODE

Only the Windows NT and Windows CE families of Win32 platforms support
Unicode completely. Windows 95/98/ME only provide limited Unicode
functionality. However, Microsoft have produced a library (UnicoWS)
that acts as a translation layer on these platforms, allowing the wide
(...W) versions of API calls to be used. The library internally converts
any string parameters and calls the ASCII (...A) version of the API.

Use of this library requires the UnicoWS.lib file to be at the start
of the list of libraries being linked to (so that it is used rather than
the normal operating system API). In the majority of cases, specifying a
dependency on the UnicodeUtil project is sufficient - fixmsvc.pl will
make the necessary changes, although sometimes it is necessary to
manually tweak the list of libraries being linked to.


CSR MFC LIBRARY

To use the Microsoft Layer for Unicode with MFC applications requires
either the MFC library to be linked to the UnicoWS library. This can be
done either by statically linking to the MFC library or by rebuilding
the MFC DLLs so that they themselves are linked to UnicoWS. The former
option is simpler, but leads to problems with memory being allocated and
released across DLL boundaries. Hence, the latter option has been used.

MSDN technical note TN033 describes how the MFC DLLs can be rebuilt and
distributed. At the time of writing this is available from:
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vcmfc98/html/_mfcnotes_tn033.asp

The following excerpt from TN033 describes the distribution conditions:

    You may modify the library and redistribute a retail (/release) of
    your modified library only if you rename it to something other than
    MFCxx.DLL. You may not redistribute the debug version of either the
    pre-built or custom built debug DLL.

Hence, the rebuilt versions of the MFC DLLs have been given new names,
as follows:

    Version     Original name       New name
    
    Release     MFC42               n/a (not Unicode)
                MFC42u              CSRMFC
                MFCS42              n/a (not Unicode)
                MFCS42u             CSRMFCS
    Debug       MFC42d              n/a (not Unicode)
                MFC42ud             CSRMFC
                MFCD42d             n/a (not Unicode)
                MFCD42ud            CSRMFCD
                MFCS42d             n/a (not Unicode)
                MFCS42ud            CSRMFCS
                MFCO42d             n/a (not Unicode)
                MFCO42ud            CSRMFCO
                MFCN42d             n/a (not Unicode)
                MFCN42ud            CSRMFCN

Pre-built versions of the new DLLs have been added to CVS, and are copied
by the ichar project. This should minimise the impact on other projects,
although it will sometimes be necessary to manually edit the list of
libraries being linked to.

The files used to rebuild the MFC libraries are in a separate CVS project
as "/imported/microsoft/mfc". To build a new version, checkout the whole
project, start an MS-DOS prompt, change to the "mfc" directory, and run
the "build.bat" batch file. The output files will be placed inside the
"result/debug" and "result/release" sub-directories; these should be
committed to CVS in "bc01/devHost/util/unicode/DebugMFC" and
"bc01/devHost/util/unicode/ReleaseMFC" respectively.

A diff between the versions of the source code that was imported and the
version used to build the binaries should be committed to CVS in
"bc01/devHost/util/unicode/MFCPatcher/res/MFCDiff.patch". The diff is
produced using:
	cvs -Q diff -r mfc42-sp5 -r mfc42-csr3 -c -N > ..\MFCDiff.patch

Both the source files used to build the version currently in this
directory and the resulting binaries have been tagged as "mfc42-csr3".
