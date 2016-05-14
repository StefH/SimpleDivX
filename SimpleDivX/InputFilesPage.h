#if !defined(AFX_INPUTFILESPAGE_H__B0C69230_5582_11D4_98A3_D7F5B61CE616__INCLUDED_)
#define AFX_INPUTFILESPAGE_H__B0C69230_5582_11D4_98A3_D7F5B61CE616__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputFilesPage.h : header file

#include "../vob_mpeg2/vstrip/vobinfo.h"
#include "SimpleDivX_Defines.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CInputFilesPage dialog

class CInputFilesPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CInputFilesPage)

	// Construction
public:
	void InitCodec();
	void UpdateVideoInfo(int);
	void My_Static_Init();
	bool m_bvobchanged;
	void GetMovieLength();
	int GetVideoInfo(VOB_FILE* pr_file, int);
	void My_Init();
	void Init_VOB_List();
	void BrowseFolder(CString& str_CurrentFolder, int i_what);
	CInputFilesPage();
	~CInputFilesPage();
	CToolTipCtrl *m_pTooltip;
	int Init_IFO_List(void);
	void GetVideoInfo_IFO();
	void Automation();

	// Dialog Data
	//{{AFX_DATA(CInputFilesPage)
	enum { IDD = IDD_DIALOG_INPUTFILES };
	CString	m_sVobFolder;
	BOOL	m_bCheckAll;
	BOOL	m_bCheckMain;
	//	CString	m_s_ProjName;
	//}}AFX_DATA


	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInputFilesPage)
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
	//{{AFX_MSG(CInputFilesPage)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCheckMain();
	afx_msg void OnCheckAll();
	afx_msg void OnVobfolder();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	void ShowEmptyInfo();
	void UpdateVobfolder();
	int my_page;

public:
	int UpdateWindowInfo(void);

private:
	CCheckListBox m_CheckListBoxVobs;
	int previousChecked;
	int previousCheckedTitle;
public:
	afx_msg void OnLbnSelchangeListVobs();
	//	virtual BOOL OnInitDialog();
private:

public:
	int AddVobFiles2IFO(CString filename, int x);
	int GetAudioInfoFromVob(VOBINFO *pr_vobinfo);
	CComboBox m_cmdTitles;
	afx_msg void OnCbnSelchangeComboTitles();
private:
	void UpdateTitlesAndChapters(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTFILESPAGE_H__B0C69230_5582_11D4_98A3_D7F5B61CE616__INCLUDED_)
