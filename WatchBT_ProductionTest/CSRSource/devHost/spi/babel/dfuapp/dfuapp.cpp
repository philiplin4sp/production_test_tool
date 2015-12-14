// dfuapp.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "common/portability.h"
#include "common/globalversioninfo.h"
#include "pttransport/pttransport.h"

static int Usage(void)
{
    printf("Usage:\n\n");
    printf("dfubabel.exe [-cmd]\n\n");
    printf("\tForces a Babel to re-enumerate as a standard BlueCore Bluetooth device.\n\n");
    printf("\t-cmd   DFUWizard will not be launched and dfubabel will run to completion\n");
    printf("\t       indicating either success or fail.\n");
    return 1;
}

static int dfu(bool cmd)
{
    const char *resultStr;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char error[1024];

    if (!pttrans_open("SPITRANS=USB", -1, -1))
    {
        pttrans_get_last_error(NULL, &resultStr);
        printf("%s\n", resultStr);
        return 1;
    }

    printf("Babel current version = 0x%s\n", pttrans_getvar("BABELVERSION"));

    resultStr = pttrans_command("DFU");
    if (strlen(resultStr) > 0)
    {
        printf("%s\n", resultStr);
        pttrans_close();
        return 1;
    }

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    printf("Your Babel is now a standard Bluecore device.\n");

    if (!cmd)
    {
        printf("Running DFU Wizard...\n");
    
        if (!CreateProcess("DFUWizard.exe", NULL, NULL, NULL, false, 0, NULL, NULL, &si, &pi))
        {
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | 
                          FORMAT_MESSAGE_IGNORE_INSERTS,
                          NULL,
                          GetLastError(),
                          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                          error,
                          1024,
                          NULL);

            printf(error);
            printf("Please run DFUWizard to complete DFU process.\n\n");
        }
    }

    pttrans_close();

    return 0;
}


int main(int argc, char* argv[])
{
    bool cmd = false;

    printf ("dfubabel, version %s\n", VERSION_BLUESUITE_STR);

    // If the special build string is set then show the date of the build
    if (strcmp (VERSION_SPECIAL_BUILD, "") != 0)
    {
        printf ("%s\n", VERSION_SPECIAL_BUILD);
    }
    printf ("%s\n", VERSION_COPYRIGHT_START_YEAR("2006"));

    if (argc > 1)
    {
        if (STRICMP(argv[1], "-cmd") == 0)
            cmd = true;
        else
            return Usage();
    }

    int retval = dfu(cmd);

    if (!cmd)
    {
        printf("Press a key...\n");
        _getch();
    }
    else
    {
        if (retval)
            printf("Failed\n");
        else
            printf("Success\n");
    }

	return retval;
}

