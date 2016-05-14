// AdvancedVideo_Settings.cpp : implementation file
//

#include "stdafx.h"
#include "SimpleDivX.h"
#include "AdvancedVideo_Settings.h"
#include "AdvancedDivX_Settings.h"
#include "SimpleDivX_Defines.h"
#include "mytooltips.h"
#include "EncodingInfo.h"
#include "ProgramInfo.h"
#include ".\advancedvideo_settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CProgramInfo Prog_Info;
extern CEncodingInfo Enc_Info;

CAdvancedVideo_Settings::CAdvancedVideo_Settings(CWnd* pParent /*=NULL*/)
	: CDialog(CAdvancedVideo_Settings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAdvancedVideo_Settings)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pTooltip = NULL;
	m_pTooltip = new CToolTipCtrl;
	m_pTooltip->Create(this);
}


void CAdvancedVideo_Settings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvancedVideo_Settings)
	DDX_Control(pDX, IDC_COMBO_REFQ, m_cmbReferenceQuality);
	DDX_Control(pDX, IDC_COMBO_POSTFILTER, m_cmbPostFilter);
	DDX_Control(pDX, IDC_COMBO_OUTPUTMODE, m_cmbOutputMode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvancedVideo_Settings, CDialog)
	//{{AFX_MSG_MAP(CAdvancedVideo_Settings)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO_REFQ, OnCbnSelchangeComboRefq)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvancedVideo_Settings message handlers

void CAdvancedVideo_Settings::My_Init()
{
	m_cmbOutputMode.SetCurSel(Enc_Info.i_outputmode_id);
}

BOOL CAdvancedVideo_Settings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (Prog_Info.b_init_videoadvancedpage == 0)
	{
		My_Static_Init();
		Prog_Info.b_init_videoadvancedpage = 1;
	}

	My_Init();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAdvancedVideo_Settings::My_Static_Init()
{
	PAGE my_page = PAGE_VIDEO_ADV;
	CMyToolTips my_tooltips;
	my_tooltips.PrepareTooltips(this, my_page, m_pTooltip);
}

void CAdvancedVideo_Settings::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (NULL != m_pTooltip) m_pTooltip->Activate(TRUE);	
	CDialog::OnMouseMove(nFlags, point);
}


BOOL CAdvancedVideo_Settings::PreTranslateMessage(MSG* pMsg) 
{
	if (NULL != m_pTooltip) m_pTooltip->RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CAdvancedVideo_Settings::DestroyWindow() 
{
	OutputMode = m_cmbOutputMode.GetCurSel();
		
	PostFilter = m_cmbPostFilter.GetCurSel();
	ReferenceQuality = m_cmbReferenceQuality.GetCurSel();
	return CDialog::DestroyWindow();
}

void CAdvancedVideo_Settings::OnCbnSelchangeComboRefq()
{
	// TODO: Add your control notification handler code here
}
