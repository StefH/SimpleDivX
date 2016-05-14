#if !defined(AFX_AUDIOOPTIONSPAGE_H__D8A20763_119E_11D5_B29D_D8B9B3AB191D__INCLUDED_)
#define AFX_AUDIOOPTIONSPAGE_H__D8A20763_119E_11D5_B29D_D8B9B3AB191D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../vob_mpeg2/vstrip/vobinfo.h"

/////////////////////////////////////////////////////////////////////////////
// CAudioOptionsPage dialog

class CAudioOptionsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CAudioOptionsPage)

// Construction
public:
	CAudioOptionsPage();
	~CAudioOptionsPage();

// Dialog Data
	//{{AFX_DATA(CAudioOptionsPage)
	enum { IDD = IDD_DIALOG_AUDIO_OPTIONS };
	CSpinButtonCtrl	m_spinAmplify;
	CComboBox	m_cmbAudioStreamID;
	CComboBox	m_cmbAudioCompression;
	BOOL	m_bEnableAudioIDCheck;
	int		m_intHertz;
	BOOL	m_bCreateAudio;
	BOOL	m_bAmplifyWave;
	BOOL	m_bNorm;
	BOOL	m_bNorm2;
	BOOL	m_bExtractAllAudio;
	int		m_intAudioFormat;
	BOOL	m_bConvertAllAudio;
	int		m_bConvertAC3_to_2_Channels;
	BOOL	m_bVBR;
	CString	m_strDelay;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAudioOptionsPage)
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
	//{{AFX_MSG(CAudioOptionsPage)
	afx_msg void OnSelchangeComboAudio();
	afx_msg void OnRadio1();
	afx_msg void OnCreateAudio();
	afx_msg void OnCheckAudio();
	afx_msg void OnRadio2();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSelchangeComboAudioStreamid();
	afx_msg void OnCheckNorm();
	afx_msg void OnCheckWaveup();
	afx_msg void OnCheckNorm2();
	afx_msg void OnRadioAc3();
	afx_msg void OnRadioMp3();
	afx_msg void OnCheckExtractAllAudio();
	afx_msg void OnRadioOgg();
	afx_msg void OnDeltaposSpinAmplify(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckVbr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CToolTipCtrl *m_pTooltip;
	int my_page;
	int m_intAmplify;
	bool b_display_ac3_kbps_from_detection;
	void UpdateSpin();
	void UpdateAudioInfo(int x);
	int i_min_kbps[AUDIO_TYPE_LAST];
	int i_max_kbps[AUDIO_TYPE_LAST];
	int i_radio_id[AUDIO_TYPE_LAST];
	int ai_language_id[MAX_AUDIO * 3];
	void ReverseLookupAudioFormat(void);
	void UpdateAc3_Mp3(int option, int);
	void My_Static_Init();
	void RefreshWindow();
	void My_Init(void);
	void ProcessVars();

public:
	afx_msg void OnBnClickedRadioRecompressac3();
	afx_msg void OnBnClickedRadioNorecompressac3();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOOPTIONSPAGE_H__D8A20763_119E_11D5_B29D_D8B9B3AB191D__INCLUDED_)
