#include "afxcmn.h"
#include "afxwin.h"
#if !defined(AFX_PROJECTPAGE_H__897FD1FC_8BAF_4906_8FAE_3CBD73A22B8D__INCLUDED_)
#define AFX_PROJECTPAGE_H__897FD1FC_8BAF_4906_8FAE_3CBD73A22B8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProjectOptionsPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProjectOptionsPage dialog

class CProjectOptionsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CProjectOptionsPage)

// Construction
public:
	void RemoveFromList(int index);
	void RefreshWindow(int);
	CProjectOptionsPage();
	~CProjectOptionsPage();
	

// Dialog Data
	//{{AFX_DATA(CProjectOptionsPage)
	enum { IDD = IDD_DIALOG_PROJECT };
	CListBox	m_ctrlBatchList;
	CString	m_s_TempFolder;
	CString	m_s_OutputFolder;
	BOOL	m_bSplitAtFileSize;
	BOOL	m_bOverWrite;
	BOOL	m_bDeleteTemp;
	long	m_lSplitAt;
	int		m_bBatchMode;
	CString	m_strProjectName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProjectOptionsPage)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CProjectOptionsPage)
	afx_msg void OnTempLocation();
	afx_msg void OnAviLocation();
	afx_msg void OnChangeTempfolder();
	afx_msg void OnChangeOutputavifolder();
	afx_msg void OnCheckSplit();
//	afx_msg void OnCheckDelTemp();
//	afx_msg void OnChangeSplitat();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnRadioNormalmode();
	afx_msg void OnRadioBatchmode();
	afx_msg void OnChangeProjname();
	afx_msg void OnButtonClear();
	afx_msg void OnButtonUp();
	afx_msg void OnButtonDown();
	//}}AFX_MSG

	void BrowseFolder(CString *pstr_CurrentFolder, int i_what);
	CToolTipCtrl *m_pTooltip;

	DECLARE_MESSAGE_MAP()

private:
	int my_page;
	void Switch(int current, int up);
	void CheckProjectName();
	void My_Static_Init();
	void My_Init();
	int GetTextLen(LPCTSTR lpszText);

public:
	afx_msg void OnLbnSelchangeListBatch();
	afx_msg void OnBnClickedButtonModify();
	int UpdateListBoxScroll(CListBox *pmyListBox);
	
private:
	CListCtrl m_ctrlList;
public:
	int ProcessVars(void);
private:
	CComboBox m_cmbSplitInParts;
public:

	afx_msg void OnBnClickedCheckSplitblack();
	BOOL m_SplitOnFadeOut;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECTPAGE_H__897FD1FC_8BAF_4906_8FAE_3CBD73A22B8D__INCLUDED_)
