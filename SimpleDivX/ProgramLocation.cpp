// ProgramLocation.cpp : implementation file
//

#include "stdafx.h"
#include "SimpleDivX.h"
#include "ProgramLocation.h"
#include "SimpleDivX_Defines.h"
#include "LanguageSupport.h"
#include "ProgramInfo.h"
#include ".\programlocation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CProgramInfo Prog_Info;
extern CLanguageSupport LanguageModule;

/////////////////////////////////////////////////////////////////////////////
// CProgramLocation dialog


CProgramLocation::CProgramLocation(CWnd* pParent /*=NULL*/)
	: CDialog(CProgramLocation::IDD, pParent)
	, page(0)
{
	//{{AFX_DATA_INIT(CProgramLocation)
	m_strMPlayer = _T("");
	m_strMEncoder = _T("");
	m_strVirtualDub = _T("");
	m_strMkvMerge = _T("");
	//}}AFX_DATA_INIT
}


void CProgramLocation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgramLocation)
	DDX_Text(pDX, IDC_MPLAYER_EDIT, m_strMPlayer);
	DDV_MaxChars(pDX, m_strMPlayer, 256);
	DDX_Text(pDX, IDC_MENCODER_EDIT, m_strMEncoder);
	DDV_MaxChars(pDX, m_strMEncoder, 256);
	DDX_Text(pDX, IDC_VIRTUALDUB_EDIT, m_strVirtualDub);
	DDV_MaxChars(pDX, m_strVirtualDub, 256);
	DDX_Text(pDX, IDC_MKVMERGE_EDIT, m_strMkvMerge);
	DDV_MaxChars(pDX, m_strMkvMerge, 256);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgramLocation, CDialog)
	//{{AFX_MSG_MAP(CProgramLocation)
	
	ON_BN_CLICKED(IDC_VIRTUALDUB_LOCATION, OnVirtualdubLocation)
	
	
	//}}AFX_MSG_MAP
//	ON_WM_SETFOCUS()
//ON_WM_CREATE()
ON_BN_CLICKED(IDC_MENCODER_LOCATION, OnBnClickedMencoderLocation)
ON_BN_CLICKED(IDC_MPLAYER_LOCATION, OnBnClickedMplayerLocation)
ON_BN_CLICKED(IDC_MKVMERGE_LOCATION, OnBnClickedMkvmergeLocation)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgramLocation message handlers
void CProgramLocation::OnVirtualdubLocation() 
{
    m_strVirtualDub = BrowseForFolder(IDC_VIRTUALDUB_LOCATION);
	UpdateData(FALSE);
}

CString CProgramLocation::BrowseForFolder(int i_option)
{
	CString name;
	CString command;
	CString Folder;
	
	switch (i_option)
	{
	case IDC_MENCODER_LOCATION:
		name = MENCODER;
		Folder = m_strMEncoder;
		break;

	case IDC_MPLAYER_LOCATION:
		name = MPLAYER;
		Folder = m_strMPlayer;
		break;
		
	case IDC_VIRTUALDUB_LOCATION:
		name = VIRTUALDUB;
		Folder = m_strVirtualDub;
		break;

	case IDC_MKVMERGE_LOCATION:
		name = MKVMERGE;
		Folder = m_strMkvMerge;
		break;


	default:
		break;
	}

	command.Format("%s|%s||", name, name);
		
	CFileDialog dlg(TRUE, "", name, 0 ,command);

	if (dlg.DoModal() == IDOK)
	{
		Folder = dlg.GetPathName();
		Folder = Folder.Left(Folder.GetLength() - strlen(name));
		//if (Folder.GetLength() > 3) Folder += '\\';
	}

	return Folder;
}

BOOL CProgramLocation::OnInitDialog()
{
	CDialog::OnInitDialog();

	page = PAGE_PROGRAM_LOCATIONS;

	LanguageModule.InitAttribs(this, page);

	return TRUE;
}

void CProgramLocation::OnBnClickedMencoderLocation()
{
	m_strMEncoder = BrowseForFolder(IDC_MENCODER_LOCATION);
	UpdateData(FALSE);
}

void CProgramLocation::OnBnClickedMplayerLocation()
{
	m_strMPlayer = BrowseForFolder(IDC_MPLAYER_LOCATION);
	UpdateData(FALSE);
}

void CProgramLocation::OnBnClickedMkvmergeLocation()
{
	m_strMkvMerge = BrowseForFolder(IDC_MKVMERGE_LOCATION);
	UpdateData(FALSE);
}
