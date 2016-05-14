#if !defined(AFX_ALLCONTROLSSHEET_H__B0C69231_5582_11D4_98A3_D7F5B61CE616__INCLUDED_)
#define AFX_ALLCONTROLSSHEET_H__B0C69231_5582_11D4_98A3_D7F5B61CE616__INCLUDED_

#include "InputFilesPage.h"	// Added by ClassView

#include "VideoOptionsPage.h"	// Added by CDII
#include "AudioOptionsPage.h"	// Added by CDII
#include "CompressionOptionsPage.h"	// Added by CDII
#include "OutputFilesPage.h"	// Added by CDII
#include "CodecOptions.h"	// Added by CDII
#include "SetupPage.h"	// Added by CDII
#include "ProjectOptionsPage.h"	// Added by CDII
#include "MiscPage.h"	// Added by me

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AllControlsSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAllControlsSheet

class CAllControlsSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CAllControlsSheet)

// Construction
public:
	CAllControlsSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CAllControlsSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

    

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAllControlsSheet)
	public:
	virtual BOOL OnInitDialog();
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	HICON m_hIcon;
	CInputFilesPage m_InputFilesPage;
	CSetupPage m_SetupPage;
	CVideoOptionsPage m_VideoOptionsPage;
	CAudioOptionsPage m_AudioOptionsPage;
	COutputFilesPage m_OutputFilesPage;
	CCodecOptions m_CodecOptionsPage;
	CCompressionOptionsPage m_CompressionOptionsPage;
	CProjectOptionsPage m_ProjectOptionsPage;
	CMiscPage m_MiscPage;

	void AddControlPages(void);
	virtual ~CAllControlsSheet();


	// Generated message map functions
protected:
	//{{AFX_MSG(CAllControlsSheet)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
		
private:
	
protected:
//	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
//	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	bool SetPageTitle(int nPage, CString pszText);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALLCONTROLSSHEET_H__B0C69231_5582_11D4_98A3_D7F5B61CE616__INCLUDED_)
