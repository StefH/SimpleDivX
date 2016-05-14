// AdvancedDivX_Settings.cpp : implementation file
//

#include "stdafx.h"
#include "SimpleDivX.h"
#include "AdvancedDivX_Settings.h"
#include "SimpleDivX_Defines.h"
#include "EncodingInfo.h"
#include "ProgramInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CProgramInfo Prog_Info;
extern CEncodingInfo Enc_Info;

int ai_all_adv_checkbox[] = { IDC_CHECK_GMC, IDC_CHECK_QP, IDC_CHECK_BI, IDC_CHECK_CHROMA, IDC_CHECK_TRELLIS, IDC_CHECK_VHQ, IDC_CHECK_LUMI, IDC_CHECK_DARK };

int ai_xvid_adv_checkbox[] = { IDC_CHECK_GMC, IDC_CHECK_QP, IDC_CHECK_BI, IDC_CHECK_CHROMA, IDC_CHECK_TRELLIS, IDC_CHECK_VHQ };

int ai_divx_adv_checkbox[] = { IDC_CHECK_QP, IDC_CHECK_BI, IDC_CHECK_LUMI, IDC_CHECK_DARK };

int ai_h264_adv_checkbox[] = { IDC_CHECK_BI };

/////////////////////////////////////////////////////////////////////////////
// CAdvancedDivX_Settings dialog
CAdvancedDivX_Settings::CAdvancedDivX_Settings(CWnd* pParent /*=NULL*/)
    : CDialog(CAdvancedDivX_Settings::IDD, pParent)

    , m_bVHQ(FALSE)
    , m_bChroma(FALSE)
    , m_bLumi(FALSE)
    , m_bTrellis(FALSE)
    , m_bDarkness(FALSE)
{
    //{{AFX_DATA_INIT(CAdvancedDivX_Settings)
    m_bGMC = FALSE;
    m_bQP = FALSE;
    m_bBI = FALSE;
    m_maxq = 0;
    m_minq = 0;
    //}}AFX_DATA_INIT
}


void CAdvancedDivX_Settings::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAdvancedDivX_Settings)
    DDX_Check(pDX, IDC_CHECK_GMC, m_bGMC);
    DDX_Check(pDX, IDC_CHECK_QP, m_bQP);
    DDX_Check(pDX, IDC_CHECK_BI, m_bBI);
    DDX_Text(pDX, IDC_EDIT_MAXQ, m_maxq);
    DDV_MinMaxInt(pDX, m_maxq, 2, 16);
    DDX_Text(pDX, IDC_EDIT_MINQ, m_minq);
    DDV_MinMaxInt(pDX, m_minq, 2, 16);
    //}}AFX_DATA_MAP
    DDX_Check(pDX, IDC_CHECK_VHQ, m_bVHQ);
    DDX_Check(pDX, IDC_CHECK_CHROMA, m_bChroma);
    DDX_Check(pDX, IDC_CHECK_LUMI, m_bLumi);
    DDX_Check(pDX, IDC_CHECK_TRELLIS, m_bTrellis);
    DDX_Check(pDX, IDC_CHECK_DARK, m_bDarkness);
}


BEGIN_MESSAGE_MAP(CAdvancedDivX_Settings, CDialog)
    //{{AFX_MSG_MAP(CAdvancedDivX_Settings)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvancedDivX_Settings message handlers

BOOL CAdvancedDivX_Settings::OnInitDialog()
{
    CDialog::OnInitDialog();

    My_Init();

    return TRUE;
}

void CAdvancedDivX_Settings::My_Init()
{
    CString s_basic;
    CString s_adv;
    int x = 22;
    int d = 24;
    int y = 120;
    int i = 0;
    int window_frame_height = 10;
    int minimal_window_height = 135;
    int ok_button_height = 20;

    GetDlgItem(IDC_STATIC_PRO)->EnableWindow(1);
    GetDlgItem(IDC_STATIC_NORMAL)->EnableWindow(0);
    GetDlgItem(IDC_STATIC_MINQ)->EnableWindow(0);
    GetDlgItem(IDC_STATIC_MAXQ)->EnableWindow(0);
    GetDlgItem(IDC_EDIT_MAXQ)->EnableWindow(0);
    GetDlgItem(IDC_EDIT_MINQ)->EnableWindow(0);

    for (i = 0; i < sizeof(ai_all_adv_checkbox) / sizeof(int); i++)
    {
        GetDlgItem(ai_all_adv_checkbox[i])->ShowWindow(0);
    }

    for (i = 0; i < _LAST_CODEC; i++)
    {
        if (Prog_Info.ar_vci[i].i_id == Enc_Info.i_codec_version)
        {
            s_basic.Format("%s basic settings", Prog_Info.ar_vci[i].s_short_name);
            s_adv.Format("%s advanced settings", Prog_Info.ar_vci[i].s_short_name);
            break;
        }
    }

    GetDlgItem(IDC_STATIC_NORMAL)->SetWindowText(s_basic);
    GetDlgItem(IDC_STATIC_PRO)->SetWindowText(s_adv);

    if (Enc_Info.i_codec_version == MPEG4)
    {
        for (i = 0; i < sizeof(ai_divx_adv_checkbox) / sizeof(int); i++)
        {
            GetDlgItem(ai_divx_adv_checkbox[i])->ShowWindow(1);
            GetDlgItem(ai_divx_adv_checkbox[i])->SetWindowPos(&CWnd::wndTop, x, y + d * i, 100, 100, SWP_NOSIZE);
        }
    }

    if (Enc_Info.i_codec_version == H264)
    {
        for (i = 0; i < sizeof(ai_h264_adv_checkbox) / sizeof(int); i++)
        {
            GetDlgItem(ai_h264_adv_checkbox[i])->ShowWindow(1);
            GetDlgItem(ai_h264_adv_checkbox[i])->SetWindowPos(&CWnd::wndTop, x, y + d * i, 100, 100, SWP_NOSIZE);
        }
    }

    if (Enc_Info.i_codec_version == XVID)
    {
        GetDlgItem(IDC_STATIC_PRO)->EnableWindow(1);

        for (i = 0; i < sizeof(ai_xvid_adv_checkbox) / sizeof(int); i++)
        {
            GetDlgItem(ai_xvid_adv_checkbox[i])->ShowWindow(1);
            GetDlgItem(ai_xvid_adv_checkbox[i])->SetWindowPos(&CWnd::wndTop, x, y + d * i, 100, 100, SWP_NOSIZE);
        }
    }

    GetDlgItem(IDC_STATIC_PRO)->SetWindowPos(&CWnd::wndTop, 5, y - window_frame_height, 263, d * (i + 1), SWP_NOMOVE);

    GetDlgItem(IDOK)->SetWindowPos(&CWnd::wndTop,
        210,
        minimal_window_height - ok_button_height + d * (i + 1) - 10, // -10 to fix bottom layout from 'Back' button
        100,
        100,
        SWP_NOSIZE);

    this->SetWindowPos(&CWnd::wndTop, 0, 0, 300,
        2 * window_frame_height + minimal_window_height + ok_button_height + d * (i + 1),
        SWP_NOMOVE);

    m_maxq = Enc_Info.i_max_qant;
    m_minq = Enc_Info.i_min_qant;

    UpdateData(0);
}
