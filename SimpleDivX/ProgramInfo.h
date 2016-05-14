#pragma once
#include "afx.h"
#include "stdafx.h"
#include "SimpleDivX_defines.h"
#include "GenericSystem.h"
#include ".\ProgramInfo.h"

class CProgramInfo :
	public CObject
{

	DECLARE_SERIAL( CProgramInfo )

public:
	CProgramInfo(void);
	~CProgramInfo(void);
	void Serialize(CArchive& ar);

	void CopyIt(const CProgramInfo& p);

	CProgramInfo( const CProgramInfo& p )
	{
		CopyIt(p);	
	}

	const CProgramInfo& operator=( const CProgramInfo& p )
    {
		CopyIt(p);

		return *this;
    }

	BOOL b_debug_mode;
	BOOL b_batch_enabled;
	BOOL b_can_use_ogg;
	BOOL b_can_use_vobsub;
	BOOL b_RunAutomated;
	BOOL b_InternalDetection;

	// property states
	bool b_init_videooptionspage;
	bool b_init_codecoptionspage;
	bool b_init_audiooptionspage;
	bool b_init_inputoptionspage;
	bool b_init_outputfilespage;
	bool b_init_compressionpage;
	bool b_init_setuppage;
	bool b_init_videoadvancedpage;
	bool b_init_projectoptionspage;
	bool b_init_subtitlespage;
	
	bool b_programs_found;
	bool b_language_change_allowed;
	bool b_shutdown;
	
	int	i_video_prio;
	int	i_disk_prio;

	int cpu_model;

	CString strVirtualDubFolder;
	CString strMEncoderFolder;
	CString strMPlayerFolder;
	CString strVobSubFolder;
	CString strMkvMergeFolder;
	CString strInstallDir;

	CString strAutomatedInputFile;

	CString currentLanguage;

	VIDEO_CODEC_INFO ar_vci[_LAST_CODEC];
};
