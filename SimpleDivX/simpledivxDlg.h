// SimpleDivXDlg.h : header file
//

#if !defined(AFX_SimpleDivXDLG_H__B0C69228_5582_11D4_98A3_D7F5B61CE616__INCLUDED_)
#define AFX_SimpleDivXDLG_H__B0C69228_5582_11D4_98A3_D7F5B61CE616__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSimpleDivXDlg dialog




class CSimpleDivXDlg : public CDialog
{
// Construction
public:
	CSimpleDivXDlg(CWnd* pParent = NULL);	// standard constructor


// Dialog Data
	//{{AFX_DATA(CSimpleDivXDlg)
	enum { IDD = IDD_SimpleDivX_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimpleDivXDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSimpleDivXDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SimpleDivXDLG_H__B0C69228_5582_11D4_98A3_D7F5B61CE616__INCLUDED_)
