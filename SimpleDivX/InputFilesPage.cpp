// InputFilesPage.cpp : implementation file
//
#include "stdafx.h"
#include "SimpleDivX.h"
#include "InputFilesPage.h"
#include "SetupPage.h"
#include "../GenericMM/GenericMM.h"
#include "GenericSystem.h"
#include "SimpleDivX_Defines.h"
#include "MyReg.h"
#include "LanguageSupport.h"
#include "mytooltips.h"
#include "XBrowseForFolder.h"
#include ".\inputfilespage.h"
#include "FileFindEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CProgramInfo Prog_Info;
extern CEncodingInfo Enc_Info;
extern CLanguageSupport LanguageModule;

#define BROWSEVOBFILE 0
#define MAXITEMS 128

/////////////////////////////////////////////////////////////////////////////
// CInputFilesPage property page

IMPLEMENT_DYNCREATE(CInputFilesPage, CPropertyPage)

CInputFilesPage::CInputFilesPage() : CPropertyPage(CInputFilesPage::IDD)
{
	//{{AFX_DATA_INIT(CInputFilesPage)
	m_sVobFolder = _T("");
	m_bCheckAll = FALSE;
	m_bCheckMain = FALSE;
	//}}AFX_DATA_INIT

	m_pTooltip = NULL;
	m_pTooltip = new CToolTipCtrl;
	m_pTooltip->Create(this);

	my_page = PAGE_INPUT;
}

CInputFilesPage::~CInputFilesPage()
{
	delete m_pTooltip;
}

void CInputFilesPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputFilesPage)
	DDX_Text(pDX, IDC_FOLDER_EDIT, m_sVobFolder);
	DDV_MaxChars(pDX, m_sVobFolder, 256);
	DDX_Check(pDX, IDC_CHECK_ALL, m_bCheckAll);
	DDX_Check(pDX, IDC_CHECK_MAIN, m_bCheckMain);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LIST_VOBS, m_CheckListBoxVobs);
	DDX_Control(pDX, IDC_COMBO_TITLES, m_cmdTitles);
}


BEGIN_MESSAGE_MAP(CInputFilesPage, CPropertyPage)
	//{{AFX_MSG_MAP(CInputFilesPage)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_CHECK_MAIN, OnCheckMain)
	ON_BN_CLICKED(IDC_CHECK_ALL, OnCheckAll)
	ON_BN_CLICKED(IDC_VOBFOLDER, OnVobfolder)
	//}}AFX_MSG_MAP

	ON_LBN_SELCHANGE(IDC_LIST_VOBS, OnLbnSelchangeListVobs)
	ON_CBN_SELCHANGE(IDC_COMBO_TITLES, OnCbnSelchangeComboTitles)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputFilesPage message handlers
void CInputFilesPage::UpdateVobfolder()
{
	Enc_Info.sVobFolder = m_sVobFolder;

	m_bCheckMain = Enc_Info.b_checkmain = 0;
	m_bCheckAll = Enc_Info.b_checkall = 0;
	previousChecked = -1;
	Enc_Info.i_selected_ifo_id = -1;

	Enc_Info.i_input_mode = IM_IFO;

	if (Enc_Info.i_input_mode == IM_IFO)
	{
		Init_IFO_List();
	}

	if (Enc_Info.i_input_mode == IM_FILE)
	{
		Init_VOB_List();
	}

	ShowEmptyInfo();
}

void CInputFilesPage::OnVobfolder() 
{
	BrowseFolder(m_sVobFolder, BROWSEVOBFILE);
	UpdateData(FALSE);
	UpdateVobfolder();
}

void CInputFilesPage::BrowseFolder(CString& str_CurrentFolder, int i_what)
{
	TCHAR szFolder[MAX_PATH*2];
	CString strWhat;
	
	strWhat = "Select the folder where ";
	if (i_what == BROWSEVOBFILE)
	{
		strWhat.Append("the (decrypted) VOB files are.");
	}
	
	szFolder[0] = _T('\0');

	BOOL bRet = XBrowseForFolder(m_hWnd,
								 strWhat, // Display title string
								 str_CurrentFolder,	// start with current directory
								 szFolder,
								 sizeof(szFolder)/sizeof(TCHAR)-2);

	if (bRet)
	{
		str_CurrentFolder = szFolder;
	}
}

int CInputFilesPage::Init_IFO_List(void)
{
	CFileFind finder;
	CString path;
	CString tmpfilename;
	CString tmpfilenameFull;
	int bWorking = 0;
	char current = '*';
	int i = 0, j = 0;
	
	path = m_sVobFolder + "\\*.ifo";

	bWorking = finder.FindFile((LPCTSTR) path);
	m_CheckListBoxVobs.SetTopIndex(0);
	m_CheckListBoxVobs.ResetContent();
	Enc_Info.i_number_ifofiles = 0;

	if (!bWorking)
	{
		m_CheckListBoxVobs.EnableWindow(0);

		for (i = 0; i < MAXIFOFILES ; i++)
		{
			Enc_Info.r_ifo_files[i].b_valid_ifo = false;
			Enc_Info.r_ifo_files[i].s_filename.Empty();
			Enc_Info.r_ifo_files[i].s_short_filename.Empty();
			Enc_Info.r_ifo_files[i].i_num_vobfiles = 0;
			Enc_Info.r_ifo_files[i].l_size = 0;
			for (j = 0; j < MAXVOBFILES ; j++)
			{
				Enc_Info.r_ifo_files[i].vob_files[j].l_size = 0;
				Enc_Info.r_ifo_files[i].vob_files[j].s_filename.Empty();
				Enc_Info.r_ifo_files[i].vob_files[j].s_short_filename.Empty();
			}

			Enc_Info.r_ifo_files[i].r_movie_info.i_audio_streams = 0;
			Enc_Info.r_ifo_files[i].r_movie_info.i_end_title_id = 0;
			Enc_Info.r_ifo_files[i].r_movie_info.i_num_subs = 0;
			Enc_Info.r_ifo_files[i].r_movie_info.i_num_titles = 0;
			Enc_Info.r_ifo_files[i].r_movie_info.i_start_title_id = 0;
		}
	}
	else
	{
		// ???????????????
	}

	int start_title = 1;
	i = 0;

	GetDlgItem(IDC_STATIC_CHECK_ALL)->EnableWindow(0);
	GetDlgItem(IDC_STATIC_CHECK_MAIN)->EnableWindow(bWorking);

	while (bWorking)
	{
		__int64 l_size = 0;
		m_CheckListBoxVobs.EnableWindow(1);

		bWorking = finder.FindNextFile();
		
		tmpfilenameFull = finder.GetFilePath();
		tmpfilename = finder.GetFileName();

		l_size = finder.GetLength();

		if (l_size > 1)
		{
			tmpfilename.MakeLower();
			if (tmpfilename.CompareNoCase("video_ts.ifo"))
			{
				m_CheckListBoxVobs.AddString(tmpfilename);
				Enc_Info.r_ifo_files[i].l_size = (long) l_size;
				AddVobFiles2IFO(tmpfilename, i);

				Enc_Info.r_ifo_files[i].b_valid_ifo =
					parse_movie_ifo(Enc_Info.r_ifo_files[i].s_filename.GetBuffer(),
					&(Enc_Info.r_ifo_files[i].r_movie_info));

                Enc_Info.r_ifo_files[i].r_movie_info.i_start_title_id = start_title;
				Enc_Info.r_ifo_files[i].r_movie_info.i_end_title_id = start_title + Enc_Info.r_ifo_files[i].r_movie_info.i_num_titles - 1;

				start_title = Enc_Info.r_ifo_files[i].r_movie_info.i_end_title_id + 1;

				i++;
			}
		}
	}

	Enc_Info.i_selected_title_id = 0;
	Enc_Info.i_number_ifofiles = i;

	finder.Close();

	if (Enc_Info.i_number_ifofiles == 0)
	{
		Enc_Info.i_input_mode = IM_FILE;
	}

	return 0;
}

void CInputFilesPage::Init_VOB_List()
{
	CString path;
	CString tmpfilename;
	CString tmpfilenameFull;
	int vobsFound = 0;
	int tsFound = 0;
	int i = 0;
	int ifoIDX = 0;
	int idx = 0;
	__int64 l_size = 0;

	m_CheckListBoxVobs.SetTopIndex(0);
	m_CheckListBoxVobs.ResetContent();

	// FIND VOBS/MPEG/MPG
	CFileFindEx vobFinder;
	
	// Include all .vob, .mpeg, or .mpeg files.
	CString csIncludeFilter = _T("*.mpeg|*.vob|*.mpg"); 
	// Note don't want .doc files that start with Tom
	CString csExcludeFilter = _T(""); 

	// Check for files based on the critera the user filled out.
    vobsFound = vobFinder.FindFile(m_sVobFolder,csIncludeFilter, csExcludeFilter, false);
	
	// FIND TS
	CFileFind tsFinder;
	path = m_sVobFolder + "\\*.ts";
	tsFound = tsFinder.FindFile((LPCTSTR) path);

	if ((vobsFound == 0) && (tsFound == 0))
	{
		m_CheckListBoxVobs.AddString("No IFO/VOB/MPEG/TS files found."); 
		m_CheckListBoxVobs.EnableWindow(0);

		Enc_Info.r_ifo_files[0].i_num_vobfiles = 0;
		Enc_Info.r_ifo_files[0].l_size = 0;
		Enc_Info.r_ifo_files[0].s_filename.Empty();
		Enc_Info.r_ifo_files[0].s_short_filename.Empty();

		for (i = 0; i < MAXVOBFILES ; i++)
		{
			Enc_Info.r_ifo_files[0].vob_files[i].l_size = 0;
			Enc_Info.r_ifo_files[0].vob_files[i].s_filename.Empty();
			Enc_Info.r_ifo_files[0].vob_files[i].s_short_filename.Empty();
		}
	}
	else
	{
		// ok
		Enc_Info.i_selected_ifo_id = 0;
	}

	GetDlgItem(IDC_STATIC_CHECK_MAIN)->EnableWindow(0);
	GetDlgItem(IDC_STATIC_CHECK_ALL)->EnableWindow(vobsFound);

	int getNextFileOk = vobsFound;

	while (getNextFileOk)
	{
		m_CheckListBoxVobs.EnableWindow(1);

		getNextFileOk = vobFinder.FindNextFile();
		tmpfilenameFull = vobFinder.GetFilePath();
		tmpfilename = vobFinder.GetFileName();

		l_size = vobFinder.GetLength();

		if (l_size > 1)
		{
			tmpfilename.MakeLower();
			if (tmpfilename.CompareNoCase("video_ts.vob"))
			{
				m_CheckListBoxVobs.AddString(tmpfilename);
			}
		}
	}

	getNextFileOk = tsFound;

	while (getNextFileOk)
	{
		m_CheckListBoxVobs.EnableWindow(1);

		getNextFileOk = tsFinder.FindNextFile();
		tmpfilenameFull = tsFinder.GetFilePath();
		tmpfilename = tsFinder.GetFileName();

		l_size = tsFinder.GetLength();

		if (l_size > 1)
		{
			tmpfilename.MakeLower();
			m_CheckListBoxVobs.AddString(tmpfilename);
		}
	}

	tsFinder.Close();
	vobFinder.Close();

	//SetCurrentDirectory(buffer);
}

BOOL CInputFilesPage::OnSetActive() 
{
	if (Prog_Info.b_init_inputoptionspage == 0)
	{
		My_Static_Init();
		Prog_Info.b_init_inputoptionspage = 1;
	}

	My_Init();

	m_bvobchanged = 0;

	return CPropertyPage::OnSetActive();
}

void CInputFilesPage::My_Init()
{
	int num = 0;

	m_sVobFolder = Enc_Info.sVobFolder;
	m_bCheckMain = Enc_Info.b_checkmain;
	m_bCheckAll = Enc_Info.b_checkall;

	UpdateData(FALSE);

	// Check if exist, if not create.
	CreatePath(Enc_Info.s_TempFolder);
	CreatePath(Enc_Info.s_OutputFolder);

	UpdateData(FALSE);

	int numSearch = 0;
	CString searchList[MAXVOBFILES];

	if ((Enc_Info.i_input_mode == IM_IFO) && ( Enc_Info.i_selected_ifo_id >= 0 ))
	{
		searchList[0] = Enc_Info.r_ifo_files[Enc_Info.i_selected_ifo_id].s_short_filename;
		numSearch = 1;
	}
	else
	{
		numSearch = Enc_Info.r_ifo_files[0].i_num_vobfiles;

		for (int i = 0 ; i < numSearch; i++ )
		{
			searchList[i] = Enc_Info.r_ifo_files[0].vob_files[i].s_short_filename;
		}
	}

	int id = -1;
	int firstIdFound = 0;

	for (int i = 0 ; i < m_CheckListBoxVobs.GetCount(); i++)
	{
		m_CheckListBoxVobs.SetCheck(i, 0);
	}

	if (Enc_Info.i_selected_ifo_id >= 0)
	{
		CString strTmp;

		for (int j = 0 ; j < numSearch; j++)
		{
			// loop
			for (int i = 0 ; i < m_CheckListBoxVobs.GetCount(); i++)
			{
				m_CheckListBoxVobs.GetText(i, strTmp);

				id = m_CheckListBoxVobs.FindString(0, searchList[j]);

				if (id >= 0)
				{
					m_CheckListBoxVobs.SetCheck(id, 1);
					break;
				}
			}
		}

		if (id >= 0)
		{
			bool updateWindow = false;
			m_CheckListBoxVobs.SetTopIndex(firstIdFound);

			if (previousChecked != id)
			{
				previousChecked = id;
				updateWindow = true;
			}

			if (previousCheckedTitle != Enc_Info.i_selected_title_id)
			{
				previousCheckedTitle = id;
				updateWindow = true;
			}

			if (updateWindow)
			{
				UpdateWindowInfo();
			}
		}
		else
		{
			ShowEmptyInfo();
		}

	}

	LanguageModule.InitAttribs(this, my_page);

	GetDlgItem(IDC_STATUS)->SetWindowText(LanguageModule.strStatusReady);
}

int CInputFilesPage::GetVideoInfo(VOB_FILE* pr_file, int vobid)
{
	int i_count = 0;
	int b_search_for_end = 0;
	int b_search_for_audio = 1;
	int b_search_for_cropping = 1;
	int i_sec = 0;
	int i_hour = 0;
	int i_min = 0;
	CString tmp_drv;
	CString filename = pr_file->s_filename;

	GetDlgItem(IDC_STATUS)->SetWindowText(LanguageModule.strStatusBusy);

	HCURSOR cursor_normal = GetCursor();
	HCURSOR cursor_wait = LoadCursor(NULL, IDC_WAIT); 
	SetCursor(cursor_wait);

	tmp_drv.Format("%s\\", Enc_Info.s_TempFolder);

	if (pr_file->r_type == FT_VOB)
	{
		VOBINFO r_vobinfo = {0};

		get_vob_info(filename.GetBuffer(), 
			b_search_for_audio, 
			b_search_for_end, 
			b_search_for_cropping,
			&r_vobinfo, tmp_drv.GetBuffer());

		Enc_Info.s_movie_format = "PAL *";
		Enc_Info.i_original_width = r_vobinfo.i_width;
		Enc_Info.i_original_height = r_vobinfo.i_height;

		Enc_Info.i_cropping_height = r_vobinfo.i_cropping_height;
		Enc_Info.f_aspect_ratio = r_vobinfo.f_aspect_ratio;
		Enc_Info.s_movie_aspect = r_vobinfo.s_aspect_ratio;

		Enc_Info.f_framerate = r_vobinfo.f_framerate;

		Enc_Info.i_found_video_streams = r_vobinfo.i_video_streams;

		for (i_count = 0; i_count < Enc_Info.i_found_video_streams ; i_count ++)
		{
			if (r_vobinfo.video_list[i_count].type == STREAM_TYPE_MPEG2_VIDEO)
			{
				Enc_Info.ai_video_id[i_count] = r_vobinfo.video_list[i_count].id;
			}
		}

		Enc_Info.i_valid_languages = GetAudioInfoFromVob(&r_vobinfo);

		// in 22 ?
		for (i_count = 0; i_count < Enc_Info.i_valid_languages ; i_count ++)
		{
			Enc_Info.ai_language_id[i_count] = r_vobinfo.audio_list[i_count].id;
			Enc_Info.ai_language_type[i_count] = r_vobinfo.audio_list[i_count].type;
		}

		GetMovieLength();
	}
	else
	{
		VOBINFO r_tsinfo = {0};

		get_ts_info(filename.GetBuffer(), 1, 1, &r_tsinfo);

		Enc_Info.s_movie_format = "PAL *";
		Enc_Info.i_original_width = r_tsinfo.i_width;
		Enc_Info.i_original_height = r_tsinfo.i_height;

		Enc_Info.i_cropping_height = 1000;
		Enc_Info.f_aspect_ratio = r_tsinfo.f_aspect_ratio;
		Enc_Info.s_movie_aspect = r_tsinfo.s_aspect_ratio;

		Enc_Info.f_framerate = r_tsinfo.f_framerate;

		Enc_Info.i_found_video_streams = r_tsinfo.i_video_streams;

		for (i_count = 0; i_count < Enc_Info.i_found_video_streams ; i_count ++)
		{
			if (r_tsinfo.video_list[i_count].type == STREAM_TYPE_MPEG2_VIDEO)
			{
				Enc_Info.ai_video_id[i_count] = r_tsinfo.video_list[i_count].id;
			}
		}

		Enc_Info.i_valid_languages = r_tsinfo.i_audio_streams;

		for (i_count = 0; i_count < Enc_Info.i_valid_languages ; i_count ++)
		{
			Enc_Info.ai_language_id[i_count] = r_tsinfo.audio_list[i_count].id;
			Enc_Info.ai_language_type[i_count] = r_tsinfo.audio_list[i_count].type;
			Enc_Info.af_audio_delay[i_count] = 0;

			Enc_Info.ar_ac3_info[i_count].b_valid = r_tsinfo.audio_info.valid;
			Enc_Info.ar_ac3_info[i_count].i_kbps = r_tsinfo.audio_info.i_kbps;
			Enc_Info.ar_ac3_info[i_count].i_khz = r_tsinfo.audio_info.i_khz;
			Enc_Info.ar_ac3_info[i_count].i_num_channels = r_tsinfo.audio_info.i_num_channels;
			Enc_Info.ar_ac3_info[i_count].uc_id = r_tsinfo.audio_info.uc_id;
		}

		Enc_Info.l_total_length = 1;
		Enc_Info.l_frames = (long) (Enc_Info.l_total_length * Enc_Info.f_framerate);
	}

	UpdateWindowInfo();
	UpdateTitlesAndChapters();

	GetDlgItem(IDC_STATUS)->SetWindowText(LanguageModule.strStatusReady);
	SetCursor(cursor_normal);

	return 1;
}

BOOL CInputFilesPage::OnKillActive() 
{
	Enc_Info.sVobFolder = m_sVobFolder;
	Enc_Info.b_checkmain = m_bCheckMain;
	Enc_Info.b_checkall = m_bCheckAll;
	
	return CPropertyPage::OnKillActive();
}

void CInputFilesPage::GetMovieLength()
{
	int i = 0;

	long l_total_length = 0;
	long total_size = 0;
	long l_delta_time_length = 0;
	long l_total_time_length = 0;

	VOBINFO r_vobinfo = {0};

	r_vobinfo.f_framerate = 25.000;
	l_total_time_length = 0;

	for ( i = 0 ; i < Enc_Info.r_ifo_files[0].i_num_vobfiles ; i++)
	{
		int b_audio_search = 0;
		int b_end_search = 1;
		int b_crop_search = 0;

		get_vob_info(Enc_Info.r_ifo_files[0].vob_files[i].s_filename.GetBuffer(), 
			b_audio_search, b_end_search, b_crop_search, &r_vobinfo, NULL);

		l_delta_time_length = (r_vobinfo.l_time_end - r_vobinfo.l_time_start);

		if (l_delta_time_length < 0)
		{
			l_delta_time_length = 0;
		}
		l_total_time_length += l_delta_time_length;
	}

	Enc_Info.l_total_length = l_total_time_length / 1000;

	Enc_Info.l_frames = (long) (Enc_Info.l_total_length * Enc_Info.f_framerate);
}

void CInputFilesPage::My_Static_Init()
{
	CMyToolTips my_tooltips;
	my_tooltips.PrepareTooltips(this, my_page, 
		m_pTooltip);

	InitCodec();
		
	CSetupPage r;

	r.FillFindList();

	m_bvobchanged = true;

	Enc_Info.i_input_mode = IM_IFO;
	Enc_Info.i_selected_ifo_id = -1;
	previousChecked = -2;
	previousCheckedTitle = -1;

	m_sVobFolder = Enc_Info.sVobFolder;
	m_bCheckMain = Enc_Info.b_checkmain;
	m_bCheckAll = Enc_Info.b_checkall;

	if (Enc_Info.i_input_mode == IM_IFO)
	{
		Init_IFO_List();
	}

	if (Enc_Info.i_input_mode == IM_FILE)
	{
		Init_VOB_List();
	}

	ShowEmptyInfo();
}

void CInputFilesPage::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (NULL != m_pTooltip) m_pTooltip->Activate(TRUE);	

	CPropertyPage::OnMouseMove(nFlags, point);
}

BOOL CInputFilesPage::PreTranslateMessage(MSG* pMsg) 
{
	if (NULL != m_pTooltip) m_pTooltip->RelayEvent(pMsg);

	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CInputFilesPage::OnCheckMain() 
{
	// Only valid for IFO's
	int i = 0;
	long max = 1;
	int index = -1;

	UpdateData(TRUE);

	// Clear all
	for (i = 0 ; i < m_CheckListBoxVobs.GetCount() ; i++)
	{
		m_CheckListBoxVobs.SetCheck(i, 0);
	}

	if (m_bCheckMain)
	{
		for (i = 0 ; i < Enc_Info.i_number_ifofiles ; i++)
		{
			if (Enc_Info.r_ifo_files[i].l_size > max)
			{
				max = Enc_Info.r_ifo_files[i].l_size;
				index = i;
			}
		}

		m_CheckListBoxVobs.SetCheck(index, 1);

		UpdateVideoInfo(-1);
	}
	else
	{
		//ShowEmptyInfo();
	}
}

void CInputFilesPage::OnCheckAll() 
{
	int i = 0;
	int num = m_CheckListBoxVobs.GetCount();

	UpdateData(TRUE);

	m_CheckListBoxVobs.SetTopIndex(0);

	for (i = 0 ; i < num ; i++)
	{
		m_CheckListBoxVobs.SetCheck(i, m_bCheckAll);
	}

	if (m_bCheckAll)
	{
		CheckDlgButton(IDC_CHECK_MAIN, 0);

		UpdateVideoInfo(-1);
	}
	else
	{
		ShowEmptyInfo();
	}
}

int CInputFilesPage::AddVobFiles2IFO(CString filename, int ifoIDX)
{
	// Copy filename to ifo-name
	Enc_Info.r_ifo_files[ifoIDX].s_filename = m_sVobFolder + "\\" +  filename;
	Enc_Info.r_ifo_files[ifoIDX].s_short_filename =  filename;
	Enc_Info.r_ifo_files[ifoIDX].i_num_vobfiles = 0;

	// IFO is like "VTS_02_0.IFO"
	int bWorking = 0;
	int i = 0;

	if (filename.GetLength() >= 12)
	{
		CFileFind finder;
		CString tmpfilename = "";
		CString tmpfilenameFull = "";
		CString vobfilename;
		char movie10 = 0;
		char movie1 = 0;
		int num = 0;
		sscanf(filename, "vts_%c%c_%d.ifo", &movie10, &movie1, &num);
		vobfilename.Format("vts_%c%c_*.vob", movie10, movie1);

		CString path = m_sVobFolder + "\\" + vobfilename;

		bWorking = finder.FindFile((LPCTSTR) path);

		int idx = 0;

		while (bWorking)
		{
			long l_size = 0;
			bWorking = finder.FindNextFile();
			tmpfilenameFull = finder.GetFilePath();
			tmpfilename = finder.GetFileName();
			tmpfilename.MakeLower();

			if (tmpfilename.GetLength() >= 12)
			{
				sscanf(tmpfilename, "vts_%c%c_%d.vob", &movie10, &movie1, &num);
			}

			if (num > 0)
			{
				(void) GetFileSizeLong(tmpfilenameFull.GetBuffer(0), &l_size);

				if (l_size > 1)
				{
					Enc_Info.r_ifo_files[ifoIDX].vob_files[idx].l_size = l_size;
					Enc_Info.r_ifo_files[ifoIDX].vob_files[idx].s_filename = tmpfilenameFull;
					Enc_Info.r_ifo_files[ifoIDX].vob_files[idx].s_short_filename = tmpfilename;
					idx++;
				}
			}
		}

		Enc_Info.r_ifo_files[ifoIDX].i_num_vobfiles = idx;
		//Enc_Info.r_ifo_files[ifoIDX].i_id = (movie10 - '0') * 10 + (movie1 - '0');
	}

	return 0;
}

void CInputFilesPage::GetVideoInfo_IFO()
{
	VOBINFO r_vobinfo = {0};
	//MOVIE_INFO r_movie_info = {0};
	int b_search_for_end = 0;
	int b_search_for_audio = 1;
	int b_search_for_cropping = 1;
	int i_count = 0;
	int i = 0;
	int j = 0;
	CString tmp_drv;

	GetDlgItem(IDC_STATUS)->SetWindowText(LanguageModule.strStatusBusy);
	HCURSOR cursor_normal = GetCursor();
	HCURSOR cursor_wait = LoadCursor(NULL, IDC_WAIT); 
	SetCursor(cursor_wait);

	tmp_drv.Format("%s\\", Enc_Info.s_TempFolder);

	int id = Enc_Info.i_selected_ifo_id;

	// HUH
	//Enc_Info.r_ifo_files[id].b_valid_ifo = parse_movie_ifo(Enc_Info.r_ifo_files[id].s_filename.GetBuffer(), &r_movie_info);

	if (Enc_Info.r_ifo_files[id].i_num_vobfiles > 0)
	{
		get_vob_info(Enc_Info.r_ifo_files[id].vob_files[0].s_filename.GetBuffer(), 
			b_search_for_audio, 
			b_search_for_end, 
			b_search_for_cropping,
			&r_vobinfo, 
			tmp_drv.GetBuffer());
	}

	Enc_Info.s_movie_format = "PAL *";

	Enc_Info.i_original_width = r_vobinfo.i_width;
	Enc_Info.i_original_height = r_vobinfo.i_height;
	Enc_Info.i_cropping_height = r_vobinfo.i_cropping_height;
	Enc_Info.f_aspect_ratio = r_vobinfo.f_aspect_ratio;
	Enc_Info.s_movie_aspect = r_vobinfo.s_aspect_ratio;

	Enc_Info.f_framerate = r_vobinfo.f_framerate;

	Enc_Info.i_found_video_streams = r_vobinfo.i_video_streams;

	for (i_count = 0; i_count < Enc_Info.i_found_video_streams ; i_count ++)
	{
		Enc_Info.ai_video_id[i_count] = r_vobinfo.video_list[i_count].id;
	}

	Enc_Info.i_valid_languages = GetAudioInfoFromVob(&r_vobinfo);

	Enc_Info.s_movie_format = Enc_Info.r_ifo_files[id].r_movie_info.s_video_info;
	
	bool b_found = false;


	// Valid are from VOB so trust them !!!
	/*
	for (j = 0 ; j < Enc_Info.i_valid_languages ; j++)
	{
		b_found = false;

		for (i = 0 ; i < Enc_Info.r_ifo_files[id]r_movie_info.i_audio_streams ; i++)
		{
			if (r_movie_info.audio_list[i] == Enc_Info.ai_language_id[j])
			{
				b_found = true;
				Enc_Info.as_audio_languages[j] = Enc_Info.r_ifo_files[id].r_movie_info.as_audio_languages[i];
				break;
			}
		}

		if (!b_found)
		{
			Enc_Info.as_audio_languages[j].Format("Language %d", j);

			sprintf(out, "%s [%d channels] %s", 
				iifoDecodeLang(audio->lang_code),
				audio->num_channels + 1,
				iifoDecodeAudioMode(audio->coding_mode));

		}
	}*/
	for (i = 0 ; i < Enc_Info.r_ifo_files[id].r_movie_info.i_audio_streams ; i++)
	{
		Enc_Info.as_audio_languages[i] = Enc_Info.r_ifo_files[id].r_movie_info.as_audio_languages[i];
		Enc_Info.ai_language_id[i] = Enc_Info.r_ifo_files[id].r_movie_info.audio_list[i];
	}

	Enc_Info.i_found_subs = Enc_Info.r_ifo_files[id].r_movie_info.i_num_subs;
	for (i = 0 ; i < Enc_Info.i_found_subs ; i++)
	{
		Enc_Info.as_subs[i] = Enc_Info.r_ifo_files[id].r_movie_info.as_subs[i];
		Enc_Info.as_subs_short[i]= Enc_Info.r_ifo_files[id].r_movie_info.as_subs_short[i];
	}

	UpdateWindowInfo();
	UpdateTitlesAndChapters();

	GetDlgItem(IDC_STATUS)->SetWindowText(LanguageModule.strStatusReady);
	SetCursor(cursor_normal);
}

void CInputFilesPage::UpdateVideoInfo(int only_for_sel_main)
{
	char tmpfilename[STRLEN] = {0};
	int i = 0;
	int selCount = 0;
	int ai_selected_items[MAXITEMS] = {0};
	CString strData;
	int num = m_CheckListBoxVobs.GetCount();
	int curSel = m_CheckListBoxVobs.GetCurSel();
	bool show = false;

	for (i = 0 ; i < num ; i++)
	{
		if (m_CheckListBoxVobs.GetCheck(i) == 1)
		{
			ai_selected_items[selCount] = i;
			long l_size = 0;

			if (Enc_Info.i_input_mode == IM_FILE)
			{
				m_CheckListBoxVobs.GetText(i, strData);
				CString tmpfilenameFull = Enc_Info.sVobFolder + "\\" + strData;

				(void) GetFileSizeLong(tmpfilenameFull.GetBuffer(0), &l_size);

				if (l_size > 0)
				{
					Enc_Info.r_ifo_files[0].vob_files[selCount].l_size = (long) l_size;
					Enc_Info.r_ifo_files[0].vob_files[selCount].s_filename = tmpfilenameFull;
					Enc_Info.r_ifo_files[0].vob_files[selCount].s_short_filename = strData;
					if (-1 != tmpfilenameFull.Find(".ts", tmpfilenameFull.GetLength() - 4))
					{
						Enc_Info.r_ifo_files[0].vob_files[selCount].r_type = FT_TS;
					}
					else
					{
						Enc_Info.r_ifo_files[0].vob_files[selCount].r_type = FT_VOB;
					}
				}
			}

			selCount++;
		}
	}

	if (Enc_Info.i_input_mode == IM_IFO)
	{
		//Enc_Info.i_number_ifofiles = selCount;
		if (selCount > 0)
		{
			Enc_Info.i_selected_ifo_id = ai_selected_items[0];
		}
		else
		{
			previousChecked = -1;
			Enc_Info.i_selected_ifo_id = -1;
		}
	}
	else
	{
		Enc_Info.i_selected_ifo_id = 0;
	}

	if (selCount > 0)
	{
		if (selCount > 1)
		{
			show = true;
		}
		else 
		{
			m_CheckListBoxVobs.GetText(ai_selected_items[0], strData);

			if (previousChecked != ai_selected_items[0])
			{
				show = true;
				previousChecked = ai_selected_items[0];
			}
		}

		if (show)
		{
			if (Enc_Info.i_input_mode == IM_IFO)
			{
				GetVideoInfo_IFO();
			}
			else
			{
				Enc_Info.r_ifo_files[0].i_num_vobfiles = selCount;
				GetVideoInfo(&(Enc_Info.r_ifo_files[0].vob_files[0]), -1000);
			}
		}
	}
	else
	{
		previousChecked = -9;
		ShowEmptyInfo();
	}
}

void CInputFilesPage::InitCodec()
{
	size_t i = 0, j = 0;
	
	for (i = 0; i < _LAST_CODEC ; i++)
	{
		Prog_Info.ar_vci[i].i_id = -1;
		Prog_Info.ar_vci[i].i_dropdown_id = -1;
		Prog_Info.ar_vci[i].b_valid = false;
		Prog_Info.ar_vci[i].b_AdvancedSettingsAllowed = false;
		Prog_Info.ar_vci[i].b_EditKeyFrameAllowed = true;
		Prog_Info.ar_vci[i].b_TwoPassAllowed = false;
		Prog_Info.ar_vci[i].b_EndCreditsRecompressionAllowed = false;
		Prog_Info.ar_vci[i].b_StartDivXAuto = false;
		Prog_Info.ar_vci[i].b_LogFileSettingsEnabled = false;
		Prog_Info.ar_vci[i].b_PassModeEnabled = false;
		Prog_Info.ar_vci[i].s_name = "";
		Prog_Info.ar_vci[i].s_short_name = "";
		Prog_Info.ar_vci[i].fourcc = "";
	}

	i = 0;

	Prog_Info.ar_vci[i].i_id = MPEG4;
	Prog_Info.ar_vci[i].b_valid = true;
	Prog_Info.ar_vci[i].b_AdvancedSettingsAllowed = true;
	Prog_Info.ar_vci[i].b_EditKeyFrameAllowed = true;
	Prog_Info.ar_vci[i].b_TwoPassAllowed = true;
	Prog_Info.ar_vci[i].b_EndCreditsRecompressionAllowed = false;
	Prog_Info.ar_vci[i].b_StartDivXAuto = false;
	Prog_Info.ar_vci[i].b_LogFileSettingsEnabled = true;
	Prog_Info.ar_vci[i].b_PassModeEnabled = true;
	Prog_Info.ar_vci[i].s_name = "mpeg4 (divx)";
	Prog_Info.ar_vci[i].s_short_name = "MPEG4 (DivX)";
	Prog_Info.ar_vci[i].fourcc = "DIVX";

	i++;
	Prog_Info.ar_vci[i].i_id = XVID;
	Prog_Info.ar_vci[i].b_valid = true;
	Prog_Info.ar_vci[i].b_AdvancedSettingsAllowed = true;
	Prog_Info.ar_vci[i].b_EditKeyFrameAllowed = true;
	Prog_Info.ar_vci[i].b_TwoPassAllowed = true;
	Prog_Info.ar_vci[i].b_EndCreditsRecompressionAllowed = false;
	Prog_Info.ar_vci[i].b_StartDivXAuto = false;
	Prog_Info.ar_vci[i].b_LogFileSettingsEnabled = true;
	Prog_Info.ar_vci[i].b_PassModeEnabled = true;
	Prog_Info.ar_vci[i].s_name = "xvid";
	Prog_Info.ar_vci[i].s_short_name = "XviD";

	i++;
	Prog_Info.ar_vci[i].i_id = H264;
	Prog_Info.ar_vci[i].b_valid = true;
	Prog_Info.ar_vci[i].b_AdvancedSettingsAllowed = true;
	Prog_Info.ar_vci[i].b_EditKeyFrameAllowed = true;
	Prog_Info.ar_vci[i].b_TwoPassAllowed = true;
	Prog_Info.ar_vci[i].b_EndCreditsRecompressionAllowed = false;
	Prog_Info.ar_vci[i].b_StartDivXAuto = false;
	Prog_Info.ar_vci[i].b_LogFileSettingsEnabled = true;
	Prog_Info.ar_vci[i].b_PassModeEnabled = true;
	Prog_Info.ar_vci[i].s_name = "h264";
	Prog_Info.ar_vci[i].s_short_name = "x264";
}

void CInputFilesPage::ShowEmptyInfo()
{
	GetDlgItem(IDC_STATIC_RESOLUTION)->SetWindowText("-");
	GetDlgItem(IDC_STATIC_FPS)->SetWindowText("-");
	GetDlgItem(IDC_STATIC_RATIO)->SetWindowText("-");
	GetDlgItem(IDC_STATIC_LENGTH)->SetWindowText("-");
	GetDlgItem(IDC_STATIC_FORMAT)->SetWindowText("-");
	GetDlgItem(IDC_STATIC_CHAPTERS)->SetWindowText("-");
	GetDlgItem(IDC_COMBO_TITLES)->EnableWindow(0);
	m_cmdTitles.ResetContent();
	m_cmdTitles.AddString("-");
	m_cmdTitles.SetCurSel(0);
}

int CInputFilesPage::UpdateWindowInfo(void)
{
	CString width_height;
	CString frame_rate;
	char s_time_string[STRLEN64];

	width_height.Format("%d x %d", 
		Enc_Info.i_original_width, 
		Enc_Info.i_original_height);

	frame_rate.Format("%2.3f fps", Enc_Info.f_framerate);

	if (frame_rate.Find("23", 0) != -1)
	{
		Enc_Info.i_framerate_id = 0;
	}

	if (frame_rate.Find("29", 0) != -1)
	{
		Enc_Info.i_framerate_id = 3;
	}

	if (frame_rate.Find("25", 0) != -1)
	{
		Enc_Info.i_framerate_id = 2;
	}

	if (!Enc_Info.r_ifo_files[Enc_Info.i_selected_ifo_id].b_valid_ifo)
	{
		if ((Enc_Info.i_framerate_id == 3) && Enc_Info.i_original_height == 480)
		{
			Enc_Info.s_movie_format = "NTSC *";
		}
		else if ((Enc_Info.i_framerate_id == 2) && Enc_Info.i_original_height == 576)
		{
			Enc_Info.s_movie_format = "PAL *";
		}
		else if ((Enc_Info.i_framerate_id == 3) && ((Enc_Info.i_original_height == 1080) || (Enc_Info.i_original_height == 1088)))
		{
			Enc_Info.s_movie_format = "NTSC *";
		}
		else
		{
			Enc_Info.s_movie_format = "?";
		}
	}

	GetDlgItem(IDC_STATIC_RESOLUTION)->SetWindowText(width_height);
	GetDlgItem(IDC_STATIC_FPS)->SetWindowText(frame_rate);
	GetDlgItem(IDC_STATIC_RATIO)->SetWindowText(Enc_Info.s_movie_aspect);

	GetDlgItem(IDC_STATIC_FORMAT)->SetWindowText(Enc_Info.s_movie_format);

	if (Enc_Info.r_ifo_files[Enc_Info.i_selected_ifo_id].b_valid_ifo)
	{
		CString strNumChapters;
		int numChapters = 0;

		// Special --> ALL option
		if (Enc_Info.i_selected_title_id == Enc_Info.r_ifo_files[Enc_Info.i_selected_ifo_id].r_movie_info.i_num_titles)
		{
			Enc_Info.b_process_all_titles = true;
			
			Enc_Info.l_total_length = 0;
			int i = 0;
			for (i = 0; i < Enc_Info.r_ifo_files[Enc_Info.i_selected_ifo_id].r_movie_info.i_num_titles; i++)
			{
				numChapters += Enc_Info.r_ifo_files[Enc_Info.i_selected_ifo_id].r_movie_info.r_titles[i].i_num_chapters;
			}
			for (i = 0 ; i < Enc_Info.r_ifo_files[Enc_Info.i_selected_ifo_id].r_movie_info.i_num_titles; i++)
			{
				Enc_Info.l_total_length += Enc_Info.r_ifo_files[Enc_Info.i_selected_ifo_id].r_movie_info.r_titles[i].l_length;
			}
			Enc_Info.l_frames = (long) (Enc_Info.l_total_length * Enc_Info.f_framerate);
		}
		else
		{
			Enc_Info.b_process_all_titles = false;
			numChapters = Enc_Info.r_ifo_files[Enc_Info.i_selected_ifo_id].r_movie_info.r_titles[Enc_Info.i_selected_title_id].i_num_chapters;

			Enc_Info.l_total_length = Enc_Info.r_ifo_files[Enc_Info.i_selected_ifo_id].r_movie_info.r_titles[Enc_Info.i_selected_title_id].l_length;
			Enc_Info.l_frames = (long) (Enc_Info.l_total_length * Enc_Info.f_framerate);
		}

		strNumChapters.Format("%d", numChapters);
		GetDlgItem(IDC_STATIC_CHAPTERS)->SetWindowText(strNumChapters);

		if (Enc_Info.i_selected_title_id > (m_cmdTitles.GetCount() + 1))
		{
			Enc_Info.i_selected_title_id = 0;
		}
		m_cmdTitles.SetCurSel(Enc_Info.i_selected_title_id);
	}
	else
	{
		GetDlgItem(IDC_STATIC_CHAPTERS)->SetWindowText("1");
		Enc_Info.i_selected_title_id = 0;
		m_cmdTitles.SetCurSel(0);
	}

	seconds2timeformat_hms(Enc_Info.l_total_length, s_time_string);
	GetDlgItem(IDC_STATIC_LENGTH)->SetWindowText(s_time_string);

	return 0;
}

void CInputFilesPage::OnLbnSelchangeListVobs()
{
	m_bvobchanged = 1;

	m_bCheckMain = 0;
	m_bCheckAll = 0;

	UpdateVideoInfo(-1000);
}

int CInputFilesPage::GetAudioInfoFromVob(VOBINFO* pr_vobinfo)
{
	int i_count = 0;
	int i_valid_streams = 0;

	for (i_count = 0; i_count < pr_vobinfo->i_audio_streams ; i_count ++)
	{
		char s_audio_file[STRLEN] = {0};
		CFile *remove_file = NULL;
		AC3_INFO r_ac3_info = {0};
	
		sprintf(s_audio_file , "%s\\_tmp_%d.audio", Enc_Info.s_TempFolder, i_count);

		if (pr_vobinfo->audio_list[i_count].type == AUDIO_TYPE_AC3)
		{
			GetAc3Info(s_audio_file, &r_ac3_info);

			if (!r_ac3_info.b_valid)
			{
				//Enc_Info.af_audio_delay[i_count] = 0;
			}
			else
			{
				Enc_Info.ar_ac3_info[i_valid_streams] = r_ac3_info;
				Enc_Info.ai_language_id[i_valid_streams] = pr_vobinfo->audio_list[i_count].id;
				Enc_Info.ai_language_type[i_valid_streams] = pr_vobinfo->audio_list[i_count].type;
				Enc_Info.af_audio_delay[i_valid_streams] = pr_vobinfo->audio_delay[i_count];
				i_valid_streams++;
			}
		}
		else if (pr_vobinfo->audio_list[i_count].type == AUDIO_TYPE_LPCM)
		{
			r_ac3_info.b_valid = true;
			r_ac3_info.i_kbps = 1536;
			r_ac3_info.i_khz = 48000;
			r_ac3_info.i_num_channels = 2;
			r_ac3_info.uc_id = pr_vobinfo->audio_list[i_count].id;

			Enc_Info.ar_ac3_info[i_valid_streams] = r_ac3_info;
			Enc_Info.ai_language_id[i_valid_streams] = pr_vobinfo->audio_list[i_count].id;
			Enc_Info.ai_language_type[i_valid_streams] = pr_vobinfo->audio_list[i_count].type;
			Enc_Info.af_audio_delay[i_valid_streams] = 0;
			i_valid_streams++;
		}
		else if (pr_vobinfo->audio_list[i_count].type == AUDIO_TYPE_MP2)
		{
			r_ac3_info.b_valid = true;
			if (pr_vobinfo->i_height == 480)
			{
				r_ac3_info.i_kbps = 224;
			}
			else
			{
				r_ac3_info.i_kbps = 192;
			}
			r_ac3_info.i_khz = 48000;
			r_ac3_info.i_num_channels = 2;
			r_ac3_info.uc_id = pr_vobinfo->audio_list[i_count].id;

			Enc_Info.ar_ac3_info[i_valid_streams] = r_ac3_info;
			Enc_Info.ai_language_id[i_valid_streams] = pr_vobinfo->audio_list[i_count].id;
			Enc_Info.ai_language_type[i_valid_streams] = pr_vobinfo->audio_list[i_count].type;
			Enc_Info.af_audio_delay[i_valid_streams] = 0;
			i_valid_streams++;
		}
		else
		{
		}

		TRY
		{
			remove_file->Remove(s_audio_file);
		}
		CATCH(CFileException, pEx) { }
		END_CATCH
	}

	return i_valid_streams;
}


//SLU
//SHE
//Bradley Code, call this to run the program
void CInputFilesPage::Automation()
{
	int validFound = 0;

	if (Prog_Info.b_InternalDetection)
	{
		// Search all cdrom/dvd drives for the correct drive and folder.
		CString validDrives[MAX_HARD_DISKS];
		int validFound = SearchForFolderOnCDDrives(validDrives, "VIDEO_TS");

		if (validFound == 0)
		{
			validFound = SearchForFolderOnAllDrives(validDrives, "VIDEO_TS");
		}

		if (validFound )
		{
			// Choose first available valid drive
			if (Prog_Info.b_InternalDetection)
			{
				// Override with internal detection
				m_sVobFolder = validDrives[0] + "\\VIDEO_TS";
				UpdateData(FALSE);
			}
		}
	}
	else
	{
		// Make sure the external program (Client.exe) has updated all reg entries.
	}
		
	UpdateVobfolder();

	m_bCheckMain = 1; //Check the button that checks the filename
	UpdateData(FALSE);
	
	//Simulate this forms buttons being pushed. And this form being left.
	OnCheckMain();

	OnKillActive();

	// Simulate the outputpage being used. 
	// Only works if the page has already been initialized, 
	// and therefore it only works if the page has been set as active once.
	QuerySiblings(NULL, PAGE_OUTPUT);

	// When the programs is done, the program is closed.
	
    return;
}
//End SLU

void CInputFilesPage::OnCbnSelchangeComboTitles()
{
	UpdateData(TRUE);
	Enc_Info.i_selected_title_id = m_cmdTitles.GetCurSel();	
	UpdateWindowInfo();
}

void CInputFilesPage::UpdateTitlesAndChapters(void)
{
	CString tmp;
	m_cmdTitles.ResetContent();

	if (Enc_Info.r_ifo_files[Enc_Info.i_selected_ifo_id].b_valid_ifo)
	{
		if (Enc_Info.r_ifo_files[Enc_Info.i_selected_ifo_id].r_movie_info.i_num_titles > 1)
		{
			GetDlgItem(IDC_COMBO_TITLES)->EnableWindow(1);
			for (int i = 0 ; i < Enc_Info.r_ifo_files[Enc_Info.i_selected_ifo_id].r_movie_info.i_num_titles ; i++)
			{
				tmp.Format("%d", i + 1);
				m_cmdTitles.AddString(tmp);
			}

			m_cmdTitles.AddString("all");

			m_cmdTitles.SetCurSel(0);
			Enc_Info.i_selected_title_id = 0;
		}
		else
		{
			GetDlgItem(IDC_COMBO_TITLES)->EnableWindow(0);
			m_cmdTitles.AddString("1");
			m_cmdTitles.SetCurSel(0);
		}
	}
	else
	{
		GetDlgItem(IDC_STATIC_CHAPTERS)->SetWindowText("1");
		Enc_Info.i_selected_title_id = 0;
	}
}
