/*
** Header file.
*/
#ifndef _GENMM_H
#define _GENMM_H

#define MAX_MEDIA_DATA 10

typedef struct
{
	int i_kbps;
	int i_num_channels;
	int i_khz;
	bool b_valid;
	unsigned char uc_id;
} AC3_INFO;

typedef struct
{
	long l_milliseconds;
	double f_fps; 
	char s_time_hmss[32];
} MEDIA_INFO;

typedef enum
{
	MT_VIDEO = 0,
	MT_AUDIO,
	MT_SUBTITLE
} MTYPE;

typedef struct
{
	char s_filename[512]; // limited to 512 chars
	char s_description[256]; // limited to 256 chars
	MTYPE r_mediatype;
} MEDIA_DATA;

typedef enum
{
	MUX_OGM = 0,
	MUX_MATROSKA
} MUX_TYPE;

typedef struct
{
	MEDIA_DATA r_mediadata[MAX_MEDIA_DATA];
	MUX_TYPE r_mux_type;
	long l_split; // Split At milliseconds
} MUX_DATA;

typedef struct SPLIT_DATA_
{
	int i_num_points;
	int *ai_points;
} SPLIT_DATA;

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the genericmm_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// genericmm_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef genericmm_EXPORTS
#define genericmm_API __declspec(dllexport)
#else
#define genericmm_API __declspec(dllimport)
#endif
/*
**
*/
genericmm_API int DirectShowCombine(MUX_DATA *pr_muxdata);

/*
**
** Function : GetAc3Info
**
** Description : 
** This function will give you info about an ac3 file.
**
** Parameters :
** (in)     char *s_file , input filename. (absolute path)
** (out)    AC3_INFO *pr_info, info about the ac3.
**
** Return values :
** version
**
*/
genericmm_API int GetAc3Info(char *szFile, AC3_INFO *pr_info);


/*
** TODO
*/
genericmm_API int CorrectAC3HeaderInAvi(char* filename, int channels);

/*
**
** Function : AddWaveHeaderToAc3
**
** Description : 
** This function will add a wave haeder to an ac3 file.
**
** Parameters :
** (in)     char *strAC3 , input filename. (absolute path)
** (in)     char *strWAV , output filename. (absolute path)
** (in)     AC3_INFO *pr_info , input ac3-info. (can be NULL)
**
** Return values :
** version
**
*/
genericmm_API void AddWaveHeaderToAc3(char *strAC3, char *strWAV, AC3_INFO *pr_info);

/*
**
** Function : get_version
**
** Description : 
** This function will give you the version from the "genericmm.dll".
**
** Parameters :
** (none)
**
** Return values :
** version
**
*/
genericmm_API char* get_version(void);

/*
**
** Function : filesize2frame
**
** Description : 
** This function will give you the keyframe from an AVI file at a certain filesize.
**
** Parameters :
** (in)     char *s_file , input video filename. (absolute path)
** (in)     long l_size, filesize in bytes.
** (in)     int i_audio_kbps, kbps from audio stream (leave 0 if no audio)
** (out)    long *pl_frame, keyframe.
**
** Return values :
**  0 , function was succesful.
** -1 , no keyframe at the given filesize was found.
**
*/
genericmm_API int filesize2frame(char *s_file, 
								  long l_size, 
								  int i_audio_kbps,
								  long *pl_frame);

/*
**
** Function : filesize2frame
**
** Description : 
** This function will give you the keyframe from an AVI file at a certain filesize.
** NOTE : this function is used when you want to use 2 audio streams in the
** avi file.  1 AC3 and 1 MP3 audio stream. THe kbps from the ac3-file must be filled 
** in 'i_audio_kbps' the kbps from the mp3 file is automatically found.
**
** Parameters :
** (in)     char *s_video_file , input video filename. (absolute path)
** (in)     char *s_mp3_file , input mp3 filename. (absolute path)
** (in)     long l_size, filesize in bytes.
** (in)     int i_audio_kbps, kbps from the ac3 audio stream (leave 0 if only mp3 is wanted)
** (out)    long *pl_frame, keyframe.
**
** Return values :
**  0 , function was succesful.
** -1 , no keyframe at the given filesize was found.
**
*/
genericmm_API int filesize2frame(char *s_video_file, 
								 char *s_mp3_file, 
								 long l_size,
								 int i_audio_kbps,
								 long *pl_frame);


/*
*/
genericmm_API int search_for_split_points(char *s_video_file, 
										  int i_resolution,
										  int i_threshold,
										  int i_min_time_between_points,
										  int i_split_on_keyframe, 
										  SPLIT_DATA *pr_data);

/*
**
** Function : get_avi_info
**
** Description : 
** This function will give you some basic info of an avi.
**
** Parameters :
** (in)     char *s_file , input filename. (absolute path)
** (out)    long *pl_frames, number of frames.
** (out)    double *pf_fps, frames per second.
** (out)    long *pl_seconds, number of seconds.
**
** Return values :
**  0 , function was succesful.
** -1 , error.
**
*/
genericmm_API int get_avi_info(	char *s_file, double *pf_fps, long *pl_frames, long *pl_seconds);

/*
**
*/
genericmm_API int get_media_info(char *s_avi_file, MEDIA_INFO *media_info);

/*
**
** Function : getfilesize
**
** Description : 
** This function will give the filesize from a file in bytes.
**
** Parameters :
** (in)     char *s_file , input filename. (absolute path)
** (out)    long *pl_size, number of bytes.
**
** Return values :
**  0 , function was succesful.
** -1 , error.
**
*/
genericmm_API int getfilesize(const char *s_file, 
							  long *pl_size);

genericmm_API int get_endcredits_pos( char *s_divx_logfile, 
									  long *pl_frame,
									  long *pl_tot_frames,
									  int i_input_motion, int notused);

genericmm_API int get_endcredits_pos_xvid(char *s_xvid_logfile, 
										long *pl_frame,
										long *pl_tot_frames,
										int i_found_max);

genericmm_API int combine_avi_ogg(char *s_avi_file, 
								  char *s_ogg_file,
								  char *s_out_file,
								  long i_split_at_milli_sec,
								  char *s_video_name,
								  char *s_audio_name);

genericmm_API int combine_avi_ogg2(char *s_avi_file, 
								  char *s_ogg_file,
								  char *s_out_file,
								  long i_split_at_milli_sec,
								  char *s_video_name,
								  char *s_audio_name);

genericmm_API int get_nearest_keyframe(char *s_file,
								       int i_seconds,
								       long *pl_frame,
									   double *f_fps);


/*
**
** Function : GetCodecInfo
**
** Description : 
** This function will search for all installed video codecs.
** (Direct Show is used for this. This function depends on the correct
** installation from all directshow compressors. Not that reliable.)
**
** Parameters :
** (in)     char *s_file , input filename. (absolute path)
** (out)    long *pl_size, number of bytes.
**
** Return values :
**  0 , function was succesful.
** -1 , error.
**
*/
genericmm_API int GetCodecInfo(char **sa_codecs);

/*
**
** Function : GetCodecs
**
** Description : 
** This function will search for all installed video codecs.
**
** Parameters :
** (out)    char **sa_codecs, allocated array from all codecs found.
**
** Return values :
**  0 , function was succesful.
** -1 , error.
**
*/
genericmm_API int GetCodecs(char ***sa_codecs);

genericmm_API void FreeCodecs(int i_num_codecs, char ***as_codecs);


/* some time conversion functions */
genericmm_API void seconds2timeformat(long seconds, long *p_hour, long *p_min, long *p_sec);
genericmm_API void milliseconds2timeformat(long mseconds, long *p_hour, long *p_min, long *p_sec, long *p_msec);
genericmm_API void timeformat2timestring(long hour, long min, long sec, long msec, char *string);
genericmm_API void milliseconds2timeformat(long mseconds, char *string);
genericmm_API void seconds2timeformat(long mseconds, char *string);
genericmm_API void seconds2timeformat_hms(long seconds, char *string);
genericmm_API void seconds2timeformat_hmss(long mseconds, char *string);


/* mp3 functions */
genericmm_API int AddWaveHeaderToMp3(char *strMp3);

genericmm_API int CorrectWaveHeaderFromMp3(char *strMp3);

genericmm_API int mp3_get_num_secs(char *strMp3);

#endif
