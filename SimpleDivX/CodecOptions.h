#include "afxwin.h"
#if !defined(AFX_CODECOPTIONS_H__C2702EB9_65DB_4AC0_BF09_DA40C5E02743__INCLUDED_)
#define AFX_CODECOPTIONS_H__C2702EB9_65DB_4AC0_BF09_DA40C5E02743__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CodecOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCodecOptions dialog

class CCodecOptions : public CPropertyPage
{
	DECLARE_DYNCREATE(CCodecOptions)

// Construction
public:
	void UpdateLogNaming(int i);
	void ProcessVars();
	void My_Static_Init();
	void My_Init();
	CCodecOptions();
	~CCodecOptions();
	CToolTipCtrl *m_pTooltip;

// Dialog Data
	//{{AFX_DATA(CCodecOptions)
	enum { IDD = IDD_DIALOG_CODEC_OPTIONS };
	int		m_intAviCreate;
	CString	m_strOpenDivxLog;
	int		m_iMaxKeyFrame;
	int		m_intPass;
	//int		m_DivxVersion;
	int		m_bAuto_or_Manual_logfilenaming;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCodecOptions)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCodecOptions)
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLogfile();
	afx_msg void OnAdvSet();
	afx_msg void OnRadioAutoLogfilenaming();
	afx_msg void OnRadioManualLogfilenaming();
	afx_msg void OnPass1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int my_page;
	int m_intOld2pass;
	bool b_PassModeEnabled;
	bool b_LogFileSettingsEnabled;
	CString BrowseForFile();
	void UpdateVersion(int i_version);
	void UpdatePassButtons(void);
public:
	CComboBox m_cmbCodecSelection;
	afx_msg void OnCbnSelchangeComboCodec();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CODECOPTIONS_H__C2702EB9_65DB_4AC0_BF09_DA40C5E02743__INCLUDED_)
