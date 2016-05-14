#include "afxwin.h"
#if !defined(AFX_ADVANCEDDIVX_SETTINGS_H__0F6AA35D_BD5C_492F_BEE9_34105C80FB6D__INCLUDED_)
#define AFX_ADVANCEDDIVX_SETTINGS_H__0F6AA35D_BD5C_492F_BEE9_34105C80FB6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvancedDivX_Settings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAdvancedDivX_Settings dialog

class CAdvancedDivX_Settings : public CDialog
{
// Construction
public:
	void My_Init();
	CAdvancedDivX_Settings(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAdvancedDivX_Settings)
	enum { IDD = IDD_DIALOG_ADV };
	BOOL	m_bGMC;
	BOOL	m_bQP;
	BOOL	m_bBI;
	BOOL	m_bTrellis;
	BOOL	m_bDarkness;
	int		m_maxq;
	int		m_minq;
	BOOL	m_bVHQ;
		
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdvancedDivX_Settings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdvancedDivX_Settings)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	
private:
	
public:
	BOOL m_bChroma;
	BOOL m_bLumi;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
