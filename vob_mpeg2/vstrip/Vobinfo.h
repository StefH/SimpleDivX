#ifndef _VOBINFO_H_INCLUDED
#define _VOBINFO_H_INCLUDED

#ifdef VSTRIP_DLL
#define VSTRIP_DLL_API __declspec(dllexport)
#else
#define VSTRIP_DLL_API
#endif

#define MAX_AUDIO	(8)
#define MAX_STRMS	(32)
#define MAX_SUBS	(64)
#define MAX_TITLES	(64)
#define STRING128	(128) // enough to hold language strings

typedef enum _AUDIO_TYPE
{
	AUDIO_TYPE_INVALID = 0,
	AUDIO_TYPE_AC3,
	AUDIO_TYPE_MP3,
	AUDIO_TYPE_MP2,
	AUDIO_TYPE_LPCM,
	AUDIO_TYPE_OGG,
	AUDIO_TYPE_DTS,
	AUDIO_TYPE_MPEG1,
	AUDIO_TYPE_MPEG2,
	AUDIO_TYPE_AAC,
	AUDIO_TYPE_LAST
} AUDIO_TYPE;

typedef enum _STREAM_TYPE
{
	STREAM_TYPE_INVALID = AUDIO_TYPE_LAST + 1,
	STREAM_TYPE_MPEG1_VIDEO,
	STREAM_TYPE_MPEG2_VIDEO,
	STREAM_TYPE_PRIVATE,
	STREAM_TYPE_SUBTITLES
} STREAM_TYPE;

typedef struct _AUDIO_LIST
{
	unsigned char	id;
	AUDIO_TYPE		type;
} AUDIO_LIST;

typedef struct _STREAM_LIST
{
	unsigned char	id;
	STREAM_TYPE		type;
} STREAM_LIST;

typedef struct
{
	int i_kbps;
	int i_num_channels;
	int i_khz;
	unsigned char uc_id;
	int valid;
} TS_AUDIO_INFO;

typedef struct _VOBINFO
{
	int i_width; // width
	int i_height; // height
	int i_cropping_height;

	double f_framerate; // framerate
	double f_aspect_ratio; // ratio
	double audio_delay[MAX_AUDIO];
	
	char *s_aspect_ratio; // ratio in text
	
	long l_filesize; // filesize in kilobyte
	long l_time_start; // start (in milli seconds)
	long l_time_end; // end (in milli seconds)

	int  i_audio_streams; // number of audio streams
	int  i_video_streams; // number of found video streams
	int  i_subs; // number of found subs

	AUDIO_LIST audio_list[MAX_AUDIO]; // list from audio languages
	STREAM_LIST video_list[MAX_STRMS]; // list from video streams

	TS_AUDIO_INFO audio_info; // only used for TS streams...
} VOBINFO;

typedef struct title_info_
{
	int i_num_chapters;
	int l_length;
} TITLE_INFO;

typedef struct movie_info
{
	char s_video_info[STRING128]; // video info
	int  i_audio_streams; // number of audio streams
	int  i_num_subs;
	unsigned char audio_list[MAX_AUDIO]; // list from audio langages (id's)
	unsigned char sub_list[MAX_AUDIO]; // list from subs (id's)
	char as_audio_languages[MAX_AUDIO][STRING128];
	char as_subs[MAX_AUDIO][STRING128];
	char as_subs_short[MAX_AUDIO][3];

	int i_num_titles;
	int i_start_title_id;
	int i_end_title_id;
	TITLE_INFO r_titles[MAX_TITLES];
} MOVIE_INFO;

#ifdef __cplusplus
extern "C" {
#endif

	VSTRIP_DLL_API int get_vob_info(char *s_inputfile, 
		int b_search_for_audio,
		int b_search_for_end,
		int b_search_for_cropping,
		VOBINFO *pr_vobinfo,
		char *s_tmp_folder);

	VSTRIP_DLL_API int get_ts_info(char *s_inputfile, 
				 int stream_type,
				 int b_search_for_cropping,
				 VOBINFO* pr_tsinfo);

	VSTRIP_DLL_API int parse_movie_ifo(char *s_ifofile, 
		MOVIE_INFO *pr_movie_info);

	VSTRIP_DLL_API char * get_vobinfo_version();

	/*
	** Used for retrieving auto-resizign and cropping.
	*/
	VSTRIP_DLL_API int auto_calc_size(int i_new_width, 
		int i_original_height, 
		int i_original_width,
		int i_cropping_height,
		float f_aspect_ratio,
		int *pi_cx, 
		int *pi_cy, 
		int *pi_dx, 
		int *pi_dy);

#ifdef __cplusplus
}
#endif

#endif
