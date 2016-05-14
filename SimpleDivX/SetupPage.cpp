// SetupPage.cpp : implementation file
//

#include "stdafx.h"
#include "SimpleDivX.h"
#include "SetupPage.h"
#include "ProgramLocation.h"
#include "mytooltips.h"
#include "SimpleDivX_Defines.h"
#include "OutputFilesPage.h"
#include "LanguageSupport.h"
#include "EncodingInfo.h"
#include "ProgramInfo.h"
#include ".\setuppage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CProgramInfo Prog_Info;
extern CEncodingInfo Enc_Info;
extern CLanguageSupport LanguageModule;

/////////////////////////////////////////////////////////////////////////////
// CSetupPage property page

IMPLEMENT_DYNCREATE(CSetupPage, CPropertyPage)

CSetupPage::CSetupPage() : CPropertyPage(CSetupPage::IDD)
{
	//{{AFX_DATA_INIT(CSetupPage)
	m_strProgramLocations = _T("");
	//}}AFX_DATA_INIT

	m_pTooltip = NULL;
	m_pTooltip = new CToolTipCtrl;
	m_pTooltip->Create(this);

	my_page = PAGE_SETUP;
}

CSetupPage::~CSetupPage()
{
	delete m_pTooltip;
}

void CSetupPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupPage)
	DDX_Control(pDX, IDC_DISK_PRIO, m_ctrlDiskPrio);
	DDX_Control(pDX, IDC_VIDEO_PRIO, m_ctrlVideoPrio);
	DDX_Text(pDX, IDC_EDIT_PROGRAM_LOCATIONS, m_strProgramLocations);
	DDV_MaxChars(pDX, m_strProgramLocations, 256);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LST_LANGUAGES, m_ctrlLanguageList);
}


BEGIN_MESSAGE_MAP(CSetupPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSetupPage)
	ON_BN_CLICKED(IDC_BROWSE_PROGRAM, OnBrowseProgram)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_GET_DEF, OnGetDef)
	ON_BN_CLICKED(IDC_BUTTON_PRINTDEBUG, OnButtonPrintdebug)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_LST_LANGUAGES, OnCbnSelchangeLstLanguages)
//	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupPage message handlers

BOOL CSetupPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupPage::OnBrowseProgram() 
{
	int status = 0;
	CProgramLocation window;

	window.m_strMEncoder = Prog_Info.strMEncoderFolder;
	window.m_strMPlayer = Prog_Info.strMPlayerFolder;
	window.m_strVirtualDub = Prog_Info.strVirtualDubFolder;
	window.m_strMkvMerge = Prog_Info.strMkvMergeFolder;
	
	status = window.DoModal();

	if (status == IDOK)
	{
		Prog_Info.strMEncoderFolder = window.m_strMEncoder;
		Prog_Info.strMPlayerFolder = window.m_strMPlayer;
		Prog_Info.strVirtualDubFolder = window.m_strVirtualDub;
		Prog_Info.strMkvMergeFolder = window.m_strMkvMerge;
	}

	FillFindList();

	UpdateData(FALSE);
}

void CSetupPage::FillFindList()
{
	m_strProgramLocations = "";
	m_strProgramLocations += CheckFile(&Prog_Info.strMEncoderFolder, MENCODER,0);
	m_strProgramLocations += CheckFile(&Prog_Info.strMPlayerFolder, MPLAYER,0);
	m_strProgramLocations += CheckFile(&Prog_Info.strVirtualDubFolder, VIRTUALDUB,0);
	//m_strProgramLocations += CheckFile(&Prog_Info.strMkvMergeFolder, MKVMERGE,0);
	
	int len = m_strProgramLocations.GetLength();
	if (len == 61) // 85 = mkvmerge , 61 = normal
	{
		Prog_Info.b_programs_found = 1;
	}
	else
	{
		Prog_Info.b_programs_found = 0;
	}
}

CString CSetupPage::CheckFile(CString *folder, CString filename, bool b_last)
{
	CString str_tmp = filename;
	FILE *fp_test;
	int i_len = 0;
	bool found = FALSE;

	char s_current_dir[STRLEN] = {0};
	char s_dir[STRLEN] = {0};
		
	sprintf(s_dir, "%s\\%s", *folder, filename);
	fp_test = fopen(s_dir,"rb");
	
	if (fp_test) 
	{
		fclose(fp_test);
		found = TRUE;
	}
	else
	{
		GetCurrentDirectory(STRLEN, s_current_dir);

		// try to find the files in the current folder
		sprintf(s_dir, "%s\\%s\\%s", s_current_dir, TOOLS_FOLDER, filename);
		fp_test = fopen(s_dir,"rb");
		if (fp_test)
		{
			CString newFolder;
			newFolder.Format("%s\\%s", s_current_dir, TOOLS_FOLDER);
			*folder = (CString) newFolder;
			fclose(fp_test);
			found = TRUE;
		}
		else
		{
			if (strlen(*folder) <= 0)
			{
				*folder = (CString)"c:\\";
			}
		}
	}

	if (found)
	{
		str_tmp += " : OK";
	}
	else
	{
		str_tmp += " : not OK";
	}
	
	if (!b_last)
	{
		str_tmp += "\r\n";
	}
	
	return str_tmp;
}

void CSetupPage::My_Init()
{
	FillFindList();

	m_ctrlVideoPrio.SetCurSel(Prog_Info.i_video_prio);
	m_ctrlDiskPrio.SetCurSel(Prog_Info.i_disk_prio);

	UpdateData(FALSE);

	LanguageModule.InitAttribs(this, my_page);
}

void CSetupPage::My_Static_Init()
{
	CMyToolTips my_tooltips;
	my_tooltips.PrepareTooltips(this, my_page, m_pTooltip);

	m_ctrlLanguageList.ResetContent();
	
	CList<CString,CString&> &list = LanguageModule.GetLanguageNames();

	if (Prog_Info.b_language_change_allowed)
	{
		int sel = 0;
		POSITION pos = list.GetHeadPosition();
		for (int i = 0 ; i < list.GetCount() ; i++)
		{
			CString lang = list.GetNext(pos);
			m_ctrlLanguageList.AddString(lang);

			if (lang.CompareNoCase(Prog_Info.currentLanguage) == 0)
			{
				sel = i;
			}
		}

		m_ctrlLanguageList.SetCurSel(sel);
	}
	else
	{
		m_ctrlLanguageList.AddString(DEF_GUI_LANG);
		m_ctrlLanguageList.SetCurSel(0);
		GetDlgItem(IDC_LST_LANGUAGES)->EnableWindow(FALSE);
	}


	GetDlgItem(IDC_BUTTON_PRINTDEBUG)->ShowWindow(Prog_Info.b_debug_mode);
}

BOOL CSetupPage::OnSetActive() 
{
	if (Prog_Info.b_init_setuppage == 0)
	{
		My_Static_Init();
		
		Prog_Info.b_init_setuppage = 1;
	}

	FillFindList();
	My_Init();
	
	return CPropertyPage::OnSetActive();
}

void CSetupPage::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (NULL != m_pTooltip) m_pTooltip->Activate(TRUE);	

	CPropertyPage::OnMouseMove(nFlags, point);
}

BOOL CSetupPage::PreTranslateMessage(MSG* pMsg) 
{
	if (NULL != m_pTooltip) m_pTooltip->RelayEvent(pMsg);	

	return CPropertyPage::PreTranslateMessage(pMsg);
}

BOOL CSetupPage::OnKillActive() 
{
	UpdateData(1);
	
	Prog_Info.i_video_prio = m_ctrlVideoPrio.GetCurSel();
	Prog_Info.i_disk_prio = m_ctrlDiskPrio.GetCurSel();

	return CPropertyPage::OnKillActive();
}

void CSetupPage::OnGetDef() 
{
	CEncodingInfo tempEnc_Info;
	CProgramInfo tempProgram_Info;

	Prog_Info = tempProgram_Info;
	Enc_Info = tempEnc_Info;

	My_Init();
	
	UpdateData(0);
}

void CSetupPage::OnButtonPrintdebug() 
{
	COutputFilesPage o;
	o.PrintVars();	
}

void CSetupPage::OnCbnSelchangeLstLanguages()
{
	UpdateData(TRUE);
	CString strtmp;

	int lang = m_ctrlLanguageList.GetCurSel();

	m_ctrlLanguageList.GetLBText(lang,strtmp);

	LanguageModule.ChangeLanguage(strtmp.GetBuffer(0));

	Prog_Info.currentLanguage = strtmp.GetBuffer(0);

	LanguageModule.InitAttribs(this, my_page);
}