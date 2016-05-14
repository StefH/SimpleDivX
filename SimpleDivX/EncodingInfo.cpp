#include "stdafx.h"
#include ".\encodinginfo.h"
#include "../vob_mpeg2/vstrip/vstrip.h"

IMPLEMENT_SERIAL( CEncodingInfo, CObject, 1 )

CEncodingInfo::CEncodingInfo(void)
{
	char buffer[STRLEN512];
	int i_max_len = STRLEN512;
	int i_len = 0;

	i_len = GetCurrentDirectory(i_max_len, buffer);

	s_ProjName = "My Movie";
	s_TempFolder.Format("%s\\temp", buffer);				
	s_OutputFolder.Format("%s\\output", buffer);
	sVobFolder = "C:\\";
	sTwoPassLogFile.Format("%s\\%s", s_TempFolder , DIVXLOG_FILE);
	enc1_execline.Empty();
	enc2_execline.Empty();
	play_execline.Empty();
	mkv_exeline.Empty();

	f_aspect_ratio = 1.0;
	f_framerate = 25.000;
	f_selected_framerate = 25.000;
	d_ratio = 1.0;

	b_amplify_audio = 0;
	b_auto_logfile_naming = 1;
	b_auto_size = false; 
	b_audio_vbr = false;
	b_adv_bi = false;
	b_checkall = false;
	b_checkmain = false;
	b_convert_ac3_to_2channels = false;
	b_create_audio = 1;
	b_create_subtitles = false;
	b_delete_temp = true;
	b_enable_audiocheck = 0;
	b_enable_videocheck = 0;
	b_enter_filesize = 1;
	b_extract_all_subtitles = false;
	b_gettimefromvob = 1;
	b_adv_gmc = false;
	b_halfresolution = 0;
	b_overwrite = true;
	b_adv_qpel = false;
	b_selectrange = 0;
	b_start_at_lba = 0;
	b_adv_chroma_motion = false;
	b_adv_lumi_mask = false;
	b_adv_dark_mask = false;
	b_adv_vhq = false;
	b_adv_trellis = false;
	b_process_all_titles = false;

	i_audio_amplify = 3;
	i_audio_format = AUDIO_TYPE_MP3;
	i_audio_kbps = 128;
	i_audio_stream_id = 0;
	i_auto_size_width_id = 10;
	i_codec_version = MPEG4;
	i_container_format = CF_AVI;
	i_create_avi_options = 0;
	i_cx = 640;
	i_cy = 352;
	i_deinterlace = 0;
	i_dx = 648;
	i_dy = 368;
	i_end_credits_pct = 5;
	i_end_credits_pct = ENDCREDITSPCT;
	i_end_time = 999;
	i_endcredits_auto_manual = 0;
	i_endcredits_option = 0;
	i_filesize = 1400;
	i_framerate_id = 2;
	i_hertz = 48000;
	i_hertz_id = 0;
	i_max_keyframe_interval = 250;
	i_max_qant = 12;
	i_min_qant = 2;
	i_num_parts_requested = 2;
	i_num_parts_created = 1;
	i_start_at_lba = 0;
	i_numSelectedSubs = 0;
	i_outputmode_id = 2;
	i_pass_mode = 0;
	i_start_time = 0;
	i_video_output_format_id = 9;
	i_split = SPLIT_NONE;
	i_source_format_id = SF_MOVIE;
	i_selected_title_id = 0;
	i_selected_audio_id = 0x80;

	l_manual_end = 199999;
	l_split_at = 700;
	int i = 0;

	for (i = 0 ; i < MAXIFOFILES ; i++)
	{
		r_ifo_files[i].b_valid_ifo = 0;
		r_ifo_files[i].i_num_vobfiles = 0;
		r_ifo_files[i].l_size = 0;
		r_ifo_files[i].s_filename = "";
		r_ifo_files[i].s_short_filename = "";

		for (int j = 0 ; j < MAXVOBFILES ; j++)
		{
			r_ifo_files[i].vob_files[j].r_type = 0;
			r_ifo_files[i].vob_files[j].l_size = 0;
			r_ifo_files[i].vob_files[j].s_filename = "";
			r_ifo_files[i].vob_files[j].s_short_filename = "";
		}
	}

	for (i = 0 ; i < _LAST_CF ; i++)
	{
		r_valid_container[i] = false;
	}

	for (i = 0 ; i < MAX_SUBS ; i++)
	{
		as_subs[i] = "";
		as_subs_short[i] = "";
		selectedSubs[i] = 0;
	}

	for (i = 0 ; i < MAX_AUDIO * 3; i++)
	{
		as_audio_languages[i] = "";
		af_audio_delay[i] = 0;
		ai_language_id[i] = 0;
		ai_language_type[i] = 0;
		ar_ac3_info[i].b_valid = false;
		ar_ac3_info[i].i_kbps = 0;
		ar_ac3_info[i].i_khz = 0;
		ar_ac3_info[i].uc_id = 0;
	}

	for (i = 0 ; i < MAX_STRMS ; i++)
	{
		ai_video_id[i] = 0;
	}

	for (i = 0 ; i < MAXSPLIT; i++)
	{
		l_split_frame[i] = 0;
	}

	for (i = 0 ; i < MAXSPLIT; i++)
	{
		subtitles_info.splitCommandLine[i] = "";
	}
}

void CEncodingInfo::Serialize(CArchive& ar)
{
	int i = 0;
	int j = 0;

	CObject::Serialize( ar );

	if( ar.IsStoring() )
	{
		ar << b_amplify_audio;
		ar << b_audio_vbr;
		ar << b_auto_logfile_naming;
		ar << b_auto_size;
		ar << b_checkall;
		ar << b_checkmain;
		ar << b_convert_ac3_to_2channels;
		ar << b_create_audio;
		ar << b_create_subtitles;
		ar << b_extract_all_subtitles;
		ar << b_delete_temp;
		ar << b_enable_audiocheck;
		ar << b_enable_videocheck;
		ar << b_enter_filesize;
		ar << b_gettimefromvob;
		ar << b_halfresolution;
		ar << b_overwrite;
		ar << b_selectrange;
		ar << b_start_at_lba;
		ar << b_process_all_titles;

		for (i = 0; i <_LAST_CF ; i++)
		{
			ar << r_valid_container[i];
		}

		for (i = 0 ; i < MAX_AUDIO * 3; i++)
		{	
			ar << af_audio_delay[i];
			ar << ai_language_id[i];
			ar << ai_language_type[i];
			ar << ar_ac3_info[i].b_valid;
			ar << ar_ac3_info[i].i_kbps;
			ar << ar_ac3_info[i].i_khz;
			ar << ar_ac3_info[i].uc_id;
			ar << as_audio_languages[i];
		}

		for (i = 0 ; i < MAX_SUBS; i++)
		{
			ar << as_subs[i];
			ar << as_subs_short[i];
			ar << selectedSubs[i];
		}

		ar << s_OutputFolder;
		ar << s_ProjName;
		ar << s_TempFolder;
		ar << sVobFolder;
		ar << sTwoPassLogFile;
		ar << s_movie_aspect;
		ar << s_movie_format;
		ar << s_status_time;
		ar << fourcc;

		ar << d_ratio;
		ar << f_aspect_ratio;
		ar << f_framerate;
		ar << f_selected_framerate;

		for (i = 0 ; i < MAX_STRMS ; i++)
		{
			ar << ai_video_id[i];
		}

		ar << b_adv_bi;
		ar << b_adv_gmc;
		ar << b_adv_qpel;
		ar << b_adv_vhq;
		ar << b_adv_trellis;
		ar << b_adv_lumi_mask;
		ar << b_adv_dark_mask;
		ar << b_adv_chroma_motion;

		ar << i_audio_amplify;
		ar << i_audio_format;
		ar << i_audio_kbps;
		ar << i_audio_stream_id;
		ar << i_auto_size_width_id;
		ar << i_create_avi_options;
		ar << i_cropping_height;
		ar << i_cx;
		ar << i_cy;
		ar << i_codec_version;
		ar << i_dx;
		ar << i_dy;
		ar << i_end_credits_kbps;
		ar << i_end_credits_pct;
		ar << i_end_time;
		ar << i_endcredits_auto_manual;
		ar << i_endcredits_option;
		ar << i_filesize;
		ar << i_valid_languages;
		ar << i_found_subs;
		ar << i_found_video_streams;
		ar << i_framerate_id;
		ar << i_hertz;
		ar << i_hertz_id;
		ar << i_max_keyframe_interval;
		ar << i_max_qant;
		ar << i_min_qant;
		ar << i_norm_wave;
		ar << i_number_ifofiles;
		ar << i_original_height;
		ar << i_original_width;
		ar << i_outputmode_id;
		ar << i_pass_mode;
		ar << i_status;
		ar << i_start_time;
		ar << i_video_bitrate;
		ar << i_video_output_format_id;
		ar << i_container_format;
		ar << i_input_mode;
		ar << i_selected_ifo_id;
		ar << i_deinterlace;
		ar << i_num_parts_requested;
		ar << i_num_parts_created;
		ar << i_numSelectedSubs;
		ar << i_start_at_lba;
		ar << i_split;
		ar << i_source_format_id;
		ar << i_selected_title_id;
		ar << i_selected_audio_id;

		ar << l_end_credits_frame;
		ar << l_frames;
		ar << l_frames_chosen;
		ar << l_manual_end;
		ar << l_seconds_chosen;
		ar << l_split_at;
		ar << l_total_length;
		for (i = 0 ; i < MAXSPLIT ; i++)
		{
			ar << l_split_frame[i];
		}

		ar << subtitles_info.projName;
		ar << subtitles_info.i_num_parts;
		ar << subtitles_info.commandLine;
		for (i = 0 ; i < MAXSPLIT ; i++)
		{
			ar << subtitles_info.splitCommandLine[i];
		}
		
		for (i = 0 ; i < MAXIFOFILES ; i++)
		{
			ar << r_ifo_files[i].b_valid_ifo;
			ar << r_ifo_files[i].i_num_vobfiles;
			ar << r_ifo_files[i].l_size;
			ar << r_ifo_files[i].s_filename;
			ar << r_ifo_files[i].s_short_filename;

			for (j = 0 ; j < MAXVOBFILES ; j++)
			{
				ar << r_ifo_files[i].vob_files[j].r_type;
				ar << r_ifo_files[i].vob_files[j].l_size;
				ar << r_ifo_files[i].vob_files[j].s_filename;
				ar << r_ifo_files[i].vob_files[j].s_short_filename;
			}
		}                                                            
	}
	else
	{
		ar >> b_amplify_audio;
		ar >> b_audio_vbr;
		ar >> b_auto_logfile_naming;
		ar >> b_auto_size;
		ar >> b_checkall;
		ar >> b_checkmain;
		ar >> b_convert_ac3_to_2channels;
		ar >> b_create_audio;
		ar >> b_create_subtitles;
		ar >> b_extract_all_subtitles;
		ar >> b_delete_temp;
		ar >> b_enable_audiocheck;
		ar >> b_enable_videocheck;
		ar >> b_enter_filesize;
		ar >> b_gettimefromvob;
		ar >> b_halfresolution;
		ar >> b_overwrite;
		ar >> b_selectrange;
		ar >> b_start_at_lba;
		ar >> b_process_all_titles;

		for (i = 0; i <_LAST_CF ; i++)
		{
			ar >> r_valid_container[i];
		}

		for (i = 0 ; i < MAX_AUDIO * 3; i++)
		{	
			ar >> af_audio_delay[i];
			ar >> ai_language_id[i];
			ar >> ai_language_type[i];
			ar >> ar_ac3_info[i].b_valid;
			ar >> ar_ac3_info[i].i_kbps;
			ar >> ar_ac3_info[i].i_khz;
			ar >> ar_ac3_info[i].uc_id;
			ar >> as_audio_languages[i];
		}

		for (i = 0 ; i < MAX_SUBS; i++)
		{
			ar >> as_subs[i];
			ar >> as_subs_short[i];
			ar >> selectedSubs[i];
		}

		ar >> s_OutputFolder;
		ar >> s_ProjName;
		ar >> s_TempFolder;
		ar >> sVobFolder;
		ar >> sTwoPassLogFile;
		ar >> s_movie_aspect;
		ar >> s_movie_format;
		ar >> s_status_time;
		ar >> fourcc;

		ar >> d_ratio;
		ar >> f_aspect_ratio;
		ar >> f_framerate;
		ar >> f_selected_framerate;

		for (i = 0 ; i < MAX_STRMS ; i++)
		{
			ar >> ai_video_id[i];
		}

		ar >> b_adv_bi;
		ar >> b_adv_gmc;
		ar >> b_adv_qpel;
		ar >> b_adv_vhq;
		ar >> b_adv_trellis;
		ar >> b_adv_lumi_mask;
		ar >> b_adv_dark_mask;
		ar >> b_adv_chroma_motion;

		ar >> i_audio_amplify;
		ar >> i_audio_format;
		ar >> i_audio_kbps;
		ar >> i_audio_stream_id;
		ar >> i_auto_size_width_id;
		ar >> i_create_avi_options;
		ar >> i_cropping_height;
		ar >> i_cx;
		ar >> i_cy;
		ar >> i_codec_version;
		ar >> i_dx;
		ar >> i_dy;
		ar >> i_end_credits_kbps;
		ar >> i_end_credits_pct;
		ar >> i_end_time;
		ar >> i_endcredits_auto_manual;
		ar >> i_endcredits_option;
		ar >> i_filesize;
		ar >> i_valid_languages;
		ar >> i_found_subs;
		ar >> i_found_video_streams;
		ar >> i_framerate_id;
		ar >> i_hertz;
		ar >> i_hertz_id;
		ar >> i_max_keyframe_interval;
		ar >> i_max_qant;
		ar >> i_min_qant;
		ar >> i_norm_wave;
		ar >> i_number_ifofiles;
		ar >> i_original_height;
		ar >> i_original_width;
		ar >> i_outputmode_id;
		ar >> i_pass_mode;
		ar >> i_status;
		ar >> i_start_time;
		ar >> i_video_bitrate;
		ar >> i_video_output_format_id;
		ar >> i_container_format;
		ar >> i_input_mode;
		ar >> i_selected_ifo_id;
		ar >> i_deinterlace;
		ar >> i_num_parts_requested;
		ar >> i_num_parts_created;
		ar >> i_numSelectedSubs;
		ar >> i_start_at_lba;
		ar >> i_split;
		ar >> i_source_format_id;
		ar >> i_selected_title_id;
		ar >> i_selected_audio_id;

		ar >> l_end_credits_frame;
		ar >> l_frames;
		ar >> l_frames_chosen;
		ar >> l_manual_end;
		ar >> l_seconds_chosen;
		ar >> l_split_at;
		ar >> l_total_length;
		for (i = 0 ; i < MAXSPLIT ; i++)
		{
			ar >> l_split_frame[i];
		}

		ar >> subtitles_info.projName;
		ar >> subtitles_info.i_num_parts;
		ar >> subtitles_info.commandLine;
		for (i = 0 ; i < MAXSPLIT ; i++)
		{
			ar >> subtitles_info.splitCommandLine[i];
		}

		for (i = 0 ; i < MAXIFOFILES ; i++)
		{
			ar >> r_ifo_files[i].b_valid_ifo;
			ar >> r_ifo_files[i].i_num_vobfiles;
			ar >> r_ifo_files[i].l_size;
			ar >> r_ifo_files[i].s_filename;
			ar >> r_ifo_files[i].s_short_filename;

			for (j = 0 ; j < MAXVOBFILES ; j++)
			{
				ar >> r_ifo_files[i].vob_files[j].r_type;
				ar >> r_ifo_files[i].vob_files[j].l_size;
				ar >> r_ifo_files[i].vob_files[j].s_filename;
				ar >> r_ifo_files[i].vob_files[j].s_short_filename;
			}
		}   
	}
}

CEncodingInfo::~CEncodingInfo(void)
{
}


void CEncodingInfo::CopyIt(const CEncodingInfo& e)
{
	int i = 0;
	b_amplify_audio = e.b_amplify_audio;
	b_audio_vbr = e.b_audio_vbr;
	b_auto_logfile_naming = e.b_auto_logfile_naming;
	b_auto_size = e.b_auto_size;
	b_checkall = e.b_checkall;
	b_checkmain = e.b_checkmain;
	b_convert_ac3_to_2channels = e.b_convert_ac3_to_2channels;
	b_create_audio = e.b_create_audio;
	b_create_subtitles = e.b_create_subtitles;
	b_extract_all_subtitles = e.b_extract_all_subtitles;
	b_delete_temp = e.b_delete_temp;
	b_enable_audiocheck = e.b_enable_audiocheck;
	b_enable_videocheck = e.b_enable_videocheck;
	b_enter_filesize = e.b_enter_filesize;
	b_gettimefromvob = e.b_gettimefromvob;
	b_halfresolution = e.b_halfresolution;
	b_overwrite = e.b_overwrite;
	b_selectrange = e.b_selectrange;
	b_start_at_lba = e.b_start_at_lba;
	b_adv_vhq = e.b_adv_vhq;
	b_adv_trellis = e.b_adv_trellis;
	b_adv_lumi_mask = e.b_adv_lumi_mask;
	b_adv_dark_mask = e.b_adv_dark_mask;
	b_adv_chroma_motion = e.b_adv_chroma_motion;
	b_process_all_titles = e.b_process_all_titles;

	for (i = 0 ; i < _LAST_CF ; i++)
	{
		r_valid_container[i] = e.r_valid_container[i];
	}

	for (i = 0 ; i < MAX_SUBS ; i++)
	{
		as_subs[i] = e.as_subs[i];
		as_subs_short[i] = e.as_subs_short[i];
		selectedSubs[i] = e.selectedSubs[i];
	}
	s_OutputFolder = e.s_OutputFolder;
	s_ProjName = e.s_ProjName;
	s_TempFolder = e.s_TempFolder;
	sVobFolder = e.sVobFolder;
	sTwoPassLogFile = e.sTwoPassLogFile;
	s_movie_aspect = e.s_movie_aspect;
	s_movie_format = e.s_movie_format;
	s_status_time = e.s_status_time;
	enc1_execline = e.enc1_execline;
	enc2_execline = e.enc2_execline;
	play_execline = e.play_execline;
	mkv_exeline = e.mkv_exeline;
	Logfile = e.Logfile;

	for (i = 0 ; i < MAX_AUDIO * 3; i++)
	{
		as_audio_languages[i] = e.as_audio_languages[i];
		af_audio_delay[i] = e.af_audio_delay[i];
		ai_language_id[i] = e.ai_language_id[i];
		ai_language_type[i] = e.ai_language_type[i];
		ar_ac3_info[i].b_valid = e.ar_ac3_info[i].b_valid;
		ar_ac3_info[i].i_kbps = e.ar_ac3_info[i].i_kbps;
		ar_ac3_info[i].i_khz = e.ar_ac3_info[i].i_khz;
		ar_ac3_info[i].uc_id = e.ar_ac3_info[i].uc_id;
	}

	d_ratio = e.d_ratio;
	f_aspect_ratio = e.f_aspect_ratio;
	f_framerate = e.f_framerate;

	for (i = 0 ; i < MAX_STRMS ; i++)
	{
		ai_video_id[i] = e.ai_video_id[i];
	}

	i_numSelectedSubs = e.i_numSelectedSubs;
	b_adv_bi = e.b_adv_bi;
	b_adv_gmc = e.b_adv_gmc;
	b_adv_qpel = e.b_adv_gmc;
	i_audio_amplify = e.i_audio_amplify;
	i_audio_format = e.i_audio_format;
	i_audio_kbps = e.i_audio_kbps;
	i_audio_stream_id = e.i_audio_stream_id;
	i_auto_size_width_id = e.i_auto_size_width_id;
	i_create_avi_options = e.i_create_avi_options;
	i_cropping_height = e.i_cropping_height;
	i_cx = e.i_cx;
	i_cy = e.i_cy;
	i_codec_version = e.i_codec_version;
	i_dx = e.i_dx;
	i_dy = e.i_dy;
	i_end_credits_kbps = e.i_end_credits_kbps;
	i_end_credits_pct = e.i_end_credits_pct;
	i_end_time = e.i_end_time;
	i_endcredits_auto_manual = e.i_endcredits_auto_manual;
	i_endcredits_option = e.i_endcredits_option;
	i_filesize = e.i_filesize;
	i_valid_languages = e.i_valid_languages;
	i_found_subs = e.i_found_subs;
	i_found_video_streams = e.i_found_video_streams;
	i_framerate_id = e.i_framerate_id;
	i_hertz = e.i_hertz;
	i_hertz_id = e.i_hertz_id;
	i_max_keyframe_interval = e.i_max_keyframe_interval;
	i_max_qant = e.i_max_qant;
	i_min_qant = e.i_min_qant;
	i_norm_wave = e.i_norm_wave;
	i_number_ifofiles = e.i_number_ifofiles;
	i_deinterlace = e.i_deinterlace;
	i_num_parts_requested = e.i_num_parts_requested;
	i_num_parts_created = e.i_num_parts_created;
	i_start_at_lba = e.i_start_at_lba;

	i_original_height = e.i_original_height;
	i_original_width = e.i_original_width;
	i_outputmode_id = e.i_outputmode_id;
	i_pass_mode = e.i_pass_mode;
	i_status = e.i_status;
	i_start_time = e.i_start_time;
	i_video_bitrate = e.i_video_bitrate;
	i_video_output_format_id = e.i_video_output_format_id;

	i_container_format = e.i_container_format;
	i_input_mode = e.i_input_mode;
	i_selected_ifo_id = e.i_selected_ifo_id;
	i_split = e.i_split;
	i_source_format_id = e.i_source_format_id;
	i_selected_title_id = e.i_selected_title_id;
	i_selected_audio_id = e.i_selected_audio_id;

	l_end_credits_frame = e.l_end_credits_frame;
	l_frames = e.l_frames;
	l_frames_chosen = e.l_frames_chosen;
	l_manual_end = e.l_manual_end;
	l_seconds_chosen = e.l_seconds_chosen;
	l_split_at = e.l_split_at;
	l_total_length = e.l_total_length;
	for (i = 0 ; i < MAXSPLIT; i++)
	{
		l_split_frame[i] = e.l_split_frame[i];
	}

	subtitles_info.projName = e.subtitles_info.projName;
	subtitles_info.i_num_parts = e.subtitles_info.i_num_parts;
	subtitles_info.commandLine = e.subtitles_info.commandLine;
	for (i = 0 ; i < MAXSPLIT; i++)
	{
		subtitles_info.splitCommandLine[i] = e.subtitles_info.splitCommandLine[i];
	}

	for (i = 0 ; i < MAXIFOFILES ; i++)
	{
		r_ifo_files[i].b_valid_ifo = e.r_ifo_files[i].b_valid_ifo;
		r_ifo_files[i].i_num_vobfiles = e.r_ifo_files[i].i_num_vobfiles;
		r_ifo_files[i].l_size = e.r_ifo_files[i].l_size;
		r_ifo_files[i].s_filename = e.r_ifo_files[i].s_filename;
		r_ifo_files[i].s_short_filename = e.r_ifo_files[i].s_short_filename;

		for (int j = 0 ; j < MAXVOBFILES ; j++)
		{
			r_ifo_files[i].vob_files[j].r_type = e.r_ifo_files[i].vob_files[j].r_type;
			r_ifo_files[i].vob_files[j].l_size = e.r_ifo_files[i].vob_files[j].l_size;
			r_ifo_files[i].vob_files[j].s_filename = e.r_ifo_files[i].vob_files[j].s_filename;
			r_ifo_files[i].vob_files[j].s_short_filename = e.r_ifo_files[i].vob_files[j].s_short_filename;
		}
	}
}

void CEncodingInfo::CheckAndCorrect(void)
{
	if ((i_max_qant <= 0) || (i_max_qant > 12))
	{
		i_max_qant = 12;
	}

	if ((i_min_qant <= 0) || (i_min_qant > i_max_qant))
	{
		i_min_qant = 2;
	}

	if ((i_end_credits_pct < 0) || (i_end_credits_pct > 100))
	{
		i_end_credits_pct = 99;
	}
}

void CEncodingInfo::CreateEncodingLine()
{
	CString vobfile;
	
	int i_num_vobfiles = r_ifo_files[i_selected_ifo_id].i_num_vobfiles;

	if (i_num_vobfiles == 1)
	{
		vobfile.Format(" \"%s\"", r_ifo_files[i_selected_ifo_id].vob_files[0].s_filename);
	}
	else
	{
		int i_extra = 0;
		if (!b_process_all_titles)
		{
			i_extra = i_selected_title_id;
		}
			
		vobfile.Format(" dvd://%d -dvd-device \"%s\"", 
			r_ifo_files[i_selected_ifo_id].r_movie_info.i_start_title_id + i_extra,
			sVobFolder);
	}

	enc1_execline.Format("%s", vobfile);
	enc2_execline.Format("%s", vobfile);
	play_execline.Format("%s", vobfile);

	if (b_selectrange)
	{
		if (i_start_time > 0)
		{
			enc1_execline.AppendFormat(" -ss %d", i_start_time);
			enc2_execline.AppendFormat(" -ss %d", i_start_time);
			//play_execline.AppendFormat(" -ss %d", i_start_time);
		}

		if (i_end_time > 0)
		{
			enc1_execline.AppendFormat(" -endpos %d", i_end_time);
			enc2_execline.AppendFormat(" -endpos %d", i_end_time);
			//play_execline.AppendFormat(" -endpos %d", i_end_time);
		}
	}
	else if (b_start_at_lba)
	{
		if (i_start_at_lba > 0)
		{
			enc1_execline.AppendFormat(" -sb %d", i_start_at_lba * 2048);
			enc2_execline.AppendFormat(" -sb %d", i_start_at_lba * 2048);
			play_execline.AppendFormat(" -sb %d", i_start_at_lba * 2048);
		}
	}
	
	sAvcOpts.Empty();
	sVideoFilter.Empty();
	sAudioFilter.Empty();

	CreateAudioParams();

	CreateVideoFilter();

	CreateAudioFilter();

	AddAvcOpts();

	CString outputFile;
	CString s_tmp;

	if (i_container_format == CF_AVI)
	{
		s_tmp = AVI;

		if (i_split != SPLIT_NONE)
		{
			outputFile.Format("\"%s\\%s%s\"" , s_TempFolder , s_ProjName , _AVI);
		}
		else
		{
			outputFile.Format("\"%s\\%s%s\"" , s_OutputFolder , s_ProjName , s_tmp);
		}
	}
	else
	{
		// MKV !!!
		// Create AVI always in TEMP FOLDER
		outputFile.Format("\"%s\\%s%s\"" , s_TempFolder , s_ProjName , _AVI);
	}

	enc1_execline.AppendFormat(" -noodml -o %s", outputFile);
	enc2_execline.AppendFormat(" -noodml -o %s", outputFile);
}

void CEncodingInfo::CreateVideoFilter(void)
{
	CString sFPS;
	CString sTmp;

	int original_fps = (int) (1000 * f_framerate);
	int selected_fps = (int) (1000 * f_selected_framerate);

	// Framerate
	if (original_fps != selected_fps)
	{
		sFPS.Format(" -ofps %2.3f", f_selected_framerate);
	}
	else
	{
		sFPS.Empty();
	}

	// de-interlace
	if (i_deinterlace == 1)
	{
		if (sVideoFilter.IsEmpty())
		{
			sVideoFilter.Format(" -vf lavcdeint");
		}
		else
		{
			sVideoFilter.AppendFormat(",lavcdeint");
		}
	}

	// Inverse telecine
	if (i_deinterlace == 2)
	{
		sTmp = "pullup,softskip";
		if (sVideoFilter.IsEmpty())
		{
			sVideoFilter.Format(" -vf %s", sTmp);
		}
		else
		{
			sVideoFilter.AppendFormat(",%s", sTmp);
		}
	}

	// Scaling & resizing
	if ((i_cy != i_original_height) && (i_cx != i_original_width))
	{
		sTmp.Format("scale=%d:%d,crop=%d:%d", i_dx, i_dy, i_cx, i_cy);

		if (sVideoFilter.IsEmpty())
		{
			sVideoFilter.Format(" -noslices -vf %s", sTmp);
		}
		else
		{
			sVideoFilter.AppendFormat(",%s", sTmp);
		}
	}

	enc1_execline.Append(sVideoFilter);
	enc2_execline.Append(sVideoFilter);
	play_execline.Append(sVideoFilter);
	enc1_execline.Append(sFPS);
	enc2_execline.Append(sFPS);
}

bool CEncodingInfo::CreateAudioParams(void)
{
	bool bAudioFiltersApplied = false;
	bool bAvcOptsAdded = false;

	CString audioArgs;

	if (b_create_audio)
	{
		//int id = ai_language_id[i_audio_stream_id];

		if ((i_selected_audio_id >= AUDIO_STREAM) && (i_selected_audio_id <= MAX_AUDIO_STREAM))
		{
			audioArgs.AppendFormat(" -aid %d", i_selected_audio_id - AUDIO_STREAM);
		}
		else
		{
			audioArgs.AppendFormat(" -aid %d", i_selected_audio_id);
		}
	
		if (i_audio_format == AUDIO_TYPE_MP3)
		{
			audioArgs.AppendFormat(" -oac mp3lame -lameopts cbr:br=%d", i_audio_kbps); 
		}
		else if (i_audio_format == AUDIO_TYPE_AC3)
		{
			
			if ((ar_ac3_info[i_audio_stream_id].i_kbps == i_audio_kbps) && 
				!b_convert_ac3_to_2channels)
			{
				// AC3 6 channel same bitrate and convert NOT to 2 channels --> do copy
				audioArgs.AppendFormat(" -oac copy");
			}
			else if ((ar_ac3_info[i_audio_stream_id].i_kbps == i_audio_kbps) && 
				(ar_ac3_info[i_audio_stream_id].i_num_channels == 2) && b_convert_ac3_to_2channels)
			{
				// AC3 2 channel same bitrate and convert to 2 channels --> do copy
				audioArgs.AppendFormat(" -oac copy");
			}
			else
			{
				// Recompress AC3
				if (i_create_avi_options == 0)
				{
					// Always do this in 1st pass
					sAvcOpts.Format(" -lavcopts acodec=ac3:abitrate=%d", i_audio_kbps);
				}

				if (b_convert_ac3_to_2channels)
				{
					audioArgs.Append(" -oac lavc -af channels=2");
				}
				else
				{
					audioArgs.AppendFormat(" -oac lavc -af channels=%d",
						ar_ac3_info[i_audio_stream_id].i_num_channels);
				}
			}
		}

		if (i_create_avi_options == 0)
		{
			enc1_execline.Append(audioArgs);
		}
		else
		{
			enc1_execline.Append(" -nosound");
			enc2_execline.Append(audioArgs);
		}
	}
	else
	{
		enc1_execline.Append(" -nosound");
		enc2_execline.Append(" -nosound");
	}

	return bAvcOptsAdded;
}

bool CEncodingInfo::AddAvcOpts(void)
{
	CString tmpStr;
	CString tmpStr1;
	CString tmpStr2;
	CString videoCodec;
	CString passLogFile;

	if (i_codec_version == MPEG4)
	{
		// mpeg4 codec
		videoCodec.Format(" -ovc lavc");

		tmpStr.Format("threads=2:vcodec=mpeg4:vbitrate=%d:keyint=%d",
			i_video_bitrate,
			i_max_keyframe_interval);

		if (b_adv_qpel)
		{
			tmpStr.Append(":qpel");
		}

		if (b_adv_lumi_mask)
		{
			tmpStr.Append(":lumi_mask=0.15");
		}

		if (b_adv_bi)
		{
			tmpStr.Append(":vmax_b_frames=1");
		}

		if (b_adv_dark_mask)
		{
			tmpStr.Append(":dark_mask=0.15");
		}

		if (b_adv_vhq)
		{
			tmpStr.Append(":vhq");
		}

		if (sAvcOpts.IsEmpty())
		{
			sAvcOpts.Format(" -lavcopts %s", tmpStr);
		}	
		else
		{
			sAvcOpts.AppendFormat(":%s", tmpStr);
		}

		tmpStr1 = sAvcOpts;
		tmpStr2 = sAvcOpts;
		
		if (i_create_avi_options == 0)
		{
			// 1 pass mode
		}
		else 
		{
			passLogFile.Format(" -passlogfile \"%s\"", sTwoPassLogFile);
			tmpStr1.AppendFormat(":vpass=%d", 1);
			tmpStr2.AppendFormat(":vpass=%d", 2);
		}
	}
	else if (i_codec_version == XVID)
	{
		// xvid
		videoCodec.Format(" -ovc xvid");
		tmpStr.AppendFormat(" -xvidencopts threads=2:bitrate=%d:max_key_interval=%d", 
			i_video_bitrate, i_max_keyframe_interval);

		if (b_adv_qpel)
		{
			tmpStr.Append(":qpel");
		}

		if (b_adv_trellis)
		{
			tmpStr.Append(":trellis");
		}

		if (b_adv_gmc)
		{
			tmpStr.Append(":gmc");
		}

		if (b_adv_lumi_mask)
		{
			tmpStr.Append(":lumi_mask=0.15");
		}

		if (b_adv_bi)
		{
			tmpStr.Append(":max_bframes=1");
		}

		if (b_adv_dark_mask)
		{
			tmpStr.Append(":dark_mask=0.15");
		}

		if (b_adv_vhq)
		{
			tmpStr.Append(":vhq=3");
		}

		if (!sAvcOpts.IsEmpty())
		{
			tmpStr.Append(sAvcOpts);
		}

		tmpStr1 = tmpStr;
		tmpStr2 = tmpStr;
		
		if (i_create_avi_options == 0)
		{
			// 1 pass mode
		}
		else 
		{
			passLogFile.Format(" -passlogfile \"%s\"", sTwoPassLogFile);
			tmpStr1.AppendFormat(":pass=%d", 1);
			tmpStr2.AppendFormat(":pass=%d", 2);
		}
	}
	else if (i_codec_version == H264)
	{
		// h264
		videoCodec.Format(" -ovc x264");
		tmpStr.AppendFormat(" -x264encopts bitrate=%d:keyint=%d:threads=auto", 
			i_video_bitrate, i_max_keyframe_interval);

		if (b_adv_bi)
		{
			tmpStr.Append(":bframes=2");
		}

		if (!sAvcOpts.IsEmpty())
		{
			tmpStr.Append(sAvcOpts);
		}

		tmpStr1 = tmpStr;
		tmpStr2 = tmpStr;
		
		if (i_create_avi_options == 0)
		{
			// 1 pass mode
		}
		else 
		{
			passLogFile.Format(" -passlogfile \"%s\"", sTwoPassLogFile);
			tmpStr1.AppendFormat(":pass=%d", 1);
			tmpStr2.AppendFormat(":pass=%d", 2);
		}
	}

	enc1_execline.Append(tmpStr1);
	enc2_execline.Append(tmpStr2);

	enc1_execline.Append(videoCodec);
	enc2_execline.Append(videoCodec);
	
	enc1_execline.Append(passLogFile);
	enc2_execline.Append(passLogFile);

	if (!fourcc.IsEmpty())
	{
		enc1_execline.AppendFormat(" -ffourcc %s", fourcc);
		enc2_execline.AppendFormat(" -ffourcc %s", fourcc);
	}
	
	return false;
}

void CEncodingInfo::CreateAudioFilter(void)
{
	if (b_create_audio)
	{
		if (i_audio_format != AUDIO_TYPE_AC3)
		{
			if (b_amplify_audio)
			{
				sAudioFilter.Format(" -af volume=%d", i_audio_amplify);
			}

			if (i_norm_wave >= 1)
			{
				if (sAudioFilter.IsEmpty())
				{
					sAudioFilter.Format(" -af volnorm");
				}
			}

			if (i_hertz != 48000)
			{
				CString sTmp;
				sTmp.Format("resample=%d", i_hertz);

				if (sAudioFilter.IsEmpty())
				{
					sAudioFilter.Format(" -af %s", sTmp);
				}
				else
				{
					sAudioFilter.AppendFormat(",%s", sTmp);
				}

				sAudioFilter.AppendFormat(" -srate %d", i_hertz);
			}
		}

		if (i_create_avi_options == 0)
		{
			enc1_execline.Append(sAudioFilter);
		}
		else
		{
			enc2_execline.Append(sAudioFilter);
		}
	}
}

void CEncodingInfo::PrintEncLine(BOOL b_debug_mode)
{
	if (b_debug_mode)
	{
		CString s_tmp;
		FILE *fp;
		s_tmp.Format("%s\\%s_%s", s_TempFolder, s_ProjName,	MENCODERARGS);

		fp = fopen(s_tmp.GetBuffer(), "wt");
		fprintf(fp, "%s %s\n", MENCODER, enc1_execline);
		fprintf(fp, "%s %s\n", MENCODER, enc2_execline);
		fprintf(fp, "%s %s\n", MPLAYER, play_execline);
		fclose(fp);
	}
}

void CEncodingInfo::CreateMKVMergeExecLine()
{

}