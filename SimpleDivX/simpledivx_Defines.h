// SimpleDivX_Defines.h:
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SimpleDivX_DEFINES_H__BEC437C9_8A4D_4ADE_B82C_C35D472CD063__INCLUDED_)
#define AFX_SimpleDivX_DEFINES_H__BEC437C9_8A4D_4ADE_B82C_C35D472CD063__INCLUDED_

#include "my_includes.h"
#include "../genericmm/genericmm.h"
#include "genericsystem.h"
#include "../vob_mpeg2/vstrip/vobinfo.h"
#include <afxtempl.h>

#define  APP_VERSION   	"1.5.0"
#define  APP_NAME	   	"SimpleDivX"
#define  EMAIL_ADDRESS 	"SimpleDivX@gmail.com"
#define  HOMEPAGE_URL	"http://www.simpledivx.org"

#define  MENCODER		"mencoder.exe"
//#define  MENCODER_AMDXP "mencoder-AMDXP.exe"
//#define  MENCODER_AMD64 "mencoder-AMD64.exe"
//#define  MENCODER_P3    "mencoder-Pentium3.exe"
//#define  MENCODER_P4    "mencoder-Pentium4.exe"
#define  MPLAYER		"mplayer.exe"
#define  VIRTUALDUB		"VirtualDubMod.exe" 
#define  MKVMERGE		"mkvmerge.exe"
#define  OGG_DLL		"ogg.dll"
//#define  ICONV_DLL	"iconv.dll"
//#define  INTL_DLL		"intl.dll"
#define  VOBSUB_DLL		"vobsub.dll"
#define  VOBSUB_IDX		".idx"
#define  VOBSUB_SUB		".sub"
#define  VOBSUB_TMP		".vobsub"

#define  MENCODERARGS	"MEncoder_Command_Line.txt"
#define  BATCH_LOG		"BatchEncoding.log"
#define  ANALYSE_LOG	"Analyse.log"

#define  VD_TMP			"VirtualDubProcessingSettings.vcf"
#define  _AVI			"_.avi"
#define  AVI			".avi"
#define  OGM			".ogm"
#define  MATROSKA		".mkv"
#define  MP4			".mp4"
#define  H264_STR		".h264"
#define  LOG			".log"
#define  NFO			".nfo"

#define  _LIST			".lst"
#define  DEBUGFILE		"SimpleDivX[debug].log"
#define  DIVXLOG_FILE	"logfile.log"
#define  DIVXLOGFILEEXT ".log"
#define  PART           " - Part "

#define  MAXFOR			38      // max outputformats (video)"
#define  MAXAFOR		15		// max audio formats (mp3/ogg)  {64,80,...,640};
#define  MAXAUTOFORMATS 14
#define  NUMQ			10
#define  MAXHANDLES		10
#define  MAXTHREADS		3
#define  MAXVOBFILES    64
#define  MAXIFOFILES    64
#define  STRLEN512		512
#define  STRLEN			256
#define  STRLEN128		128
#define  STRLEN64		64
#define  STRLEN32		32
#define  STRLEN16		16
#define  DIVXMODE0		"1-pass"
#define  DIVXMODE2		"2-pass, first pass"
#define  DIVXMODE3		"2-pass, second pass"
#define	 ENDCREDITSKBPS 50
#define	 ENDCREDITSPCT	20
#define  MINDIVX5KBPS	10
#define  MAXDIVX5KBPS	16000
#define  MAXSPLIT       20
#define  STR_AUDIO_MP3	"MP3"
#define  STR_AUDIO_OGG	"OGG"
#define  STR_AUDIO_AC3	"AC3"
#define  STR_AUDIO_PCM	"LPCM"
#define  STR_AUDIO_MP2	"MP2"
#define  WAITING		0
#define  BUSY			1
#define  READY			2
#define  INVALID		3
#define  DEFAULTAC3KBPS 448
#define  DEFAULTAC3CHANNELS 5
#define  AUTO_END		0
#define  MANUAL_END		1
#define  ENDCREDITS_MAX_QUANT 12
#define  ENDCREDITS_MIN_QUANT 8
#define  DIVX4DEFAULTENDKBPS 50
#define  MAX_BATCH		200
#define  MAXDB			20

#define  LANG			".lang"
#define  LANGUAGE_FOLDER "Languages"
#define  TOOLS_FOLDER	"Tools"
#define  MAX_GUI_LANG	99
#define  DEF_GUI_LANG	"English"
#define  NORMAL_PRIORITY 3

typedef enum _src_fmt_
{
	SF_MOVIE = 0,
	SF_VIDEO,
	SF_CUSTOM
} SOURCE_FORMAT;

typedef enum _ifo_
{
	IM_IFO = 0,
	IM_FILE,
	_LAST_IM
} INPUT_MODE;

typedef enum _ftype_
{
	FT_VOB = 0,
	FT_TS,
	_LAST_FT
} FILE_TYPE;

typedef enum _con_
{
	CF_AVI = 0,
	CF_OGM,
	CF_MATROSKA,
	CF_MP4,
	_LAST_CF
} CONTAINER_FORMAT;

typedef enum _subtitles_way_to_do_
{
	VOBSUB = 0
} SUBTITLES;

typedef enum _video_codec
{
	MPEG4 = 0,
	XVID,
	H264,
	CUSTOM_CODEC,
	_LAST_CODEC
} VIDEO_CODEC;

typedef struct _cont
{
	CString s_file_ext;
	CString s_vdub_type;
} C_NFO;

typedef struct __aud
{
	CString s_file_ext;
	CString s_descr;
	CString s_vdub_tag;
	CString s_transcoding_start;
	CString s_transcoding_end;
} A_NFO;

typedef enum __splitmode
{
	SPLIT_NONE = 0,
	SPLIT_SIZE,
	SPLIT_FADEOUT
} SPLIT_MODE;

typedef enum __endmode
{
	NORMAL_END = 0,
	REMOVE_END
} END_MODE;

// Order is important
typedef enum _pages
{
   PAGE_INPUT = 0,     
   PAGE_VIDEO,
   PAGE_AUDIO,
   PAGE_CODEC,
   PAGE_COMPRESSION,
   PAGE_MISC,
   PAGE_PROJECT,
   PAGE_OUTPUT,
   PAGE_SETUP,
   PAGE_VIDEO_ADV,
   PAGE_PROGRAM_LOCATIONS,
   PAGE_GENERAL,
   _PAGE_LAST
} PAGE;


// --------------------------------------------------------------

typedef struct vob_file
{
	CString s_filename;
	CString s_short_filename;
	long l_size;
	int r_type;
} VOB_FILE;

typedef struct _ifo_file
{
	CString s_filename;
	CString s_short_filename;

	int b_valid_ifo;
	int i_num_vobfiles;

	long l_size;

	VOB_FILE vob_files[MAXVOBFILES];

	MOVIE_INFO r_movie_info;

} IFO_FILE;

typedef struct _video_formats
{
	int CX;
	int CY;
	int DX;
	int DY;
	CString str;
	double ratio;
} OF;

typedef struct _subtitles_
{
	CString commandLine;
	CString splitCommandLine[MAXSPLIT];
	CString projName;
	int i_num_parts;
} SUBTITLES_INFO;

typedef struct _vci
{
	CString s_name;
	CString s_short_name;
	CString fourcc;
	bool b_valid;
	int  i_id;
	int  i_dropdown_id;
	bool b_TwoPassAllowed;
	bool b_EditKeyFrameAllowed;
	bool b_AdvancedSettingsAllowed;
	bool b_EndCreditsRecompressionAllowed;
	bool b_StartDivXAuto;
	bool b_LogFileSettingsEnabled;
	bool b_PassModeEnabled;
} VIDEO_CODEC_INFO;

typedef struct _divx_quality
{
	double f_bits_per_pixel;
	CString s_q;
} DIVX_QUALITY;

#endif 


