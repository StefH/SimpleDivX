// SubtitlesPage.cpp : implementation file
//

#include "stdafx.h"
#include "simpledivx.h"
#include "MiscPage.h"
#include "mytooltips.h"
#include "SimpleDivX_Defines.h"
#include "LanguageSupport.h"
#include "EncodingInfo.h"
#include "ProgramInfo.h"

extern CProgramInfo Prog_Info;
extern CEncodingInfo Enc_Info;
extern CLanguageSupport LanguageModule;

// SubtitlesPage dialog

IMPLEMENT_DYNAMIC(CMiscPage, CPropertyPage)
CMiscPage::CMiscPage()
: CPropertyPage(CMiscPage::IDD)
, m_bCreateSubtitles(FALSE)
, m_bCreateAllSubs(FALSE)
, m_ContainerFormat(FALSE)
{
	m_pTooltip = NULL;
	m_pTooltip = new CToolTipCtrl;
	m_pTooltip->Create(this);

	my_page = PAGE_MISC;

	b_can_do_subtitles = false;
}

BEGIN_MESSAGE_MAP(CMiscPage, CPropertyPage)

	ON_LBN_SELCHANGE(IDC_LIST_LANG, OnLbnSelchangeListLang)
	ON_BN_CLICKED(IDC_CREATE_SUBTITLES, OnBnClickedCreateSubtitles)
	ON_BN_CLICKED(IDC_CHECK_ALL_LANG, OnBnClickedCheckAllLang)
END_MESSAGE_MAP()

CMiscPage::~CMiscPage()
{
	if (m_pTooltip) delete m_pTooltip;
}

void CMiscPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CREATE_SUBTITLES, m_bCreateSubtitles);
	DDX_Control(pDX, IDC_LIST_LANG, m_CheckListBoxSubLang);
	DDX_Check(pDX, IDC_CHECK_ALL_LANG, m_bCreateAllSubs);
	DDX_Radio(pDX, IDC_RADIO_CONTAINER_AVI, m_ContainerFormat);
}

void CMiscPage::My_Init(void)
{
	BOOL b_valid_ifo = Enc_Info.r_ifo_files[Enc_Info.i_selected_ifo_id].b_valid_ifo;
	b_can_do_subtitles = Prog_Info.b_can_use_vobsub * b_valid_ifo * (Enc_Info.i_found_subs > 0);
	
	GetDlgItem(IDC_CREATE_SUBTITLES)->EnableWindow(b_can_do_subtitles);

	GetDlgItem(IDC_RADIO_CONTAINER_AVI)->EnableWindow(Enc_Info.r_valid_container[CF_AVI]);
	GetDlgItem(IDC_RADIO_CONTAINER_OGM)->EnableWindow(Enc_Info.r_valid_container[CF_OGM]);
	GetDlgItem(IDC_RADIO_CONTAINER_MATROSKA)->EnableWindow(Enc_Info.r_valid_container[CF_MATROSKA]);
	GetDlgItem(IDC_RADIO_CONTAINER_MP4)->EnableWindow(Enc_Info.r_valid_container[CF_MP4]);

	//Enc_Info.b_create_subtitles *= b_can_do_subtitles;
	m_bCreateSubtitles = b_can_do_subtitles * Enc_Info.b_create_subtitles;

	m_bCreateAllSubs = Enc_Info.b_extract_all_subtitles;

	m_ContainerFormat = Enc_Info.i_container_format;

	UpdateData(0);
	
	RefreshList();

	OnBnClickedCreateSubtitles();

	OnBnClickedCheckAllLang();

	for (int index = 0 ; index < Enc_Info.i_numSelectedSubs ; index++)
	{
		m_CheckListBoxSubLang.SetCheck(Enc_Info.selectedSubs[index], 
			Enc_Info.b_create_subtitles);
	}

	LanguageModule.InitAttribs(this,my_page);
}

BOOL CMiscPage::OnSetActive()
{
	if (Prog_Info.b_init_subtitlespage == false)
	{
		My_Static_Init();
		Prog_Info.b_init_subtitlespage = true;
	}

	My_Init();

	return CPropertyPage::OnSetActive();
}

BOOL CMiscPage::OnKillActive()
{
	int subCount = 0;

	for (int index = 0 ; index < Enc_Info.i_found_subs ; index++)
	{
		Enc_Info.selectedSubs[index] = -1;
		if (m_CheckListBoxSubLang.GetCheck(index) == 1)
		{
			Enc_Info.selectedSubs[subCount] = index;
			subCount++;
		}
	}

	Enc_Info.i_numSelectedSubs = subCount;

	Enc_Info.b_create_subtitles = m_bCreateSubtitles;

	Enc_Info.b_extract_all_subtitles = m_bCreateAllSubs;

	UpdateData(TRUE);

	Enc_Info.i_container_format = m_ContainerFormat;

	return CPropertyPage::OnKillActive();
}

void CMiscPage::My_Static_Init(void)
{
	CMyToolTips my_tooltips;
	my_tooltips.PrepareTooltips(this, my_page, 
		m_pTooltip);
}

void CMiscPage::RefreshList(void)
{
	int subs = 0;

	m_CheckListBoxSubLang.ResetContent();

	if (b_can_do_subtitles)
	{
		m_CheckListBoxSubLang.EnableWindow(TRUE);

		for (subs = 0 ; subs < Enc_Info.i_found_subs ; subs++)
		{
			m_CheckListBoxSubLang.AddString(Enc_Info.as_subs[subs]);
		}
	}
	else
	{
		m_CheckListBoxSubLang.AddString("No Subtitles found");
		m_CheckListBoxSubLang.SetCheck(0, 0);
		m_CheckListBoxSubLang.EnableWindow(FALSE);
	}
}

void CMiscPage::OnLbnSelchangeListLang()
{
	//int id = m_CheckListBoxSubLang.GetCurSel();

//	int num = m_CheckListBoxSubLang.GetCount();

//	int numSel = m_CheckListBoxSubLang.GetSelCount();

//	int y = 0;
}

void CMiscPage::OnBnClickedCreateSubtitles()
{
	UpdateData(TRUE);
	Enc_Info.b_create_subtitles = m_bCreateSubtitles;
	m_CheckListBoxSubLang.EnableWindow(m_bCreateSubtitles);
	m_bCreateAllSubs *= Enc_Info.b_create_subtitles;
	
	GetDlgItem(IDC_CHECK_ALL_LANG)->EnableWindow(Enc_Info.b_create_subtitles * Enc_Info.b_create_subtitles);

	OnBnClickedCheckAllLang();
}

void CMiscPage::OnBnClickedCheckAllLang()
{
	UpdateData(TRUE);

	Enc_Info.b_extract_all_subtitles = m_bCreateAllSubs;

	if (Enc_Info.i_found_subs)
	{
		for (int i = 0 ; i < m_CheckListBoxSubLang.GetCount() ; i++)
		{
			m_CheckListBoxSubLang.SetCheck(i, Enc_Info.b_extract_all_subtitles);
		}
	}

	m_CheckListBoxSubLang.SetTopIndex(0);
}
