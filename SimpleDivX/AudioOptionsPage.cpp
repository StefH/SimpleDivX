// AudioOptionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "SimpleDivX.h"
#include "AudioOptionsPage.h"
#include "mytooltips.h"
#include "SimpleDivX_Defines.h"
#include "LanguageSupport.h"
#include "EncodingInfo.h"
#include "ProgramInfo.h"
#include "../vob_mpeg2/vstrip/vobinfo.h"
#include "../vob_mpeg2/vstrip/vstrip.h"
#include ".\audiooptionspage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CProgramInfo Prog_Info;
extern CEncodingInfo Enc_Info;
extern CLanguageSupport LanguageModule;

int ai_audio_kbps[MAXAFOR] = {64,80,96,112,128,160,192,224,256,320,384,448,512,576,640};

/////////////////////////////////////////////////////////////////////////////
// CAudioOptionsPage property page

IMPLEMENT_DYNCREATE(CAudioOptionsPage, CPropertyPage)

CAudioOptionsPage::CAudioOptionsPage() : CPropertyPage(CAudioOptionsPage::IDD)
{
	//{{AFX_DATA_INIT(CAudioOptionsPage)
	m_bEnableAudioIDCheck = FALSE;
	m_intHertz = -1;
	m_bCreateAudio = FALSE;
	m_bAmplifyWave = FALSE;
	m_bNorm = FALSE;
	m_bNorm2 = FALSE;
	m_bExtractAllAudio = FALSE;
	m_intAudioFormat = -1;
	m_bConvertAllAudio = FALSE;
	m_bConvertAC3_to_2_Channels = -1;
	m_bVBR = FALSE;
	m_strDelay = _T("");
	//}}AFX_DATA_INIT

	m_pTooltip = NULL;
	m_pTooltip = new CToolTipCtrl;
	m_pTooltip->Create(this);

	my_page = PAGE_AUDIO;
}

CAudioOptionsPage::~CAudioOptionsPage()
{
	delete m_pTooltip;
}

void CAudioOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAudioOptionsPage)
	DDX_Control(pDX, IDC_SPIN_AMPLIFY, m_spinAmplify);
	DDX_Control(pDX, IDC_COMBO_AUDIO_STREAMID, m_cmbAudioStreamID);
	DDX_Control(pDX, IDC_COMBO_AUDIO, m_cmbAudioCompression);
	DDX_Check(pDX, IDC_CHECK_AUDIO, m_bEnableAudioIDCheck);
	DDX_Radio(pDX, IDC_RADIO1, m_intHertz);
	DDX_Check(pDX, IDC_CREATE_AUDIO, m_bCreateAudio);
	DDX_Check(pDX, IDC_CHECK_WAVEUP, m_bAmplifyWave);
	DDX_Check(pDX, IDC_CHECK_NORM, m_bNorm);
	DDX_Check(pDX, IDC_CHECK_NORM2, m_bNorm2);
	DDX_Radio(pDX, IDC_RADIO_MP3, m_intAudioFormat);
	DDX_Radio(pDX, IDC_RADIO_NORECOMPRESSAC3, m_bConvertAC3_to_2_Channels);
	DDX_Check(pDX, IDC_CHECK_VBR, m_bVBR);
	DDX_Text(pDX, IDC_STATIC_DELAY, m_strDelay);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAudioOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CAudioOptionsPage)
	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO, OnSelchangeComboAudio)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_CREATE_AUDIO, OnCreateAudio)
	ON_BN_CLICKED(IDC_CHECK_AUDIO, OnCheckAudio)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_WM_MOUSEMOVE()
	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_STREAMID, OnSelchangeComboAudioStreamid)
	ON_BN_CLICKED(IDC_CHECK_NORM, OnCheckNorm)
	ON_BN_CLICKED(IDC_CHECK_WAVEUP, OnCheckWaveup)
	ON_BN_CLICKED(IDC_CHECK_NORM2, OnCheckNorm2)
	ON_BN_CLICKED(IDC_RADIO_AC3, OnRadioAc3)
	ON_BN_CLICKED(IDC_RADIO_MP3, OnRadioMp3)
	
	ON_BN_CLICKED(IDC_RADIO_OGG, OnRadioOgg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_AMPLIFY, OnDeltaposSpinAmplify)
	ON_BN_CLICKED(IDC_CHECK_VBR, OnCheckVbr)
	//}}AFX_MSG_MAP
//	ON_WM_MOUSEWHEEL()
ON_BN_CLICKED(IDC_RADIO_RECOMPRESSAC3, OnBnClickedRadioRecompressac3)
ON_BN_CLICKED(IDC_RADIO_NORECOMPRESSAC3, OnBnClickedRadioNorecompressac3)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioOptionsPage message handlers

void CAudioOptionsPage::My_Init()
{
	int i_num = 0;
	int id = 0;

	m_bNorm = false;
	m_bNorm = false;
	
	m_bEnableAudioIDCheck = Enc_Info.b_enable_audiocheck;
	m_bAmplifyWave = Enc_Info.b_amplify_audio;
	m_bCreateAudio = Enc_Info.b_create_audio;
	m_intAmplify = Enc_Info.i_audio_amplify;
	m_bVBR = Enc_Info.b_audio_vbr;
	if (m_intAmplify > MAXDB) m_intAmplify = MAXDB;
	if (m_intAmplify < -MAXDB) m_intAmplify = -MAXDB;
	m_spinAmplify.SetPos(m_intAmplify);
	
	if (Enc_Info.i_norm_wave == 1) m_bNorm = 1;
	if (Enc_Info.i_norm_wave == 2) m_bNorm2 = 1;
	
	CheckDlgButton(IDC_CHECK_VBR, m_bVBR);
	CheckDlgButton(IDC_CHECK_WAVEUP, m_bAmplifyWave);
	CheckDlgButton(IDC_CREATE_AUDIO, m_bCreateAudio);
		
	CheckDlgButton(IDC_CHECK_NORM, m_bNorm);
	CheckDlgButton(IDC_CHECK_NORM2, m_bNorm2);
	CheckDlgButton(IDC_CHECK_AUDIO, m_bEnableAudioIDCheck);
		
	int temp_radio = 0;
	if (Enc_Info.i_hertz_id == 0)
	{
		temp_radio = IDC_RADIO1;
		Enc_Info.i_hertz = 48000;
	}
	else
	{
		temp_radio = IDC_RADIO2;
		Enc_Info.i_hertz = 44100;
	}

	CheckRadioButton(IDC_RADIO1, IDC_RADIO2, temp_radio);   
	
	UpdateData(1);

	m_intAudioFormat = 0;

	ReverseLookupAudioFormat();

	m_bConvertAC3_to_2_Channels = Enc_Info.b_convert_ac3_to_2channels;

	UpdateData(0);

	OnCreateAudio();
	OnCheckAudio();
	OnSelchangeComboAudio();
	

	UpdateSpin();

	UpdateData(0);

	GetDlgItem(IDC_EDIT_AMPLIFY)->EnableWindow(m_bAmplifyWave);
	GetDlgItem(IDC_SPIN_AMPLIFY)->EnableWindow(m_bAmplifyWave);

	LanguageModule.InitAttribs(this, my_page);
}

BOOL CAudioOptionsPage::OnSetActive() 
{
	if (Prog_Info.b_init_audiooptionspage == 0)
	{
		My_Static_Init();
		Prog_Info.b_init_audiooptionspage = 1;
	}

	My_Init();
		
	return CPropertyPage::OnSetActive();
}

void CAudioOptionsPage::OnSelchangeComboAudio() 
{
	CString strtmp;
	UpdateData(TRUE);

	int idx = m_cmbAudioCompression.GetCurSel();	
	m_cmbAudioCompression.GetLBText(idx,strtmp);

	Enc_Info.i_audio_kbps = atoi(strtmp);
}

void CAudioOptionsPage::OnRadio1() 
{
	Enc_Info.i_hertz = 48000;
	
	Enc_Info.i_hertz_id = 0;
}

BOOL CAudioOptionsPage::OnKillActive() 
{
	UpdateData(TRUE);
	
	Enc_Info.b_create_audio = m_bCreateAudio;
	Enc_Info.i_audio_amplify = m_intAmplify;
	Enc_Info.b_amplify_audio = m_bAmplifyWave;
	Enc_Info.i_norm_wave = m_bNorm + 2 * m_bNorm2;
	Enc_Info.b_audio_vbr = m_bVBR;
 
	ProcessVars();
/*
	if (!Enc_Info.b_create_audio)
	{
		Enc_Info.r_valid_container[CF_AVI] = true;
		Enc_Info.r_valid_container[CF_OGM] = true;
		Enc_Info.r_valid_container[CF_MATROSKA] = true;
	}
	else
	{
		if (Enc_Info.i_audio_format == AUDIO_TYPE_OGG)
		{
			Enc_Info.r_valid_container[CF_AVI] = false;
			Enc_Info.r_valid_container[CF_OGM] = true;
			Enc_Info.r_valid_container[CF_MATROSKA] = true;
			// AVI -> OGM, but MKV is ok
			if (Enc_Info.i_container_format == CF_AVI)
			{
				Enc_Info.i_container_format = CF_OGM;
			}
		}
		else
		{
			Enc_Info.r_valid_container[CF_AVI] = true;
			Enc_Info.r_valid_container[CF_OGM] = false;
			Enc_Info.r_valid_container[CF_MATROSKA] = true;

			if (Enc_Info.i_container_format == CF_OGM)
			{
				Enc_Info.i_container_format = CF_AVI;
			}
		}
	}
	*/

	Enc_Info.r_valid_container[CF_AVI] = true;
	Enc_Info.r_valid_container[CF_OGM] = true;
	Enc_Info.r_valid_container[CF_MATROSKA] = true;
	Enc_Info.r_valid_container[CF_MP4] = false;

	//Enc_Info.i_container_format = CF_AVI;

	return CPropertyPage::OnKillActive();
}

void CAudioOptionsPage::OnCreateAudio() 
{
	UpdateData(TRUE);

	GetDlgItem(IDC_CHECK_AUDIO)->EnableWindow(m_bCreateAudio);
	GetDlgItem(IDC_COMBO_AUDIO_STREAMID)->EnableWindow(m_bCreateAudio);
	GetDlgItem(IDC_COMBO_AUDIO)->EnableWindow(m_bCreateAudio);
	GetDlgItem(IDC_RADIO1)->EnableWindow(m_bCreateAudio);
	GetDlgItem(IDC_RADIO2)->EnableWindow(m_bCreateAudio);
	GetDlgItem(IDC_RADIO_AC3)->EnableWindow(m_bCreateAudio);
	GetDlgItem(IDC_RADIO_MP3)->EnableWindow(m_bCreateAudio);
	GetDlgItem(IDC_STATIC_AUDIO_KBPS)->EnableWindow(m_bCreateAudio);
	
	
	GetDlgItem(IDC_STATIC_CONV)->EnableWindow(m_bCreateAudio);
	GetDlgItem(IDC_STATIC_AUDIOSTRID)->EnableWindow(m_bCreateAudio);
	GetDlgItem(IDC_RADIO_OGG)->EnableWindow(m_bCreateAudio);
	GetDlgItem(IDC_CHECK_WAVEUP)->EnableWindow(m_bCreateAudio);
	GetDlgItem(IDC_CHECK_NORM)->EnableWindow(m_bCreateAudio);
	GetDlgItem(IDC_CHECK_NORM2)->EnableWindow(m_bCreateAudio);
	GetDlgItem(IDC_RADIO_NORECOMPRESSAC3)->EnableWindow(m_bCreateAudio);
	GetDlgItem(IDC_RADIO_RECOMPRESSAC3)->EnableWindow(m_bCreateAudio);
		
	if (m_bCreateAudio == 0)
	{
		m_bAmplifyWave = 0;
	}
	
	UpdateAc3_Mp3(i_radio_id[m_intAudioFormat], b_display_ac3_kbps_from_detection);
}

/* van deze functie afblijven...*/
void CAudioOptionsPage::RefreshWindow()
{
	int i = 0;
	int j = 0;
	CString tmp;
		
	m_cmbAudioStreamID.ResetContent();
	
	if (!m_bEnableAudioIDCheck)
	{
		// Audio check disabled.
		for (i = SUBSTREAM_AC3 ; i <= MAX_SUBSTREAM_AC3 ; i++)
		{	
			Enc_Info.as_audio_languages[j].Format("Language %d", i - SUBSTREAM_AC3);
			ai_language_id[j] = i;
			//Enc_Info.ai_language_type[j] = AUDIO_TYPE_AC3; not needed yet
			tmp.Format("0x%X - %s [ ? channels] %s - ? kbps", 
				i,
				Enc_Info.as_audio_languages[j],
				STR_AUDIO_AC3);
			m_cmbAudioStreamID.InsertString(j, tmp);
			j++;
		}
		for (i = SUBSTREAM_PCM ; i <= MAX_SUBSTREAM_PCM ; i++)
		{	
			Enc_Info.as_audio_languages[j].Format("Language %d", i - SUBSTREAM_PCM);
			ai_language_id[j] = i;
			//Enc_Info.ai_language_type[j] = AUDIO_TYPE_LPCM; not needed yet
			tmp.Format("0x%X - %s [ ? channels] %s - ? kbps", 
				i,
				Enc_Info.as_audio_languages[j],
				STR_AUDIO_PCM);
			m_cmbAudioStreamID.InsertString(j, tmp);
			j++;
		}
		for (i = AUDIO_STREAM ; i <= MAX_AUDIO_STREAM ; i++)
		{	
			Enc_Info.as_audio_languages[j].Format("Language %d", i - AUDIO_STREAM);
			ai_language_id[j] = i;
			//Enc_Info.ai_language_type[j] = AUDIO_TYPE_MP2; not needed yet
			tmp.Format("0x%X - %s [ ? channels] %s - ? kbps", 
				i,
				Enc_Info.as_audio_languages[j],
				STR_AUDIO_MP2);
			m_cmbAudioStreamID.InsertString(j, tmp);
			j++;
		}

		//m_cmbAudioStreamID.SetCurSel(0);
		//Enc_Info.i_audio_stream_id = 0;
	}
	else
	{
		if (Enc_Info.i_audio_stream_id >= Enc_Info.i_valid_languages)
		{
			Enc_Info.i_audio_stream_id = 0;
		}
		
		if (Enc_Info.i_valid_languages > 0)
		{
			UpdateAudioInfo(1);
		}
		else
		{
			m_cmbAudioStreamID.InsertString(0, LanguageModule.strNoAudioFound);
		}
	}
	
	m_cmbAudioStreamID.SetCurSel(Enc_Info.i_audio_stream_id);
}


void CAudioOptionsPage::OnCheckAudio() 
{
	UpdateData(TRUE);
	Enc_Info.b_enable_audiocheck = m_bEnableAudioIDCheck;
	RefreshWindow();
}

void CAudioOptionsPage::OnRadio2() 
{
	Enc_Info.i_hertz = 44100;
	Enc_Info.i_hertz_id = 1;
}

void CAudioOptionsPage::My_Static_Init()
{
	// mp3, ac3, ogg
	i_min_kbps[AUDIO_TYPE_MP3] = 64;
	i_min_kbps[AUDIO_TYPE_OGG] = 64;
	i_min_kbps[AUDIO_TYPE_AC3] = 128;

	i_max_kbps[AUDIO_TYPE_MP3] = 320;
	i_max_kbps[AUDIO_TYPE_OGG] = 512;
	i_max_kbps[AUDIO_TYPE_AC3] = 640;

	i_radio_id[0] = AUDIO_TYPE_MP3;
	i_radio_id[1] = AUDIO_TYPE_AC3;
	i_radio_id[2] = AUDIO_TYPE_OGG;
	
	m_spinAmplify.SetRange32(0, 20);
	m_spinAmplify.SetBase(10);
	
	CMyToolTips my_tooltips;
	my_tooltips.PrepareTooltips(this, my_page, 
		m_pTooltip);
}

void CAudioOptionsPage::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (NULL != m_pTooltip) m_pTooltip->Activate(TRUE);	
	
	CPropertyPage::OnMouseMove(nFlags, point);
}

BOOL CAudioOptionsPage::PreTranslateMessage(MSG* pMsg) 
{
	if (NULL != m_pTooltip) m_pTooltip->RelayEvent(pMsg);	

	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CAudioOptionsPage::OnSelchangeComboAudioStreamid() 
{
	UpdateData(TRUE);
	b_display_ac3_kbps_from_detection = true;
	Enc_Info.i_audio_stream_id = m_cmbAudioStreamID.GetCurSel();
	UpdateAc3_Mp3(i_radio_id[m_intAudioFormat], b_display_ac3_kbps_from_detection);
}

void CAudioOptionsPage::OnCheckNorm() 
{
	UpdateData(1);
	CheckDlgButton(IDC_CHECK_WAVEUP, 0);
	CheckDlgButton(IDC_CHECK_NORM2, 0);

	GetDlgItem(IDC_EDIT_AMPLIFY)->EnableWindow(0);
	GetDlgItem(IDC_SPIN_AMPLIFY)->EnableWindow(0);
}

void CAudioOptionsPage::OnCheckWaveup() 
{
	UpdateData(1);
	CheckDlgButton(IDC_CHECK_NORM, 0);
	CheckDlgButton(IDC_CHECK_NORM2, 0);

	GetDlgItem(IDC_EDIT_AMPLIFY)->EnableWindow(m_bAmplifyWave);
	GetDlgItem(IDC_SPIN_AMPLIFY)->EnableWindow(m_bAmplifyWave);
}

void CAudioOptionsPage::OnCheckNorm2() 
{
	UpdateData(1);
	CheckDlgButton(IDC_CHECK_WAVEUP, 0);
	CheckDlgButton(IDC_CHECK_NORM, 0);

	GetDlgItem(IDC_EDIT_AMPLIFY)->EnableWindow(0);
	GetDlgItem(IDC_SPIN_AMPLIFY)->EnableWindow(0);
}

void CAudioOptionsPage::OnRadioOgg() 
{
	UpdateData(TRUE);
	UpdateAc3_Mp3(i_radio_id[m_intAudioFormat], b_display_ac3_kbps_from_detection);
	UpdateData(FALSE);
}

void CAudioOptionsPage::OnRadioAc3() 
{
	UpdateData(TRUE);
	UpdateAc3_Mp3(i_radio_id[m_intAudioFormat], b_display_ac3_kbps_from_detection);
	UpdateData(FALSE);
}

void CAudioOptionsPage::OnRadioMp3() 
{
	UpdateData(TRUE);
	UpdateAc3_Mp3(i_radio_id[m_intAudioFormat], b_display_ac3_kbps_from_detection);
	UpdateData(FALSE);
}

void CAudioOptionsPage::UpdateAc3_Mp3(int option, int update_ac3_from_detection)
{
	CString strtmp;
	CString s_audio_kbps;
	Enc_Info.i_audio_format = option;
	
	int i_current_id = 0;
	int i_current_kbps = 0;
	int i_idx = 0;
	bool b_changed_it = false;

	/*
	if (Enc_Info.ai_language_type[Enc_Info.i_audio_stream_id] != AUDIO_TYPE_AC3)
	{
		b_changed_it = true;
		Enc_Info.i_audio_format = AUDIO_TYPE_MP3;
		GetDlgItem(IDC_RADIO_AC3)->EnableWindow(0);
	}
	else
	{
		GetDlgItem(IDC_RADIO_AC3)->EnableWindow(m_bCreateAudio);
	}*/

	if (Prog_Info.b_can_use_ogg == false)
	{
		b_changed_it = true;
		if (Enc_Info.i_audio_format == AUDIO_TYPE_OGG)
		{
			Enc_Info.i_audio_format = AUDIO_TYPE_MP3;
		}
		GetDlgItem(IDC_RADIO_OGG)->EnableWindow(0);
	}

	if (b_display_ac3_kbps_from_detection)
	{
		Enc_Info.i_audio_kbps = Enc_Info.ar_ac3_info[Enc_Info.i_audio_stream_id].i_kbps;

		

		if (Enc_Info.ar_ac3_info[Enc_Info.i_audio_stream_id].i_num_channels == 2)
		{
			m_bConvertAC3_to_2_Channels = 1;
			Enc_Info.b_convert_ac3_to_2channels = 1;
		}
		else
		{
			m_bConvertAC3_to_2_Channels = 0;
			Enc_Info.b_convert_ac3_to_2channels = 0;
		}
	}

	b_display_ac3_kbps_from_detection = false;
	
	i_current_kbps = Enc_Info.i_audio_kbps;
	
	i_current_id = m_cmbAudioCompression.GetCurSel();
	
	if (i_current_id != -1)
	{
	}
	else
	{
		i_current_id = 2;
	}
	
	m_cmbAudioCompression.ResetContent();
	
	if (i_current_kbps > i_max_kbps[Enc_Info.i_audio_format])
	{
		i_current_kbps = i_max_kbps[Enc_Info.i_audio_format];
	}
	
	if (i_current_kbps < i_min_kbps[Enc_Info.i_audio_format])
	{
		i_current_kbps = i_min_kbps[Enc_Info.i_audio_format];
	}
	
	for (int i = 0 ; i < MAXAFOR ; i++)
	{
		if ((ai_audio_kbps[i] >= i_min_kbps[Enc_Info.i_audio_format]) && 
			(ai_audio_kbps[i] <= i_max_kbps[Enc_Info.i_audio_format]))
		{
			s_audio_kbps.Format("%d", ai_audio_kbps[i]);
			m_cmbAudioCompression.InsertString(i_idx,s_audio_kbps);
			if (ai_audio_kbps[i] == i_current_kbps)
			{
				i_current_id = i_idx;
			}
			i_idx++;
		}
	}
	
	Enc_Info.i_audio_kbps = i_current_kbps;
	m_cmbAudioCompression.SetCurSel(i_current_id);

	if ((Enc_Info.i_audio_format == AUDIO_TYPE_MP3) || (Enc_Info.i_audio_format == AUDIO_TYPE_OGG))
	{
		GetDlgItem(IDC_RADIO1)->EnableWindow(m_bCreateAudio);
		
		// mp3 or ogg
		GetDlgItem(IDC_RADIO2)->EnableWindow(m_bCreateAudio);
		
		GetDlgItem(IDC_CHECK_WAVEUP)->EnableWindow(m_bCreateAudio);
		GetDlgItem(IDC_CHECK_NORM)->EnableWindow(m_bCreateAudio);
		GetDlgItem(IDC_CHECK_NORM2)->EnableWindow(m_bCreateAudio);
		GetDlgItem(IDC_EDIT_AMPLIFY)->EnableWindow(m_bCreateAudio);
		GetDlgItem(IDC_SPIN_AMPLIFY)->EnableWindow(m_bCreateAudio);
		
		GetDlgItem(IDC_RADIO_RECOMPRESSAC3)->EnableWindow(0);
		GetDlgItem(IDC_RADIO_NORECOMPRESSAC3)->EnableWindow(0);
	
		if (Enc_Info.i_audio_format == AUDIO_TYPE_MP3)
		{
			GetDlgItem(IDC_STATIC_MP3_CHANNELS)->EnableWindow(m_bCreateAudio);
			GetDlgItem(IDC_STATIC_OGG_CHANNELS)->EnableWindow(0);
		}
		else
		{
			GetDlgItem(IDC_STATIC_MP3_CHANNELS)->EnableWindow(0);
			GetDlgItem(IDC_STATIC_OGG_CHANNELS)->EnableWindow(m_bCreateAudio);
			GetDlgItem(IDC_CHECK_VBR)->EnableWindow(0);
		}
	}
	else if (Enc_Info.i_audio_format == AUDIO_TYPE_AC3)
	{
		// ac3
		Enc_Info.i_hertz = 48000;
		m_intHertz = 0;
		GetDlgItem(IDC_RADIO1)->EnableWindow(m_bCreateAudio);
		
		GetDlgItem(IDC_RADIO_RECOMPRESSAC3)->EnableWindow(m_bCreateAudio);
		GetDlgItem(IDC_RADIO2)->EnableWindow(0);
		GetDlgItem(IDC_CHECK_WAVEUP)->EnableWindow(0);
		GetDlgItem(IDC_CHECK_NORM)->EnableWindow(0);
		GetDlgItem(IDC_CHECK_NORM2)->EnableWindow(0);
		GetDlgItem(IDC_EDIT_AMPLIFY)->EnableWindow(0);
		GetDlgItem(IDC_SPIN_AMPLIFY)->EnableWindow(0);
		GetDlgItem(IDC_CHECK_VBR)->EnableWindow(0);
			
		GetDlgItem(IDC_STATIC_MP3_CHANNELS)->EnableWindow(0);
		GetDlgItem(IDC_STATIC_OGG_CHANNELS)->EnableWindow(0);
		
		if (Enc_Info.i_valid_languages > 0)
		{	
			if (Enc_Info.ar_ac3_info[Enc_Info.i_audio_stream_id].i_num_channels == 2)
			{
				GetDlgItem(IDC_RADIO_NORECOMPRESSAC3)->EnableWindow(0);
				Enc_Info.b_convert_ac3_to_2channels = 0;
				m_bConvertAC3_to_2_Channels = 1;
				UpdateData(0);
			}
			else
			{
				m_bConvertAC3_to_2_Channels = Enc_Info.b_convert_ac3_to_2channels; // was 0
				GetDlgItem(IDC_RADIO_NORECOMPRESSAC3)->EnableWindow(m_bCreateAudio);
				UpdateData(0);
			}
		}

		UpdateData(0);
	}

	if (b_changed_it)
	{
		ReverseLookupAudioFormat();
		UpdateData(0);
	}

	//sprintf(s_tmp, "A/V delay = %ld ms.", (long) Enc_Info.af_audio_delay[Enc_Info.i_audio_stream_id]);
	//GetDlgItem(IDC_STATIC_DELAY)->SetWindowText(s_tmp);
}

void CAudioOptionsPage::ProcessVars()
{
	if (Enc_Info.b_create_audio)
	{
		if (m_bEnableAudioIDCheck)
		{
			Enc_Info.i_selected_audio_id = Enc_Info.ai_language_id[Enc_Info.i_audio_stream_id];
		}
		else
		{
			Enc_Info.i_selected_audio_id = ai_language_id[Enc_Info.i_audio_stream_id];
		}
	}
}

/* van deze functie afblijven... */
void CAudioOptionsPage::UpdateAudioInfo(int x)
{
	if (Enc_Info.b_enable_audiocheck) 
	{
		CString sTmp;
		int valid = 0;

		for (int i = 0 ; i < Enc_Info.i_valid_languages ; i++)
		{	
			if (Enc_Info.r_ifo_files[Enc_Info.i_selected_ifo_id].b_valid_ifo)
			{
				sTmp.Format("0x%X - %s - %d kbps", 
						Enc_Info.ai_language_id[i],
						Enc_Info.as_audio_languages[i],
						Enc_Info.ar_ac3_info[i].i_kbps);
			}
			else
			{
				Enc_Info.as_audio_languages[i].Format("Language %d", i);

				CString stmp;

				if (Enc_Info.ai_language_type[i] == AUDIO_TYPE_AC3)
				{
					stmp = STR_AUDIO_AC3;
				}
				else if (Enc_Info.ai_language_type[i] == AUDIO_TYPE_LPCM)
				{
					stmp = STR_AUDIO_PCM;
				}
				else if (Enc_Info.ai_language_type[i] == AUDIO_TYPE_MP2)
				{
					stmp = STR_AUDIO_MP2;
				}
				sTmp.Format("0x%X - %s [%d channels] %s - %d kbps", 
					Enc_Info.ai_language_id[i],
					Enc_Info.as_audio_languages[i],
					Enc_Info.ar_ac3_info[i].i_num_channels,
					stmp,
					Enc_Info.ar_ac3_info[i].i_kbps);
			}

			m_cmbAudioStreamID.InsertString(i, sTmp);;
		}
	}
}

void CAudioOptionsPage::OnDeltaposSpinAmplify(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	m_intAmplify += pNMUpDown->iDelta;

	if (m_intAmplify > MAXDB) m_intAmplify = MAXDB;
	if (m_intAmplify < 0) m_intAmplify = 0;

	UpdateSpin();

	*pResult = 0;
}

void CAudioOptionsPage::UpdateSpin()
{
	CString str;
	str.Format("(%2d dB)", m_intAmplify);
	GetDlgItem(IDC_EDIT_AMPLIFY)->SetWindowText(str);	
}

void CAudioOptionsPage::OnCheckVbr() 
{
	UpdateData(TRUE);	
	Enc_Info.b_audio_vbr = m_bVBR;
}

void CAudioOptionsPage::ReverseLookupAudioFormat(void)
{
	int id = 0;

	// Loop till max audio radio = LAST (mp3,ac3 and ogg)
	for (id = 0 ; id < AUDIO_TYPE_LAST - 1; id++)
	{
		if (Enc_Info.i_audio_format == i_radio_id[id])
		{
			// found
			m_intAudioFormat = id;
			break;
		}
	}
}
void CAudioOptionsPage::OnBnClickedRadioRecompressac3()
{
	UpdateData(TRUE);
	Enc_Info.b_convert_ac3_to_2channels = 1;
}

void CAudioOptionsPage::OnBnClickedRadioNorecompressac3()
{
	UpdateData(TRUE);
	Enc_Info.b_convert_ac3_to_2channels = 0;
}
