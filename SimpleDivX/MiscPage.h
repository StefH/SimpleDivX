#pragma once

//#include "ChkListBox.h"
#include "afxwin.h"

// Misc dialog
class CMiscPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMiscPage)

public:
	CMiscPage();
	virtual ~CMiscPage();

// Dialog Data
	enum { IDD = IDD_DIALOG_MISC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
public:
	
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	
private:
	CRect rect;
	
	CToolTipCtrl *m_pTooltip;
	void My_Static_Init(void);
	void My_Init(void);
	BOOL m_bCreateSubtitles;
	BOOL b_can_do_subtitles;
	int my_page;
	void RefreshList(void);
public:
	DECLARE_MESSAGE_MAP()
private:
	
	CCheckListBox m_CheckListBoxSubLang;
public:
	afx_msg void OnLbnSelchangeListLang();
	afx_msg void OnBnClickedCreateSubtitles();
	afx_msg void OnBnClickedCheckAllLang();
	BOOL m_bCreateAllSubs;
	BOOL m_ContainerFormat;
};
