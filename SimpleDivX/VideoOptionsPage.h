#include "afxwin.h"
#if !defined(AFX_VIDEOOPTIONSPAGE_H__D8A20762_119E_11D5_B29D_D8B9B3AB191D__INCLUDED_)
#define AFX_VIDEOOPTIONSPAGE_H__D8A20762_119E_11D5_B29D_D8B9B3AB191D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VideoOptionsPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVideoOptionsPage dialog

class CVideoOptionsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CVideoOptionsPage)

// Construction
public:
	void UpdateCheckAuto(int x);
	void deltempfile();
	int CheckSavedSettings();
	void ProcessVars();
	void My_Static_Init();
	void My_Init(void);
	CVideoOptionsPage();
	~CVideoOptionsPage();
	CToolTipCtrl *m_pTooltip;

// Dialog Data
	//{{AFX_DATA(CVideoOptionsPage)
	enum { IDD = IDD_DIALOG_VIDEO_OPTIONS };
	CComboBox	m_cmbAutoSize;
	CEdit	m_ctrlDY;
	CEdit	m_ctrlDX;
	CEdit	m_ctrlCY;
	CEdit	m_ctrlCX;
	
	CComboBox	m_cmbFPS;
	CComboBox	m_cmbOutputFormat;
	BOOL	m_bStartAtLBA;
	BOOL	m_bSelectFrameRange;
	int		m_intCX;
	int		m_intCY;
	int		m_intDX;
	int		m_intDY;
	int		m_intStartAtLBA;
	int		m_intEndFrame;
	int		m_intStartFrame;
	
	BOOL	m_bGetAuto;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CVideoOptionsPage)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CVideoOptionsPage)
	afx_msg void OnSelchangeComboOutputformat();
	afx_msg void OnCheckLba();
	afx_msg void OnCheckFramerange();
	afx_msg void OnVideopreview();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	
	afx_msg void OnCheckAuto();
	afx_msg void OnSelchangeComboSize();
	afx_msg void OnButtonAdv();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int my_page;
	int AutoCalcSize();
	void GetDialogThings_Info();
public:
	afx_msg void OnEnChangeEditLba();
	CComboBox m_cmbDeInterlace;
	CComboBox m_cmbSourceFormat;
	afx_msg void OnCbnSelchangeComboSourceFormat();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEOOPTIONSPAGE_H__D8A20762_119E_11D5_B29D_D8B9B3AB191D__INCLUDED_)
