#include "stdafx.h"
#include "SimpleDivX.h"
#include "SimpleDivXDlg.h"
#include "AllControlsSheet.h"
#include "MyReg.h"
#include "mytooltips.h"
#include "LanguageSupport.h"
#include "SimpleDivXBatch.h"
#include "EncodingInfo.h"
#include "ConfigData.h"
#include "ParamContainer.h"
#include "GenericSystem.h"

#pragma data_seg("Shared")
LONG g_Counter = -1;
#pragma data_seg()

#pragma comment(linker, "/section:Shared,rws")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

OF OutputFormats[MAXFOR+1];

CProgramInfo Prog_Info;
CEncodingInfo Enc_Info;
CLanguageSupport LanguageModule;
CSimpleDivXBatch* pBatch_List;
CConfigData Config_Data;

C_NFO r_c_nfo[] =
{
	{AVI, "AVI"},
	{OGM, "OGM"},
	{MATROSKA, "MKV"},
	{MP4, "MP4"}
};

A_NFO r_a_nfo[] =
{
	{".mp3", STR_AUDIO_MP3, "0x00000202", "Transcoding AC3 to MP3 started", "Transcoding AC3 to MP3 finished"},
	{".ac3", STR_AUDIO_AC3, "0x00000203", "Transcoding AC3 started", "Transcoding AC3 finished"},
	{".ogg", STR_AUDIO_OGG, "0x00000204", "Transcoding AC3 to Ogg started", "Transcoding AC3 to Ogg finished"},
};

BEGIN_MESSAGE_MAP(CSimpleDivXApp, CWinApp)
	//{{AFX_MSG_MAP(CSimpleDivXApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimpleDivXApp construction

CSimpleDivXApp::CSimpleDivXApp()
{
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSimpleDivXApp object

CSimpleDivXApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSimpleDivXApp initialization

BOOL CSimpleDivXApp::InitInstance()
{
	// Standard initialization

	// Check if app is already running.
	BOOL fFirstInstance = (InterlockedIncrement(&g_Counter) == 0);

	if (!fFirstInstance) 
	{
		// Just exit if app is already running.
		exit(-1);
	}

	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	My_Init();
	CString s_tmp;
	
	s_tmp.Format("%s %s", APP_NAME, APP_VERSION);
	if (Prog_Info.b_debug_mode)
	{
		s_tmp.Append(" -DEBUG-");
	}

	CAllControlsSheet dlg(s_tmp);

	dlg.DoModal();

	return FALSE;
}

void CSimpleDivXApp::My_Init()
{
	int res = 0;

	Config_Data.Read_Settings();

	if (Config_Data.status != 0)
	{
		
	}
	else
	{
		pBatch_List = Config_Data.GetBatchInfo();
	}

	pBatch_List = Config_Data.GetBatchInfo();

	res = Read_SimpleDivXSettings(Prog_Info, Enc_Info);

	Enc_Info.CheckAndCorrect();


	// 05-07-2007 : disable CPU detection
	//CPU_info info;

	//GetCpuInfo(&info);

	//if (Prog_Info.cpu_model != info.model)
	/*
	{
		Prog_Info.cpu_model = info.model;

		switch (info.model)
		{
		case CPU_I_P3:
			CopyFile(Prog_Info.strMEncoderFolder + '\\' + MENCODER_P3, 
				Prog_Info.strMEncoderFolder + '\\' + MENCODER, false);
			break;
		case CPU_I_P4:
			CopyFile(Prog_Info.strMEncoderFolder + '\\' + MENCODER_P4, 
				Prog_Info.strMEncoderFolder + '\\' + MENCODER, false);
			break;
		case CPU_A_XP:
			CopyFile(Prog_Info.strMEncoderFolder + '\\' + MENCODER_AMDXP, 
				Prog_Info.strMEncoderFolder + '\\' + MENCODER, false);
			break;
		case CPU_A_64:
			CopyFile(Prog_Info.strMEncoderFolder + '\\' + MENCODER_AMD64, 
				Prog_Info.strMEncoderFolder + '\\' + MENCODER, false);
			break;
		default:
			CopyFile(Prog_Info.strMEncoderFolder + '\\' + MENCODER_P3, 
				Prog_Info.strMEncoderFolder + '\\' + MENCODER, false);
			break;
		}
	}*/
	

	InitDlls();
	
	CWinApp* pApp = AfxGetApp();

	Prog_Info.b_language_change_allowed = LanguageModule.Init(pApp, Prog_Info.currentLanguage);

	if (!Prog_Info.b_language_change_allowed)
	{
		Prog_Info.currentLanguage = DEF_GUI_LANG;
	}

	// Parse commandline (-a --internal-detection=test.ini)
    ParamContainer p;
	
    p.addParam("automation", 'a', ParamContainer::novalue, "description");
	p.addParam("internal-detection", 'i', ParamContainer::regular, "description", "");
	p.addParam("debug", 'd', ParamContainer::novalue, "description");
    
    p.parseCommandLine(__argc, __argv);

	if (p["debug"] != "")
	{
		Prog_Info.b_debug_mode = TRUE;
	}

	if (p["automation"] != "")
	{
		Prog_Info.b_RunAutomated = TRUE;
	}

	if (p["internal-detection"] != "")
	{
		std::string str = p["internal-detection"];
		Prog_Info.b_InternalDetection = TRUE;
		Prog_Info.strAutomatedInputFile.Format("%s", str.c_str());
	}
}

int CSimpleDivXApp::ExitInstance() 
{
	Write_SimpleDivXSettings(Prog_Info, Enc_Info);

	Config_Data.Write_Settings();

	return CWinApp::ExitInstance();
}


void CSimpleDivXApp::InitDlls()
{
	bool init_error = false;
	bool found = false;
	int i = 0;
	CString str;
	CString str_msg;

	char* dlls[] = 
	{
		VOBSUB_DLL,
		//ICONV_DLL,
		//INTL_DLL,
		NULL
	};

	while (dlls[i] != NULL)
	{
		FILE *fp = NULL;

		fp = fopen(dlls[i], "rb");
		if (fp)
		{
			fclose(fp);
		}
		else
		{
			char s_system_dir[STRLEN] = {0};
			char s_current_dir[STRLEN] = {0};
			char s_file_name[STRLEN] = {0};
			char s_path[STRLEN] = {0};
			GetSystemDirectory(s_system_dir, STRLEN);
			GetCurrentDirectory(STRLEN, s_current_dir);

			sprintf(s_path, "%s", s_system_dir);
			sprintf(s_file_name, "%s\\%s", s_path, dlls[i]);
			fp = fopen(s_file_name, "rb");
			if (fp)
			{
				fclose(fp);
				found = true;
			}
			else
			{
				sprintf(s_path, "%s\\%s", s_current_dir, TOOLS_FOLDER);
				sprintf(s_file_name, "%s\\%s", s_path, dlls[i]);
				fp = fopen(s_file_name, "rb");
				if (fp)
				{
					found = true;
					fclose(fp);
				}
			}

			if (found)
			{
				if (strcmp(dlls[i], VOBSUB_DLL) == 0)
				{
					Prog_Info.strVobSubFolder = s_path;
					Prog_Info.b_can_use_vobsub = true;
				}

				found = false;
			}
			else
			{
				if (strcmp(dlls[i], OGG_DLL) == 0)
				{
					Prog_Info.b_can_use_ogg = false;
				}
				else if (strcmp(dlls[i], VOBSUB_DLL) == 0)
				{
					Prog_Info.b_can_use_vobsub = false;
				}
				else
				{
					str_msg.Format("DLL ERROR : '%s' not found ! Please reinstall SimpleDivX !", s_file_name);
					AfxMessageBox( str_msg, MB_ICONSTOP | MB_OK );
					exit(1);
				}
			}
		}

		i++;
	}
}