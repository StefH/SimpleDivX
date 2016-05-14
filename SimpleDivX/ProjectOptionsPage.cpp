// ProjectOptionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "SimpleDivX.h"
#include "ProjectOptionsPage.h"
#include "mytooltips.h"
#include "SimpleDivX_Defines.h"
#include "SimpleDivXBatch.h"
#include "LanguageSupport.h"
#include "ProgramInfo.h"
#include ".\projectoptionspage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TEMPFOLDER          0
#define FINALFOLDER         1

extern CProgramInfo Prog_Info;
extern CEncodingInfo Enc_Info;
extern CLanguageSupport LanguageModule;
extern CSimpleDivXBatch* pBatch_List;

/////////////////////////////////////////////////////////////////////////////
// CProjectOptionsPage property page

IMPLEMENT_DYNCREATE(CProjectOptionsPage, CPropertyPage)

CProjectOptionsPage::CProjectOptionsPage() : CPropertyPage(CProjectOptionsPage::IDD)
, m_SplitOnFadeOut(FALSE)
{
	//{{AFX_DATA_INIT(CProjectOptionsPage)
	m_s_TempFolder = _T("");
	m_s_OutputFolder = _T("");
	m_bSplitAtFileSize = FALSE;
	m_bOverWrite = FALSE;
	m_bDeleteTemp = FALSE;
	m_lSplitAt = 0;
	m_bBatchMode = -1;
	m_strProjectName = _T("");
	//}}AFX_DATA_INIT

	m_pTooltip = NULL;
	m_pTooltip = new CToolTipCtrl;
	m_pTooltip->Create(this);

	my_page = PAGE_PROJECT;
}

CProjectOptionsPage::~CProjectOptionsPage()
{
	delete m_pTooltip;
}

void CProjectOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjectOptionsPage)
	DDX_Control(pDX, IDC_LIST_BATCH, m_ctrlBatchList);
	DDX_Text(pDX, IDC_TEMPFOLDER, m_s_TempFolder);
	DDX_Text(pDX, IDC_OUTPUTAVIFOLDER, m_s_OutputFolder);
	DDX_Check(pDX, IDC_CHECK_SPLIT, m_bSplitAtFileSize);
	DDX_Check(pDX, IDC_CHECK_OVERWRITE_AVI, m_bOverWrite);
	DDX_Check(pDX, IDC_CHECK_DEL_TEMP, m_bDeleteTemp);
	DDX_Text(pDX, IDC_SPLITAT, m_lSplitAt);
	DDX_Radio(pDX, IDC_RADIO_NORMALMODE, m_bBatchMode);
	DDX_Text(pDX, IDC_PROJNAME, m_strProjectName);
	DDV_MaxChars(pDX, m_strProjectName, 256);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_CMB_SPLIT_PARTS, m_cmbSplitInParts);
	DDX_Check(pDX, IDC_CHECK_SPLIT_FADEOUT, m_SplitOnFadeOut);
}


BEGIN_MESSAGE_MAP(CProjectOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CProjectOptionsPage)
	ON_BN_CLICKED(IDC_TEMP_LOCATION, OnTempLocation)
	ON_BN_CLICKED(IDC_AVI_LOCATION, OnAviLocation)
	ON_EN_CHANGE(IDC_TEMPFOLDER, OnChangeTempfolder)
	ON_EN_CHANGE(IDC_OUTPUTAVIFOLDER, OnChangeOutputavifolder)
	ON_BN_CLICKED(IDC_CHECK_SPLIT, OnCheckSplit)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_RADIO_NORMALMODE, OnRadioNormalmode)
	ON_BN_CLICKED(IDC_RADIO_BATCHMODE, OnRadioBatchmode)
	ON_EN_CHANGE(IDC_PROJNAME, OnChangeProjname)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnButtonDown)
	//}}AFX_MSG_MAP
	ON_LBN_SELCHANGE(IDC_LIST_BATCH, OnLbnSelchangeListBatch)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, OnBnClickedButtonModify)
	ON_BN_CLICKED(IDC_CHECK_SPLIT_FADEOUT, OnBnClickedCheckSplitblack)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectOptionsPage message handlers

void CProjectOptionsPage::OnTempLocation() 
{
	BrowseFolder(&m_s_TempFolder, TEMPFOLDER);
	UpdateData(FALSE);
	Enc_Info.s_TempFolder = m_s_TempFolder;
}

void CProjectOptionsPage::OnAviLocation() 
{
	BrowseFolder(&m_s_OutputFolder, FINALFOLDER);
	UpdateData(FALSE);
	Enc_Info.s_OutputFolder = m_s_OutputFolder;	
}

void CProjectOptionsPage::BrowseFolder(CString *pstr_CurrentFolder, int i_what)
{
	char pDir[_MAX_PATH] = "";
	char pPath[_MAX_PATH] = "";
	char strWhat[STRLEN64] = "";
	CString Folder;
	LPITEMIDLIST pIDList;
	
	strcpy(strWhat,"Select the folder where ");
	if (i_what == TEMPFOLDER)
	{
		strcat(strWhat, "the temporary files are created.");
		Folder = *pstr_CurrentFolder;
	}
	
	if (i_what == FINALFOLDER)
	{
		strcat(strWhat, "the final movie is created.");
		Folder = *pstr_CurrentFolder;
	}
	
	BROWSEINFO BInfo = {m_hWnd,NULL,pDir, strWhat,
		BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS 
		,NULL,NULL};
	
	// display the browse dialog
	pIDList = ::SHBrowseForFolder( &BInfo );
	
	// get the folder the user selected
	BOOL Rtn = ::SHGetPathFromIDList( pIDList, pPath );	
	
	if (Rtn)
	{
		if (strlen( pPath ) > 3)
		{
			Folder = pPath;
		}
		else
		{
			Folder = pPath;
			Folder = Folder.Left(2);
		}
		*pstr_CurrentFolder = Folder;
	}
	else
	{
		// huh ?
	}
}

void CProjectOptionsPage::OnChangeTempfolder() 
{
	UpdateData(1);
	Enc_Info.s_TempFolder = m_s_TempFolder;
}

void CProjectOptionsPage::OnChangeOutputavifolder() 
{
	UpdateData(1);
	Enc_Info.s_OutputFolder = m_s_OutputFolder;
}

BOOL CProjectOptionsPage::OnKillActive() 
{
	Prog_Info.b_batch_enabled = m_bBatchMode;

	ProcessVars();

	size_t x = sizeof(Enc_Info);

	if (Prog_Info.b_batch_enabled == 0)
	{
		pBatch_List->SetEncInfo(Enc_Info);
	}
		
	return CPropertyPage::OnKillActive();
}

void CProjectOptionsPage::OnCheckSplit() 
{
	UpdateData(TRUE);

	m_SplitOnFadeOut = false;
	CheckDlgButton(IDC_CHECK_SPLIT_FADEOUT, 0);

	Enc_Info.i_split = m_bSplitAtFileSize * SPLIT_SIZE;
	
	GetDlgItem(IDC_SPLITAT)->EnableWindow(m_bSplitAtFileSize);	
	GetDlgItem(IDC_CMB_SPLIT_PARTS)->EnableWindow(m_bSplitAtFileSize);
	GetDlgItem(IDC_STATIC_SPLIT_IN_PARTS)->EnableWindow(m_bSplitAtFileSize);
}

void CProjectOptionsPage::My_Init()
{
	m_s_TempFolder = Enc_Info.s_TempFolder;
	m_strProjectName = Enc_Info.s_ProjName;
	m_s_OutputFolder = Enc_Info.s_OutputFolder;
	
	m_lSplitAt = Enc_Info.l_split_at;
	m_bOverWrite = Enc_Info.b_overwrite;
	m_bDeleteTemp = Enc_Info.b_delete_temp;
	m_bBatchMode  = Prog_Info.b_batch_enabled;
	m_bSplitAtFileSize = Enc_Info.i_split & SPLIT_SIZE;
	m_SplitOnFadeOut = Enc_Info.i_split & SPLIT_FADEOUT;
	if (Enc_Info.i_num_parts_requested <= 1) Enc_Info.i_num_parts_requested = 2;
	if (Enc_Info.i_num_parts_requested >= MAXSPLIT) Enc_Info.i_num_parts_requested = 2;
	UpdateData(FALSE);

	CheckDlgButton(IDC_CHECK_OVERWRITE_AVI, Enc_Info.b_overwrite);
	CheckDlgButton(IDC_CHECK_SPLIT, m_bSplitAtFileSize);
	CheckDlgButton(IDC_CHECK_SPLIT_FADEOUT, m_SplitOnFadeOut);
	CheckDlgButton(IDC_CHECK_DEL_TEMP, Enc_Info.b_delete_temp);
	
	//OnCheckSplit();
	GetDlgItem(IDC_SPLITAT)->EnableWindow(m_bSplitAtFileSize);	
	GetDlgItem(IDC_CMB_SPLIT_PARTS)->EnableWindow(m_bSplitAtFileSize);
	GetDlgItem(IDC_STATIC_SPLIT_IN_PARTS)->EnableWindow(m_bSplitAtFileSize);
	
	if (Prog_Info.b_batch_enabled)
	{
		OnRadioBatchmode();
	}
	else
	{
		OnRadioNormalmode();
	}

	m_cmbSplitInParts.SetCurSel(Enc_Info.i_num_parts_requested - 2);

	LanguageModule.InitAttribs(this, my_page);
}

BOOL CProjectOptionsPage::OnSetActive() 
{
	if (Prog_Info.b_init_projectoptionspage == 0)
	{
		My_Static_Init();
		Prog_Info.b_init_projectoptionspage = 1;
	}	

	My_Init();

	return CPropertyPage::OnSetActive();
}

void CProjectOptionsPage::My_Static_Init()
{
	int i = 0;
	CMyToolTips my_tooltips;
	my_tooltips.PrepareTooltips(this, my_page, m_pTooltip);

	m_ctrlBatchList.ResetContent();

	for (i = 0 ; i < pBatch_List->GetNum() ; i++)
	{
		CEncodingInfo Enc_Info_Tmp = pBatch_List->GetEncInfoAt(i);
		
		CString s_tmp;
		s_tmp.Format("%s\t%s", Enc_Info_Tmp.s_ProjName, Enc_Info_Tmp.s_status_time);
		m_ctrlBatchList.AddString(s_tmp);
	}

	m_cmbSplitInParts.ResetContent();
	for (i = 2 ; i < MAXSPLIT; i++)
	{
		CString tmp;
		tmp.Format("%2d", i);
		m_cmbSplitInParts.AddString(tmp);
	}

	UpdateListBoxScroll(&m_ctrlBatchList);

	m_ctrlBatchList.SetCurSel(0);
}

void CProjectOptionsPage::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (NULL != m_pTooltip) m_pTooltip->Activate(TRUE);		
	CPropertyPage::OnMouseMove(nFlags, point);
}

BOOL CProjectOptionsPage::PreTranslateMessage(MSG* pMsg) 
{
	if (NULL != m_pTooltip) m_pTooltip->RelayEvent(pMsg);	
	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CProjectOptionsPage::OnAdd() 
{
	CString str;
	int i = 0;

	ProcessVars();

	if (pBatch_List->GetNum() < MAX_BATCH)
	{
		CTime t = CTime::GetCurrentTime();

		Enc_Info.s_status_time = t.Format( "[%d-%m-%y, %H:%M:%S]");

		Enc_Info.i_status = WAITING;
		
		str.Format("%s\t%s", 
			Enc_Info.s_ProjName,
			Enc_Info.s_status_time);

		pBatch_List->AddEncInfo(Enc_Info);

		m_ctrlBatchList.AddString(str);
		m_ctrlBatchList.SetCurSel(pBatch_List->GetNum() - 1);

		UpdateListBoxScroll(&m_ctrlBatchList);
	}
	
}

void CProjectOptionsPage::OnRemove() 
{
	int sel = m_ctrlBatchList.GetCurSel();

	if (sel >= 0)
	{
		int j = 0;
//		pBatch_List->status[sel] = INVALID;
		m_ctrlBatchList.DeleteString(sel);

		pBatch_List->RemoveEncInfo(sel);

		if (sel > 1) 
		{
			m_ctrlBatchList.SetCurSel(sel-1);
		}
		else 
		{
			m_ctrlBatchList.SetCurSel(0);
		}

		UpdateListBoxScroll(&m_ctrlBatchList);
	}
}

void CProjectOptionsPage::OnRadioNormalmode() 
{
	UpdateData(TRUE);

	RefreshWindow(m_bBatchMode);
}

void CProjectOptionsPage::OnRadioBatchmode() 
{
	UpdateData(TRUE);
	RefreshWindow(m_bBatchMode);
}

void CProjectOptionsPage::RefreshWindow(int batch)
{
	GetDlgItem(IDC_LIST_BATCH)->EnableWindow(batch);
	GetDlgItem(IDC_ADD)->EnableWindow(batch);
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(batch);
	GetDlgItem(IDC_REMOVE)->EnableWindow(batch);
	GetDlgItem(IDC_BUTTON_UP)->EnableWindow(batch);
	GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(batch);
	GetDlgItem(IDC_BUTTON_MODIFY)->EnableWindow(batch);
	GetDlgItem(IDC_CHECK_DELETE_BATCH)->EnableWindow(batch);
}

void CProjectOptionsPage::CheckProjectName()
{
	char alllowedChars[] = {' ', '-', ',' , '\'', NULL};
	if (m_strProjectName.IsEmpty())
	{
		m_strProjectName = "My Movie";
	}

	int i = 0;
	int j = 0;

	for (i = 0 ; i < m_strProjectName.GetLength() ; i++)
	{
		j = 0;
		char ch = m_strProjectName.GetAt(i);

		if (!isalnum(ch))
		{
			while (alllowedChars[j] != NULL)
			{
				if (alllowedChars[j] == ch)
				{
					break;
				}

				j++;
			}

			if (alllowedChars[j] == NULL)
			{
				m_strProjectName.SetAt(i, '_');
			}
		}
	}

	Enc_Info.s_ProjName = m_strProjectName;
	if (Enc_Info.b_auto_logfile_naming)
	{
		Enc_Info.sTwoPassLogFile.Format("%s\\%s%s", Enc_Info.s_TempFolder, Enc_Info.s_ProjName, DIVXLOGFILEEXT);
	}
}

void CProjectOptionsPage::OnChangeProjname() 
{
	UpdateData(1);
	//CheckProjectName();
	//strcpy(Enc_Info.s_ProjName , m_strProjectName);
	//UpdateData(0);
}

void CProjectOptionsPage::OnButtonClear() 
{
	m_ctrlBatchList.ResetContent();
	pBatch_List->ClearEncList();
}

void CProjectOptionsPage::OnButtonUp() 
{
	int sel = m_ctrlBatchList.GetCurSel();

	if (sel >= 1)
	{
		Switch(sel, 1);
	}
}

void CProjectOptionsPage::OnButtonDown() 
{
	int sel = m_ctrlBatchList.GetCurSel();

	if (sel < m_ctrlBatchList.GetCount() - 1)
	{
		Switch(sel, -1);
	}
}

void CProjectOptionsPage::Switch(int sel, int up)
{
	CEncodingInfo Enc_Info_1;
	CEncodingInfo Enc_Info_2;
	CString strtmp;
	
	int n = m_ctrlBatchList.GetTextLen( sel );
	
	m_ctrlBatchList.GetText(sel, strtmp.GetBuffer(n) );
	strtmp.ReleaseBuffer();

	pBatch_List->Swap(sel, sel - up);
	
	if (up == 1) 
	{
		m_ctrlBatchList.InsertString(sel - 1, strtmp.GetBuffer(0));
		m_ctrlBatchList.DeleteString(sel + 1 );
	}
	else
	{
		m_ctrlBatchList.DeleteString(sel);
		m_ctrlBatchList.InsertString(sel + 1, strtmp.GetBuffer(0));
	}
	
	m_ctrlBatchList.SelectString(0,  strtmp.GetBuffer(0));
}

void CProjectOptionsPage::OnLbnSelchangeListBatch()
{
	int sel = m_ctrlBatchList.GetCurSel();	
	
	Enc_Info = pBatch_List->GetEncInfoAt(sel);

	m_s_OutputFolder = Enc_Info.s_OutputFolder;
	m_s_TempFolder = Enc_Info.s_TempFolder;
	m_strProjectName = Enc_Info.s_ProjName;
	m_bDeleteTemp = Enc_Info.b_delete_temp;
	m_bOverWrite = Enc_Info.b_overwrite;
	m_lSplitAt = Enc_Info.l_split_at;
	m_cmbSplitInParts.SetCurSel(Enc_Info.i_num_parts_requested - 2);

	m_bSplitAtFileSize = Enc_Info.i_split & SPLIT_SIZE;
	


	GetDlgItem(IDC_SPLITAT)->EnableWindow(m_bSplitAtFileSize);
	GetDlgItem(IDC_CMB_SPLIT_PARTS)->EnableWindow(m_bSplitAtFileSize);
	GetDlgItem(IDC_STATIC_SPLIT_IN_PARTS)->EnableWindow(m_bSplitAtFileSize);

	UpdateData(0);
}

void CProjectOptionsPage::OnBnClickedButtonModify()
{
	int sel = m_ctrlBatchList.GetCurSel();

	if (sel >= 0)
	{
		ProcessVars();

		CTime t = CTime::GetCurrentTime();
		CString str;

		Enc_Info.s_status_time = t.Format( "[%d-%m-%y, %H:%M:%S]");
		Enc_Info.i_status = WAITING;

		str.Format("%s\t%s", 
			Enc_Info.s_ProjName,
			Enc_Info.s_status_time);
			//str_status[WAITING]);

		m_ctrlBatchList.DeleteString(sel);
		m_ctrlBatchList.InsertString(sel, str);
		m_ctrlBatchList.SetCurSel(sel);

		pBatch_List->SetEncInfoAt(sel, Enc_Info);

		UpdateListBoxScroll(&m_ctrlBatchList);
	}
}

int CProjectOptionsPage::UpdateListBoxScroll(CListBox *pmyListBox)
{
	CString      str;
	int max_text_len = 0;
	int max_tot_len = 0;
	int cnt = pmyListBox->GetCount();
	int cur_sel = pmyListBox->GetCurSel();
	
	for (int i = 0; i < cnt; i++)
	{
		int tabplace = 0;
		pmyListBox->GetText(i, str);

		int iExt = GetTextLen(str);

		if (iExt > max_tot_len)
			max_tot_len = iExt;

		if ((tabplace=str.Find('\t')) != -1)
		{
			str = str.Left(tabplace);
		}

		iExt = GetTextLen(str);

		if (iExt > max_text_len)
			max_text_len = iExt;
	}

	pmyListBox->SetTabStops(10 + ((max_text_len * 3) / 4) ); // 157 + 20
	
	pmyListBox->SetHorizontalExtent(max_tot_len + 10);

	m_ctrlBatchList.SetCurSel(cur_sel);

	return 0;
}

int CProjectOptionsPage::GetTextLen(LPCTSTR lpszText)
{
	ASSERT(AfxIsValidString(lpszText));

	CDC *pDC = GetDC();
	ASSERT(pDC);

	CSize size;
	CFont* pOldFont = pDC->SelectObject(GetFont());
	if ((m_ctrlBatchList.GetStyle() & LBS_USETABSTOPS) == 0)
	{
		size = pDC->GetTextExtent(lpszText, (int) _tcslen(lpszText));
		size.cx += 3;
	}
	else
	{
		// Expand tabs as well
		size = pDC->GetTabbedTextExtent(lpszText, (int) _tcslen(lpszText), 0, NULL);
		size.cx += 2;
	}
	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);

	return size.cx;
}

int CProjectOptionsPage::ProcessVars(void)
{
	UpdateData(TRUE);
	Enc_Info.l_split_at = m_lSplitAt;
	Enc_Info.i_split = m_bSplitAtFileSize * SPLIT_SIZE + m_SplitOnFadeOut * SPLIT_FADEOUT;
	Enc_Info.b_overwrite = m_bOverWrite;
	Enc_Info.b_delete_temp = m_bDeleteTemp;
	Enc_Info.i_num_parts_requested = m_cmbSplitInParts.GetCurSel() + 2;

	CheckProjectName();
	UpdateData(FALSE);
	return 0;
}

void CProjectOptionsPage::OnBnClickedCheckSplitblack()
{
	UpdateData(TRUE);
	
	m_bSplitAtFileSize = false;
	CheckDlgButton(IDC_CHECK_SPLIT, 0);	

	Enc_Info.i_split = m_SplitOnFadeOut * SPLIT_FADEOUT;

	GetDlgItem(IDC_SPLITAT)->EnableWindow(0);	
	GetDlgItem(IDC_CMB_SPLIT_PARTS)->EnableWindow(0);
	GetDlgItem(IDC_STATIC_SPLIT_IN_PARTS)->EnableWindow(0);
}