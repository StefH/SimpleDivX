#ifndef _VSTRIP_H_INCLUDED


#define _VSTRIP_H_INCLUDED

//#define vs_DECRYPT

#ifdef vs_DECRYPT
#define vs_VERSION "0.8d_css"
#define vs_AUTHOR "[maven] (maven@maven.de), CSS-code by R0TfL"
#else // !vs_DECRYPT
#define vs_VERSION "0.8d"
#define vs_AUTHOR "[maven] (maven@maven.de)"
#endif // vs_DECRYPT

#define vs_STDERR stdout
#define vs_STDOUT stdout

#include <stdio.h>
#include "s_types.h"
#include "file_io.h"
#include "in_buffer.h"
#include "vobinfo.h"


#define PACKET_START_CODE_PREFIX 0x000001

/* code definition */
#define PICTURE_START_CODE			0x100
#define SLICE_START_CODE_MIN		0x101
#define SLICE_START_CODE_MAX		0x1AF
#define USER_DATA_START_CODE		0x1B2
#define SEQUENCE_HEADER_CODE		0x1B3
#define EXTENSION_START_CODE		0x1B5
#define SEQUENCE_END_CODE			0x1B7
#define GROUP_START_CODE			0x1B8

#define SYSTEM_END_CODE				0x1B9
#define PACK_START_CODE				0x1BA
#define SYSTEM_START_CODE			0x1BB

#define SUBSTREAM_AC3            0x80 // 128
#define MAX_SUBSTREAM_AC3		 0x87 // 135

#define SUBSTREAM_PCM            0xa0 // 160
#define MAX_SUBSTREAM_PCM		 0xa7 // 167

#define AUDIO_STREAM  			 0xc0 // 192
#define MAX_AUDIO_STREAM		 0xc7 // 197

#define VIDEO_STREAM  			 0xe0
#define MAX_VIDEO_STREAM		 0xe7

#define PROGRAM_STREAM_MAP       0xbc
#define PRIVATE_STREAM_1         0xbd
#define PADDING_STREAM           0xbe
#define PRIVATE_STREAM_2         0xbf
#define ECM_STREAM               0xf0
#define EMM_STREAM               0xf1
#define PROGRAM_STREAM_DIRECTORY 0xff
#define DSMCC_STREAM             0xf2
#define ITUTRECH222TYPEE_STREAM  0xf8

#define MAX_FRAMERATE			 8
#define MAX_ASPECTRATIO			 4
#define MAX_VOBIDS				 256
#define MAX_CELLIDS				 256

#define vs_MACROVISION_BITS		 0x80

#define CSS_OFFSET				 0x80
#ifdef vs_DECRYPT
#define	CSS_BYTES_NEEDED		 10
#define CSS_MAX_ASSUME_PADDING	 (fio_SECTOR_SIZE - CSS_OFFSET)
#define CSS_MIN_GUESS_REPEAT		 2
#define CSS_MIN_GUESS_LENGTH		 16
#define CSS_MIN_GUESS_PERIOD		 2
#define CSS_MAX_GUESS_PERIOD		 64

typedef union
{
	dword	d;
	byte	b[4];
} t_vs_key_union;

typedef struct
{
	t_vs_key_union key;
	dword	found;
	void *next;
} t_vs_4key, *tp_vs_4key;

typedef struct
{
	// input
	dword				same_guess;	// number of guesses of the same key to consider the percentage
	dword				pad_guess;	// how much correct guesses we give for a padding-stream guess
	dword				pc_guess;		// percentage our candidate key has to be of all the keys that we found
	// output
	dword				total_keys;	// total number of keys found
	tp_vs_4key	*keys;			// if != NULL, pointer to 256 tp_vs_4keys linked list (index represents 1st byte)
	tp_vs_4key	key;				// != if found (to key that first fullfilled above conditions)
	byte				k0; 				// idx into keys of result key
} t_vs_keysearch, *tp_vs_keysearch;

#endif

typedef enum
{
	vs_PRINT_SUMMARY = 1, 
	vs_PRINT_INFO = 2, 
	vs_PRINT_ERROR = 4,
	vs_IGNORE_UNKNOWN_CELLS = 8, 
	vs_NO_VOB = 16, 
	vs_DEMACRO = 32,
	vs_FORCE_DWORD = -1
} t_vs_flags;

typedef enum
{
	vso_APPEND = 1, 
	vso_SPLIT_VOBID = 2, 
	vso_SPLIT_CELLID = 4, 
	vso_DEMUX = 8,
	vso_KEEP_AC3_IDENT_BYTES = 16, 
	vso_KEEP_PCM_IDENT_BYTES = 32, 
	vso_ONLY_KEEP_GOPS = 64,
	vso_FORCE_DWORD = -1
} t_vs_outputflags;

typedef enum
{
	vse_OK = 0, 
	vse_DONE = 1, 
	vse_USER_FUNC_EXIT = 2, 
	vse_INIT_FAILED = 3, 
	vse_CANT_OPEN_INPUT = 4,
	vse_CANT_CREATE_OUTPUT = 5, 
	vse_CANT_WRITE_OUTPUT = 6, 
	vse_CANT_CRACK = 7, 
	vse_LOST_SYNC = 8,
	vse_FORCE_DWORD = -1
} t_vs_errorcode;

// -----------------------------------------------------------------------------

typedef struct
{
	dword		stream_id; 		// valid if > 0
	dword		substream_id;	// valid if stream_id == PRIVATE_STREAM_1
	dword		vob_id;				// valid if != -1
	dword		cell_id;			// valid if != -1
	dword		lba;					// logical block address (relative to 1st file (or offset from there))
	dword		idx;					// offset from the beginning of the sector
	dword		length;				// length of this chunk
	vstripbool	encrypted;		// are we encrypted?
} t_vs_streaminfo, *tp_vs_streaminfo;

typedef struct
{
	vstripbool	opened;
#ifdef vs_DECRYPT
	vstripbool	valid_key;
	byte	key[5];
#endif
} t_vs_vobcellinfo, *tp_vobcellinfo;

typedef vstripbool (*tp_vstrip_stream_func) (byte *data, tp_vs_streaminfo const si, void *user_data);

typedef struct
{
	tp_vstrip_stream_func	user_func;			// set before calling vs_vstrip
	void					*user_data;			// set before calling vs_vstrip
	dword					packets, bytes;
	double					pts;						// PTS of first occurence (ms)
	byte					remap_to;				// set before calling vs_vstrip
	byte					save;						// represents outputs0-7 (lowest bit ^= output 0)
	vstripbool				found;
	int						found_order;
} t_vs_streamflags, *tp_vs_streamflags;

typedef struct
{
	dword	start_lba;
	dword	end_lba;
	word	vob_id;
	byte	cell_id;
	byte	angle;
	byte	chapter;
	byte	time[4]; // hour, minute, second, frame (not BCD!)
} t_vs_vobcellid, *tp_vs_vobcellid;

// -----------------------------------------------------------------------------

typedef struct
{
  char							outfileprep[inb_MAX_OUTPUT][256];
  char							outfileext[inb_MAX_OUTPUT][256];
	tp_inb_buffer			buffer;
	t_vs_streaminfo		sti;
	tp_vs_streamflags	cur_stream;
  dword							framerate, aspectratio;
  dword							pack_packets, system_packets;
	tp_vobcellinfo		opened;
	byte							only_gop_mask;
  vstripbool 							did_video_info; // already printed video-stream info
  vstripbool							save_cell;
  vstripbool							init_ok;
#ifdef vs_DECRYPT
  vstripbool							valid_key, vob_key, cell_key;
#endif
} t_vs_internal_data, *tp_vs_internal_data;

typedef struct
{
	char				outfile[256]; 	// can be ""
	dword				split_output;		// autosplits output after x sectors (0 ^= no splitting)
	t_vs_outputflags	flags;
} t_vs_output_data;

typedef struct
{
	char				*infile;				// better exist (list of strings (\0\0 terminates) if it starts with ':')
	dword				num_outputs;		// 0 -> no output
	t_vs_output_data outputs[inb_MAX_OUTPUT];
	dword				framerate;			// -1 == don't change
	dword				aspectratio;		// -1 == don't change
	dword				max_sync_bytes;	// max. number of bytes to search for resync
	dword				start_lba;			// start at that lba
	dword				end_lba;				// end at his lba, 0 ^= whole file
	t_vs_flags	flags;					// user_func is only called if vs_DEMUX is set
#ifdef vs_DECRYPT
	dword				same_guess;			// number of guesses of the same key to consider the percentage
	dword				pad_guess;			// how much correct guesses we give for a padding-stream guess
	dword				pc_guess;				// percentage our candidate key has to be of all the keys that we found
	byte				key[5];					// all 0 or key for css
#endif
	t_vs_internal_data _in;			// data that vstrip needs between calls
} t_vs_data, *tp_vs_data;

// -----------------------------------------------------------------------------

int vstrip_getfilesize(const char * s_file, long *pl_size);

#ifdef __cplusplus
extern "C" {
#endif

VSTRIP_DLL_API const char *vs_get_stream_name(const dword id);
VSTRIP_DLL_API const double vs_get_framerate(const dword id);
VSTRIP_DLL_API char *vs_get_aspectratio(const dword id, double *pf_ratio);
VSTRIP_DLL_API const char *vs_get_time(const double time);
VSTRIP_DLL_API t_vs_errorcode vs_init(tp_vs_data vsd, t_vs_streamflags streams[256], t_vs_streamflags substreams[256]);

VSTRIP_DLL_API t_vs_errorcode vs_strip_one_block(tp_vs_data vsd, 
												 t_vs_streamflags streams[256], 
												 t_vs_streamflags substreams[256], 
												 dword num_idl, 
												 tp_vs_vobcellid idl, 
												 vstripbool end_or_start, 
												 VOBINFO *pr_vobinfo);

VSTRIP_DLL_API t_vs_errorcode vs_strip(tp_vs_data vsd, 
									   t_vs_streamflags streams[256], 
									   t_vs_streamflags substreams[256], 
									   dword num_idl, 
									   tp_vs_vobcellid idl, 
									   vstripbool end_or_start, 
									   VOBINFO *pr_vobinfo);


VSTRIP_DLL_API t_vs_errorcode vs_done(tp_vs_data vsd, t_vs_streamflags streams[256], t_vs_streamflags substreams[256]);

VSTRIP_DLL_API void dummyfprintf(FILE *fp, const char *in, ...);

int mpeg2dec_lib_get_info(char *s_filename, VOBINFO *pr_vobinfo);

#ifdef __cplusplus
}
#endif

#endif
