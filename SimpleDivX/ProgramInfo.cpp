#include "stdafx.h"
#include "GenericSystem.h"
#include ".\programinfo.h"

#define  DEBUGDETECT	"C:\\SimpleDivX.debug"

IMPLEMENT_SERIAL( CProgramInfo, CObject, 1 )

CProgramInfo::CProgramInfo(void)
{
	if (strInstallDir.IsEmpty())
	{
		char s_current_dir[STRLEN512] = {0};

		GetCurrentDirectory(STRLEN, s_current_dir);
		strInstallDir = s_current_dir;
	}

	b_debug_mode = 0;
	b_batch_enabled = 0;
	b_can_use_ogg = 0;
	b_can_use_vobsub = 0;

	b_programs_found = false;
	b_language_change_allowed = false;
	b_shutdown = false;

	strMkvMergeFolder.Format("%s\\%s", strInstallDir, TOOLS_FOLDER);
	strVirtualDubFolder.Format("%s\\%s", strInstallDir, TOOLS_FOLDER);
	strMEncoderFolder.Format("%s\\%s", strInstallDir, TOOLS_FOLDER);
	strMPlayerFolder.Format("%s\\%s", strInstallDir, TOOLS_FOLDER);
	strVobSubFolder.Format("%s\\%s", strInstallDir, TOOLS_FOLDER);

	b_init_videooptionspage = 0;
	b_init_audiooptionspage = 0;
	b_init_codecoptionspage = 0;
	b_init_compressionpage = 0;
	b_init_videoadvancedpage = 0;
	b_init_projectoptionspage = 0;

	i_video_prio = 3;
	i_disk_prio = 3;

	cpu_model = CPU_OTHER;
	
	currentLanguage = DEF_GUI_LANG;

	FILE *fp = NULL;
	fp = fopen(DEBUGDETECT, "rb");
	if (fp)
	{
		b_debug_mode = 1;
		fclose(fp);
	}
}

CProgramInfo::~CProgramInfo(void)
{

}

void CProgramInfo::CopyIt(const CProgramInfo& e)
{
	int i = 0;

	b_debug_mode = e.b_debug_mode;
	b_batch_enabled = e.b_batch_enabled;
	b_can_use_ogg = e.b_can_use_ogg;
	b_can_use_vobsub = e.b_can_use_vobsub;
	b_RunAutomated = e.b_RunAutomated;
	b_InternalDetection = e.b_InternalDetection;

	b_init_videooptionspage = e.b_init_videooptionspage;
	b_init_codecoptionspage = e.b_init_codecoptionspage;
	b_init_audiooptionspage = e.b_init_audiooptionspage;
	b_init_inputoptionspage = e.b_init_inputoptionspage;
	b_init_outputfilespage = e.b_init_outputfilespage;
	b_init_compressionpage = e.b_init_compressionpage;
	b_init_setuppage = e.b_init_setuppage;
	b_init_videoadvancedpage = e.b_init_videoadvancedpage;
	b_init_projectoptionspage = e.b_init_projectoptionspage;
	b_init_subtitlespage = e.b_init_subtitlespage;
	
	b_programs_found = e.b_programs_found;
	b_language_change_allowed = e.b_language_change_allowed;
	b_shutdown = e.b_shutdown;
	
	i_video_prio = e.i_video_prio;
	i_disk_prio = e.i_disk_prio;

	cpu_model = e.cpu_model;

	strVirtualDubFolder = e.strVirtualDubFolder;
	strMEncoderFolder = e.strMEncoderFolder;
	strVobSubFolder = e.strVobSubFolder;
	strMPlayerFolder = e.strMPlayerFolder;
	strAutomatedInputFile = e.strAutomatedInputFile;
	strMkvMergeFolder = e.strMkvMergeFolder;
	strInstallDir = e.strInstallDir;

	currentLanguage = e.currentLanguage;

	for (i = 0 ; i < _LAST_CODEC; i++)
	{
		ar_vci[i].b_AdvancedSettingsAllowed = e.ar_vci[i].b_AdvancedSettingsAllowed;
		ar_vci[i].b_EditKeyFrameAllowed = e.ar_vci[i].b_EditKeyFrameAllowed;
		ar_vci[i].b_EndCreditsRecompressionAllowed = e.ar_vci[i].b_EndCreditsRecompressionAllowed;
		ar_vci[i].b_LogFileSettingsEnabled = e.ar_vci[i].b_LogFileSettingsEnabled;
		ar_vci[i].b_PassModeEnabled = e.ar_vci[i].b_PassModeEnabled;
		ar_vci[i].b_StartDivXAuto = e.ar_vci[i].b_StartDivXAuto;
		ar_vci[i].b_TwoPassAllowed = e.ar_vci[i].b_TwoPassAllowed;
		ar_vci[i].b_valid = e.ar_vci[i].b_valid;
		ar_vci[i].i_id = e.ar_vci[i].i_id;
		ar_vci[i].i_dropdown_id = e.ar_vci[i].i_dropdown_id;
		ar_vci[i].s_name = e.ar_vci[i].s_name;
		ar_vci[i].s_short_name =  e.ar_vci[i].s_short_name;
	}
}

void CProgramInfo::Serialize(CArchive& ar)
{
	CObject::Serialize( ar );

	CString AppVersion = APP_VERSION; 

	if( ar.IsStoring() )
	{
		// Write
		ar << AppVersion;
		ar << b_batch_enabled;
		ar << b_can_use_ogg;
		ar << b_can_use_vobsub;
		ar << b_RunAutomated;
		ar << b_InternalDetection;

		ar << i_video_prio;
		ar << i_disk_prio;

		ar << cpu_model;

		ar << strVirtualDubFolder;
		ar << strMEncoderFolder;
		ar << strVobSubFolder;
		ar << strMkvMergeFolder;

		ar << currentLanguage;
	}
	else
	{
		// Read
		ar >> AppVersion;
		ar >> b_batch_enabled;
		ar >> b_can_use_ogg;
		ar >> b_can_use_vobsub;
		ar >> b_RunAutomated;
		ar >> b_InternalDetection;

		ar >> i_video_prio;
		ar >> i_disk_prio;

		ar >> cpu_model;

		ar >> strVirtualDubFolder;
		ar >> strMEncoderFolder;
		ar >> strVobSubFolder;
		ar >> strMkvMergeFolder;

		ar >> currentLanguage;
	}
}