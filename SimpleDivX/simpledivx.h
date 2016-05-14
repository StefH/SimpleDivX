// SimpleDivX.h : main header file for the SimpleDivX application
//

#if !defined(AFX_SimpleDivX_H__B0C69226_5582_11D4_98A3_D7F5B61CE616__INCLUDED_)
#define AFX_SimpleDivX_H__B0C69226_5582_11D4_98A3_D7F5B61CE616__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSimpleDivXApp:
// See SimpleDivX.cpp for the implementation of this class
//

class CSimpleDivXApp : public CWinApp
{
public:
	void My_Init(void);
	CSimpleDivXApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimpleDivXApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSimpleDivXApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void InitDlls();
	
public:

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SimpleDivX_H__B0C69226_5582_11D4_98A3_D7F5B61CE616__INCLUDED_)
