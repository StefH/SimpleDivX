// AllControlsSheet.cpp : implementation file
//

#include "stdafx.h"
#include "SimpleDivX.h"
#include "mytooltips.h"
#include "AllControlsSheet.h"
#include "SimpleDivX_Defines.h"
#include "MyReg.h"
#include "EncodingInfo.h"
#include "ProgramInfo.h"
#include ".\allcontrolssheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CProgramInfo Prog_Info;
extern CEncodingInfo Enc_Info;

CString strCredits[] = 
{
	"[WebPage]\r\n",
	" - Fre3.com, for the hosting from my web-page\r\n",
	" - EdMcVinny, for the public relations and creating the guide.\r\n",
	"\r\n[Code & Graphics]\r\n",
	" - biorainbow.com, for the commandline parse code,\r\n",
	" - Don Cross, for the riff + wave write routines.\r\n",
	" - Maven, for the vstrip + ifo parsing code.\r\n", 
	" - www.iconstree.com, for some icons.\r\n",
	" - Hans Dietrich, for XBrowseForFolder.\r\n",
	" - Donald Graft, for DGIndex.\r\n",
	" - Tom Serface, for FileFindEx. \r\n",
	//" - Ilias Sarantoglou, for the XviD analyzer code.\r\n",
	"\r\n[Tools]\r\n",
	" - http://mplayerhq.hu for the mencoder and mplayer.\r\n",
	" - BlackStar & Sergio, for the divxauto program.\r\n",
	" - Avery Lee, for VirtualDub.\r\n",
	" - mplayer/mencoder team\r\n",
	" - Tobias Waldvogel, for the Ogg Vorbis DirectShow filter.\r\n",
	" - Gabest, for the vobsub.dll\r\n",
	"\r\n[Other]\r\n",
	" - Danni Din, for the besweet gui.\r\n",
	"\r\n[Testing]\r\n",
	" - [fränK], for the new layout and self-sacrificing betatesting\r\n",
	" - Gizmo, for the endless testing and bug-shooting!\r\n",
	" - Player1, for testing and finding bugs.\r\n",
	" - Postality, for helping me solve the 1-vob file bug.\r\n",
	" - All other Beta-Testers for testing this program !!!\r\n",
	" \r\n[Translations]\r\n",
	" - Brazilian; onthewayhome\r\n",
	" - Bulgarian; K12 and MM\r\n",
	" - Croatian; Lizard_ago\r\n", // lizard_ago@hotmail.com
	" - Czech; Lagardere,Veronika and Karel\r\n", // lagardere@seznam.cz
	" - Danish; Morten Christiansen\r\n", // mkc@studmed.au.dk
	" - French; djibeXX , XpoZ and Kiof\r\n", // djibe87@skynet.be ; kiof@free.fr
	" - German; [fränK], Gizmo , Player1, Shogun and h3nk42\r\n", // shogun_sliver@yahoo.de ,  <henk42@gmx.net>
	" - Greek; <<Neo the Hellenic from Peristeri>>\r\n", // john_r@freemail.gr
	" - Italian; Fabrizio Degni, Diego Rondini, Luca Snoriguzz and Mateo\r\n", // panda84@inwind.it
	" - Magyar; Haraszti Zsolt\r\n", // hazso@westel900.net
	" - Norwegian; Wiak", // tommarnk@gmail.com
	" - Polish; Roy and Andrzej Rudnik\r\n", // roy@divx.pl ; arudnik@beep.pl
	" - Portuguese; Antonio Barbosa\r\n", // barbosa_antonio@hotmail.com
	" - Romanian; crysty1\r\n",	// crysty1@go.ro
	" - Russian; Artem Kostyuk\r\n", // artemkostyuk@hotmail.com
	" - Simplified Chinese; tingpu\r\n", // tingpu@hotmail.com
	" - Slovak; 'not yet'\r\n",
	" - Spanish; Francisco Rodriguez\r\n" // frodrigu@agbar.es
	" - Swedish; Patryk and Robert Hedberg\r\n", // patryk83@hotmail.com;robert_hedberg@swipnet.se
	" - Turkish; serge_tc\r\n" // serge@url.ro
};


class CAboutDlg : public CDialog
{
public:
	CAboutDlg();


// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_strEditCredits;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange * pDX);    // DDX/D999DV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHomepage();
	afx_msg void OnEmail();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_strEditCredits = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange * pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_EDIT_CREDITS, m_strEditCredits);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_HOMEPAGE, OnHomepage)
	ON_BN_CLICKED(IDC_EMAIL, OnEmail)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAllControlsSheet

IMPLEMENT_DYNAMIC(CAllControlsSheet, CPropertySheet)

CAllControlsSheet::CAllControlsSheet(UINT nIDCaption, CWnd * pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	AddControlPages();
}

CAllControlsSheet::CAllControlsSheet(LPCTSTR pszCaption, CWnd * pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	// This method is used.
	AddControlPages();
}

CAllControlsSheet::~CAllControlsSheet()
{
	delete m_VideoOptionsPage;
	delete m_AudioOptionsPage;
	delete m_CompressionOptionsPage;
	delete m_OutputFilesPage;
    delete m_InputFilesPage;
	delete m_CodecOptionsPage;
	delete m_ProjectOptionsPage;
	delete m_SetupPage;
	delete m_MiscPage;
}

BEGIN_MESSAGE_MAP(CAllControlsSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CAllControlsSheet)
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
//	ON_WM_ACTIVATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAllControlsSheet message handlers

void CAllControlsSheet::AddControlPages()
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_NET);
	m_psh.dwFlags |= PSP_USEHICON;
	m_psh.hIcon = m_hIcon;
	m_psh.dwFlags |= PSH_NOAPPLYNOW;    // Lose the 'Apply Now' button
	m_psh.dwFlags &= ~(PSH_HASHELP ); // Lose the 'Help' button
			
	// remove all help buttons
	m_InputFilesPage.m_psp.dwFlags &= ~(PSP_HASHELP);
	m_VideoOptionsPage.m_psp.dwFlags &= ~(PSP_HASHELP);
	m_AudioOptionsPage.m_psp.dwFlags &= ~(PSP_HASHELP);
	m_CompressionOptionsPage.m_psp.dwFlags &= ~(PSP_HASHELP);
	m_OutputFilesPage.m_psp.dwFlags &= ~(PSP_HASHELP);
	m_CodecOptionsPage.m_psp.dwFlags &= ~(PSP_HASHELP);
	m_SetupPage.m_psp.dwFlags &= ~(PSP_HASHELP);
	m_ProjectOptionsPage.m_psp.dwFlags &= ~(PSP_HASHELP);
	m_MiscPage.m_psp.dwFlags &= ~(PSP_HASHELP);

	// volgorde is belangrijk
	AddPage(&m_InputFilesPage);
	AddPage(&m_VideoOptionsPage);
	AddPage(&m_AudioOptionsPage);
	AddPage(&m_CodecOptionsPage);
	AddPage(&m_CompressionOptionsPage);
	AddPage(&m_MiscPage);
	AddPage(&m_ProjectOptionsPage);
	AddPage(&m_OutputFilesPage);
	AddPage(&m_SetupPage);
}

BOOL CAllControlsSheet::OnInitDialog() 
{
	// IDM_ABOUTBOX must be in the system command range.
	m_hIcon = AfxGetApp()->LoadIcon(IDI_NET);

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CMenu * pSysMenu = GetSystemMenu(FALSE);
	
	GetDlgItem(IDOK)->ShowWindow(0);
	GetDlgItem(IDOK)->EnableWindow(0);
	
	// add the minimize button to the window
	::SetWindowLong( m_hWnd, GWL_STYLE, GetStyle() | WS_MINIMIZEBOX );
		
	// add the minimize command to the system menu
	GetSystemMenu( FALSE )->InsertMenu( -1, MF_BYPOSITION | MF_STRING, SC_ICON, "Minimize" );

	if (pSysMenu != NULL)
	{
		char s_about[32] = "";
		sprintf(s_about, "About %s ...", APP_NAME);
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, s_about);
	}

	// activate the new sysmenu
	DrawMenuBar();
	
	int res = CPropertySheet::OnInitDialog();

	if (Prog_Info.b_RunAutomated)
	{
		//SLU
		//Force output files to instantiate
		SetActivePage(&m_AudioOptionsPage); //Lance (Init Audio page)
		SetActivePage(&m_OutputFilesPage);  //(Init Output page)
		//SetActivePage(&m_InputFilesPage);

		ShowWindow(SW_MINIMIZE); //Minimize SimpleDivX

		m_InputFilesPage.Automation(); //Start SimpleDivX
		//End SLU
	}
	
	return res;
}

void CAllControlsSheet::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if (nID == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CPropertySheet::OnSysCommand(nID, lParam);
	}
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	for (int i = 0 ; i < sizeof(strCredits) / sizeof(CString); i ++)
	{
		m_strEditCredits += strCredits[i];
	}
	
	UpdateData(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control           
}

void CAboutDlg::OnHomepage() 
{
	ShellExecute(NULL, _T("open"), HOMEPAGE_URL, NULL,NULL, SW_NORMAL);
}

void CAboutDlg::OnEmail() 
{
	ShellExecute(NULL, _T("open"), "mailto: " + (CString) EMAIL_ADDRESS, NULL,NULL, SW_NORMAL);	
}

bool CAllControlsSheet::SetPageTitle(int nPage, CString sTitle)
{
	CString sTitleLocal(sTitle);
	int nTitleLen(sTitleLocal.GetLength());

	TCITEM tci = { 0 };
	tci.mask = TCIF_TEXT;
	tci.pszText = sTitleLocal.GetBuffer(nTitleLen);

	CTabCtrl* pTabCtrl = ((CPropertySheet*) GetParent())->GetTabControl();
	ASSERT_VALID(pTabCtrl);

	//int nPageNumber(((CPropertySheet*) GetParent())->GetActiveIndex());
	pTabCtrl->SetItem(nPage, &tci);

	sTitleLocal.ReleaseBuffer();

	return false;
}
