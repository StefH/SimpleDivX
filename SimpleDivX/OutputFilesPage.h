#if !defined(AFX_OUTPUTFILESPAGE_H__73E7BBC1_1627_11D5_B29D_93C7EF2A3914__INCLUDED_)
#define AFX_OUTPUTFILESPAGE_H__73E7BBC1_1627_11D5_B29D_93C7EF2A3914__INCLUDED_

#include "SimpleDivX_Defines.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// COutputFilesPage dialog

class COutputFilesPage : public CPropertyPage
{
	DECLARE_DYNCREATE(COutputFilesPage)

// Construction
public:
	COutputFilesPage();
	~COutputFilesPage();
	void MEncoder();
	void MPlayer();
	void PrintVars();
	LRESULT OnQuerySiblings(WPARAM wParam, LPARAM lParam);
	

// Dialog Data
	//{{AFX_DATA(COutputFilesPage)
	enum { IDD = IDD_DIALOG_OUTPUTFILES };
	CEdit	m_ctrlEdit;
	CProgressCtrl	m_ctrlProgress;
	CString	m_strInfo;
	BOOL	m_bShutDown;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COutputFilesPage)
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
	//{{AFX_MSG(COutputFilesPage)
	afx_msg void OnStart();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCheckOverwriteAvi();
	afx_msg void OnCheckShutdown();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void DoStuff(int x);
	void Print_Info_File(int x);
	int DoSomeBasicChecks(int option);
	void SetEncodingThreadPriority(int priority, HANDLE handle);
	int i_tmp_kbps;
	int i_num_projects;
	int my_page;
	
	int CheckTabs();
	bool m_bEndCreditsValid;
	void PrepareVars(int i_option);
	void AbortConversion(int i_option);
	int CheckFolders();
	int CheckOverWrite_AllFiles(int x);
	void CalcNewBitrate(int *pi_new, int, long, long);
	int m_intMEncoderTimer;
	int m_intNormalizeTimer;
	int m_intWaveUpTimer;
	int m_intVirtualDubTimer;
	int m_intAc3WavTimer;
	int m_intDivxAutoTimer;
	int m_intTimer;
	void Do_EndCredits();

	BOOL m_bSplitOK;
	int ShutDown_(int mode);
	void My_Static_Init();
	int CheckOverWrite(CString s_filename, char type, BOOL overwrite);
	void PrintFinalLog();
	void execute_steps(int option);
	void VirtualDub();
	int CreateVirtualDubJobFile(int option);
	
	void ConvertAc3(int i_audio_format);

	void Log(CString log);
	void My_Init(void);
	CToolTipCtrl *m_pTooltip;

	
	void CreateVobSubCommandLine(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTPUTFILESPAGE_H__73E7BBC1_1627_11D5_B29D_93C7EF2A3914__INCLUDED_)
