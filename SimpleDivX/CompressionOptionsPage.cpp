// CompressionOptionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "SimpleDivX.h"
#include "CompressionOptionsPage.h"
#include "AudioOptionsPage.h"
#include "LanguageSupport.h"
#include "mytooltips.h"
#include "SimpleDivX_Defines.h"
#include "MyReg.h"
#include "../vob_mpeg2/vstrip/vobinfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DIVX_QUALITY r_quality[NUMQ] = 
{
	{0.00, "very bad quality"},
	{0.075, "very bad quality"},
	{0.10, "bad quality"},
	{0.125, "medium quality"},
	{0.15, "normal quality"},
	{0.17, "good quality"},
	{0.20, "very good quality"},
	{0.25, "near dvd quality"},
	{0.30, "dvd quality"},
	{99.99, "dvd quality"}
};

#define SLOW_MOTION   0
#define NORMAL_MOTION 1
#define FAST_MOTION   2

extern CProgramInfo Prog_Info;
extern CEncodingInfo Enc_Info;
extern CLanguageSupport LanguageModule;

/////////////////////////////////////////////////////////////////////////////
// CCompressionOptionsPage property page

IMPLEMENT_DYNCREATE(CCompressionOptionsPage, CPropertyPage)

CCompressionOptionsPage::CCompressionOptionsPage() : CPropertyPage(CCompressionOptionsPage::IDD)
{
	//{{AFX_DATA_INIT(CCompressionOptionsPage)
	m_intMinBitrate = 0;
	m_hour = 0;
	m_min = 0;
	m_sec = 0;
	m_intFileSize = 0;
	m_lFrames = 0;
	m_intEnterFileSize = -1;
	m_bGetTime = FALSE;
	m_intEndCreditsOption = -1;
	m_intAutoOrManual = -1;
	m_intManualEnd = 0;
	m_intEndCreditsPct = 0;
	//}}AFX_DATA_INIT

	m_pTooltip = NULL;
	m_pTooltip = new CToolTipCtrl;
	m_pTooltip->Create(this);

	my_page = PAGE_COMPRESSION;
}

CCompressionOptionsPage::~CCompressionOptionsPage()
{
	delete m_pTooltip;
}

void CCompressionOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCompressionOptionsPage)
	DDX_Text(pDX, IDC_MINBITRATE, m_intMinBitrate);
	DDX_Text(pDX, IDC_EDIT_HOUR, m_hour);
	DDV_MinMaxInt(pDX, m_hour, 0, 99);
	DDX_Text(pDX, IDC_EDIT_MIN, m_min);
	DDV_MinMaxInt(pDX, m_min, 0, 59);
	DDX_Text(pDX, IDC_EDIT_SEC, m_sec);
	DDV_MinMaxInt(pDX, m_sec, 0, 59);
	DDX_Text(pDX, IDC_EDIT_FileSize, m_intFileSize);
	DDX_Text(pDX, IDC_EDIT_FRAMES, m_lFrames);
	DDV_MinMaxLong(pDX, m_lFrames, 0, 9999999);
	DDX_Radio(pDX, IDC_RADIO20, m_intEnterFileSize);
	DDX_Check(pDX, IDC_GET_TIME, m_bGetTime);
	DDX_Radio(pDX, IDC_RADIO_END1, m_intEndCreditsOption);
	DDX_Radio(pDX, IDC_RADIO_AUTO, m_intAutoOrManual);
	DDX_Text(pDX, IDC_EDIT_ENDCREDITS_FRAMES, m_intManualEnd);
	DDV_MinMaxInt(pDX, m_intManualEnd, 0, 999999);
	DDX_Text(pDX, IDC_EC_BR, m_intEndCreditsPct);
	DDV_MinMaxInt(pDX, m_intEndCreditsPct, 0, 100);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCompressionOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCompressionOptionsPage)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_EN_CHANGE(IDC_EDIT_FRAMES, OnChangeEditFrames)
	ON_EN_CHANGE(IDC_EDIT_HOUR, OnChangeEditHour)
	ON_EN_CHANGE(IDC_EDIT_MIN, OnChangeEditMin)
	ON_EN_CHANGE(IDC_EDIT_SEC, OnChangeEditSec)
	ON_BN_CLICKED(IDC_RADIO20, OnRadio20)
	ON_BN_CLICKED(IDC_RADIO21, OnRadio21)
	ON_EN_CHANGE(IDC_EDIT_FileSize, OnChangeEDITFileSize)
	ON_BN_CLICKED(IDC_GET_TIME, OnGetTime)
	ON_WM_MOUSEMOVE()
	ON_EN_CHANGE(IDC_MINBITRATE, OnChangeMinbitrate)
	ON_BN_CLICKED(IDC_RADIO_END1, OnRadioEnd1)
	ON_BN_CLICKED(IDC_RADIO_END2, OnRadioEnd2)
	ON_BN_CLICKED(IDC_RADIO_END3, OnRadioEnd3)
	ON_BN_CLICKED(IDC_RADIO_AUTO, OnRadioAuto)
	ON_BN_CLICKED(IDC_RADIO_MANUAL, OnRadioManual)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCompressionOptionsPage message handlers

BOOL CCompressionOptionsPage::OnKillActive() 
{
	UpdateData(TRUE);

	Enc_Info.b_gettimefromvob = m_bGetTime;
	Enc_Info.i_filesize = m_intFileSize;
	Enc_Info.i_endcredits_option = m_intEndCreditsOption;
	Enc_Info.i_endcredits_auto_manual = m_intAutoOrManual;
	Enc_Info.i_video_bitrate = m_intMinBitrate;
	
	Enc_Info.l_manual_end = m_intManualEnd;
	Enc_Info.i_end_credits_pct = m_intEndCreditsPct;

	return CPropertyPage::OnKillActive();
}

void CCompressionOptionsPage::My_Init()
{
	m_intEnterFileSize = Enc_Info.b_enter_filesize;
	m_intMinBitrate = Enc_Info.i_video_bitrate;
	m_bGetTime = Enc_Info.b_gettimefromvob;
	
	m_lSeconds = 0;
	m_intFileSize = Enc_Info.i_filesize;
	m_intManualEnd = Enc_Info.l_manual_end;
	m_intEndCreditsPct = Enc_Info.i_end_credits_pct;

	Enc_Info.i_endcredits_option = NORMAL_END;
	
	m_intEndCreditsOption = Enc_Info.i_endcredits_option;
	m_intAutoOrManual = Enc_Info.i_endcredits_auto_manual;

	CheckDlgButton(IDC_GET_TIME, Enc_Info.b_gettimefromvob);

	GetDlgItem(IDC_RADIO_END2)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_END3)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ENDCREDITS_FRAMES)->EnableWindow(FALSE);

	UpDateAuto_Manual();
	
	RefreshWindow();
	UpdateData(FALSE);

	Frames2Seconds();

	if (Enc_Info.b_enter_filesize == 0)
	{
		CalcFileSize();
	}
	else
	{
		Calc();
	}

	LanguageModule.InitAttribs(this,my_page);
}

void CCompressionOptionsPage::RefreshWindow()
{
	if (Enc_Info.b_create_audio)
	{
		GetDlgItem(IDC_STATIC_FINAL)->SetWindowText(LanguageModule.strVideoAndAudio);
	}
	else
	{
		GetDlgItem(IDC_STATIC_FINAL)->SetWindowText(LanguageModule.strVideoOnly);
	}

	GetDlgItem(IDC_GET_TIME)->EnableWindow(!Enc_Info.b_selectrange);

	((CEdit*) GetDlgItem(IDC_EDIT_FileSize))->SetReadOnly(m_intEnterFileSize == 0);
	((CEdit*) GetDlgItem(IDC_MINBITRATE))->SetReadOnly(m_intEnterFileSize != 0);
	((CEdit*) GetDlgItem(IDC_EDIT_FRAMES))->SetReadOnly((m_bGetTime) || (Enc_Info.b_selectrange));
	((CEdit*) GetDlgItem(IDC_EDIT_HOUR))->SetReadOnly((m_bGetTime) || (Enc_Info.b_selectrange));
	((CEdit*) GetDlgItem(IDC_EDIT_MIN))->SetReadOnly((m_bGetTime) || (Enc_Info.b_selectrange));
	((CEdit*) GetDlgItem(IDC_EDIT_SEC))->SetReadOnly((m_bGetTime) || (Enc_Info.b_selectrange));
	
	
	if (Enc_Info.b_selectrange)
	{
		m_lFrames = (long) ((Enc_Info.i_end_time - Enc_Info.i_start_time) * Enc_Info.f_framerate);
		m_lSeconds = Enc_Info.i_end_time - Enc_Info.i_start_time;
	}
	else
	{
		if (Enc_Info.b_gettimefromvob)
		{
			m_lFrames = Enc_Info.l_frames;
		}
		else
		{
			m_lFrames = Enc_Info.l_frames_chosen;
		}

		m_lSeconds = (long) (m_lFrames / Enc_Info.f_framerate);
	}

	Enc_Info.l_seconds_chosen = m_lSeconds;

	m_hour = m_lSeconds / 3600;
	m_min = (-m_hour * 3600 + m_lSeconds) / 60;
	m_sec = (-m_hour * 3600 - m_min * 60 + m_lSeconds);

	//UpdateData(FALSE);
}

void CCompressionOptionsPage::OnRadio2() 
{
	UpdateData(TRUE);

	if (m_intEnterFileSize == 0)
	{
		CalcFileSize();
	}
	else
	{
		Calc();
	}
}

void CCompressionOptionsPage::OnChangeEditFrames() 
{
	Frames2Seconds();

	if (m_intEnterFileSize == 0)
	{
		CalcFileSize();
	}
	else
	{
		Calc();
	}
}

void CCompressionOptionsPage::OnChangeEditHour() 
{
	Seconds2Frames();
	if (m_intEnterFileSize == 0)
	{
		CalcFileSize();
	}
	else
	{
		Calc();
	}
}

void CCompressionOptionsPage::OnChangeEditMin() 
{
	Seconds2Frames();
	if (m_intEnterFileSize == 0)
	{
		CalcFileSize();
	}
	else
	{
		Calc();
	}
}

void CCompressionOptionsPage::OnChangeEditSec() 
{
	Seconds2Frames();
	if (m_intEnterFileSize == 0)
	{
		CalcFileSize();
	}
	else
	{
		Calc();
	}
}

void CCompressionOptionsPage::OnRadio20() 
{
	UpdateData(TRUE);
	Enc_Info.b_enter_filesize = m_intEnterFileSize;
	RefreshWindow();
}

void CCompressionOptionsPage::OnRadio21() 
{
	UpdateData(TRUE);
	Enc_Info.b_enter_filesize = m_intEnterFileSize;
	RefreshWindow();
}

void CCompressionOptionsPage::Calc()
{
	long system_bitrate = 0;
	long video_bitrate = 0;
	long audio_bitrate = 0;
	int kilo = 1000;
	double extra_audio_mux_overhead = 0;

	if (Enc_Info.b_gettimefromvob && !Enc_Info.b_selectrange)
	{
		m_lFrames = Enc_Info.l_frames;
		m_lSeconds = (long) (m_lFrames / Enc_Info.f_framerate);
	}

	if (m_lSeconds != 0)
	{
		if (Enc_Info.b_create_audio)
		{
			if (Enc_Info.i_audio_format == AUDIO_TYPE_AC3)
			{
				extra_audio_mux_overhead = m_lSeconds * 0.04292 / 60.0;
			}
			else
			{
				extra_audio_mux_overhead = m_lSeconds * 0.06866 / 60.0;
			}
		}

		audio_bitrate = Enc_Info.b_create_audio * Enc_Info.i_audio_kbps;
		system_bitrate = (int) ((((1.0 * m_intFileSize - extra_audio_mux_overhead) * MEGA * 8.0) / m_lSeconds) / kilo);
		video_bitrate = system_bitrate - audio_bitrate;
		m_intMinBitrate = video_bitrate;
	}

	int max = MAXDIVX5KBPS;
	
	if (m_intMinBitrate > max)
	{
		m_intMinBitrate = max - 1;
	}
	
	DisplayQuality(m_intMinBitrate);

	UpdateData(FALSE);
}

void CCompressionOptionsPage::OnChangeEDITFileSize() 
{
	UpdateData(TRUE);
	Calc();
}

void CCompressionOptionsPage::OnGetTime() 
{
	if (!Enc_Info.b_selectrange)
	{
		UpdateData(TRUE);
		
		Enc_Info.b_gettimefromvob = m_bGetTime;
		if (m_bGetTime)
		{
			m_lFrames = Enc_Info.l_frames;
			m_lSeconds = (long) (m_lFrames / Enc_Info.f_framerate);
		}
		
		m_hour = m_lSeconds / 3600;
		m_min = (-m_hour * 3600 + m_lSeconds) / 60;
		m_sec = (-m_hour * 3600 - m_min * 60 + m_lSeconds);
		
		RefreshWindow();
		
		if (Enc_Info.b_enter_filesize == 1)
		{
			Calc();
		}
		else
		{
			CalcFileSize();
		}
	}
}

void CCompressionOptionsPage::Seconds2Frames()
{
	if (!Enc_Info.b_selectrange)
	{
		UpdateData(TRUE);
		m_lSeconds = m_hour * 3600 + m_min * 60 + m_sec;
		m_lFrames = (long) (m_lSeconds * Enc_Info.f_framerate);
		UpdateData(FALSE);
	}
}

void CCompressionOptionsPage::Frames2Seconds()
{
	if (!Enc_Info.b_selectrange)
	{
		UpdateData(TRUE);
		m_lSeconds = (long) (m_lFrames / Enc_Info.f_framerate);
		m_hour = m_lSeconds / 3600;
		m_min = (-m_hour * 3600 + m_lSeconds) / 60;
		m_sec = (-m_hour * 3600 - m_min * 60 + m_lSeconds);
		UpdateData(FALSE);
	}
}

void CCompressionOptionsPage::MyStaticInit()
{
	CMyToolTips my_tooltips;
	my_tooltips.PrepareTooltips(this, my_page, 
		m_pTooltip);
}

void CCompressionOptionsPage::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (NULL != m_pTooltip) m_pTooltip->Activate(TRUE);	
	
	CPropertyPage::OnMouseMove(nFlags, point);
}

BOOL CCompressionOptionsPage::PreTranslateMessage(MSG* pMsg) 
{
	if (NULL != m_pTooltip) m_pTooltip->RelayEvent(pMsg);
	
	return CPropertyPage::PreTranslateMessage(pMsg);
}

BOOL CCompressionOptionsPage::OnSetActive() 
{
	if (Prog_Info.b_init_compressionpage == 0)
	{
		MyStaticInit();
		My_Init();
		Prog_Info.b_init_compressionpage = 1;
	}
	else
	{
		My_Init();
	}
	
	return CPropertyPage::OnSetActive();
}

void CCompressionOptionsPage::OnChangeMinbitrate() 
{
	UpdateData(TRUE);
	CalcFileSize();
}

void CCompressionOptionsPage::CalcFileSize()
{
	int audio_bitrate = 0;
	int video_bitrate = 0;
	int kilo = 1000;

	DisplayQuality(m_intMinBitrate);

	audio_bitrate = Enc_Info.b_create_audio * Enc_Info.i_audio_kbps;

	long tmp = (long) (0.50 + (((audio_bitrate + m_intMinBitrate) * 1.0 * m_lSeconds * kilo) / (8.0 * MEGA)));
	m_intFileSize = (int) tmp;
	UpdateData(FALSE);
}

void CCompressionOptionsPage::DisplayQuality(int bitrate_)
{
	int bitrate = bitrate_;
	double f_bits_per_pixel = (KILO * bitrate) / (Enc_Info.i_cx * Enc_Info.i_cy * Enc_Info.f_framerate);
	
	for (int i = 1 ; i < NUMQ ; i++)
	{
		if (f_bits_per_pixel <= r_quality[i].f_bits_per_pixel)
		{
			char s_tmp[32] = "";
			sprintf(s_tmp, "[ %s ]", r_quality[i-1].s_q);
			GetDlgItem(IDC_STATIC_Q)->SetWindowText(s_tmp);
			break;
		}
	}
}

void CCompressionOptionsPage::OnRadioEnd1() 
{
	UpdateData(1);
	UpDateAuto_Manual();
}

void CCompressionOptionsPage::OnRadioEnd2() 
{
	UpdateData(1);
	UpDateAuto_Manual();
}

void CCompressionOptionsPage::OnRadioEnd3() 
{
	UpdateData(1);
	UpDateAuto_Manual();
}

void CCompressionOptionsPage::OnRadioAuto() 
{
	UpdateData(1);
	GetDlgItem(IDC_EDIT_ENDCREDITS_FRAMES)->EnableWindow(m_intAutoOrManual);
}

void CCompressionOptionsPage::OnRadioManual() 
{
	UpdateData(1);
	GetDlgItem(IDC_EDIT_ENDCREDITS_FRAMES)->EnableWindow(m_intAutoOrManual);
}

void CCompressionOptionsPage::UpDateAuto_Manual()
{
	GetDlgItem(IDC_RADIO_AUTO)->EnableWindow(m_intEndCreditsOption);
	GetDlgItem(IDC_RADIO_MANUAL)->EnableWindow(m_intEndCreditsOption);
	GetDlgItem(IDC_STATIC_FRAMES)->EnableWindow(m_intEndCreditsOption);
	GetDlgItem(IDC_STATIC_KBPS)->EnableWindow(m_intEndCreditsOption);
	
	GetDlgItem(IDC_EDIT_ENDCREDITS_FRAMES)->EnableWindow(m_intAutoOrManual * m_intEndCreditsOption);
	GetDlgItem(IDC_STATIC_RECOMPRESS_PCT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EC_BR)->EnableWindow(FALSE);
	
	if (m_intEndCreditsOption == 2)
	{
		GetDlgItem(IDC_EC_BR)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC_RECOMPRESS_PCT)->EnableWindow(m_intEndCreditsOption);
	}
}
