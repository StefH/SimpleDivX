// CodecOptions.cpp : implementation file
//

#include "stdafx.h"
#include "SimpleDivX.h"
#include "CodecOptions.h"
#include "LanguageSupport.h"
#include "mytooltips.h"
#include "SimpleDivX_Defines.h"
#include "MyReg.h"
#include "AdvancedDivX_Settings.h"
#include "EncodingInfo.h"
#include "ProgramInfo.h"
#include "../vob_mpeg2/vstrip/vobinfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CProgramInfo Prog_Info;
extern CEncodingInfo Enc_Info;
extern CLanguageSupport LanguageModule;

/////////////////////////////////////////////////////////////////////////////
// CCodecOptions property page

IMPLEMENT_DYNCREATE(CCodecOptions, CPropertyPage)

CCodecOptions::CCodecOptions() : CPropertyPage(CCodecOptions::IDD)
{
	//{{AFX_DATA_INIT(CCodecOptions)
	m_intAviCreate = -1;
	m_strOpenDivxLog = _T("");
	m_iMaxKeyFrame = 0;
	m_intPass = -1;
	m_bAuto_or_Manual_logfilenaming = -1;
	//}}AFX_DATA_INIT

	m_pTooltip = NULL;
	m_pTooltip = new CToolTipCtrl;
	m_pTooltip->Create(this);

	my_page = PAGE_CODEC;
}

CCodecOptions::~CCodecOptions()
{
	delete m_pTooltip;
}

void CCodecOptions::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCodecOptions)
	DDX_Radio(pDX, IDC_RADIO1, m_intAviCreate);
	DDX_Text(pDX, IDC_EDIT_DIVXLOG, m_strOpenDivxLog);
	DDV_MaxChars(pDX, m_strOpenDivxLog, 128);
	DDX_Text(pDX, IDC_EDIT_KEYFRAME, m_iMaxKeyFrame);
	DDV_MinMaxInt(pDX, m_iMaxKeyFrame, 0, 999999);
	DDX_Radio(pDX, IDC_PASS_0, m_intPass);
	//DDX_Radio(pDX, IDC_RADIO_3, m_DivxVersion);
	DDX_Radio(pDX, IDC_RADIO_AUTO_LOGFILENAMING, m_bAuto_or_Manual_logfilenaming);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO_CODEC, m_cmbCodecSelection);
}


BEGIN_MESSAGE_MAP(CCodecOptions, CPropertyPage)
	//{{AFX_MSG_MAP(CCodecOptions)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_LOGFILE, OnLogfile)
	ON_BN_CLICKED(IDC_ADV_SET, OnAdvSet)
	ON_BN_CLICKED(IDC_RADIO_AUTO_LOGFILENAMING, OnRadioAutoLogfilenaming)
	ON_BN_CLICKED(IDC_RADIO_MANUAL_LOGFILENAMING, OnRadioManualLogfilenaming)
	ON_BN_CLICKED(IDC_PASS_1, OnPass1)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO_CODEC, OnCbnSelchangeComboCodec)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCodecOptions message handlers

void CCodecOptions::My_Init()
{
	m_bAuto_or_Manual_logfilenaming = !Enc_Info.b_auto_logfile_naming;
	m_intAviCreate = Enc_Info.i_create_avi_options;
	m_iMaxKeyFrame = Enc_Info.i_max_keyframe_interval;
	m_intPass = Enc_Info.i_pass_mode;

	CString formattedMessage = LanguageModule.strAudioFormat;
	
	if (Enc_Info.b_create_audio)
	{
		if(Enc_Info.i_audio_format == AUDIO_TYPE_MP3)
		{
			formattedMessage.Replace("XXX", STR_AUDIO_MP3);
		}
		else if (Enc_Info.i_audio_format == AUDIO_TYPE_AC3)
		{
			formattedMessage.Replace("XXX", STR_AUDIO_AC3);
		}
		else if (Enc_Info.i_audio_format == AUDIO_TYPE_OGG)
		{
			formattedMessage.Replace("XXX", STR_AUDIO_OGG);
		}
	}
	else
	{
		formattedMessage = LanguageModule.strNoAudioFormat;
	}

	GetDlgItem(IDC_RADIO1)->SetWindowText(LanguageModule.str1PassVideo + " " + formattedMessage);
	GetDlgItem(IDC_RADIO2)->SetWindowText(LanguageModule.str2PassVideo + " " + formattedMessage);
	
	m_strOpenDivxLog = Enc_Info.sTwoPassLogFile;
	
	m_intOld2pass = 2;

	UpdatePassButtons();

	UpdateLogNaming(Enc_Info.b_auto_logfile_naming);

	UpdateData(0);

	int sel = -1;
	for (int i = 0 ; i < _LAST_CODEC; i++)
	{
		if ((Prog_Info.ar_vci[i].b_valid) && (Prog_Info.ar_vci[i].i_id == Enc_Info.i_codec_version))
		{
			sel = Prog_Info.ar_vci[i].i_dropdown_id;
		}
	}

	if (sel < 0) sel = 0;
	
	m_cmbCodecSelection.SetCurSel(sel);
	UpdateVersion(sel);

	LanguageModule.InitAttribs(this,my_page);
}

BOOL CCodecOptions::OnSetActive() 
{
	if (Prog_Info.b_init_codecoptionspage == 0)
	{
		My_Static_Init();
		Prog_Info.b_init_codecoptionspage = 1;
	}	

	My_Init();
	return CPropertyPage::OnSetActive();
}

void CCodecOptions::OnRadio1() 
{
	int old = 0;
	UpdateData(TRUE);
	old = m_intPass;
	Enc_Info.i_create_avi_options = m_intAviCreate;

	if ((Enc_Info.i_create_avi_options == 0)/* && (old >= 1)*/)
	{
		m_intOld2pass = old;
	}
	
	UpdatePassButtons();
	
	UpdateData(FALSE);
}

void CCodecOptions::OnRadio2() 
{
	int old = 0;
	UpdateData(TRUE);

	old = m_intPass;

	Enc_Info.i_create_avi_options = m_intAviCreate;

	UpdatePassButtons();
	if ((old == 0) && (Enc_Info.i_create_avi_options == 1))
	{
		if (m_intOld2pass < 0)
		{
			m_intOld2pass = 2;
		}
		m_intPass = m_intOld2pass;
	}

	UpdateData(FALSE);
}

BOOL CCodecOptions::OnKillActive() 
{
	UpdateData(TRUE);

	ProcessVars();
	
	if (strlen(Enc_Info.sTwoPassLogFile) > 0)
	{
		
	}
	else
	{
		if (Enc_Info.i_create_avi_options != 0)
		{
			m_strOpenDivxLog = Enc_Info.s_TempFolder + '\\' + DIVXLOG_FILE;
			Enc_Info.sTwoPassLogFile = m_strOpenDivxLog;
		}
	}

	return CPropertyPage::OnKillActive();
}

BOOL CCodecOptions::PreTranslateMessage(MSG* pMsg) 
{
	if (NULL != m_pTooltip) m_pTooltip->RelayEvent(pMsg);
	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CCodecOptions::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (NULL != m_pTooltip) m_pTooltip->Activate(TRUE);	
	CPropertyPage::OnMouseMove(nFlags, point);
}

void CCodecOptions::My_Static_Init()
{
	int i = 0;
	int sel = -1;
	int dropdown_id = 0;

	if ((Enc_Info.i_pass_mode < 0) || (Enc_Info.i_pass_mode > 2))
	{
		Enc_Info.i_pass_mode = 0;
	}

	m_cmbCodecSelection.ResetContent();

	for (i = 0 ; i < _LAST_CODEC; i++)
	{
		if (Prog_Info.ar_vci[i].b_valid)
		{
			m_cmbCodecSelection.AddString(Prog_Info.ar_vci[i].s_short_name);
			Prog_Info.ar_vci[i].i_dropdown_id = dropdown_id;
			if (sel == -1) sel = i;
			dropdown_id++;
		}
	}
	
	CMyToolTips my_tooltips;
	my_tooltips.PrepareTooltips(this, my_page, 
		m_pTooltip);

	if (strlen(Enc_Info.sTwoPassLogFile) == 0)
	{
		Enc_Info.sTwoPassLogFile.Format("%s\\%s",  Enc_Info.s_TempFolder , DIVXLOG_FILE);
	}

	if (Enc_Info.i_max_keyframe_interval <= 0)
	{
		Enc_Info.i_max_keyframe_interval = 250;
	}
}

void CCodecOptions::UpdatePassButtons()
{
	GetDlgItem(IDC_PASS_1)->EnableWindow(Enc_Info.i_create_avi_options * b_PassModeEnabled);
	GetDlgItem(IDC_PASS_2)->EnableWindow(Enc_Info.i_create_avi_options * b_PassModeEnabled);
	
	UpdateLogNaming(Enc_Info.b_auto_logfile_naming);

	GetDlgItem(IDC_RADIO_AUTO_LOGFILENAMING)->EnableWindow(Enc_Info.i_create_avi_options * b_LogFileSettingsEnabled); 
	GetDlgItem(IDC_RADIO_MANUAL_LOGFILENAMING)->EnableWindow(Enc_Info.i_create_avi_options * b_LogFileSettingsEnabled); 
	
	m_intPass *= Enc_Info.i_create_avi_options * b_PassModeEnabled;
}

void CCodecOptions::UpdateVersion(int dropdown_sel)
{
	bool b_TwoPassAllowed = false;
	bool b_EditKeyFrameAllowed = false;
	bool b_AdvancedSettingsAllowed = false;
	bool b_StartDivXAuto = false;
	
	// module vars
	b_PassModeEnabled = false;
	b_LogFileSettingsEnabled = false;

	for (int i = 0 ; i < _LAST_CODEC ; i++)
	{
		if (Prog_Info.ar_vci[i].i_dropdown_id == dropdown_sel)
		{
			Enc_Info.i_codec_version = Prog_Info.ar_vci[i].i_id;
			Enc_Info.fourcc = Prog_Info.ar_vci[i].fourcc;
			b_TwoPassAllowed = Prog_Info.ar_vci[i].b_TwoPassAllowed;
			b_EditKeyFrameAllowed = Prog_Info.ar_vci[i].b_EditKeyFrameAllowed;
			b_AdvancedSettingsAllowed = Prog_Info.ar_vci[i].b_AdvancedSettingsAllowed;
			b_StartDivXAuto = Prog_Info.ar_vci[i].b_StartDivXAuto;
			b_PassModeEnabled = Prog_Info.ar_vci[i].b_PassModeEnabled;
			b_LogFileSettingsEnabled = Prog_Info.ar_vci[i].b_LogFileSettingsEnabled;
			break;
		}
	}

	// Radio and buttons...
	GetDlgItem(IDC_ADV_SET)->EnableWindow(b_AdvancedSettingsAllowed);
	GetDlgItem(IDC_RADIO2)->EnableWindow(b_TwoPassAllowed);
	GetDlgItem(IDC_EDIT_KEYFRAME)->EnableWindow(b_EditKeyFrameAllowed);

	// Var settings
	Enc_Info.i_create_avi_options *= b_TwoPassAllowed; 
	m_intAviCreate = Enc_Info.i_create_avi_options;

	UpdatePassButtons();
}

CString CCodecOptions::BrowseForFile()
{
	CString name;
	CString command;

	name = m_strOpenDivxLog;
		
	command.Format("%s|%s||", "*.log", "*.log");
		
	CFileDialog dlg(TRUE, "", name, 0 ,command);

	if (dlg.DoModal() == IDOK)
	{
		return dlg.GetPathName();
	}

	return name;
}

void CCodecOptions::OnLogfile() 
{
	m_strOpenDivxLog = BrowseForFile();	
	UpdateData(FALSE);
}

void CCodecOptions::OnAdvSet() 
{
	int status = 0;

	CAdvancedDivX_Settings window;

	UpdateData(1);

	window.m_bBI = Enc_Info.b_adv_bi;
	window.m_bQP = Enc_Info.b_adv_qpel;
	window.m_bGMC = Enc_Info.b_adv_gmc;
	window.m_maxq = Enc_Info.i_max_qant;
	window.m_minq = Enc_Info.i_min_qant;
	window.m_bVHQ = Enc_Info.b_adv_vhq;
	window.m_bChroma = Enc_Info.b_adv_chroma_motion;
	window.m_bLumi = Enc_Info.b_adv_lumi_mask;
	window.m_bDarkness = Enc_Info.b_adv_dark_mask;
	window.m_bTrellis = Enc_Info.b_adv_trellis;
	
	status = window.DoModal();

	if (status == IDOK)
	{
		Enc_Info.b_adv_gmc = window.m_bGMC;
		Enc_Info.b_adv_bi = window.m_bBI;
		Enc_Info.b_adv_qpel = window.m_bQP;
		Enc_Info.i_max_qant = window.m_maxq;
		Enc_Info.i_min_qant = window.m_minq;
		Enc_Info.b_adv_vhq = window.m_bVHQ;
		Enc_Info.b_adv_chroma_motion = window.m_bChroma;
		Enc_Info.b_adv_lumi_mask = window.m_bLumi;
		Enc_Info.b_adv_dark_mask = window.m_bDarkness;
		Enc_Info.b_adv_trellis = window.m_bTrellis;

		if (Enc_Info.i_min_qant > Enc_Info.i_max_qant)
		{
			if (Enc_Info.i_max_qant >= 3)
			{
				Enc_Info.i_min_qant = Enc_Info.i_max_qant - 1;
			}
			else
			{
				Enc_Info.i_min_qant = 2;
			}
		}
	}

	UpdateData(FALSE);
}

void CCodecOptions::ProcessVars()
{
	Enc_Info.i_create_avi_options = m_intAviCreate;
	Enc_Info.i_max_keyframe_interval = m_iMaxKeyFrame;
	Enc_Info.i_pass_mode = m_intPass;
	
	if (Enc_Info.b_auto_logfile_naming)
	{
		m_strOpenDivxLog.Format("%s\\%s%s", Enc_Info.s_TempFolder, Enc_Info.s_ProjName, DIVXLOGFILEEXT);
	}

	Enc_Info.sTwoPassLogFile = m_strOpenDivxLog;
}

void CCodecOptions::OnRadioAutoLogfilenaming() 
{
	UpdateData(TRUE);
	Enc_Info.b_auto_logfile_naming = !m_bAuto_or_Manual_logfilenaming;
	
	UpdateLogNaming(Enc_Info.b_auto_logfile_naming);
	
}

void CCodecOptions::OnRadioManualLogfilenaming() 
{
	UpdateData(TRUE);
	
	Enc_Info.b_auto_logfile_naming = !m_bAuto_or_Manual_logfilenaming;

	UpdateLogNaming(Enc_Info.b_auto_logfile_naming);
}

void CCodecOptions::UpdateLogNaming(int i)
{
	GetDlgItem(IDC_EDIT_DIVXLOG)->EnableWindow(b_LogFileSettingsEnabled * Enc_Info.i_create_avi_options * !i); 
	GetDlgItem(IDC_LOGFILE)->EnableWindow(b_LogFileSettingsEnabled * Enc_Info.i_create_avi_options * !i); 
}

void CCodecOptions::OnPass1() 
{
	UpdateData(1);

	ProcessVars();
}

void CCodecOptions::OnCbnSelchangeComboCodec()
{
	int i = 0;
	
	UpdateData(TRUE);
	i = m_cmbCodecSelection.GetCurSel();
	
	UpdateVersion(i);

	UpdateData(0);
}