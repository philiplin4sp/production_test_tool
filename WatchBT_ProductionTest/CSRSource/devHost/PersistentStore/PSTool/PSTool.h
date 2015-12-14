// PSTool.h : main header file for the PSTool application
//

#if !defined(AFX_PSTOOL_H__30EA23CA_A6B1_11D4_8FE6_00B0D065A91C__INCLUDED_)
#define AFX_PSTOOL_H__30EA23CA_A6B1_11D4_8FE6_00B0D065A91C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "unicode/ichar.h"
#include "ps/pshelp.h"
#include "PSCmdLineInfo.h"

/////////////////////////////////////////////////////////////////////////////
// CPSToolApp:
// See PSTool.cpp for the implementation of this class
//

class CPSToolApp : public CWinApp
{
public:
	CPSToolApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPSToolApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPSToolApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

int HexCharToInt(ichar c);

#define MAX_HEX_DATA        (500)       // maximum allowable uint16s in string
// general helper function to retrieve data from a hex string
template <typename T>
bool GetHexData(CString str, T* data, unsigned int max_len, int* len, int size)
{
    bool valid = true;
	int i; 
    uint32 usedLen = 0;
	uint32 usedChars = 0;
	data[usedLen] = 0;

    str.Trim();

    ASSERT(sizeof(T) * 2 >= size);

    for (i = 0; i < str.GetLength(); i++)
	{
        if (usedChars == size || (!isxdigit(str[i]) && (usedChars > 0)))
		{
            // enforce space delimiting
            if (str[i] != II(' '))
            {
                valid = false;
                break;
            }

			usedChars = 0;
			usedLen++;
            if (usedLen >= MAX_HEX_DATA || usedLen >= max_len)
            {
                valid = false;
                break;
            }
            else
            {
                data[usedLen] = 0;
            }
        }

        if (isxdigit(str[i]))
		{
			data[usedLen] *= 16;
			data[usedLen] += HexCharToInt((ichar)(str[i]));
			usedChars ++;
		}
	}

    // if we have processed characters but not reached 4 hex digits
    // then count this as valid. Allows entering 'a' instead of '000a'
    if (usedChars > 0)
	{
		usedLen++;
	}

    *len = usedLen;
    
    return valid;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.



#endif // !defined(AFX_PSTOOL_H__30EA23CA_A6B1_11D4_8FE6_00B0D065A91C__INCLUDED_)
