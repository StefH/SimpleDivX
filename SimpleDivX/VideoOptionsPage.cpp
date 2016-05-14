// VideoOptionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "stdio.h"
#include <math.h>
#include "SimpleDivX.h"
#include "VideoOptionsPage.h"
#include "AdvancedVideo_Settings.h"
#include "OutputFilesPage.h"
#include "SimpleDivX_Defines.h"
#include "LanguageSupport.h"
#include "mytooltips.h"
#include "../vob_mpeg2/vstrip/vobinfo.h"
#include "EncodingInfo.h"
#include "ProgramInfo.h"
#include ".\videooptionspage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PREVIEW_TIMER 1
int i_timer = 0;

extern CProgramInfo Prog_Info;
extern CEncodingInfo Enc_Info;
extern OF OutputFormats[MAXFOR+1];
extern CLanguageSupport LanguageModule;

static 	char *str[MAXAUTOFORMATS] =
{	
	{"320"},
	{"352"},
	{"384"},
	{"416"},
	{"480"},
	{"512"},
	{"544"},
	{"576"},
	{"608"},
	{"640"},
	{"656"},
	{"672"},
	{"688"},
	{"704"}
};

static OF tmp_a[MAXFOR+1] = 
	{
		{0,0,0,0,"custom\0"},
		{-1,-1,-1,-1,"Half Resolution"},
		{-1,-1,-1,-1,"Original\0"},
		{704,352,720,528, "PAL 2.35:1 LetterBox    704x352", 1.05}, // tested with 'the thing' 2.35
		{640,320,656,480, "PAL 2.35:1 LetterBox    640x320", 1.04},
		{544,272,544,400, "PAL 2.35:1 LetterBox    544x272", 1.03},
		{480,240,480,352, "PAL 2.35:1 LetterBox    480x240", 1.02},
		{384,192,384,288, "PAL 2.35:1 LetterBox    384x192", 1.01},
		{704,304,720,400, "PAL 2.35:1 WideScreen   704x304", 1.02},   // tested with 'eraser', 'sphere', 'twister', con-air u-571 (ntsc), glad
		{640,272,656,360, "PAL 2.35:1 WideScreen   640x272", 1.01},
		{544,240,560,320, "PAL 2.35:1 WideScreen   544x240", 1.005},
		{480,208,488,272, "PAL 2.35:1 WideScreen   480x208", 1.0025},
		{352,160,360,216, "PAL 2.35:1 WideScreen   352x160", 1.00},
		{704,400,720,408, "PAL 16:9   WideScreen   704x400", 1.05},        // tested with starship troopers
		{640,352,648,368, "PAL 16:9   WideScreen   640x352", 1.05},		 // stir echo's
		{544,304,544,304, "PAL 16:9   WideScreen   544x304", 1.04},
		{480,272,480,272, "PAL 16:9   WideScreen   480x272", 1.02},
		{352,200,352,200, "PAL 16:9   WideScreen   352x192", 1.01},
		{400,304,720,400, "PAL 2.35:1 to  4:3      400x304", 1.0},   // tested with 'eraser', 'sphere', 'twister', con-air u-571 (ntsc), glad
		{360,272,656,360, "PAL 2.35:1 to  4:3      360x272", 1.33},
		{320,240,560,320, "PAL 2.35:1 to  4:3      320x240", 1.0},
		{528,400,720,408, "PAL 16:9   to  4:3      528x400", 1.05},
		{480,352,648,368, "PAL 16:9   to  4:3      480x352", 1.025},
		{360,272,480,272, "PAL 16:9   to  4:3      360x208", 1.01},
		{704,304,704,528, "PAL  4:3   WideScreen   704x304" ,1.05}, // armageddon
		{640,272,640,480, "PAL  4:3   WideScreen   640x272" ,1.04},
		{544,232,544,408, "PAL  4:3   WideScreen   544x232" ,1.03},
		{480,208,480,360, "PAL  4:3   WideScreen   480x208" ,1.02},
		{360,160,360,272, "PAL  4:3   WideScreen   360x160" ,1.01},
		{704,544,720,544, "PAL  4:3   Full screen  704x544", 1.11},
		{640,480,656,480, "PAL  4:3   Full screen  640x480", 1.10},
		{544,400,560,400, "PAL  4:3   Full screen  544x400", 1.09},
		{480,352,488,360, "PAL  4:3   Full screen  480x352", 1.08},
		{352,264,360,264, "PAL  4:3   Full screen  352x264", 1.07},
		{704,480,720,480, "NTSC 4:3   Full screen  704x480", 1.0}, // tested with dolby
		{544,352,544,360, "NTSC 4:3   Full screen  544x352", 1.0},
		{480,320,480,320, "NTSC 4:3   Full screen  480x320", 1.0},
		{352,224,352,224, "NTSC 4:3   Full screen  352x224", 1.0}
	};	

/////////////////////////////////////////////////////////////////////////////
// CVideoOptionsPage property page

IMPLEMENT_DYNCREATE(CVideoOptionsPage, CPropertyPage)

CVideoOptionsPage::CVideoOptionsPage() : CPropertyPage(CVideoOptionsPage::IDD)
{
	//{{AFX_DATA_INIT(CVideoOptionsPage)
	m_bStartAtLBA = FALSE;
	m_bSelectFrameRange = FALSE;
	m_intCX = 0;
	m_intCY = 0;
	m_intDX = 0;
	m_intDY = 0;
	m_intStartAtLBA = 0;
	m_intEndFrame = 0;
	m_intStartFrame = 0;
	
	m_bGetAuto = FALSE;
	//}}AFX_DATA_INIT

	m_pTooltip = NULL;
	m_pTooltip = new CToolTipCtrl;
	m_pTooltip->Create(this);

	my_page = PAGE_VIDEO;
}

CVideoOptionsPage::~CVideoOptionsPage()
{
	delete m_pTooltip;
}

void CVideoOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVideoOptionsPage)
	DDX_Control(pDX, IDC_COMBO_SIZE, m_cmbAutoSize);
	DDX_Control(pDX, IDC_EDIT_DY, m_ctrlDY);
	DDX_Control(pDX, IDC_EDIT_DX, m_ctrlDX);
	DDX_Control(pDX, IDC_EDIT_CY, m_ctrlCY);
	DDX_Control(pDX, IDC_EDIT_CX, m_ctrlCX);

	DDX_Control(pDX, IDC_COMBO_FPS, m_cmbFPS);
	DDX_Control(pDX, IDC_COMBO_OUTPUTFORMAT, m_cmbOutputFormat);
	DDX_Check(pDX, IDC_CHECK_LBA, m_bStartAtLBA);
	DDX_Check(pDX, IDC_CHECK_FRAMERANGE, m_bSelectFrameRange);
	DDX_Text(pDX, IDC_EDIT_CX, m_intCX);
	DDV_MinMaxInt(pDX, m_intCX, 0, 720);
	DDX_Text(pDX, IDC_EDIT_CY, m_intCY);
	DDV_MinMaxInt(pDX, m_intCY, 0, 720);
	DDX_Text(pDX, IDC_EDIT_DX, m_intDX);
	DDV_MinMaxInt(pDX, m_intDX, 0, 720);
	DDX_Text(pDX, IDC_EDIT_DY, m_intDY);
	DDV_MinMaxInt(pDX, m_intDY, 0, 720);
	DDX_Text(pDX, IDC_EDIT_LBA, m_intStartAtLBA);
	DDX_Text(pDX, IDC_END_FRAME, m_intEndFrame);
	DDX_Text(pDX, IDC_START_FRAME, m_intStartFrame);

	DDX_Check(pDX, IDC_CHECK_AUTO, m_bGetAuto);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO_DEINTERLACE, m_cmbDeInterlace);
	DDX_Control(pDX, IDC_COMBO_SOURCE_FORMAT, m_cmbSourceFormat);
}


BEGIN_MESSAGE_MAP(CVideoOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CVideoOptionsPage)
	ON_CBN_SELCHANGE(IDC_COMBO_OUTPUTFORMAT, OnSelchangeComboOutputformat)
	ON_BN_CLICKED(IDC_CHECK_LBA, OnCheckLba)
	ON_BN_CLICKED(IDC_CHECK_FRAMERANGE, OnCheckFramerange)
	ON_BN_CLICKED(IDC_VIDEOPREVIEW, OnVideopreview)
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	
	ON_BN_CLICKED(IDC_CHECK_AUTO, OnCheckAuto)
	ON_CBN_SELCHANGE(IDC_COMBO_SIZE, OnSelchangeComboSize)
	ON_BN_CLICKED(IDC_BUTTON_ADV, OnButtonAdv)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO_SOURCE_FORMAT, OnCbnSelchangeComboSourceFormat)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVideoOptionsPage message handlers

BOOL CVideoOptionsPage::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

BOOL CVideoOptionsPage::OnSetActive() 
{
	if (Prog_Info.b_init_videooptionspage == 0)
	{
		My_Static_Init();
		Prog_Info.b_init_videooptionspage = 1;
	}

	My_Init();

	return CPropertyPage::OnSetActive();
}

void CVideoOptionsPage::My_Init()
{
	m_cmbFPS.SetCurSel(Enc_Info.i_framerate_id);
	m_cmbOutputFormat.SetCurSel(Enc_Info.i_video_output_format_id);
	m_cmbDeInterlace.SetCurSel(Enc_Info.i_deinterlace);
	m_cmbSourceFormat.SetCurSel(Enc_Info.i_source_format_id);
	
	if (Enc_Info.r_ifo_files[Enc_Info.i_selected_ifo_id].i_num_vobfiles == 1)
	{
		m_bStartAtLBA = Enc_Info.b_start_at_lba;
	}
	else
	{
		m_bStartAtLBA = false;
	}

//	m_bHalfResolution = Enc_Info.b_halfresolution;
	m_bSelectFrameRange = Enc_Info.b_selectrange;
	
	CheckDlgButton(IDC_CHECK_FRAMERANGE, Enc_Info.b_selectrange);
	CheckDlgButton(IDC_CHECK_LBA, Enc_Info.b_start_at_lba);
			
	OnCheckFramerange();
	OnCheckLba();

	m_cmbAutoSize.SetCurSel(Enc_Info.i_auto_size_width_id);

	m_bGetAuto = Enc_Info.b_auto_size;
	m_intStartFrame = Enc_Info.i_start_time;
	m_intEndFrame = Enc_Info.i_end_time;
	m_intStartAtLBA = Enc_Info.i_start_at_lba;

	if (Enc_Info.r_ifo_files[Enc_Info.i_selected_ifo_id].i_num_vobfiles > 0)
	{
		GetDlgItem(IDC_CHECK_AUTO)->EnableWindow(1);
	}
	else
	{
		m_bGetAuto = 0;
		Enc_Info.b_auto_size = 0;
		GetDlgItem(IDC_CHECK_AUTO)->EnableWindow(m_bGetAuto);
	}

	CheckDlgButton(IDC_CHECK_AUTO, Enc_Info.b_auto_size);

	//UpdateCheckAuto(Enc_Info.b_auto_size);

	if (Enc_Info.i_video_output_format_id == 0)
	{
		m_intCX = Enc_Info.i_cx;
		m_intCY = Enc_Info.i_cy;
		m_intDX = Enc_Info.i_dx;
		m_intDY = Enc_Info.i_dy;
	}

	UpdateData(FALSE);

	OnCbnSelchangeComboSourceFormat();

	UpdateCheckAuto(m_bGetAuto);
	
	LanguageModule.InitAttribs(this,my_page);
}

void CVideoOptionsPage::OnSelchangeComboOutputformat() 
{
	UpdateData(TRUE);
	Enc_Info.i_video_output_format_id = m_cmbOutputFormat.GetCurSel();
	
	if (Enc_Info.i_video_output_format_id == 0)
	{
		m_ctrlCX.SetReadOnly(FALSE);
		m_ctrlCY.SetReadOnly(FALSE);
		m_ctrlDX.SetReadOnly(FALSE);
		m_ctrlDY.SetReadOnly(FALSE);
	}
	
	if (Enc_Info.i_video_output_format_id == 1)
	{
		m_ctrlCX.SetReadOnly(TRUE);
		m_ctrlCY.SetReadOnly(TRUE);
		m_ctrlDX.SetReadOnly(TRUE);
		m_ctrlDY.SetReadOnly(TRUE);
		m_intCX = Enc_Info.i_original_width / 2;
		m_intCY = Enc_Info.i_original_height / 2;
		m_intDX = Enc_Info.i_original_width / 2;
		m_intDY = Enc_Info.i_original_height / 2;
	}

	if (Enc_Info.i_video_output_format_id == 2)
	{
		m_ctrlCX.SetReadOnly(TRUE);
		m_ctrlCY.SetReadOnly(TRUE);
		m_ctrlDX.SetReadOnly(TRUE);
		m_ctrlDY.SetReadOnly(TRUE);
		m_intCX = Enc_Info.i_original_width;
		m_intCY = Enc_Info.i_original_height;
		m_intDX = Enc_Info.i_original_width;
		m_intDY = Enc_Info.i_original_height;
	}
	
	if (Enc_Info.i_video_output_format_id > 2)
	{
		m_ctrlCX.SetReadOnly(TRUE);
		m_ctrlCY.SetReadOnly(TRUE);
		m_ctrlDX.SetReadOnly(TRUE);
		m_ctrlDY.SetReadOnly(TRUE);
		m_intCX = OutputFormats[Enc_Info.i_video_output_format_id].CX;
		m_intCY = OutputFormats[Enc_Info.i_video_output_format_id].CY;
		m_intDX = OutputFormats[Enc_Info.i_video_output_format_id].DX;
		m_intDY = OutputFormats[Enc_Info.i_video_output_format_id].DY;
	}

	Enc_Info.d_ratio = OutputFormats[Enc_Info.i_video_output_format_id].ratio;

	m_bGetAuto = 0;
	Enc_Info.b_auto_size = 0;
	CheckDlgButton(IDC_CHECK_AUTO, Enc_Info.b_auto_size);
	
	UpdateData(FALSE);	
}

void CVideoOptionsPage::OnCheckLba() 
{
	UpdateData(TRUE);
	Enc_Info.b_start_at_lba = m_bStartAtLBA;
	if (m_bStartAtLBA)
	{
		GetDlgItem(IDC_EDIT_LBA)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_LBA)->EnableWindow(FALSE);
	}
}

void CVideoOptionsPage::OnCheckFramerange() 
{
	UpdateData(TRUE);
	Enc_Info.b_selectrange = m_bSelectFrameRange;
	if (m_bSelectFrameRange)
	{
		GetDlgItem(IDC_END_FRAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_FRAME)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_END_FRAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_START_FRAME)->EnableWindow(FALSE);
	}
}

BOOL CVideoOptionsPage::OnKillActive() 
{
	UpdateData(TRUE);
	
	GetDialogThings_Info();
	ProcessVars();

	//for safety :
	KillTimer(PREVIEW_TIMER);

	return CPropertyPage::OnKillActive();
}

void CVideoOptionsPage::My_Static_Init()
{
	int i = 0;
	
	m_cmbOutputFormat.ResetContent();
	for (i = 0 ; i < MAXFOR ; i++)
	{
		OutputFormats[i].CX = tmp_a[i].CX;
		OutputFormats[i].CY = tmp_a[i].CY;
		OutputFormats[i].DX = tmp_a[i].DX;
		OutputFormats[i].DY = tmp_a[i].DY;
		OutputFormats[i].ratio = tmp_a[i].ratio;
		OutputFormats[i].str = tmp_a[i].str;
		m_cmbOutputFormat.InsertString(i,OutputFormats[i].str);
	}

	m_cmbAutoSize.ResetContent();
	
	for (i = 0 ; i < MAXAUTOFORMATS ; i++)
	{
		m_cmbAutoSize.InsertString(i,str[i]);
	}

	if (Enc_Info.i_auto_size_width_id < 0)
	{
		Enc_Info.i_auto_size_width_id = 0;
	}

	if (Enc_Info.i_auto_size_width_id > MAXAUTOFORMATS)
	{
		Enc_Info.i_auto_size_width_id = MAXAUTOFORMATS - 1;
	}
	
	CMyToolTips my_tooltips;
	my_tooltips.PrepareTooltips(this, my_page, m_pTooltip);

}

void CVideoOptionsPage::OnVideopreview() 
{
	UpdateData(TRUE);

	if (Prog_Info.b_programs_found == 0)
	{
		AfxMessageBox("Not all required programs where found !");
	}
	else if (Enc_Info.r_ifo_files[Enc_Info.i_selected_ifo_id].i_num_vobfiles > 0)
	{
		COutputFilesPage tmp;
		
		GetDialogThings_Info();
		
		ProcessVars();
		Enc_Info.CreateEncodingLine();
		Enc_Info.PrintEncLine(Prog_Info.b_debug_mode);
		tmp.MPlayer();
	}
}

void CVideoOptionsPage::ProcessVars()
{
	if (Enc_Info.i_video_output_format_id <= 2)
	{
		// hack !
		if (Enc_Info.i_cx < 400)
		{
			Enc_Info.d_ratio = 0.985;
		}
		else if (Enc_Info.i_cx <= 480)
		{
			Enc_Info.d_ratio = 0.990;
		}
		else if (Enc_Info.i_cx <= 544)
		{
			Enc_Info.d_ratio = 0.995;
		}
		else if (Enc_Info.i_cx <= 640)
		{
			Enc_Info.d_ratio = 1.00;
		}
		else if (Enc_Info.i_cx <= 720)
		{
			Enc_Info.d_ratio = 1.01;
		}
	}
}

BOOL CVideoOptionsPage::PreTranslateMessage(MSG* pMsg) 
{
	if (NULL != m_pTooltip) m_pTooltip->RelayEvent(pMsg);	
	
	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CVideoOptionsPage::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (NULL != m_pTooltip) m_pTooltip->Activate(TRUE);		
	CPropertyPage::OnMouseMove(nFlags, point);
}

int CVideoOptionsPage::CheckSavedSettings()
{
	/*
	FILE *fp = NULL;
	int i_len = 0;
	int i = 0;
	int found = 0;
	int i_return = 0;
	CString s = "";
	CString s_tmp = "";
	char list[STRLEN512] = {0};
   
	fp = fopen(M2AFILE, "rt");
	
	if (fp)
	{
		fread( list, sizeof( char ), STRLEN512, fp);
		s = list;
		i_len = strlen(s);
		
		if (i_len > 1)
		{
			found = s.Find("-3X", 40);
			if (found > 0)
			{
				s_tmp = s.Mid(found + 4, 3);
				m_intDX = atoi(s_tmp);
			}
			else
			{
				m_intDX = 720;
			}
			
			found = s.Find("-3Y", 40);
			if (found > 0)
			{
				s_tmp = s.Mid(found + 4, 3);
				m_intDY = atoi(s_tmp);
			}
			
			found = s.Find("-1", 40);
			if (found > 0)
			{
				s_tmp = s.Mid(found + 3, 3);
				m_intCX = atoi(s_tmp);
				s_tmp = s.Mid(found + 1 + 3 + 3, 3);
				m_intCY = atoi(s_tmp);
			}
		
			Enc_Info.i_cx = m_intCX;
			Enc_Info.i_cy = m_intCY;
			Enc_Info.i_dx = m_intDX;
			Enc_Info.i_dy = m_intDY;
			
			if ((Enc_Info.i_cx != OutputFormats[Enc_Info.i_video_output_format_id].CX) ||
				(Enc_Info.i_cy != OutputFormats[Enc_Info.i_video_output_format_id].CY) ||
				(Enc_Info.i_dx != OutputFormats[Enc_Info.i_video_output_format_id].DX) ||
				(Enc_Info.i_dy != OutputFormats[Enc_Info.i_video_output_format_id].DY))
			{
				Enc_Info.i_video_output_format_id = 0;
				m_cmbOutputFormat.SetCurSel(Enc_Info.i_video_output_format_id);
				m_ctrlCX.SetReadOnly(FALSE);
				m_ctrlCY.SetReadOnly(FALSE);
				m_ctrlDX.SetReadOnly(FALSE);
				m_ctrlDY.SetReadOnly(FALSE);
			}
			
			i_return = 1;
			fclose(fp);
			deltempfile();
		}
	}

	return i_return;	*/
	return 1;
}


void CVideoOptionsPage::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == PREVIEW_TIMER)
	{
		i_timer++;
		int result = CheckSavedSettings();

		if (result)
		{
			KillTimer(PREVIEW_TIMER);
			UpdateData(0);
		}

		if (i_timer > 100)
		{
			KillTimer(PREVIEW_TIMER);
		}
	}
}

void CVideoOptionsPage::deltempfile()
{
}

void CVideoOptionsPage::GetDialogThings_Info()
{
	CString tmp_fps;

	Enc_Info.b_selectrange = m_bSelectFrameRange;
	Enc_Info.i_framerate_id = m_cmbFPS.GetCurSel();
	Enc_Info.i_deinterlace = m_cmbDeInterlace.GetCurSel();
	m_cmbFPS.GetLBText(m_cmbFPS.GetCurSel(),tmp_fps);
	Enc_Info.f_selected_framerate = atof(tmp_fps);
	Enc_Info.i_cx = m_intCX;
	Enc_Info.i_cy = m_intCY;
	Enc_Info.i_dx = m_intDX;
	Enc_Info.i_dy = m_intDY;
	Enc_Info.i_start_time = m_intStartFrame;
	Enc_Info.i_end_time = m_intEndFrame;
	Enc_Info.i_start_at_lba = m_intStartAtLBA;
	Enc_Info.b_auto_size = m_bGetAuto;
}

void CVideoOptionsPage::UpdateCheckAuto(int x)
{
	if (x)
	{
		m_ctrlCX.SetReadOnly(1);
		m_ctrlCY.SetReadOnly(1);
		m_ctrlDX.SetReadOnly(1);
		m_ctrlDY.SetReadOnly(1);
		Enc_Info.i_video_output_format_id = 0;
		AutoCalcSize();
		UpdateData(FALSE);
	}
	else
	{
		OnSelchangeComboOutputformat();	
	}

	GetDlgItem(IDC_COMBO_SIZE)->EnableWindow(m_bGetAuto);
	GetDlgItem(IDC_COMBO_OUTPUTFORMAT)->EnableWindow(!m_bGetAuto);
}

void CVideoOptionsPage::OnCheckAuto() 
{
	UpdateData(1);
	
	UpdateCheckAuto(m_bGetAuto);
	Enc_Info.b_auto_size = m_bGetAuto;
}

int CVideoOptionsPage::AutoCalcSize()
{
	CString strtmp;
	int i_new_width = 0;
	int result = -1;

	UpdateData(TRUE);
	Enc_Info.i_auto_size_width_id = m_cmbAutoSize.GetCurSel();	
	m_cmbAutoSize.GetLBText(Enc_Info.i_auto_size_width_id,strtmp);

	i_new_width = atoi(strtmp);

	result = auto_calc_size(i_new_width, 
		Enc_Info.i_original_height, 
		-99,
		Enc_Info.i_cropping_height,
		Enc_Info.f_aspect_ratio,
		&m_intCX, 
		&m_intCY, 
		&m_intDX, 
		&m_intDY);

	if (m_intCX < 0) m_intCX = 0;
	if (m_intCY < 0) m_intCY = 0;
	if (m_intDX < 0) m_intDX = 0;
	if (m_intDY < 0) m_intDY = 0;

	return result;
}

void CVideoOptionsPage::OnSelchangeComboSize() 
{
	AutoCalcSize();

	if (m_bGetAuto)
	{
		UpdateData(0);
	}
}

void CVideoOptionsPage::OnButtonAdv() 
{
	int status = 0;

	CAdvancedVideo_Settings window;

	UpdateData(1);

	status = window.DoModal();

	if (status == IDOK)
	{
		Enc_Info.i_outputmode_id = window.OutputMode;
	}
}

void CVideoOptionsPage::OnCbnSelchangeComboSourceFormat()
{
	UpdateData(TRUE);
	int idx = m_cmbSourceFormat.GetCurSel();

	switch (idx)
	{
	case SF_MOVIE:
		GetDlgItem(IDC_COMBO_FPS)->EnableWindow(0);
		GetDlgItem(IDC_COMBO_DEINTERLACE)->EnableWindow(0);
		
		if (Enc_Info.s_movie_format[0] == 'N')
		{
			m_cmbFPS.SetCurSel(0); // 23fps
			m_cmbDeInterlace.SetCurSel(2); // IVTC
		}
		else
		{
			m_cmbFPS.SetCurSel(2); // 25fps
			m_cmbDeInterlace.SetCurSel(0); // none
		}
		break;
	case SF_VIDEO:
		GetDlgItem(IDC_COMBO_FPS)->EnableWindow(0);
		GetDlgItem(IDC_COMBO_DEINTERLACE)->EnableWindow(0);

		if (Enc_Info.s_movie_format[0] == 'N')
		{
			m_cmbFPS.SetCurSel(3); // 29fps
			m_cmbDeInterlace.SetCurSel(1); // de-int
		}
		else
		{
			m_cmbFPS.SetCurSel(2); // 25fps
			m_cmbDeInterlace.SetCurSel(1); // de-int
		}
		break;
	default:
		GetDlgItem(IDC_COMBO_FPS)->EnableWindow(1);
		GetDlgItem(IDC_COMBO_DEINTERLACE)->EnableWindow(1);
		break;
	}

	Enc_Info.i_source_format_id = idx;
}
