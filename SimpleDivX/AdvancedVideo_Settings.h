#if !defined(AFX_ADVANCEDVIDEO_SETTINGS_H__8DA242E9_F953_4A5D_B871_E4B0F6F79262__INCLUDED_)
#define AFX_ADVANCEDVIDEO_SETTINGS_H__8DA242E9_F953_4A5D_B871_E4B0F6F79262__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvancedVideo_Settings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAdvancedVideo_Settings dialog

class CAdvancedVideo_Settings : public CDialog
{
// Construction
public:
	CAdvancedVideo_Settings(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAdvancedVideo_Settings)
	enum { IDD = IDD_DIALOG_VIDEO_ADV };
	CComboBox	m_cmbReferenceQuality;
	CComboBox	m_cmbPostFilter;
	CComboBox	m_cmbOutputMode;
	//}}AFX_DATA
	
		int ReferenceQuality;
	int PostFilter;
	int OutputMode;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdvancedVideo_Settings)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdvancedVideo_Settings)
	virtual BOOL OnInitDialog();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CToolTipCtrl *m_pTooltip;
	void My_Static_Init();
	void My_Init();
public:
	afx_msg void OnCbnSelchangeComboRefq();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVANCEDVIDEO_SETTINGS_H__8DA242E9_F953_4A5D_B871_E4B0F6F79262__INCLUDED_)
