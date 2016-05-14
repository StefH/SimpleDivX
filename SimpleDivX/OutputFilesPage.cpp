// OutputFilesPage.cpp : implementation file
#include "stdafx.h"
#include "SimpleDivX.h"

#include "OutputFilesPage.h"
#include "VideoOptionsPage.h"
#include "AudioOptionsPage.h"
#include "ProjectOptionsPage.h"
#include "CodecOptions.h"
#include "../genericmm/genericmm.h"
#include "mytooltips.h"
#include "MyReg.h"
#include "LanguageSupport.h"
#include "SimpleDivXBatch.h"
#include "ConfigData.h"
#include "ProgramInfo.h"
#include "../vob_mpeg2/vstrip/vobinfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int convert_line2args(char *line, char**ARGV);

typedef enum
{
	START_MENCODER = 0,
	START_MPLAYER,
	START_VIRTUALDUB,
	START_MKVMERGE,
	START_VOBSUB
};

typedef enum
{
	TIMER = 1000,
	MENCODER_TIMER,
	MENCODER_TIMER2,
	COMBINE_TIMER,
	SHUTDOWN_TIMER,
	BATCH_TIMER
};

#define TIMER_LENGTH 5000
#define BATCH_TIMER_LENGTH 5060
#define MENCODER_POLL_TIMER 7181
#define MENCODER_START_TIMER 5107
#define WAIT_TIME 6013

typedef enum
{
	STEP_START = 0,
	STEP_INIT,
	STEP_MENCODER2,
	STEP_CORRECTAC3,
	STEP_COMBINE,
	STEP_SUBTITLES,
	STEP_MKVMERGE,
	STEP_DONE
};

extern CConfigData Config_Data;
extern CSimpleDivXBatch* pBatch_List;
extern CProgramInfo Prog_Info;
extern CEncodingInfo Enc_Info;
extern CLanguageSupport LanguageModule;
extern C_NFO r_c_nfo[];
extern A_NFO r_a_nfo[];

UINT Thread_MEncoder(LPVOID param);
UINT Thread_MPlayer(LPVOID param);
void Execute_(int, char*, int);

// global vars
int g_MEncoder_finished = 0;
int g_MEncoder_started = 0;
int g_step = STEP_START;
int g_started = 0;
int g_pass = 1;
int gi_num_handles = 0;
int gi_num_threads = 0;
int g_combining_ready = 0;
int g_mkvmerge_ready = 0;
int g_combining_started = 0;
int g_mkvmerge_started = 0;
int g_all_ready = 0;
int g_batch_counter = 0;
int g_encoding_error = 0;
int g_batch_ready = 0;
int g_failed_batch_counter = 0;

#define BUFSIZE (80)
DWORD dwRead, dwWritten; 
CHAR chBuf[BUFSIZE]; 


HANDLE r_handle[MAXHANDLES] = {0};
HANDLE r_thread[MAXTHREADS] = {0};
HANDLE hChildStdoutRd, hChildStdoutWr;

/////////////////////////////////////////////////////////////////////////////
// COutputFilesPage property page

IMPLEMENT_DYNCREATE(COutputFilesPage, CPropertyPage)

COutputFilesPage::COutputFilesPage() : CPropertyPage(COutputFilesPage::IDD)
{
	//{{AFX_DATA_INIT(COutputFilesPage)
	m_strInfo = _T("");
	m_bShutDown = FALSE;
	//}}AFX_DATA_INIT

	m_pTooltip = NULL;
	m_pTooltip = new CToolTipCtrl;
	m_pTooltip->Create(this);

	my_page = PAGE_OUTPUT;
}

COutputFilesPage::~COutputFilesPage()
{
	delete m_pTooltip;
}

void COutputFilesPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COutputFilesPage)
	DDX_Control(pDX, IDC_INFO, m_ctrlEdit);
	DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress);
	DDX_Text(pDX, IDC_INFO, m_strInfo);
	DDX_Check(pDX, IDC_CHECK_SHUTDOWN, m_bShutDown);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COutputFilesPage, CPropertyPage)
	//{{AFX_MSG_MAP(COutputFilesPage)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_CHECK_OVERWRITE_AVI, OnCheckOverwriteAvi)
	ON_BN_CLICKED(IDC_CHECK_SHUTDOWN, OnCheckShutdown)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
	ON_MESSAGE(PSM_QUERYSIBLINGS, OnQuerySiblings)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputFilesPage message handlers

BOOL COutputFilesPage::OnSetActive() 
{
	//if (Enc_Info.b_init_outputfilespage == 0)
	{
		My_Static_Init();

		//	Enc_Info.b_init_outputfilespage = true;
	}

	My_Init();

	return CPropertyPage::OnSetActive();
}

BOOL COutputFilesPage::OnKillActive() 
{
	return CPropertyPage::OnKillActive();
}

void COutputFilesPage::My_Init()
{
	m_bShutDown = Prog_Info.b_shutdown;
	UpdateData(0);

	LanguageModule.InitAttribs(this, my_page);
}

void COutputFilesPage::OnStart() 
{
	int i = 0;
	UpdateData(TRUE);

	if (g_started == 1)
	{
		AbortConversion(0);
	}
	else
	{
		m_strInfo = "";
		PrepareVars(0);

		CTime t = CTime::GetCurrentTime();
		Log(t.Format("SimpleDivX started at : [%d-%m-%y, %H:%M:%S]"));

		if (Prog_Info.b_batch_enabled == 0)
		{
			g_batch_counter = 0; //?
			m_ctrlProgress.SetRange32(0, STEP_DONE);

			if (!Prog_Info.b_init_projectoptionspage)
			{
				pBatch_List->SetEncInfo(Enc_Info);
				Prog_Info.b_init_projectoptionspage = 1;
			}

			Enc_Info = pBatch_List->GetEncInfo();
			DoStuff(-147);
		}
		else
		{
			if (pBatch_List->GetNum() > 0)
			{
				i_num_projects = pBatch_List->GetNum();

				m_ctrlProgress.SetRange32(0, STEP_DONE * (i_num_projects+1));
				
				Enc_Info = pBatch_List->GetEncInfoAt(g_batch_counter);

				g_batch_counter++;

				int Timer = SetTimer(BATCH_TIMER, BATCH_TIMER_LENGTH , NULL);

				DoStuff(-1);
			}
			else
			{
				AfxMessageBox("No Projects have been added to the batch queue !");
			}
		}
	}	
}

void Execute_(int i_option, char *command_line, int prio)
{
	CString command;
	CString execname;
	int result = 0;
	int i_num_proc = 0;
	int i = 0;
	int i_priority = NORMAL_PRIORITY_CLASS;
	DWORD wait_time = INFINITE;

	switch (prio)
	{
	case 0:
	case 1:
		i_priority = IDLE_PRIORITY_CLASS;
		break;
	case 2:
	case 3:
		i_priority = NORMAL_PRIORITY_CLASS;
		break;
	case 4:
	case 5:
		i_priority = HIGH_PRIORITY_CLASS;
		break;
	case 6:
		i_priority = REALTIME_PRIORITY_CLASS;
		break;
	default:
		i_priority = NORMAL_PRIORITY_CLASS;
		break;
	}

	switch (i_option)
	{
	case START_MENCODER:
		execname.Format("%s\\%s" , Prog_Info.strMEncoderFolder , MENCODER);
		if (g_pass == 1)
		{
			command = Enc_Info.enc1_execline;
		}
		else
		{
			command = Enc_Info.enc2_execline;
		}
		g_MEncoder_started = 1;
		break;
		
	case START_MPLAYER:
		execname.Format("%s\\%s", Prog_Info.strMPlayerFolder, MPLAYER);
		command = Enc_Info.play_execline;
		break;

	case START_VIRTUALDUB:
		wait_time = INFINITE;
		g_combining_ready = 0;
		execname.Format("%s\\%s", Prog_Info.strVirtualDubFolder, VIRTUALDUB );
		command.Format(" /h /x /r /s\"%s\\%s\"", Enc_Info.s_TempFolder, VD_TMP);
		break;

	case START_MKVMERGE:
		wait_time = INFINITE;
		g_mkvmerge_ready = 0;
		execname.Format("%s\\%s", Prog_Info.strMkvMergeFolder, MKVMERGE );
		command = Enc_Info.mkv_exeline;
		break;

	case START_VOBSUB:
		wait_time = INFINITE;
		execname = "rundll32.exe";
		command = command_line;
		command.Format("%s %s", execname, command_line);
		break;

	default:
		break;
	}

	//SECURITY_ATTRIBUTES saAttr;
    
    // Set the bInheritHandle flag so pipe handles are inherited. 
	/*
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL;*/

    // Create a pipe for the child process's STDOUT.
	/*
    if (! CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)) {
		AfxMessageBox("Stdout pipe creation failed.");
	}*/

	// Ensure the read handle to the pipe for STDOUT is not inherited.
    // SetHandleInformation( hChildStdoutRd, HANDLE_FLAG_INHERIT, 0);

	STARTUPINFO st;
	PROCESS_INFORMATION pi;

	ZeroMemory( &st, sizeof(STARTUPINFO) );
	st.cb=sizeof(STARTUPINFO);
	st.wShowWindow = true; 
	//st.dwFlags |= STARTF_USESTDHANDLES;        
	//st.hStdOutput = hChildStdoutWr;
		
	if (i_option == START_MPLAYER)
	{
		st.lpTitle="SimpleDivX : MPlayer Preview...";
	}
	else if (i_option == START_MENCODER)
	{
		st.lpTitle="SimpleDivX : MEncoder video encoding...";
	}
	else if (i_option == START_MKVMERGE)
	{
		st.lpTitle="SimpleDivX : creating Matroska file...";
	}
	
	// Start app.
	if (i_option != START_VOBSUB)
	{
		result = CreateProcess(execname, command.GetBuffer(), 
			NULL, NULL, false, i_priority, NULL, NULL, &st, &pi);
	}
	else
	{
		char s_current_dir[STRLEN] = {0};
		GetCurrentDirectory(STRLEN, s_current_dir);

		result = CreateProcess(NULL, command.GetBuffer(), 
			NULL, NULL, false, i_priority, NULL, s_current_dir, &st, &pi);
	}

	r_handle[gi_num_handles] = pi.hProcess;
	gi_num_handles++;

	//CloseHandle(pi.hProcess);
    //CloseHandle(pi.hThread);

	WaitForSingleObject(pi.hProcess, wait_time);

	if (START_VIRTUALDUB == i_option)
	{
		g_combining_ready = 1;
	}

	if (START_MKVMERGE == i_option)
	{
		g_mkvmerge_ready = 1;
	}

	for (i_num_proc = 0; i_num_proc < gi_num_handles; i_num_proc++)
	{
		if (r_handle[i_num_proc] == pi.hProcess)
		{
			r_handle[i_num_proc] = 0;
		}
	}

	switch (i_option)
	{
	case START_MENCODER:
		g_MEncoder_finished = 1;
		break;
	}
}

void COutputFilesPage::OnTimer(UINT nIDEvent) 
{
	m_ctrlProgress.SetPos(g_step + g_batch_counter * STEP_DONE);

	if (nIDEvent == TIMER)
	{
		execute_steps(Enc_Info.i_create_avi_options);
	}

	if (nIDEvent == MENCODER_TIMER)
	{
		// MEncoder is ready, so ac3 is also created.
		if (g_MEncoder_finished == 1)
		{
			KillTimer(MENCODER_TIMER);
			g_step = STEP_INIT;
			Log("MEncoder 1-pass finished");
			g_MEncoder_finished = 0;

			m_intTimer = SetTimer(TIMER, TIMER_LENGTH , NULL);
		}
	}

	if (nIDEvent == MENCODER_TIMER2)
	{
		// MEncoder is ready (2pass) so go on
		if (g_MEncoder_finished == 1)
		{
			KillTimer(MENCODER_TIMER2);
			g_step = STEP_CORRECTAC3;
			Log("MEncoder 2-pass finished");
			g_MEncoder_finished = 0;

			m_intTimer = SetTimer(TIMER, TIMER_LENGTH , NULL);
		}
	}

	if (nIDEvent == SHUTDOWN_TIMER)
	{
		Write_SimpleDivXSettings(Prog_Info, Enc_Info);

		Config_Data.Write_Settings();

		int i_shutdown_result = ShutDown_(1);
	}

	if (nIDEvent == BATCH_TIMER)
	{
		if (g_batch_ready == 1)
		{
			g_batch_ready = 0;

			int cnt = m_ctrlEdit.GetLineCount();
			m_ctrlEdit.LineScroll(cnt + 1, 0);

			if (g_batch_counter < i_num_projects)
			{
				PrepareVars(1);

				// TODO
				Enc_Info = pBatch_List->GetEncInfoAt(g_batch_counter);
				DoStuff(-1);
				g_batch_counter++;
			}
			else
			{
				g_batch_counter = i_num_projects;
				KillTimer(BATCH_TIMER);
			}
		}
	}

	CPropertyPage::OnTimer(nIDEvent);
}

void COutputFilesPage::Log(CString str)
{
	CTime t = CTime::GetCurrentTime();
	CString s = t.Format( " [%H:%M:%S]  " );
	m_strInfo += (s + str + "\r\n");
	UpdateData(FALSE);
}

UINT Thread_MEncoder(LPVOID pParam)
{
	Execute_(START_MENCODER,0, Prog_Info.i_video_prio);
	return 0;
}
UINT Thread_MPlayer(LPVOID pParam)
{
	Execute_(START_MPLAYER,0, Prog_Info.i_video_prio);
	return 0;
}

void COutputFilesPage::MEncoder(void)
{
	AfxBeginThread(Thread_MEncoder,this, THREAD_PRIORITY_ABOVE_NORMAL);
}

void COutputFilesPage::MPlayer(void)
{
	AfxBeginThread(Thread_MPlayer,this, THREAD_PRIORITY_BELOW_NORMAL);
}

int COutputFilesPage::CreateVirtualDubJobFile(int option)
{
	FILE *fp = NULL;
	int i = 0;
	CString vd_file;

	CString strOutputTotal;
	CString strInputAvi;
	CString fileExt = r_c_nfo[Enc_Info.i_container_format].s_file_ext;

	unsigned long begin = 0, end = 0;
	long l_frames = 0;
	long l_seconds = 0;
	double f_fps = 1.0;
	int i_avi_status = 0;
	int x = 1;

	vd_file.Format("%s\\%s", Enc_Info.s_TempFolder, VD_TMP);

	fp = fopen(vd_file, "wt");

	strInputAvi.Format("%s\\%s%s" , Enc_Info.s_TempFolder , Enc_Info.s_ProjName , _AVI);
	strOutputTotal.Format("%s\\%s%s" , Enc_Info.s_OutputFolder , Enc_Info.s_ProjName , 
		fileExt);

	(void) get_avi_info(strInputAvi.GetBuffer(), &f_fps, &l_frames, &l_seconds);

	//make compatible with virtualdub:
	strOutputTotal.Replace("\\","\\\\");
	strInputAvi.Replace("\\","\\\\");

	m_bSplitOK = Enc_Info.i_split;

	if (Enc_Info.i_split != SPLIT_NONE)
	{
		for (i = 0; i < MAXSPLIT; i++)
		{
			Enc_Info.l_split_frame[i] = 0;
		}

		if (Enc_Info.i_split == SPLIT_SIZE)
		{
			__int64 l_video_filesize = 0;
			long l_split_filesize = 0;
			long l_total_filesize = 0;

			i_avi_status = GetFileSizeInt64(strInputAvi.GetBuffer(), &l_video_filesize);

			l_total_filesize = (long) l_video_filesize;
			l_split_filesize = Enc_Info.l_split_at * MEGA;

			if (l_split_filesize > l_total_filesize)
			{
				m_bSplitOK = 0;
				Enc_Info.i_num_parts_created = 1;
			}
			else
			{
				int i_status = 0;
				long lSplitFrame = 0;
		
				for (i = 1; i < Enc_Info.i_num_parts_requested; i++)
				{
					i_status = filesize2frame(strInputAvi.GetBuffer(), 
						(long) (l_split_filesize * 0.99 * i), // 1% extra
						Enc_Info.i_audio_kbps * Enc_Info.b_create_audio,
						&lSplitFrame);

					if ((lSplitFrame > 0) && (i_status == 0))
					{
						Enc_Info.l_split_frame[i] = lSplitFrame;
					}
					else
					{
						break;
					}
				}

				Enc_Info.l_split_frame[i] = l_frames;

				Enc_Info.i_num_parts_created = i;
			}
		}
		else
		{
			// Split on fade-out
			SPLIT_DATA r_data = {0};

			int i_nof_sample_frames = 5; // Sample 5 frames
			int i_minumum_time_between_points = 30; // 30 seconds
			int i_threshold = 2; // Threshold value (if 5 frames are below 2kbyte its considred black)
			int i_jump_to_next_keyframe = 1; // Jump to next keyframe if a split point is found.

			// Maybe too much memory allocated, but its safe now.
			r_data.ai_points = (int*) calloc(1 + (l_frames / i_nof_sample_frames), sizeof(int));
			search_for_split_points(strInputAvi.GetBuffer(), 
				i_nof_sample_frames, 
				i_threshold, 
				i_minumum_time_between_points, 
				i_jump_to_next_keyframe, 
				&r_data);

			if (r_data.i_num_points > MAXSPLIT - 1)
			{
				r_data.i_num_points = MAXSPLIT - 1;
			}

			if (r_data.i_num_points == 1)
			{
				m_bSplitOK = 0;
			}
			else
			{
				for (i = 0 ; i < r_data.i_num_points; i++)
				{
					if (r_data.ai_points[i] > 0)
					{
						Enc_Info.l_split_frame[i + 1] = r_data.ai_points[i];
					}
					else
					{
						break;
					}
				}

				Enc_Info.l_split_frame[i + 1] = l_frames;

				Enc_Info.i_num_parts_created = i + 1;
			}

			free(r_data.ai_points);
		}
	}
	else
	{
		Enc_Info.i_num_parts_created = 1;
	}

	begin = 0;
	end = 0;

	CString strOutputTotalBackup = strOutputTotal;

	for (i = 0 ; i < Enc_Info.i_num_parts_created; i++)
	{
		if (m_bSplitOK)
		{
			CString part = "";
			part.Format("%d", i + 1);
			strOutputTotal = strOutputTotalBackup;
			strOutputTotal.Replace(fileExt," - Part " + part + fileExt);

			begin = (long) ((Enc_Info.l_split_frame[i]) * 1000.0 / Enc_Info.f_framerate);;
			end = (long) (((l_frames) - Enc_Info.l_split_frame[i+1]) * 1000.0 / Enc_Info.f_framerate);
		}

		// Only for 1st part
		if (i == 0)
		{
			// Add no-endcredits part
			if (m_bEndCreditsValid)
			{
				//fprintf(fp, "VirtualDub.Open(\"%s\",0,0);\n", strTempNo_End);
				//fprintf(fp, "VirtualDub.Append(\"%s\");\n", strTempEnd);
			}
			else
			{
				fprintf(fp, "VirtualDub.Open(\"%s\",0,0);\n", strInputAvi);
			}

			fprintf(fp, "VirtualDub.video.SetMode(0);\n");
			fprintf(fp, "VirtualDub.video.SetFrameRate(0,1);\n");
			fprintf(fp, "VirtualDub.video.SetRange(%d,%d);\n",begin,end);
			fprintf(fp, "VirtualDub.video.SetCompression();\n");
			fprintf(fp, "VirtualDub.video.filters.Clear();\n");
			fprintf(fp, "VirtualDub.Save%s(\"%s\");\n", r_c_nfo[Enc_Info.i_container_format].s_vdub_type, strOutputTotal);
		}
		else
		{
			fprintf(fp, "VirtualDub.video.SetRange(%d,%d);\n",begin,end);
			fprintf(fp, "VirtualDub.Save%s(\"%s\");\n", r_c_nfo[Enc_Info.i_container_format].s_vdub_type, strOutputTotal);
		}
	}

	fclose(fp);

	return 0;
}

void COutputFilesPage::VirtualDub()
{
	Execute_(START_VIRTUALDUB,0, Prog_Info.i_disk_prio);
}

void COutputFilesPage::execute_steps(int option)
{
	int b_end = 0;
	int i = 0;
	CString filename;

	HCURSOR cursor_normal = LoadCursor(NULL, IDC_ARROW);
	HCURSOR cursor_wait = LoadCursor(NULL, IDC_WAIT); 

	switch(g_step)
	{
	case STEP_INIT:

		if (option == 0)
		{
			// 1-pass mode only
			g_step = STEP_CORRECTAC3;
		}
		else
		{
			// divx/xvid/h264 mode , 2-pass mode
			if (Enc_Info.i_pass_mode == 0)
			{
				// only 1ste pass
				g_step = STEP_DONE;
			}

			if (Enc_Info.i_pass_mode == 2)
			{
				g_step = STEP_MENCODER2;
			}
		}

		break;

	case STEP_MENCODER2:
		KillTimer(TIMER);

		// poll timer for ending MEncoder.
		m_intMEncoderTimer = SetTimer(MENCODER_TIMER2, MENCODER_POLL_TIMER , NULL);

		// MEncoder and so on...
		g_pass = 2;
		//KillTimer(TIMER);
		Log("Starting 2nd-pass from 2-pass...");
		Log("MEncoder started");

		MEncoder();
		break;

	case STEP_COMBINE:

		if (g_combining_started	== 0)
		{
			g_combining_started	= 1;

			if ((Enc_Info.i_split != SPLIT_NONE) || (Enc_Info.i_container_format != CF_AVI))
			{
				CreateVirtualDubJobFile(0);
			}

			if (m_bSplitOK)
			{
				Log("Splitting AVI.");
			}

			if ((m_bSplitOK) || (Enc_Info.i_container_format != CF_AVI))
			{
				VirtualDub();
			}

			g_step = STEP_SUBTITLES;
		}

		if (g_combining_ready)
		{
			g_step = STEP_SUBTITLES;
		}


		m_ctrlProgress.SetPos(g_step + g_batch_counter * STEP_DONE);

		break;

	case STEP_SUBTITLES:

		if (Prog_Info.b_can_use_vobsub)
		{
			if (Enc_Info.b_create_subtitles)
			{
				if ((Enc_Info.b_extract_all_subtitles) || (Enc_Info.i_numSelectedSubs > 0))
				{
					char baseFileSrc[STRLEN] = {0};
					char baseFileDst[STRLEN] = {0};
					char srcFile[STRLEN] = {0};
					char dstFile[STRLEN] = {0};
					CString s_cmdLine;

					Log("Creating Subtitle(s) started.");
					CreateVobSubCommandLine();

					s_cmdLine = Enc_Info.subtitles_info.commandLine;
					Execute_(START_VOBSUB, s_cmdLine.GetBuffer(), NORMAL_PRIORITY);

					if (m_bSplitOK)
					{
						for (i = 0 ; i < Enc_Info.subtitles_info.i_num_parts ; i++)
						{
							s_cmdLine = Enc_Info.subtitles_info.splitCommandLine[i];
							Execute_(START_VOBSUB, s_cmdLine.GetBuffer(), NORMAL_PRIORITY);
						}

						for (int i = 1; i <= Enc_Info.subtitles_info.i_num_parts ; i++)
						{
							sprintf(baseFileSrc, "%s%d",
								Enc_Info.subtitles_info.projName,
								i);

							sprintf(baseFileDst, "%s\\%s%s%d",
								Enc_Info.s_OutputFolder,
								Enc_Info.s_ProjName,
								PART,
								i);

							sprintf(srcFile, "%s%s", baseFileSrc, VOBSUB_IDX);
							sprintf(dstFile, "%s%s", baseFileDst, VOBSUB_IDX);

							MoveFile(srcFile, dstFile);

							sprintf(srcFile, "%s%s", baseFileSrc, VOBSUB_SUB);
							sprintf(dstFile, "%s%s", baseFileDst, VOBSUB_SUB);

							MoveFile(srcFile, dstFile);

							sprintf(srcFile, "%s%s", Enc_Info.subtitles_info.projName, VOBSUB_IDX);
							DeleteFile(srcFile);
							sprintf(srcFile, "%s%s", Enc_Info.subtitles_info.projName, VOBSUB_SUB);
							DeleteFile(srcFile);
						}
					}
					else
					{
						sprintf(baseFileSrc, "%s",
							Enc_Info.subtitles_info.projName);

						sprintf(baseFileDst, "%s\\%s",
							Enc_Info.s_OutputFolder,
							Enc_Info.s_ProjName);

						sprintf(srcFile, "%s%s", baseFileSrc, VOBSUB_IDX);
						sprintf(dstFile, "%s%s", baseFileDst, VOBSUB_IDX);

						MoveFile(srcFile, dstFile);

						sprintf(srcFile, "%s%s", baseFileSrc, VOBSUB_SUB);
						sprintf(dstFile, "%s%s", baseFileDst, VOBSUB_SUB);

						MoveFile(srcFile, dstFile);
					}

					Log("Creating Subtitle(s) finished.");
				}
			}
		}

		g_step = STEP_MKVMERGE;

		break;

	case STEP_CORRECTAC3:
		if ((Enc_Info.i_audio_format == AUDIO_TYPE_AC3) && (!Enc_Info.b_convert_ac3_to_2channels))
		{
			int channels = Enc_Info.ar_ac3_info[Enc_Info.i_audio_stream_id].i_num_channels;

			if ((Enc_Info.i_split != SPLIT_NONE) || (Enc_Info.i_container_format != CF_AVI))
			{
				filename = Enc_Info.s_TempFolder + "\\" + Enc_Info.s_ProjName + _AVI;
			}
			else
			{
				filename = Enc_Info.s_OutputFolder + "\\" + Enc_Info.s_ProjName + AVI;
			}

			CorrectAC3HeaderInAvi(filename.GetBuffer(), channels);
		}

		g_step = STEP_COMBINE;

		m_ctrlProgress.SetPos(g_step + g_batch_counter * STEP_DONE);

		break;

	case STEP_MKVMERGE:

		/*
		if (g_mkvmerge_started == 0)
		{
			g_mkvmerge_started = 1;

			Enc_Info.CreateMKVMergeExecLine();
		}*/

		g_step = STEP_DONE;

		break;

	case STEP_DONE:
		KillTimer(TIMER);
		m_ctrlProgress.SetPos(g_step + g_batch_counter * STEP_DONE);
		PrintFinalLog();
		if (Prog_Info.b_RunAutomated)
		{
			exit(ERROR_SUCCESS);
		}
		break;

	default:
		break;
	}
}

void COutputFilesPage::PrintFinalLog()
{
	CString s_tmp;
	int i = 0;
	
	// TODO !!!
	if (Enc_Info.i_container_format == CF_AVI)
	{
		s_tmp = AVI;
	}
	else if (Enc_Info.i_container_format == CF_OGM)
	{
		s_tmp = OGM;
	}
	else if (Enc_Info.i_container_format == CF_MATROSKA)
	{
		s_tmp = MATROSKA;
	}
	
	if (g_encoding_error == 0)
	{
		if ((Enc_Info.i_pass_mode == 0) && (Enc_Info.i_create_avi_options != 0))
		{
			Log("1st pass done...");	
		}
		else
		{
			CString filename;

			if (m_bSplitOK)
			{
				for (i = 0 ; i < Enc_Info.i_num_parts_created; i++)
				{
					CString logLine;
					filename.Format("%s\\%s%s%d%s", Enc_Info.s_OutputFolder,
						Enc_Info.s_ProjName,
						PART,
						i + 1,
						s_tmp);
					logLine.Format("Outputfile %s is created.", filename);
					Log(logLine);
				}
			}
			else
			{
				if ((Enc_Info.i_split != SPLIT_NONE) && (Enc_Info.i_container_format == CF_AVI))
				{
					Log("Outputfile " + Enc_Info.s_TempFolder + "\\" + Enc_Info.s_ProjName + _AVI + " is created.");
				}
				else
				{
					Log("Outputfile " + Enc_Info.s_OutputFolder + "\\" + Enc_Info.s_ProjName + s_tmp + " is created.");
				}
			}

			if (Enc_Info.b_delete_temp)
			{
				Log("Deleting Temp files...");
				CheckOverWrite_AllFiles(Enc_Info.b_delete_temp);
			}

			Log("Done...\r\n");
		}
		Print_Info_File(0);

		if (Prog_Info.b_batch_enabled == 0)
		{
			g_all_ready = 1;
		}
		else
		{
			g_batch_ready = 1;
		}

	}
	else
	{
		g_batch_ready = 1;
	}

	int cnt = m_ctrlEdit.GetLineCount();
	m_ctrlEdit.LineScroll(cnt + 1, 0);

	if ((g_all_ready) || ((Prog_Info.b_batch_enabled && ((i_num_projects - g_failed_batch_counter) == g_batch_counter))))
		//|| (Prog_Info.b_batch_enabled == 0)))
	{
		char s_tmp[STRLEN + 32] = {0};
		g_all_ready = 0;

		Log("All Done...");

		if (Prog_Info.b_batch_enabled)
		{
			sprintf(s_tmp, "%s\\%s", Enc_Info.s_OutputFolder, 
				BATCH_LOG);
		}
		else
		{
			sprintf(s_tmp, "%s\\%s%s", Enc_Info.s_OutputFolder, 
				Enc_Info.s_ProjName, LOG);
		}

		FILE *fp;
		fp = fopen(s_tmp, "wt");
		if (fp)
		{
			CString s_tmp = m_strInfo;
			s_tmp.Replace("\r", "");
			fprintf(fp,"%s", s_tmp);
			fclose(fp);
		}

		g_started = 0;
		GetDlgItem(IDC_START)->SetWindowText("Start");
		if (m_bShutDown)
		{
			int m_intShutdownTimer = 0;
			m_intShutdownTimer = SetTimer(SHUTDOWN_TIMER, 60000 , NULL);
			int i_status = IDCANCEL;
			i_status = MessageBox("The system will shutdown in 60 seconds.\n\nPress 'Cancel' if you want to abort shutdown.\nPress 'OK' if you want to shutdown now.", 
				"Shutting down...",
				MB_ICONEXCLAMATION | MB_OKCANCEL | MB_DEFBUTTON2 | MB_TOPMOST);

			if (i_status == IDCANCEL)
			{
				// stop shutdown
				KillTimer(SHUTDOWN_TIMER);
			}
			else if (i_status == IDOK)
			{
				Write_SimpleDivXSettings(Prog_Info, Enc_Info);

				Config_Data.Write_Settings();

				Log("System will shutdown now...");

				int i_shutdown_result = ShutDown_(1);
			}
		}
	}
}

int COutputFilesPage::CheckOverWrite(CString s_filename, 
									 char type, 
									 BOOL overwrite)
{
	FILE *fp_test;
	int msgbox_result = IDYES;
	CString info;
	CString tmp;

	if (!overwrite)
	{
		if (type == 'A') tmp = "audio";
		if (type == 'V') tmp = "video";
		if (type == 'F') tmp = "total";
		if (type == 'T') tmp = "temp";
		if (type == 'L') tmp = "log";

		fp_test = fopen(s_filename.GetBuffer(),"r");

		if (fp_test != NULL)
		{
			fclose(fp_test);

			info.Format(
				"Specified %s file %s does exist.\nOverwrite ?", 
				tmp , 
				s_filename);

			msgbox_result = MessageBox(info, "WARNING", MB_ICONEXCLAMATION | MB_YESNO);
		}
	}

	if (msgbox_result == IDYES)
	{
		// remove file
		TRY
		{
			CFile::Remove(s_filename);
		}
		CATCH(CFileException, pEx) { }
		END_CATCH
	}

	return msgbox_result;
}

void COutputFilesPage::My_Static_Init()
{
	CMyToolTips my_tooltips;
	my_tooltips.PrepareTooltips(this, my_page, 
		m_pTooltip);
}

BOOL COutputFilesPage::PreTranslateMessage(MSG* pMsg) 
{
	if (NULL != m_pTooltip) m_pTooltip->RelayEvent(pMsg);

	return CPropertyPage::PreTranslateMessage(pMsg);
}

void COutputFilesPage::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (NULL != m_pTooltip) m_pTooltip->Activate(TRUE);		
	CPropertyPage::OnMouseMove(nFlags, point);
}

int COutputFilesPage::ShutDown_(int mode)
{
	//--------------------------------------------------------------------------

	// Various shutdown methods
	// 1 = Forcefully close apps, shutdown and Power Off
	// 2 = Forcefully close apps, shutdown and leave system at shutoff point
	// 3 = Forcefully close apps, shutdown and reboot
	// 4 = Forcefully close apps, and logoff
	//--------------------------------------------------------------------------
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	CString error;

	// Get a token for this process.

	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		error = "OpenProcessToken";
	}

	// Get the LUID for the shutdown privilege.
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;  // one privilege to set
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get the shutdown privilege for this process.
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	// Cannot test the return value of AdjustTokenPrivileges.
	if (GetLastError() != ERROR_SUCCESS)
	{
		error = "AdjustTokenPrivileges";
	}

	switch (mode)
	{
	case 1:
		if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE | EWX_POWEROFF, 0))
		{
			error = "ExitWindowsEx";return 0;
		}
		else return 1;
		break;
	case 2:
		if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0))
		{
			error = "ExitWindowsEx";return 0;
		}
		else return 1;
		break;
	case 3:
		if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0))
		{
			error = "ExitWindowsEx";return 0;
		}
		else return 1;
		break;
	case 4:
		if (!ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, 0))
		{
			error = "ExitWindowsEx";return 0;
		}
		else return 1;
		break;
	default:
		break;
	}

	return 1; // error
}

void COutputFilesPage::CreateVobSubCommandLine(void)
{
	FILE *fp;
	CString vobsubFile;
	char subList[STRLEN] = {0};

	vobsubFile.Format("%s\\%s%s", 
		Enc_Info.s_TempFolder, 
		Enc_Info.s_ProjName, 
		VOBSUB_TMP);

	// ALL
	if (Enc_Info.b_extract_all_subtitles)
	{
		strcpy(subList, "ALL");
	}
	else
	{
		for (int i = 0 ; i < Enc_Info.i_numSelectedSubs ; i++)
		{
			strcat(subList, Enc_Info.as_subs_short[Enc_Info.selectedSubs[i]]);
			strcat(subList, " ");
		}
	}

	Enc_Info.subtitles_info.projName.Format("%c:\\%s%s", 
		Enc_Info.s_OutputFolder[0],
		APP_NAME,
		VOBSUB_TMP);

	fp = fopen(vobsubFile.GetBuffer(), "wt");

	if (fp)
	{
		fprintf(fp, "%s\n", Enc_Info.r_ifo_files[Enc_Info.i_selected_ifo_id].s_filename);
		fprintf(fp, "%s\n", Enc_Info.subtitles_info.projName);
		fprintf(fp, "%d\n", 1);
		fprintf(fp, "%d\n", 0);
		fprintf(fp, "%s\n", subList);
		fprintf(fp, "CLOSE");
		fclose(fp);
	}

	// Create commandline for creating subtitles
	Enc_Info.subtitles_info.i_num_parts = 0;
	Enc_Info.subtitles_info.commandLine.Format( 
		"\"%s\\%s\",Configure %s",
		Prog_Info.strVobSubFolder,
		VOBSUB_DLL, 
		vobsubFile);

	if (m_bSplitOK)
	{
		// In some way, vobsub does not support long filenames
		// therefore, the splitted files are written to
		// the root from the temp-drive.
		char sBase[STRLEN512] = {0};
		sprintf(sBase, "\"%s\\%s\",Cutter %s %s",
			Prog_Info.strVobSubFolder,
			VOBSUB_DLL,
			Enc_Info.subtitles_info.projName, 
			Enc_Info.subtitles_info.projName);

		Enc_Info.subtitles_info.i_num_parts = Enc_Info.i_num_parts_created;

		for (int i = 0 ; i < Enc_Info.subtitles_info.i_num_parts; i++)
		{
			char StartTimeFormat[STRLEN32] = {0};
			char EndTimeFormat[STRLEN32] = {0};

			milliseconds2timeformat(1000.0 * Enc_Info.l_split_frame[i] / Enc_Info.f_framerate, 
				StartTimeFormat);

			milliseconds2timeformat(1000.0 * Enc_Info.l_split_frame[i + 1] / Enc_Info.f_framerate, 
				EndTimeFormat);

			Enc_Info.subtitles_info.splitCommandLine[i].Format("%s%d 1 %s %s",
				sBase,
				i + 1,
				StartTimeFormat,
				EndTimeFormat);
		}
	}
}

void COutputFilesPage::OnCheckOverwriteAvi() 
{
	UpdateData(1);
}

void COutputFilesPage::OnCheckShutdown() 
{
	UpdateData(1);
	Prog_Info.b_shutdown = m_bShutDown;
}

void COutputFilesPage::PrintVars()
{
	int i = 0;
	int j = 0;

	FILE *fp = NULL;
	CString s_tmp;

	s_tmp.Format("%s\\%s", Enc_Info.s_TempFolder, DEBUGFILE);

	fp = fopen(s_tmp.GetBuffer(), "wt");

	if (fp)
	{
		fprintf(fp, "Version:               = %s\n\n", APP_VERSION);

		fprintf(fp, "[Audio]\n");
		fprintf(fp, "amplify_wave           = %d\n", Enc_Info.b_amplify_audio);
		fprintf(fp, "norm_wave              = %d\n", Enc_Info.i_norm_wave);
		fprintf(fp, "create_audio           = %d\n", Enc_Info.b_create_audio);
		fprintf(fp, "audio_format           = %d\n", Enc_Info.i_audio_format);
		fprintf(fp, "audio_kbps             = %d\n", Enc_Info.i_audio_kbps);
		fprintf(fp, "audio_stream_id        = %d\n", Enc_Info.i_audio_stream_id);
		fprintf(fp, "enable_audiocheck      = %d\n", Enc_Info.b_enable_audiocheck);
		fprintf(fp, "hertz                  = %d\n", Enc_Info.i_hertz);
		fprintf(fp, "valid_languages        = %d\n", Enc_Info.i_valid_languages);
		for (i = 0 ; i < Enc_Info.i_valid_languages ; i++)
		{
			fprintf(fp, "audio_languages[%d]     = %s\n", i, Enc_Info.as_audio_languages[i]);
			fprintf(fp, "language_id[%d]         = %X\n", i, Enc_Info.ai_language_id[i]);     
			fprintf(fp, "language_type[%d]       = %d\n", i, Enc_Info.ai_language_type[i]);
		}

		fprintf(fp, "\n[Codec]\n");
		fprintf(fp, "Codec Version          = %d\n", Enc_Info.i_codec_version);
		
		fprintf(fp, "\n[Video]\n");
		fprintf(fp, "video_id               = %X\n", Enc_Info.ai_video_id[0]);        
		fprintf(fp, "auto_size              = %d\n", Enc_Info.b_auto_size);
		fprintf(fp, "enable_videocheck      = %d\n", Enc_Info.b_enable_videocheck);    
		fprintf(fp, "enter_filesize         = %d\n", Enc_Info.b_enter_filesize);       
		fprintf(fp, "gettimefromvob         = %d\n", Enc_Info.b_gettimefromvob);       
		fprintf(fp, "halfresolution         = %d\n", Enc_Info.b_halfresolution);       
		fprintf(fp, "selectframerange       = %d\n", Enc_Info.b_selectrange);     
		fprintf(fp, "ratio                  = %f\n", Enc_Info.d_ratio);                
		fprintf(fp, "framerate              = %f\n", Enc_Info.f_framerate);            
		fprintf(fp, "create_avi_options     = %d\n", Enc_Info.i_create_avi_options);   
		fprintf(fp, "cx                     = %d\n", Enc_Info.i_cx);                   
		fprintf(fp, "cy                     = %d\n", Enc_Info.i_cy);                   
		fprintf(fp, "dx                     = %d\n", Enc_Info.i_dx);                   
		fprintf(fp, "dy                     = %d\n", Enc_Info.i_dy);                   
		fprintf(fp, "end_frame              = %d\n", Enc_Info.i_end_time);            
		fprintf(fp, "filesize               = %d\n", Enc_Info.i_filesize);             
		fprintf(fp, "found_video_streams    = %d\n", Enc_Info.i_found_video_streams);  
		fprintf(fp, "framerate_id           = %d\n", Enc_Info.i_framerate_id);         
		fprintf(fp, "max_keyframe_interval  = %d\n", Enc_Info.i_max_keyframe_interval);
		fprintf(fp, "original_height        = %d\n", Enc_Info.i_original_height);      
		fprintf(fp, "original_width         = %d\n", Enc_Info.i_original_width);       
		fprintf(fp, "outputmode_id          = %d\n", Enc_Info.i_outputmode_id);        
		fprintf(fp, "start_frame            = %d\n", Enc_Info.i_start_time);
		fprintf(fp, "video_output_format_id = %d\n", Enc_Info.i_video_output_format_id);
		fprintf(fp, "frames                 = %d\n", Enc_Info.l_frames);       
		fprintf(fp, "frames_chosen          = %ld\n", Enc_Info.l_frames_chosen);
		fprintf(fp, "split_at               = %ld\n", Enc_Info.l_split_at);         
		fprintf(fp, "total_length           = %ld\n", Enc_Info.l_total_length);
		fprintf(fp, "format                 = %s\n", Enc_Info.s_movie_format);
		fprintf(fp, "pass mode              = %ld\n", Enc_Info.i_pass_mode);

		fprintf(fp, "\n[Input]\n");
		for (i = 0 ; i < Enc_Info.i_number_ifofiles ; i++)
		{
			fprintf(fp, "IFO [%d] :\n", i);
			fprintf(fp, "valid_ifo              = %d\n", Enc_Info.r_ifo_files[i].b_valid_ifo);       
			fprintf(fp, "num_vobfiles           = %d\n", Enc_Info.r_ifo_files[i].i_num_vobfiles);
			fprintf(fp, "filename               = %s\n", Enc_Info.r_ifo_files[i].s_filename);
			fprintf(fp, "short_filename         = %s\n", Enc_Info.r_ifo_files[i].s_short_filename);
			fprintf(fp, "num audio streams      = %d\n", Enc_Info.r_ifo_files[i].r_movie_info.i_audio_streams);
			for (j = 0 ; j < Enc_Info.r_ifo_files[i].r_movie_info.i_audio_streams ; j++)
			{
				fprintf(fp, "audio [%d] :\n", j);
				fprintf(fp, "id                     = %d\n", Enc_Info.r_ifo_files[i].r_movie_info.audio_list[j]);
				fprintf(fp, "string                 = %s\n", Enc_Info.r_ifo_files[i].r_movie_info.as_audio_languages[j]);
			}
			fprintf(fp, "num subs               = %d\n", Enc_Info.r_ifo_files[i].r_movie_info.i_num_subs);
			for (j = 0 ; j < Enc_Info.r_ifo_files[i].r_movie_info.i_num_subs ; j++)
			{
				fprintf(fp, "sub [%d] :\n", j);
				fprintf(fp, "id                     = %d\n", Enc_Info.r_ifo_files[i].r_movie_info.sub_list[j]);
				fprintf(fp, "string                 = %s\n", Enc_Info.r_ifo_files[i].r_movie_info.as_subs[j]);
				fprintf(fp, "string short           = %s\n", Enc_Info.r_ifo_files[i].r_movie_info.as_subs_short[j]);
			}
			fprintf(fp, "num titles             = %d\n", Enc_Info.r_ifo_files[i].r_movie_info.i_num_titles);
			for (j = 0 ; j < Enc_Info.r_ifo_files[i].r_movie_info.i_num_titles ; j++)
			{
				fprintf(fp, "title [%d] :\n", j);
				fprintf(fp, "number chapters        = %d\n", Enc_Info.r_ifo_files[i].r_movie_info.r_titles[j].i_num_chapters);
				fprintf(fp, "length in seconds      = %d\n", Enc_Info.r_ifo_files[i].r_movie_info.r_titles[j].l_length);
			}
			fprintf(fp, "start_title_id         = %d\n", Enc_Info.r_ifo_files[i].r_movie_info.i_start_title_id);
			fprintf(fp, "end_title_id           = %d\n", Enc_Info.r_ifo_files[i].r_movie_info.i_end_title_id);
	
			for (j = 0 ; j < Enc_Info.r_ifo_files[i].i_num_vobfiles ; j++)
			{
				fprintf(fp, "VOB [%d] :                 \n", j);
				fprintf(fp, "l_size                 = %ld\n", Enc_Info.r_ifo_files[i].vob_files[j].l_size);
				fprintf(fp, "r_type                 = %d\n", Enc_Info.r_ifo_files[i].vob_files[j].r_type);
				fprintf(fp, "filename               = %s\n", Enc_Info.r_ifo_files[i].vob_files[j].s_filename);
				fprintf(fp, "short_filename         = %s\n", Enc_Info.r_ifo_files[i].vob_files[j].s_short_filename);
			}
			fprintf(fp, "\n");
		}

		fprintf(fp, "\n[System]\n");
		//fprintf(fp, "cpu_id                 = %d\n", Prog_Info.cpu_model);
		fprintf(fp, "language               = %s\n", Prog_Info.currentLanguage);
		fprintf(fp, "batch-mode             = %d\n", Prog_Info.b_batch_enabled);
		fprintf(fp, "overwrite              = %d\n", Enc_Info.b_overwrite);
		fprintf(fp, "delete_temp            = %d\n", Enc_Info.b_delete_temp);
		fprintf(fp, "projectname            = %s\n", Enc_Info.s_ProjName);
		fprintf(fp, "auto_logfile_naming    = %d\n", Enc_Info.b_auto_logfile_naming);
		fprintf(fp, "programs_found         = %d\n", Prog_Info.b_programs_found);       
		fprintf(fp, "split                  = %d\n", Enc_Info.i_split);                
		fprintf(fp, "start_at_lba           = %d\n", Enc_Info.b_start_at_lba);         
		fprintf(fp, "VobFolder              = %s\n", Enc_Info.sVobFolder);
		fprintf(fp, "OutputFolder           = %s\n", Enc_Info.s_OutputFolder);
		fprintf(fp, "TempFolder             = %s\n", Enc_Info.s_TempFolder);
		fprintf(fp, "MEncoderFolder         = %s\n", Prog_Info.strMEncoderFolder);
		fprintf(fp, "MPlayerFolder          = %s\n", Prog_Info.strMPlayerFolder);
		fprintf(fp, "VirtualDubFolder       = %s\n", Prog_Info.strVirtualDubFolder);    

		fclose(fp);
	}
}

void COutputFilesPage::CalcNewBitrate(int *pi_new, 
									  int i_endkbps,
									  long l_end_credits_frame,
									  long l_tot_frames)
{
	int audio_bitrate = 0;
	int system_bitrate = 0;
	int video_bitrate = 0;
	int i_seconds = 0;
	int i_main_video = 0;
	int i_end_video = 0;

	if (Enc_Info.i_endcredits_option == REMOVE_END)
	{
		i_seconds = int (l_end_credits_frame / Enc_Info.f_framerate);
	}
	
	audio_bitrate = Enc_Info.b_create_audio * Enc_Info.i_audio_kbps;
	system_bitrate = (Enc_Info.i_filesize * KILO * 8 ) / i_seconds;
	video_bitrate = system_bitrate - audio_bitrate;

	if ((Enc_Info.i_endcredits_option == REMOVE_END) || (l_tot_frames == l_end_credits_frame))
	{
		*pi_new = (int) video_bitrate;
	}
	
	if (*pi_new > MAXDIVX5KBPS)
	{
		*pi_new = MAXDIVX5KBPS-1;
	}
	
	if (*pi_new < MINDIVX5KBPS)
	{
		*pi_new = MINDIVX5KBPS;
	}
}

int COutputFilesPage::CheckOverWrite_AllFiles(int setting)
{
	CString filename;
	int msgbox_result = IDYES;
	BOOL m_bOverwrite = Enc_Info.b_overwrite;

	if (setting == 2)
	{
		// Ask to overwrite final avi file.
		filename.Format("%s\\%s%s", Enc_Info.s_OutputFolder, Enc_Info.s_ProjName , AVI);
		msgbox_result = CheckOverWrite(filename, 'F', Enc_Info.b_overwrite);

		// Ask to overwrite final avi files. (split files, -Part 1, - Part 2...)
		if (msgbox_result == IDYES)
		{
			for (int i = 0; i < (Enc_Info.i_num_parts_created) && (msgbox_result == IDYES); i++)
			{
				filename.Format("%s\\%s%d%s", Enc_Info.s_OutputFolder, Enc_Info.s_ProjName, 0, AVI);
				msgbox_result = CheckOverWrite(filename, 'F', Enc_Info.b_overwrite);
			}
		}

		// Ask to overwrite the temp avi file.
		if (msgbox_result == IDYES)
		{
			filename.Format("%s\\%s%s", Enc_Info.s_TempFolder, Enc_Info.s_ProjName, _AVI);
			msgbox_result = CheckOverWrite(filename, 'T', m_bOverwrite);
		}
	}

	// only done when delete temp files is YES so only in the END. 
	if (setting == 1)
	{
		// If splitting was requested but was needed/valid OR when container != AVI 
		// --> delete _avi file.
		if (((Enc_Info.i_split != SPLIT_NONE) && (m_bSplitOK)) || (Enc_Info.i_container_format != CF_AVI))
		{
			if (msgbox_result == IDYES)
			{
				filename.Format("%s\\%s%s", Enc_Info.s_TempFolder, Enc_Info.s_ProjName, _AVI);
				msgbox_result = CheckOverWrite(filename, 'T', m_bOverwrite);
			}
		}
	}

	// Ask to overwrite the VirtaulDub temp file.
	if (msgbox_result == IDYES)
	{
		filename.Format("%s\\%s", Enc_Info.s_TempFolder, VD_TMP);
		msgbox_result = CheckOverWrite(filename, 'T', TRUE);
	}

	// Ask to overwrite the VOBSUB temp file.
	if (msgbox_result == IDYES)
	{
		filename.Format("%s\\%s", Enc_Info.s_TempFolder, VOBSUB_TMP);
		msgbox_result = CheckOverWrite(filename, 'T', TRUE);
	}

	return msgbox_result;
}

void COutputFilesPage::OnKillFocus(CWnd* pNewWnd) 
{
	CPropertyPage::OnKillFocus(pNewWnd);
}

int COutputFilesPage::CheckFolders()
{
	int result = 0;
	CString tmp = Enc_Info.s_TempFolder;
	CString output = Enc_Info.s_OutputFolder;
	CString s_tmp;
	long l_needed_tmp = 0;
	long l_needed_output = 0;
	PULARGE_INTEGER TotalNumberOfBytes = NULL;
	PULARGE_INTEGER FreeBytesAvailableToCaller = NULL;
	PULARGE_INTEGER TotalNumberOfFreeBytes = NULL;

	tmp.TrimRight();
	output.TrimRight();
	tmp.MakeLower();
	output.MakeLower();

	// check if equal :
	if (0 == tmp.CompareNoCase(output))
	{
		result = 1;
	}
	else
	{
		//long ac3_size = 0;
		//long audio_size = 0;
		//long one_audio_size = 0;
		int i = 0;
		TotalNumberOfFreeBytes = (PULARGE_INTEGER) calloc(1, sizeof(ULARGE_INTEGER));
		TotalNumberOfBytes = (PULARGE_INTEGER) calloc(1, sizeof(ULARGE_INTEGER));
		FreeBytesAvailableToCaller = (PULARGE_INTEGER) calloc(1, sizeof(ULARGE_INTEGER));

		// Check if exist, if not create.
		CreatePath(Enc_Info.s_TempFolder);
		CreatePath(Enc_Info.s_OutputFolder);

//		ac3_size = ((Enc_Info.l_seconds_chosen * Enc_Info.ar_ac3_info[Enc_Info.i_audio_stream_id].i_kbps / 8) / KILO);
//		audio_size = ((Enc_Info.l_seconds_chosen * Enc_Info.i_audio_kbps / 8) / KILO);


//		one_audio_size = ((Enc_Info.l_seconds_chosen * Enc_Info.i_audio_kbps / 8) / KILO);

		l_needed_tmp = Enc_Info.i_filesize;
		l_needed_output = Enc_Info.i_filesize;
		
		if (tmp.GetAt(0) == output.GetAt(0))
		{
			// same drive
			long l_needed = l_needed_output + l_needed_tmp;

			// check diskspace
			(void) GetDiskFreeSpaceEx(tmp, 
				FreeBytesAvailableToCaller,
				TotalNumberOfBytes,
				TotalNumberOfFreeBytes);

			if ((TotalNumberOfFreeBytes->QuadPart / MEGA) < l_needed)
			{
				if (TotalNumberOfFreeBytes->QuadPart == 0)
				{
					result = 4;
				}
				else
				{
					result = 2;
				}
			}
		}
		else
		{
			// check diskspace
			(void) GetDiskFreeSpaceEx(tmp, 
				FreeBytesAvailableToCaller,
				TotalNumberOfBytes,
				TotalNumberOfFreeBytes);

			if ((TotalNumberOfFreeBytes->QuadPart / MEGA) < l_needed_tmp)
			{
				if (TotalNumberOfFreeBytes->QuadPart == 0)
				{
					result = 4;
				}
				else
				{
					result = 2;
				}
			}

			(void) GetDiskFreeSpaceEx(output, 
				FreeBytesAvailableToCaller,
				TotalNumberOfBytes,
				TotalNumberOfFreeBytes);

			if ((TotalNumberOfFreeBytes->QuadPart / MEGA) < l_needed_output)
			{
				if (TotalNumberOfFreeBytes->QuadPart == 0)
				{
					result = 5;
				}
				else
				{
					result = 3;
				}
			}
		}

		free(TotalNumberOfFreeBytes);
		free(TotalNumberOfBytes);
		free(FreeBytesAvailableToCaller);
	}

	switch(result)
	{
	case 1:
		s_tmp = "Temp Folder and Output Folder are the same!";
		break;
	case 2:
		s_tmp.Format("You need at least %ld MegaBytes free on %s .", 
			l_needed_tmp,
			Enc_Info.s_TempFolder);
		break;
	case 3:
		s_tmp.Format("You need at least %ld MegaBytes free on %s .", 
			l_needed_output,
			Enc_Info.s_OutputFolder);
		break;
	case 4:
		s_tmp.Format("'%s' is invalid as temp folder .", 
			Enc_Info.s_TempFolder);
		break;
	case 5:
		s_tmp.Format("'%s' is invalid as output folder .", 
			Enc_Info.s_TempFolder);
	default:
		break;
	};

	if (result != 0)
	{
		if (Prog_Info.b_batch_enabled)
		{
			Log(s_tmp);
		}
		else
		{
			AfxMessageBox(s_tmp);
		}
	}

	return result;
}

void COutputFilesPage::AbortConversion(int i_option)
{
	int i_num = 0;

	// vars
	g_started = 0;
	g_step = STEP_START;

	// kill timers
	KillTimer(TIMER);
	KillTimer(MENCODER_TIMER);
	KillTimer(MENCODER_TIMER2);
	KillTimer(SHUTDOWN_TIMER);
	KillTimer(BATCH_TIMER);
	
	// closing all processes :
	for (i_num = 0; i_num < gi_num_handles ; i_num++)
	{
		if (r_handle[i_num] != 0)
		{
			(void) TerminateProcess(r_handle[i_num], 0);
			r_handle[i_num] = 0;
		}
	}

	// closing all threads
	for (i_num = 0; i_num < gi_num_threads ; i_num++)
	{
		if (r_thread[i_num] != 0)
		{
			(void) TerminateThread(r_thread[i_num], 0);
			r_thread[i_num] = 0;
		}
	}

	// updating dialog
	GetDlgItem(IDC_START)->SetWindowText("Start");
	Log("Conversion Aborted...");
	m_ctrlProgress.SetPos(STEP_START);
}

void COutputFilesPage::PrepareVars(int i_option)
{
	int i_num_handles = 0;

	if (i_option == 0)
	{
		for (i_num_handles = 0 ; i_num_handles < MAXHANDLES ; i_num_handles++)
		{
			r_handle[i_num_handles] = 0;
		}

		//	g_batch_counter = 0;
		g_started = 1;
		g_MEncoder_finished = 0;
		g_MEncoder_started = 0;
		g_step = STEP_START;
		g_pass = 1;
		gi_num_handles = 0;
		gi_num_threads = 0;
		m_bEndCreditsValid = 0;
		m_bSplitOK = 0;
		g_combining_ready = 0;
		g_combining_started = 0;
		g_all_ready = 0;
		g_batch_counter = 0;
		g_failed_batch_counter = 0;
		g_encoding_error = 0;
		//g_batch_ready = 0;
		g_mkvmerge_ready = 0;
		g_mkvmerge_started = 0;
	}
	else
	{
		g_MEncoder_finished = 0;
		g_MEncoder_started = 0;
		//g_step = STEP_START;
		g_pass = 1;
		gi_num_handles = 0;
		gi_num_threads = 0;
		m_bEndCreditsValid = 0;
		m_bSplitOK = 0;
		g_combining_ready = 0;
		g_combining_started = 0;
		g_all_ready = 0;
		//g_batch_counter = 0;
		g_encoding_error = 0;
		//g_batch_ready = 0;
		g_mkvmerge_ready = 0;
		g_mkvmerge_started = 0;
	}
}

int COutputFilesPage::CheckTabs()
{
	return 0;
}

void COutputFilesPage::Do_EndCredits()
{
	/*
	CString s_tmp;
	char hms[16] = {0};
	int i_version = 0;

	for (int i = 0 ; i < _LAST_CODEC ; i++)
	{
		if (Prog_Info.ar_vci[i].i_id == Enc_Info.i_codec_version)
		{
			i_version = Prog_Info.ar_vci[i].i_version;
			break;
		}
	}

	i_tmp_kbps = Enc_Info.divxauto_info.bitrate;

	if (Enc_Info.i_endcredits_option != NORMAL_END)
	{
		long l_tot_frames = 0;

		if (Enc_Info.i_codec_version != XVID)
		{
			(void) get_endcredits_pos(r_divx_settings.s_logfile.GetBuffer(), 
				&Enc_Info.l_end_credits_frame,
				&l_tot_frames,
				r_divx_settings.i_max_q,
				i_version); 
		}
		else
		{
			(void) get_endcredits_pos_xvid(r_divx_settings.s_logfile.GetBuffer(), 
				&Enc_Info.l_end_credits_frame,
				&l_tot_frames,
				i_version);
		}

		seconds2timeformat_hms((long) (Enc_Info.l_end_credits_frame / Enc_Info.f_framerate), hms);

		if (Enc_Info.i_endcredits_auto_manual == MANUAL_END)
		{
			Enc_Info.l_end_credits_frame = Enc_Info.l_manual_end;
		}

		if (Enc_Info.b_selectrange)
		{
			if (Enc_Info.l_end_credits_frame > Enc_Info.i_end_time)
			{
				Enc_Info.l_end_credits_frame = Enc_Info.i_end_time;
				l_tot_frames = Enc_Info.i_end_time;
				//m_bEndCreditsValid = FALSE;
			}
		}

		if (Enc_Info.l_end_credits_frame < l_tot_frames)
		{
			if (Enc_Info.i_endcredits_option == REMOVE_END)
			{
				CalcNewBitrate(&i_tmp_kbps, 
					0, 
					Enc_Info.l_end_credits_frame,
					l_tot_frames);

				int i_start_time = 0;

				if (Enc_Info.b_selectrange)
				{
					i_start_time = Enc_Info.i_start_time;
					Enc_Info.l_seconds_chosen = (long) ((Enc_Info.l_end_credits_frame - Enc_Info.i_start_time  ) / Enc_Info.f_framerate);
				}
				else
				{
					Enc_Info.l_seconds_chosen = (long) (Enc_Info.i_start_time / Enc_Info.f_framerate);
				}

				// mod
				Enc_Info.MEncoder_info.frame_range.Format( 
					"-# %d %d", 
					i_start_time, 
					Enc_Info.l_end_credits_frame);
				m_bEndCreditsValid = FALSE;

				Enc_Info.l_seconds_chosen = (long) ((Enc_Info.l_frames_chosen+1) / Enc_Info.f_framerate);
			}
			else // must be recompress
			{
				if (Enc_Info.i_codec_version != XVID)
				{
					if ((Enc_Info.i_codec_version == DIVX5) || 
						(Enc_Info.i_codec_version == DIVX5PRO) ||
						(Enc_Info.i_codec_version == DIVX52X))
					{
						Enc_Info.i_end_credits_kbps = (int) (Enc_Info.i_end_credits_pct * 0.01 * Enc_Info.divxauto_info.bitrate * Enc_Info.d_ratio);;
					}
					else
					{
						Enc_Info.i_end_credits_kbps = DIVX4DEFAULTENDKBPS;
					}

					CalcNewBitrate(&i_tmp_kbps, 
						Enc_Info.i_end_credits_kbps,
						Enc_Info.l_end_credits_frame,
						l_tot_frames);
					m_bEndCreditsValid = TRUE;
				}
				else
				{
					m_bEndCreditsValid = FALSE;
				}
			}

			s_tmp.Format("EndCredits found at %s, bitrate set to %d.", 
				hms, i_tmp_kbps);

			Log(s_tmp);
		}
		else
		{
			m_bEndCreditsValid = FALSE;
		}
	}*/
}

void COutputFilesPage::SetEncodingThreadPriority(int priority, HANDLE r_handle)
{
	BOOL b_status = false;
	switch (priority)
	{
	case 0:
		b_status = SetThreadPriority(r_handle, THREAD_PRIORITY_IDLE);
		break;
	case 1:
		b_status = SetThreadPriority(r_handle, THREAD_PRIORITY_LOWEST);
		break;
	case 2:
		b_status = SetThreadPriority(r_handle, THREAD_PRIORITY_BELOW_NORMAL);
		break;
	case 3:
		b_status = SetThreadPriority(r_handle, THREAD_PRIORITY_NORMAL);
		break;
	case 4:
		b_status = SetThreadPriority(r_handle, THREAD_PRIORITY_ABOVE_NORMAL);
		break;
	case 5:
		b_status = SetThreadPriority(r_handle, THREAD_PRIORITY_HIGHEST);
		break;
	case 6:
		b_status = SetThreadPriority(r_handle, THREAD_PRIORITY_TIME_CRITICAL);
		break;
	default:
		b_status = SetThreadPriority(r_handle, THREAD_PRIORITY_NORMAL);
		break;
	}

	if (b_status == false)
	{

		CHAR szBuf[80]; 
		DWORD dw = GetLastError(); 

		sprintf(szBuf, "GetLastError returned %u\n", 
			dw); 

		MessageBox(szBuf); 
	}
}

void COutputFilesPage::DoStuff(int x)
{
	int result = -190921;
	CString s_tmp;
	CString filename;

	s_tmp.Format("Starting Project '%s'", Enc_Info.s_ProjName);
	Log(s_tmp);

	GetDlgItem(IDC_START)->SetWindowText("Abort");

	(void) CheckTabs();

	result = DoSomeBasicChecks(0);

	if (result == 0)
	{
		result = CheckFolders();
		if (result == 0)
		{
			Enc_Info.CreateEncodingLine();
			Enc_Info.PrintEncLine(Prog_Info.b_debug_mode);

			if (Enc_Info.i_outputmode_id == 0)
			{
				// preview mode
				Log("Starting Preview...");
			}
			else
			{
				int msgbox_result = IDYES;

				if (Enc_Info.i_pass_mode == 1)
				{
					// only 2nd pass from 2-pass
					
					filename.Format("%s\\%s%s", 
						Enc_Info.s_OutputFolder, 
						Enc_Info.s_ProjName, 
						_AVI);
					msgbox_result = CheckOverWrite(filename, 'T', Enc_Info.b_overwrite);
					if (msgbox_result == IDYES)
					{
						m_ctrlProgress.SetPos(STEP_MENCODER2 + STEP_DONE * g_batch_counter );
						g_step = STEP_MENCODER2;

						m_intTimer = SetTimer(TIMER, TIMER_LENGTH , NULL);
					}
					else
					{
						AfxMessageBox("Please move video '" + filename + "' to a save location!");
					}
				}
				else
				{
					// Overwrite :
					if (msgbox_result == IDYES)
					{
						msgbox_result = CheckOverWrite_AllFiles(2);
					}

					if (msgbox_result == IDYES)
					{
						if (Enc_Info.i_create_avi_options == 0)
						{
							Log("Starting 1-pass...");
						}
						else
						{
							// 2 pass mode :
							Log("Starting 1st-pass from 2-pass...");
						}

						Log("MEncoder started");
						MEncoder();

						m_intMEncoderTimer = SetTimer(MENCODER_TIMER, MENCODER_START_TIMER , NULL);

						m_ctrlProgress.SetPos(STEP_START + STEP_DONE * g_batch_counter);
					}
					else
					{
						AfxMessageBox("Please move old audio,video + final files to a save location!");
					}
				}
			}
		}
	}

	if (result != 0)
	{
		if (Prog_Info.b_batch_enabled == 0)
		{
			g_all_ready = 1;
		}
		else
		{
			g_failed_batch_counter++;
			g_batch_ready = 1;
		}

		g_encoding_error = 1;
	}

}

int COutputFilesPage::DoSomeBasicChecks(int option)
{
	int result = 0;
	CString s_tmp;

	if (Prog_Info.b_programs_found == 0)
	{
		if (!Prog_Info.b_batch_enabled)
		{
			AfxMessageBox("Not all required programs were found !\nPlease go to the '9. Setup' tab and choose the correct location.");
		}
		else
		{
			Log("Not all required programs were found : skipping this batch entry.");
		}
		result = 1;
	}

	if (Enc_Info.i_pass_mode == 1)
	{
		/*
		FILE *fp = fopen(r_divx_settings.s_logfile, "rb");

		if (fp)
		{
			fclose(fp);
		}
		else
		{
			CString s_tmp;
			s_tmp.Format("ERROR : Logfile '%s' not found. Not possible to do only 2nd pass from 2pass.", 
				r_divx_settings.s_logfile);
			Log(s_tmp);
			result = 3;
		}*/
	}

	return result;
}

void COutputFilesPage::Print_Info_File(int x)
{
	FILE *fp;
	char s_tmp[256] = {0};

	sprintf(s_tmp, "%s\\%s%s", Enc_Info.s_OutputFolder, 
		Enc_Info.s_ProjName, NFO);
	fp = fopen(s_tmp, "wt");

	if (fp)
	{
		int i = 0;
		char s_time[STRLEN64] = {0};
		char s_chan[STRLEN64] = {0};
		char s_audio_format[STRLEN64] = {0};
		char s_codec[STRLEN128] = {0};
		char s_audio_post[STRLEN128] = "None";
		CString s_subtitles;

		seconds2timeformat_hms(Enc_Info.l_seconds_chosen, s_time);

		for (i = 0 ; i < _LAST_CODEC; i++)
		{
			int id = Prog_Info.ar_vci[i].i_id;

			if (id == Enc_Info.i_codec_version)
			{
				strcpy(s_codec, Prog_Info.ar_vci[i].s_short_name); 
				break;
			}
		}

		if (Enc_Info.i_codec_version == MPEG4) 
		{
			if (Enc_Info.b_adv_bi) strcat(s_codec, " BI");
			if (Enc_Info.b_adv_gmc) strcat(s_codec, " QP");
			if (Enc_Info.b_adv_lumi_mask) strcat(s_codec, " Lumi-Masking");
			if (Enc_Info.b_adv_dark_mask) strcat(s_codec, " Darkness-Masking");
		}
		else if (Enc_Info.i_codec_version == XVID)
		{
			if (Enc_Info.b_adv_gmc) strcat(s_codec, " GMC");
			if (Enc_Info.b_adv_bi) strcat(s_codec, " BI");
			if (Enc_Info.b_adv_gmc) strcat(s_codec, " QP");
			if (Enc_Info.b_adv_chroma_motion) strcat(s_codec, " Chroma Motion");
			if (Enc_Info.b_adv_trellis) strcat(s_codec, " Trellis Optimization");
			if (Enc_Info.b_adv_vhq) strcat(s_codec, "VHQ");
		}

		if (Enc_Info.i_pass_mode == 0) 
		{
			strcat(s_codec, ", 1 Pass mode");
		}
		else
		{
			strcat(s_codec, ", 2 Pass mode");
		}

		if (Enc_Info.i_audio_format == AUDIO_TYPE_MP3)
		{
			strcpy(s_chan, "2 Channels (Surround Stereo)");
			strcpy(s_audio_format, "MP3");
		}
		else if (Enc_Info.i_audio_format == AUDIO_TYPE_OGG)
		{
			strcpy(s_chan, "2 Channels (Surround Stereo)");
			strcpy(s_audio_format, "OGG Vorbis");
		}
		else if (Enc_Info.i_audio_format == AUDIO_TYPE_AC3)
		{
			if (Enc_Info.ar_ac3_info[Enc_Info.i_audio_stream_id].i_num_channels == 2)
			{
				strcpy(s_chan, "2 Channels (Surround Stereo)");
			}
			else 
			{
				strcpy(s_chan, "6 Channels (5.1 Surround)");	
			}

			strcpy(s_audio_format, "AC3 (Dolby Digital)");
		}

		if (Enc_Info.b_create_subtitles)
		{
			for (i = 0 ; i < Enc_Info.i_numSelectedSubs ; i ++)
			{
				s_subtitles += Enc_Info.as_subs[Enc_Info.selectedSubs[i]];

				if (i < Enc_Info.i_numSelectedSubs - 1)
				{
					s_subtitles += ", ";
				}
			}
		}

		fprintf(fp, "[Movie]\n");
		fprintf(fp, "Name.........................: %s\n", Enc_Info.s_ProjName);
		fprintf(fp, "Length.......................: %s\n", s_time);
		fprintf(fp, "\n[Video]\n");
		fprintf(fp, "Format.......................: %s\n", Enc_Info.s_movie_format);
		fprintf(fp, "Orig. Source Aspect Ratio....: %s\n", Enc_Info.s_movie_aspect);
		fprintf(fp, "Display Aspect Ratio.........: %.2f\n", 1.0 * Enc_Info.i_cx / Enc_Info.i_cy);
		fprintf(fp, "Resolution...................: %d x %d\n", Enc_Info.i_cx , Enc_Info.i_cy);
		fprintf(fp, "ColorDepth...................: 24 bit\n");
		fprintf(fp, "Framerate....................: %.3f Frames/Sec\n", Enc_Info.f_framerate);
		fprintf(fp, "Codec........................: %s\n", s_codec);
		fprintf(fp, "Bitrate......................: Avg. %d kBit/s\n", Enc_Info.i_video_bitrate);
		if (Enc_Info.i_endcredits_option == 2)
		{
			fprintf(fp, "End-Credits Recompression....: Yes\n");
		}

		if (Enc_Info.b_create_audio)
		{
			fprintf(fp, "\n[Audio]\n");
			fprintf(fp, "Language.....................: %s\n", Enc_Info.as_audio_languages[Enc_Info.i_audio_stream_id]);
			fprintf(fp, "Format.......................: %s\n", s_audio_format);
			fprintf(fp, "Channels.....................: %s\n", s_chan);
			fprintf(fp, "SampleRate...................: %d Hz\n", Enc_Info.i_hertz);
			fprintf(fp, "Bitrate......................: %d kBit/s\n", Enc_Info.i_audio_kbps);
			if (Enc_Info.i_audio_format != AUDIO_TYPE_AC3)
			{
				if (Enc_Info.b_amplify_audio)
				{
					sprintf(s_audio_post, "Amplify with %d dB", Enc_Info.i_audio_amplify);
				}
				if (Enc_Info.i_norm_wave == 1)
				{
					strcpy(s_audio_post, "Normalizing");
				}
				if (Enc_Info.i_norm_wave == 2)
				{
					strcpy(s_audio_post, "Normalizing with MidNight mode");
				}
			}
			fprintf(fp, "Audio Post-Processing........: %s\n", s_audio_post);
		}

		if (Enc_Info.b_create_subtitles)
		{
			fprintf(fp, "\n[Subtitles]\n");
			fprintf(fp, "Format.......................: VobSub\n");
			fprintf(fp, "Language(s)..................: %s\n", s_subtitles);
		}

		fprintf(fp, "\n[Applications]\n");
		fprintf(fp, "All in One tool..............: %s %s\n", APP_NAME, APP_VERSION);
		fprintf(fp, "Video Converter..............: MEncoder\n");
		fprintf(fp, "Dubbing/Splitting/Merging....: VirtualDub\n");

		fclose(fp);
	}
}

//SLU
LRESULT COutputFilesPage::OnQuerySiblings(WPARAM wParam, LPARAM lParam)
{
	if (lParam == PAGE_OUTPUT)
	{
		//Author: Bradley Pierson
		//Purpose: Take control, and run the core code.

		//Simulate this form being activated and the appropriate buttons being pushed.
		//OnSetActive();//Activate
		//My_Static_Init();//Part one of the old OnSetActive
		//My_Init();//Part two of the old OnSetActive
		OnSetActive();
		OnStart();//The same function as the button of power.
	}

	return 0;
}
//End SLU

