#if !defined(AFX_COMPRESSIONOPTIONSPAGE_H__BA8AB501_13F7_11D5_B29D_D5CDC3B33603__INCLUDED_)
#define AFX_COMPRESSIONOPTIONSPAGE_H__BA8AB501_13F7_11D5_B29D_D5CDC3B33603__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CompressionOptionsPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCompressionOptionsPage dialog

class CCompressionOptionsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCompressionOptionsPage)

// Construction
public:
	void UpDateAuto_Manual();
	void DisplayQuality(int);
	void CalcFileSize();
	void MyStaticInit();
	long m_lSeconds;
	void Frames2Seconds();
	void Seconds2Frames();
	void Calc();
	void RefreshWindow();
	void My_Init();
	CCompressionOptionsPage();
	~CCompressionOptionsPage();
	CToolTipCtrl *m_pTooltip;

// Dialog Data
	//{{AFX_DATA(CCompressionOptionsPage)
	enum { IDD = IDD_DIALOG_COMPRESSION };
	int		m_intMinBitrate;
	int		m_hour;
	int		m_min;
	int		m_sec;
	int		m_intFileSize;
	long	m_lFrames;
	int		m_intEnterFileSize;
	BOOL	m_bGetTime;
	int		m_intEndCreditsOption;
	int		m_intAutoOrManual;
	int		m_intBitrateAdjust;
	int		m_intManualEnd;
	int		m_intEndCreditsPct;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCompressionOptionsPage)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCompressionOptionsPage)
	afx_msg void OnRadio2();
	afx_msg void OnChangeEditFrames();
	afx_msg void OnChangeEditHour();
	afx_msg void OnChangeEditMin();
	afx_msg void OnChangeEditSec();
	afx_msg void OnRadio20();
	afx_msg void OnRadio21();
	afx_msg void OnChangeEDITFileSize();
	afx_msg void OnGetTime();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnChangeMinbitrate();
	afx_msg void OnRadioEnd1();
	afx_msg void OnRadioEnd2();
	afx_msg void OnRadioEnd3();
	afx_msg void OnRadioAuto();
	afx_msg void OnRadioManual();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int my_page;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPRESSIONOPTIONSPAGE_H__BA8AB501_13F7_11D5_B29D_D5CDC3B33603__INCLUDED_)
