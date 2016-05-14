#if !defined(AFX_PROGRAMLOCATION_H__C9AA3281_2073_11D5_B29D_BF3C0153E501__INCLUDED_)
#define AFX_PROGRAMLOCATION_H__C9AA3281_2073_11D5_B29D_BF3C0153E501__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgramLocation.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProgramLocation dialog

class CProgramLocation : public CDialog
{
// Construction
public:
	CString BrowseForFolder(int i_option);
	CProgramLocation(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProgramLocation)
	enum { IDD = IDD_DIALOG1 };
	CString	m_strMPlayer;
	CString	m_strMEncoder;
	CString	m_strVirtualDub;
	CString	m_strMkvMerge;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgramLocation)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProgramLocation)
	afx_msg void OnVirtualdubLocation();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
private:
	int page;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedMencoderLocation();
	afx_msg void OnBnClickedMplayerLocation();
	afx_msg void OnBnClickedMkvmergeLocation();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRAMLOCATION_H__C9AA3281_2073_11D5_B29D_BF3C0153E501__INCLUDED_)
