#include "afxwin.h"
#if !defined(AFX_SETUPPAGE_H__EDCB89D9_7210_452D_9EF2_CF9F53F5E431__INCLUDED_)
#define AFX_SETUPPAGE_H__EDCB89D9_7210_452D_9EF2_CF9F53F5E431__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetupPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetupPage dialog

class CSetupPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSetupPage)

// Construction
public:
	void My_Static_Init();
	CString CheckFile(CString* folder, CString filename, bool b_last);
	void FillFindList();
	CSetupPage();
	~CSetupPage();

// Dialog Data
	//{{AFX_DATA(CSetupPage)
	enum { IDD = IDD_DIALOG_SETUP };
	CComboBox	m_ctrlDiskPrio;
	CComboBox	m_ctrlVideoPrio;
	CComboBox	m_ctrlAudioPrio;
	CString	m_strProgramLocations;
	CToolTipCtrl *m_pTooltip;
	
	
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSetupPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSetupPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseProgram();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnGetDef();
	afx_msg void OnButtonPrintdebug();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void My_Init();
	int my_page;
protected:
	CComboBox m_ctrlLanguageList;
public:
	afx_msg void OnCbnSelchangeLstLanguages();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPPAGE_H__EDCB89D9_7210_452D_9EF2_CF9F53F5E431__INCLUDED_)
