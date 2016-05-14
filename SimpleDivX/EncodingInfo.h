#pragma once
#include "afx.h"
#include "SimpleDivX_defines.h"
#include "../vob_mpeg2/vstrip/vobinfo.h"

class CEncodingInfo : public CObject
{
	DECLARE_SERIAL( CEncodingInfo )

public:
	CEncodingInfo(void);
	~CEncodingInfo(void);
	void Serialize(CArchive& ar);
	void CopyIt(const CEncodingInfo& e);

	CEncodingInfo( const CEncodingInfo& e )
	{
		CopyIt(e);
	}

	const CEncodingInfo& operator=( const CEncodingInfo& e )
    {
		CopyIt(e);
        	 
		return *this;
    }

	BOOL b_amplify_audio;
	BOOL b_audio_vbr;
	BOOL b_auto_logfile_naming;
	BOOL b_auto_size;
	BOOL b_checkall;
	BOOL b_checkmain;
	BOOL b_convert_ac3_to_2channels;
	BOOL b_create_audio;
	BOOL b_create_subtitles;
	BOOL b_extract_all_subtitles;
	BOOL b_delete_temp;
	BOOL b_enable_audiocheck;
	BOOL b_enable_videocheck;
	BOOL b_enter_filesize;
	BOOL b_gettimefromvob;
	BOOL b_halfresolution;
	BOOL b_overwrite;
	BOOL b_selectrange;
	BOOL b_start_at_lba;
	BOOL r_valid_container[_LAST_CF];
	BOOL b_process_all_titles;

	CString as_audio_languages[MAX_AUDIO * 3];
	CString as_subs[MAX_SUBS];
	CString as_subs_short[MAX_SUBS];
	CString s_OutputFolder;
	CString s_ProjName;
	CString s_TempFolder;
	CString sVobFolder;
	CString sTwoPassLogFile;
	CString s_movie_aspect;
	CString s_movie_format; // PAL or NTSC
	CString s_status_time;
	CString enc1_execline;
	CString enc2_execline;
	CString play_execline;
	CString mkv_exeline;
	CString Logfile;
	CString sAvcOpts;
	CString sVideoFilter;
	CString sAudioFilter;
	CString fourcc;

	double af_audio_delay[MAX_AUDIO * 3];
	double d_ratio;
	double f_aspect_ratio;
	double f_framerate;
	double f_selected_framerate;

	int ai_language_type[MAX_AUDIO * 3];
	int ai_language_id[MAX_AUDIO * 3];
	int selectedSubs[MAX_SUBS];
	int ai_video_id[MAX_STRMS];

	int b_adv_bi;
	int b_adv_gmc;
	int b_adv_qpel;
	int b_adv_lumi_mask;
	int b_adv_dark_mask;
	int b_adv_vhq;
	int b_adv_trellis;
	int b_adv_chroma_motion;

	int i_audio_amplify;
	int i_audio_format; // mp3 = 0  or  ac3 = 1 , ogg = 2
	int i_audio_kbps;
	int i_audio_stream_id;
	int i_auto_size_width_id;
	int i_create_avi_options;
	int i_cropping_height;
	int i_cx;
	int i_cy;
	int i_codec_version;
	int i_dx;
	int i_dy;
	int i_end_credits_kbps;
	int i_end_credits_pct;
	int i_end_time;
	int i_endcredits_auto_manual; // 0 = auto , 1 = manual
	int i_endcredits_option; // 0 = normal, 1 = remove , 2 = recompress.
	int i_filesize;
	int i_valid_languages;
	int i_found_subs;
	int i_found_video_streams;
	int i_framerate_id;
	int i_hertz;
	int i_hertz_id;
	int i_max_keyframe_interval;
	int i_max_qant;
	int i_min_qant;
	int i_norm_wave;
	int i_number_ifofiles;
	int i_numSelectedSubs;
	int i_original_height;
	int i_original_width;
	int i_outputmode_id;
	int i_pass_mode;
	int i_status;
	int i_start_time;
	int i_video_bitrate;
	int i_video_output_format_id;
	int i_container_format;
	int i_input_mode;
	int i_selected_ifo_id;
	int i_deinterlace;
	int i_num_parts_requested;
	int i_num_parts_created;
	int i_start_at_lba;
	int i_split;
	int i_source_format_id;
	int i_selected_title_id;
	int i_selected_audio_id;
			
	long l_end_credits_frame;
	long l_frames;
	long l_frames_chosen;
	long l_manual_end;
	long l_seconds_chosen;
	long l_split_at;
	long l_total_length;
	long l_split_frame[MAXSPLIT];

	AC3_INFO ar_ac3_info[MAX_AUDIO * 3];
	SUBTITLES_INFO subtitles_info;
	IFO_FILE r_ifo_files[MAXIFOFILES];

	// Implementation
protected:
public:
	void CheckAndCorrect(void);
	void CreateEncodingLine();
	void CreateVideoFilter(void);
	bool CreateAudioParams(void);
	bool AddAvcOpts(void);
	void CreateAudioFilter(void);
	void PrintEncLine(BOOL b);
	void CreateMKVMergeExecLine(void);
};

