#ifndef _PARSE_IFO_H_INCLUDED

/*************************************************************************
  vStrip by [maven] (maven@maven.de)

  based on code originally written by Thomas Mirlacher (dent@cosy.sbg.ac.at)
  for the "free InFormatiOn project" (http://www.linuxvideo.org)
*************************************************************************/

#define _PARSE_IFO_H_INCLUDED

#include "s_types.h"
#include "vstrip.h"

/* Swap bytes in 16 bit value.  */
#define be2me_16(x) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))
/* Swap bytes in 32 bit value.  */
#define be2me_32(x) ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

typedef byte uint8_t;
typedef word uint16_t;
typedef dword uint32_t;

#ifdef _WIN32
#pragma pack(push, 1)
#endif

typedef struct
{
	uint8_t	hour;
	uint8_t	minute;
	uint8_t	second;
	uint8_t	frame_u;		// The two high bits are the frame rate
} dvd_time_t;

#define DVD_TIME_AND 0x3f // to and the frame_u member with

typedef struct
{ // Cell Playback Information
	uint8_t	chain_info; // 0x5e 0xde(2 angles, no overlay), 0x5f 0x9f 0x9f 0xdf(4 angles overlay), 0x2 0xa 0x8(1 angle)
	uint8_t foo;				// parental control ??
	uint8_t still_time;
	uint8_t cell_cmd;
  dvd_time_t playback_time;
  uint32_t vobu_start;  			// 1st vobu start
  uint32_t ilvu_end;
  uint32_t vobu_last_start;
  uint32_t vobu_last_end;
} ifo_pgci_caddr_t;

typedef struct
{ // Cell Position Information
	uint16_t vob_id		: 16;	// Video Object Identifier
	uint8_t  foo		: 8;	// Unknown
	uint8_t  cell_id	: 8;	// Cell Identifier
} ifo_pgc_cpos_t;

typedef struct
{ // Audio Status
	uint8_t link		: 7;
	uint8_t available	: 1;
	uint8_t foo		: 8;	// UNKNOWN
} audio_status_t;

typedef struct
{ // Subpicture status
	uint8_t format4_3	: 7;
	uint8_t available	: 1;
	uint8_t wide		: 8;
	uint8_t letter		: 8;
	uint8_t pan		: 8;
} subp_status_t;

typedef struct
{ // CLUT == Color LookUp Table
	uint8_t foo		: 8;    // UNKNOWN: 0x00?
	uint8_t y		: 8;
	uint8_t cr		: 8;
	uint8_t cb		: 8;
} clut_t;

typedef uint8_t command_data_t[8];

typedef struct
{ // PGC Command Table
	uint16_t nr_of_pre;
	uint16_t nr_of_post;
	uint16_t nr_of_cell;
	uint16_t tbl_len;
	command_data_t *pre_commands;
	command_data_t *post_commands;
	command_data_t *cell_commands;
} pgc_command_tbl_t;

typedef uint8_t pgc_program_map_t;

#define IFO_NUM_AUDIO 8
#define IFO_NUM_SPU 32

typedef struct
{ // Program Chain Information
	uint16_t zero_1;
	uint8_t	 nr_of_programs;
	uint8_t	 nr_of_cells;
	dvd_time_t playback_time;
	uint32_t prohibited_ops;	// New type?
	audio_status_t audio_status[IFO_NUM_AUDIO];
	subp_status_t subp_status[IFO_NUM_SPU];
	uint16_t next_pgc_nr;
	uint16_t prev_pgc_nr;
	uint16_t goup_pgc_nr;
	uint8_t	 still_time;
	uint8_t	 pg_playback_mode;
	clut_t   clut[16];
	uint16_t pgc_command_tbl_offset;
	uint16_t pgc_program_map_offset;
	uint16_t cell_playback_tbl_offset;
	uint16_t cell_position_tbl_offset;
/*	pgc_command_tbl_t	*pgc_command_tbl;
	pgc_program_map_t	*pgc_program_map;
	ifo_pgci_caddr_t	*cell_playback_tbl;
	ifo_pgc_cpos_t		*cell_position_tbl;*/
} pgc_t;

#define IFO_HDR_LEN 8

typedef struct
{
	uint16_t num : 16; // number of entries
	uint16_t reserved: 16; // don't known (reserved?)
	uint32_t len	: 32; // length of table
} ifo_hdr_t;

typedef struct
{
	uint16_t id	: 16;	// Language
	uint16_t	: 16;	// don't know
	uint32_t start	: 32;	// Start of unit
} pgci_sub_t;

typedef struct
{ // 8 bytes
	uint8_t appl_mode		: 2;
	uint8_t type			: 2;
	uint8_t multichannel_extension	: 1;
	uint8_t coding_mode		: 3;

	uint8_t num_channels		: 3;
	uint8_t				: 1;
	uint8_t sample_freq		: 2;
	uint8_t quantization		: 2;
	uint16_t lang_code		: 16;   // <char> description
	uint8_t  foo			: 8;    // 0x00000000 ?
	uint8_t  caption		: 8;
	uint8_t  bar			: 8;    // 0x00000000 ?
	uint8_t  bar2			: 8;
} ifo_audio_t;

typedef struct {
	uint16_t	num		: 16;	// number of subchannels
	ifo_audio_t	audio[IFO_NUM_AUDIO]; // info
} audio_hdr_t;

typedef struct
{
	uint16_t prefix		: 16;	// 0x0100 ?
	uint16_t lang_code		: 16;	// <char> description
	uint8_t foo			: 8;	// dont know
	uint8_t caption		: 8;	// 0x00 ?
} ifo_spu_t;

typedef struct {
	uint16_t  num	: 16;	// number of SPUs
	ifo_spu_t	spu[IFO_NUM_SPU];
} spu_hdr_t;

typedef struct
{
	uint8_t			no_auto_letterbox		: 1;
	uint8_t			no_auto_pan_scan		: 1;
	uint8_t			apect_ratio				: 2;
	uint8_t			video_standard			: 2;
	uint8_t			coding_mode				: 2;

	uint8_t			film					: 1;
	uint8_t			letterboxed				: 1;
	uint8_t			resolution				: 2;
	uint8_t			bitrate					: 1;
	uint8_t			foo						: 3;
} ifo_video_t;

#ifdef _WIN32
#pragma pack(pop)
#endif

#define IFO_OFFSET_VIDEO	0x0200
#define IFO_OFFSET_AUDIO	0x0202
#define IFO_OFFSET_SUBPIC	0x0254

typedef struct
{
	byte *mat;
	byte *title_pgci;
} ifo_t;

typedef enum
{
	ifo_PRINT_INFO = 0x01, ifo_PRINT_CELL_POS = 0x02, ifo_USE_ASPI = 0x04, ifo_PREFER_ASPI = 0x08, ifo_FORCE_DWORD = -1
} t_ifo_flags;

VSTRIP_DLL_API ifo_t *ifoOpen(const char *name, const dword fio_flags);
VSTRIP_DLL_API vstripbool ifoClose (ifo_t *ifo);
VSTRIP_DLL_API int ifoGetNumPGCI(const ifo_t *ifo);
VSTRIP_DLL_API int ifoGetPGCIInfo(const ifo_t *ifo, const dword title, unsigned long*);
VSTRIP_DLL_API vstripbool ifoGetPGCICells(const ifo_t *ifo, const dword title, tp_vs_vobcellid cells);
VSTRIP_DLL_API char *ifoGetVideoDesc(const ifo_t *ifo);

VSTRIP_DLL_API int ifoGetNumAudio(const ifo_t *ifo);
VSTRIP_DLL_API char *ifoGetAudioDesc(const ifo_t *ifo, const int audio_idx, int *pi);

VSTRIP_DLL_API int ifoGetNumSubPic(const ifo_t *ifo);
VSTRIP_DLL_API uint16_t ifoGetSubPicCode(const ifo_t *ifo, const int subp_idx);

vstripbool ifoParse(const char *ifo_name, 
					const t_ifo_flags flags, 
					tp_vs_vobcellid *cell_ids,
					MOVIE_INFO *pr_movie_info);

#endif