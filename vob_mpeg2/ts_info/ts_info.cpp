#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>

extern "C"
{
#include "ts_global.h"	
}

#include "ts_pat.h"
#include "../vstrip/vstrip.h"
#include "../vstrip/vobinfo.h"

#define DEMUX_AC3															\
while (Packet_Length > 0)													\
{																			\
	if (Packet_Length+Rdptr > BUFFER_SIZE+Rdbfr)							\
	{																		\
		fwrite(Rdptr, BUFFER_SIZE+Rdbfr-Rdptr, 1, ac3[This_Track].file);	\
		Packet_Length -= BUFFER_SIZE+Rdbfr-Rdptr;							\
		Read = _donread(Infile[CurrentFile], Rdbfr, BUFFER_SIZE);				\
		if (Read < BUFFER_SIZE) Next_File();								\
		Rdptr = Rdbfr;														\
	}																		\
	else																	\
	{																		\
		fwrite(Rdptr, Packet_Length, 1, ac3[This_Track].file);				\
		Rdptr += Packet_Length;												\
		Packet_Length = 0;													\
	}																		\
}


#define LOCATE												\
	while (Rdptr >= (Rdbfr + BUFFER_SIZE))						\
{															\
	Read = _donread(Infile[CurrentFile], Rdbfr, BUFFER_SIZE);	\
	if (Read < BUFFER_SIZE) Next_File();					\
	Rdptr -= BUFFER_SIZE;									\
}

// Skips ahead in transport stream by specified number of bytes.
#define SKIP_TRANSPORT_PACKET_BYTES(bytes_to_skip)					\
{																	\
	int temp = (bytes_to_skip);										\
	while (temp  > 0) 												\
{ 																\
	if (temp + Rdptr > BUFFER_SIZE + Rdbfr)						\
{ 															\
	temp  -= BUFFER_SIZE + Rdbfr - Rdptr;					\
	Read = _donread(Infile[CurrentFile], Rdbfr, BUFFER_SIZE);	\
	if (Read < BUFFER_SIZE) Next_File();					\
	Rdptr = Rdbfr;											\
} 															\
		else														\
{ 															\
	Rdptr += temp; 											\
	temp = 0;												\
} 															\
}																\
	Packet_Length -= (bytes_to_skip);								\
}


static int FTType[] = {
	48000, 44100, 44100, 44100, 44100
};

static int AC3Channels[] = {
	2, 1, 2, 3, 2, 4, 4, 6
};

static int AC3Rate[] = {
	32, 40, 48, 56, 64, 80, 96, 112, 128, 160,
		192, 224, 256, 320, 384, 448, 512, 576, 640,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static int ChromaFormat[] = {
	0, 6, 8, 12
};

static double frame_rate_Table[] = 
{
	0.0,
		((24.0*1000.0)/1001.0),
		24.0,
		25.0,
		((30.0*1000.0)/1001.0),
		30.0,
		50.0,
		((60.0*1000.0)/1001.0),
		60.0,

		-1,		// reserved
		-1,
		-1,
		-1,
		-1,
		-1,
		-1
};

static char *AspectRatioStr[] = {
	"", "1:1", "4:3", "16:9", "2.21:1"
};

static double AspectRatio[] = {
	1.0, 1.0, 4.0/3.0, 16.0/9.0, 2.21
};

static char *AspectRatioMPEG1[] = {
	"", "1:1", "0.6735", "16:9,625", "0.7615", "0.8055", "16:9,525", "0.8935", "4:3,625", "0.9815", "1.0255",
	"1.0695", "4:3,525", "1.575", "1.2015"
};

transport_packet tp_zeroed = { 0 };

FILE *mpafp = NULL;
FILE *mpvfp = NULL;

TS_AUDIO_INFO audio_info = {0};
int Channel[CHANNEL];
AC3Stream ac3[CHANNEL];
RAWStream mpa[CHANNEL], dts[CHANNEL];
PCMStream pcm[CHANNEL];

int mpeg_type;
int is_program_stream;

int closed_gop;
int stream_type;
int file;
int state, found;
// Should hold maximum size PES packet.
unsigned char buffer[256000];
int buffer_length, buffer_ndx;
int EOF_reached;

unsigned int VideoPTS, AudioPTS;
int ac3_demux_count = 0;

unsigned char *buffer_invalid;
int profile_and_level_indication;
int MPEG2_Transport_VideoPID;
int MPEG2_Transport_AudioPID;
int MPEG2_Transport_AudioType;

int load_intra_quantizer_matrix;
int load_non_intra_quantizer_matrix;
int load_chroma_intra_quantizer_matrix;
int load_chroma_non_intra_quantizer_matrix;
int frame_rate_code;

/* decoder operation control flags */
bool D2V_Flag;
int CurrentFile;
int NumLoadedFiles;
int FO_Flag;
bool Info_Flag;
bool Pause_Flag;
bool Scale_Flag;
bool Start_Flag;
bool Stop_Flag;
int SystemStream_Flag;

__int64 PackHeaderPosition;

int LeadingBFrames;
int ForceOpenGops;

int Method_Flag;
// Track_Flag is now bit-mapped: bit 0 means track 1 enabled,
// bit 1 means track 2 enabled, etc.
unsigned char Track_Flag;
int DRC_Flag;
bool DSDown_Flag;
bool Decision_Flag;
double PreScale_Ratio;

/* Global Value */
int VOB_ID, CELL_ID;

char szOutput[_MAX_PATH] = {0};
char szBuffer[_MAX_PATH] = {0};

int horizontal_size, vertical_size, bit_rate_value;

float frame_rate;
int Bitrate_Monitor;

int Infile[MAX_FILE_NUMBER];
char *Infilename[MAX_FILE_NUMBER];
__int64 Infilelength[MAX_FILE_NUMBER];
__int64	Infiletotal;

int intra_quantizer_matrix[64];
int intra_quantizer_matrix_log[64];
int non_intra_quantizer_matrix[64];
int non_intra_quantizer_matrix_log[64];
int chroma_intra_quantizer_matrix[64];
int chroma_intra_quantizer_matrix_log[64];
int chroma_non_intra_quantizer_matrix[64];
int chroma_non_intra_quantizer_matrix_log[64];
int full_pel_forward_vector;
int full_pel_backward_vector;
int forward_f_code;
int backward_f_code;

int q_scale_type;
int alternate_scan;
int quantizer_scale;

short *block[8];

/* ISO/IEC 13818-2 section 6.2.2.1:  sequence_header() */
int aspect_ratio_information;

/* ISO/IEC 13818-2 section 6.2.2.3:  sequence_extension() */
int progressive_sequence;
int chroma_format;

/* ISO/IEC 13818-2 section 6.2.3: picture_header() */
int temporal_reference;
int picture_coding_type;
int progressive_frame;
int StartTemporalReference;
int PTSAdjustDone;

// Default to ITU-709.
int matrix_coefficients;
int closed_gop_prev;
int progressive_sequence_prev;

/* ISO/IEC 13818-2 section 6.2.3.1: picture_coding_extension() header */
int f_code[2][2];
int picture_structure;
int frame_pred_frame_dct;
int concealment_motion_vectors;
int intra_dc_precision;
int top_field_first;
int repeat_first_field;
int intra_vlc_format;

unsigned char Rdbfr[BUFFER_SIZE] = {0};
unsigned char *Rdptr;
unsigned char *Rdmax;
unsigned int BitsLeft, CurrentBfr, NextBfr, Val, Read;
__int64 CurrentPackHeaderPosition;

// functions

void sequence_extension(void);
void sequence_display_extension(void);
void quant_matrix_extension(void);
void picture_display_extension(void);
void picture_coding_extension(void);
void copyright_extension(void);
int  extra_bit_information(void);
void extension_and_user_data(void);
void next_start_code();
unsigned int Get_Bits(unsigned int N);
void Fill_Next();
void reset_vars();
static void determine_stream_type(void);
static void video_parser(int *);
static void pack_parser(void);
static unsigned char get_byte(void);


int get_ts_info(char *s_inputfile, 
				int stream_type, // 0=vob, 1=ts
				int b_search_for_cropping,
				VOBINFO* pr_tsinfo)
{
	unsigned char buf[200] = {0};
	int i = 0;
	int count = 0;

	reset_vars();

	// INIT !
	ac3_demux_count = 0;
	Infilename[0] = s_inputfile;
	//Infilename[0] = "F:\\trailers(vob)\\goth.VOB";
	Infile[0] = _open(Infilename[0], _O_BINARY | _O_RDONLY );

	Scale_Flag = true;

	FO_Flag = FO_NONE;
	//Method_Flag = AUDIO_DEMUXALL;
	Method_Flag = 0;
	Track_Flag = 1 << TRACK_1;
	DSDown_Flag = false;
	D2V_Flag = true;

	MPEG2_Transport_VideoPID = 0x00;
	MPEG2_Transport_AudioPID = 0x00;
	ForceOpenGops = 0;

	memset(&audio_info, 0 , sizeof(TS_AUDIO_INFO));
	ac3_demux_count = 0;

	CurrentFile = 0;
	_lseeki64(Infile[0], 0, SEEK_SET);
	Initialize_Buffer();

	// First see if it is a transport stream.
	// Skip any leading null characters, because some
	// captured transport files were seen to start with a large
	// number of nulls.
	_lseeki64(Infile[0], 0, SEEK_SET);
	for (;;)
	{
		if (_read(Infile[0], buf, 1) == 0)
		{
			return 0;
		}
		if (buf[0] != 0)
		{
			// Unread the non-null byte and exit.
			_lseeki64(Infile[0], _lseeki64(Infile[0], 0, SEEK_CUR) - 1, SEEK_SET);
			break;
		}
	}

	// Search for four sync bytes 188 bytes apart.
	// Gives good protection against sync byte emulation.
	// Look in the first 10000 good data bytes of the file only,
	// then give up.
	for (i = 0, count = 0; i < MAX_TS_SEARCH; i++)
	{
		read(Infile[0], buf, 1);
		if (buf[0] == 0x47)
		{
			if (count++ >= 4)
			{
				SystemStream_Flag = TRANSPORT_STREAM;
				mpeg_type = IS_MPEG2;
				is_program_stream = 0;
				break;
			}
			_read(Infile[0], buf, 187);
		}
		else
			count = 0;
	}

	// Now try for PVA streams. Look for a packet start in the first 1024 bytes.
	if (SystemStream_Flag != TRANSPORT_STREAM)
	{
		CurrentFile = 0;
		_lseeki64(Infile[0], 0, SEEK_SET);
		Initialize_Buffer();

		for(i = 0; i < 1024; i++)
		{
			if (Rdbfr[i] == 0x41 && Rdbfr[i+1] == 0x56 && (Rdbfr[i+2] == 0x01 || Rdbfr[i+2] == 0x02))
			{
				SystemStream_Flag = PVA_STREAM;
				mpeg_type = IS_MPEG2;
				is_program_stream = 0;
				break;
			}
			Rdptr++;
		}
	}

	// Determine whether this is an MPEG2 file and whether it is a program stream.
	if (SystemStream_Flag != TRANSPORT_STREAM && SystemStream_Flag != PVA_STREAM)
	{
		mpeg_type = IS_NOT_MPEG;
		is_program_stream = 0;
		if (initial_parse(Infilename[0], &mpeg_type, &is_program_stream) == -1)
		{
			return 0;
		}
		if (is_program_stream)
		{
			SystemStream_Flag = PROGRAM_STREAM;
		}
	}
	
	bool Check_Flag = false;
	int code = 0;
	int foundMPEG2_Transport_VideoPID = 0x00;

	// settings to search for audio :
	Start_Flag = false;

	if (SystemStream_Flag == TRANSPORT_STREAM)
	{
		PATParser pat_parser;

		pat_parser.AnalyzePAT(s_inputfile, &(pr_tsinfo->i_audio_streams), pr_tsinfo->audio_list,
			&(pr_tsinfo->i_video_streams), pr_tsinfo->video_list);

		if (pr_tsinfo->audio_list[0].type == AUDIO_TYPE_AC3)
		{
			MPEG2_Transport_AudioPID = pr_tsinfo->audio_list[0].id;
			MPEG2_Transport_AudioType = 0x81;
		}

		if (pr_tsinfo->video_list[0].type == STREAM_TYPE_MPEG2_VIDEO)
		{
			foundMPEG2_Transport_VideoPID = pr_tsinfo->video_list[0].id;
		}
	}

	// SEARCH for HEADER INFO + AUDIO
	MPEG2_Transport_VideoPID = 0x00;
	Method_Flag = AUDIO_DEMUXALL;
	Check_Flag = false;
	CurrentFile = 0;
	_lseeki64(Infile[0], 0, SEEK_SET);
	Initialize_Buffer();

	while (!Check_Flag)
	{
		// Move to the next video start code.
		next_start_code();
		code = Get_Bits(32);
		switch (code)
		{
		case SEQUENCE_HEADER_CODE:
			sequence_header();
			Check_Flag = true;
			break;
		}
	}

	pr_tsinfo->f_aspect_ratio = AspectRatio[aspect_ratio_information];;
	pr_tsinfo->i_height = vertical_size;
	pr_tsinfo->i_width = horizontal_size;
	pr_tsinfo->f_framerate = frame_rate_Table[frame_rate_code];
	pr_tsinfo->s_aspect_ratio = AspectRatioStr[aspect_ratio_information];

	pr_tsinfo->audio_info.i_kbps = audio_info.i_kbps;
	pr_tsinfo->audio_info.i_khz = audio_info.i_khz;
	pr_tsinfo->audio_info.i_num_channels = audio_info.i_num_channels;
	pr_tsinfo->audio_info.uc_id = audio_info.uc_id;
	pr_tsinfo->audio_info.valid = audio_info.valid;
	
	_close(Infile[0]);

	return 0;
}

void Initialize_Buffer()
{
	Rdptr = Rdbfr + BUFFER_SIZE;
	Rdmax = Rdptr;
	buffer_invalid = (unsigned char *) 0xffffffff;

	if (SystemStream_Flag != ELEMENTARY_STREAM)
	{
		if (Rdptr >= Rdmax)
			Next_Packet();
		CurrentBfr = *Rdptr++ << 24;

		if (Rdptr >= Rdmax)
			Next_Packet();
		CurrentBfr += *Rdptr++ << 16;

		if (Rdptr >= Rdmax)
			Next_Packet();
		CurrentBfr += *Rdptr++ << 8;

		if (Rdptr >= Rdmax)
			Next_Packet();
		CurrentBfr += *Rdptr++;

		Fill_Next();
	}
	else
	{
		Fill_Buffer();

		CurrentBfr = (*Rdptr << 24) + (*(Rdptr+1) << 16) + (*(Rdptr+2) << 8) + *(Rdptr+3);
		Rdptr += 4;

		Fill_Next();
	}

	BitsLeft = 32;
}

unsigned int Show_Bits(unsigned int N)
{
	if (N <= BitsLeft)
	{
		return (CurrentBfr << (32 - BitsLeft)) >> (32 - N);
	}
	else
	{
		N -= BitsLeft;
		return (((CurrentBfr << (32 - BitsLeft)) >> (32 - BitsLeft)) << N) + (NextBfr >> (32 - N));
	}
}

unsigned int Get_Bits(unsigned int N)
{
	if (N < BitsLeft)
	{
		Val = (CurrentBfr << (32 - BitsLeft)) >> (32 - N);
		BitsLeft -= N;
		return Val;
	}
	else
		return Get_Bits_All(N);
}

void Flush_Buffer(unsigned int N)
{
	if (N < BitsLeft)
		BitsLeft -= N;
	else
		Flush_Buffer_All(N);	
}

int _donread(int fd, void *buffer, unsigned int count);

unsigned int Get_Byte()
{
	extern unsigned char *buffer_invalid;

	if (Rdptr >= buffer_invalid)
	{
		// Ran out of good data.
		//		ThreadKill();
		Stop_Flag = 1;
		return 0xff;
	}

	while (Rdptr >= Rdbfr+BUFFER_SIZE)
	{
		Read = _donread(Infile[CurrentFile], Rdbfr, BUFFER_SIZE);
		if (Read < BUFFER_SIZE)	Next_File();

		Rdptr -= BUFFER_SIZE;
		Rdmax -= BUFFER_SIZE;
	}

	return *Rdptr++;
}

void Fill_Next()
{
	extern unsigned char *buffer_invalid;

	if (Rdptr >= buffer_invalid)
	{
		// Ran out of good data.
		Stop_Flag = 1;
		NextBfr = 0xffffffff;
		return;
	}

	CurrentPackHeaderPosition = PackHeaderPosition;
	
	if (Rdptr <= Rdbfr+BUFFER_SIZE - 4)
	{
		NextBfr = (*Rdptr << 24) + (*(Rdptr+1) << 16) + (*(Rdptr+2) << 8) + *(Rdptr+3);
		Rdptr += 4;
	}
	else
	{
		if (Rdptr >= Rdbfr+BUFFER_SIZE)
			Fill_Buffer();
		NextBfr = *Rdptr++ << 24;

		if (Rdptr >= Rdbfr+BUFFER_SIZE)
			Fill_Buffer();
		NextBfr += *Rdptr++ << 16;

		if (Rdptr >= Rdbfr+BUFFER_SIZE)
			Fill_Buffer();
		NextBfr += *Rdptr++ << 8;

		if (Rdptr >= Rdbfr+BUFFER_SIZE)
			Fill_Buffer();
		NextBfr += *Rdptr++;
	}
}

void next_start_code()
{
	unsigned int show;

	Flush_Buffer(BitsLeft & 7);

	while ((show = Show_Bits(24)) != 1)
	{
		if (Stop_Flag == true)
			return;
		Flush_Buffer(8);
	}
}

unsigned int Get_Short()
{
	unsigned int i, j;

	i = Get_Byte();
	j = Get_Byte();
	return ((i << 8) | j);
}

int _donread(int fd, void *buffer, unsigned int count)
{
	int bytes;
	bytes = _read(fd, buffer, count);
	return bytes;
}

int PTSDifference(unsigned int apts, unsigned int vpts, int *result)
{
	int diff;

	if (apts > vpts)
	{
		diff = (apts - vpts) / 90;
		//		if (diff > 5000) return 1;
		*result = diff;
	}
	else
	{
		diff = (vpts - apts) / 90;
		//		if (diff > 5000) return 1;
		*result = -diff;
	}
	return 0;
}

FILE *OpenAudio(char *path, char *mode)
{
	//strcpy(AudioFilePath, path);
	return fopen(path, mode);
}

// ATSC transport stream parser.
// We ignore the 'continuity counter' because with some DTV
// broadcasters, this isnt a reliable indicator.
void Next_Transport_Packet()
{
	static int i, Packet_Length, Packet_Header_Length, size;
	static unsigned int code, flags, VOBCELL_Count, This_Track = 0, MPA_Track;
	__int64 PES_PTS, PES_DTS;
	unsigned int pts_stamp = 0, dts_stamp = 0;
	int PTSDiff;
	unsigned int bytes_left;
	transport_packet tp;

	double picture_period;

	//start = gettimeofday();
	for (;;)
	{
		PES_PTS = 0;
		bytes_left = 0;
		Packet_Length = 188; // total length of an MPEG-2 transport packet
		tp = tp_zeroed; // to avoid warnings

		// Search for a sync byte. Gives some protection against emulation.
		for(;;)
		{
			if (Get_Byte() != 0x47)
				continue;
			if (Rdptr + 187 >= Rdbfr + BUFFER_SIZE)
			{
				if (Rdptr[-189] == 0x47)
					break;
			}
			else
			{
				if (Rdptr[+187] == 0x47)
					break;
			}
		}

		// Record the location of the start of the packet. This will be used
		// for indexing when an I frame is detected.
		if (D2V_Flag)
		{
			PackHeaderPosition = _telli64(Infile[CurrentFile])
				- (__int64)BUFFER_SIZE + (__int64)Rdptr - (__int64)Rdbfr - 1;
		}
		--Packet_Length; // swallow the sync_byte

		code = Get_Short();
		Packet_Length = Packet_Length - 2; // swallow the two bytes we just got
		tp.pid = (unsigned short) (code & 0x1FFF);
		tp.transport_error_indicator = (unsigned char) ((code >> 15) & 0x01);
		tp.payload_unit_start_indicator = (unsigned char) ((code >> 14) & 0x01);
		tp.transport_priority = (unsigned char) ((code >> 13) & 0x01);

		if (tp.transport_error_indicator)
		{
			// Skip remaining bytes in current packet.
			SKIP_TRANSPORT_PACKET_BYTES(Packet_Length);
			// Try the next packet in the stream.
			continue;
		}

		code = Get_Byte();
		--Packet_Length; // decrement the 1 byte we just got;
		tp.transport_scrambling_control = (unsigned char) ((code >> 6) & 0x03);//		2	bslbf
		tp.adaptation_field_control = (unsigned char) ((code >> 4 ) & 0x03);//		2	bslbf
		tp.continuity_counter = (unsigned char) (code & 0x0F);//		4	uimsbf

		// we don't care about the continuity counter

		if (tp.adaptation_field_control == 0)
		{
			// no payload
			// skip remaining bytes in current packet
			SKIP_TRANSPORT_PACKET_BYTES( Packet_Length )
				continue;
		}

		// 3) check the Adaptation-header, only so we can determine
		//    the exact #bytes to skip
		if ( tp.adaptation_field_control == 2 || tp.adaptation_field_control == 3)
		{
			// adaptation field is present
			tp.adaptation_field_length = (unsigned char) Get_Byte(); // 8-bits
			--Packet_Length; // decrement the 1 byte we just got;

			if ( tp.adaptation_field_length != 0 ) // end of field already?
			{
				// if we made it this far, we no longer need to decrement
				// Packet_Length.  We took care of it up there!
				code = Get_Byte();
				--Packet_Length; // decrement the 1 byte we just got;
				tp.discontinuity_indicator = (unsigned char) ((code >> 7) & 0x01); //	1	bslbf
				tp.random_access_indicator = (unsigned char) ((code >> 6) & 0x01); //	1	bslbf
				tp.elementary_stream_priority_indicator = (unsigned char) ((code >> 5) & 0x01); //	1	bslbf
				tp.PCR_flag = (unsigned char) ((code >> 4) & 0x01); //	1	bslbf
				tp.OPCR_flag = (unsigned char) ((code >> 3) & 0x01); //	1	bslbf
				tp.splicing_point_flag = (unsigned char) ((code >> 2) & 0x01); //	1	bslbf
				tp.transport_private_data_flag = (unsigned char) ((code >> 1) & 0x01); //	1	bslbf
				tp.adaptation_field_extension_flag = (unsigned char) ((code >> 0) & 0x01); //	1	bslbf
				bytes_left = tp.adaptation_field_length - 1;
				// skip the remainder of the adaptation_field
				SKIP_TRANSPORT_PACKET_BYTES( bytes_left )
			} // if ( tp.adaptation_field_length != 0 )
		} // if ( tp.adaptation_field_control != 1 )

		// We've processed the MPEG-2 transport header. 
		// Any data left in the current transport packet?
		if (Packet_Length == 0) continue;

		if (tp.pid && tp.pid == MPEG2_Transport_VideoPID) 
		{
			LOCATE

				if (tp.payload_unit_start_indicator)
				{
					Get_Short();
					Get_Short();
					Get_Short(); // MPEG2-PES total Packet_Length
					Get_Byte(); // skip a byte
					flags = Get_Byte();
					Packet_Header_Length = Get_Byte();
					Packet_Length = Packet_Length - 9; // compensate the bytes we extracted

					// Get timestamp, and skip rest of PES-header.
					if ((flags & 0x80) && (Packet_Header_Length > 4))
					{
						PES_PTS = (Get_Byte() & 0x0e) << 29;
						PES_PTS |= (Get_Short() & 0xfffe) << 14;
						PES_PTS |= (Get_Short()>>1) & 0x7fff;
						pts_stamp = (unsigned int) (PES_PTS & 0xffffffff);
						Packet_Length -= 5;
						// DTS is not used. The code is here for analysis and debugging.
						if ((flags & 0xc0) == 0xc0)
						{
							PES_DTS = (Get_Byte() & 0x0e) << 29;
							PES_DTS |= (Get_Short() & 0xfffe) << 14;
							PES_DTS |= (Get_Short()>>1) & 0x7fff;
							dts_stamp = (unsigned int) (PES_DTS & 0xffffffff);
							Packet_Length -= 5;
							SKIP_TRANSPORT_PACKET_BYTES(Packet_Header_Length - 10)
						}
						else
							SKIP_TRANSPORT_PACKET_BYTES(Packet_Header_Length - 5)
					}
					else
						SKIP_TRANSPORT_PACKET_BYTES(Packet_Header_Length);
					if (!Start_Flag)
					{
						// Start_Flag becomes true after the first I frame.
						// So VideoPTS will be left at the value corresponding to
						// the first I frame.
						VideoPTS = pts_stamp;
					}
				} // if ( code != 0x000001E0 )

				Rdmax = Rdptr + Packet_Length;

				Bitrate_Monitor += (Rdmax - Rdptr);
				return;
		}  // if ( (tp.pid == MPEG2_Transport_VideoPID ) ) 

		else if ((Method_Flag == AUDIO_DEMUXALL || Method_Flag == AUDIO_DEMUX) &&
			(Start_Flag || MPEG2_Transport_VideoPID == 0) &&    // User sets video pid 0 for audio only.
			tp.pid && (tp.pid == MPEG2_Transport_AudioPID) &&
			(MPEG2_Transport_AudioType == 3 || MPEG2_Transport_AudioType == 4 || MPEG2_Transport_AudioType == 0xffffffff)) 
		{
			// Both MPEG and AAC audio come here. The sync word will be checked to
			// distinguish between them.
			if (mpafp)
			{
				if (tp.payload_unit_start_indicator)
				{
					Get_Short(); // start code and stream id
					Get_Short();
					Get_Short(); // packet length
					Get_Byte(); // flags
					code = Get_Byte(); // more flags
					Packet_Header_Length = Get_Byte();
					Packet_Length -= 9;
					if (code & 0x80)
					{
						code = Get_Byte();
						PES_PTS = (code & 0x0e) << 29;
						PES_PTS |= (Get_Short() & 0xfffe) << 14;
						PES_PTS |= (Get_Short()>>1) & 0x7fff;
						Packet_Length = Packet_Length - 5;
						SKIP_TRANSPORT_PACKET_BYTES(Packet_Header_Length-5)
							AudioPTS = (unsigned int) (PES_PTS & 0xffffffff);
					}
				}
				//DEMUX_MPA_AAC(mpafp);
			}
			else if (tp.payload_unit_start_indicator)
			{
				Get_Short(); // start code
				Get_Short(); // rest of start code and stream id
				Get_Short(); // packet length
				Get_Byte(); // flags
				code = Get_Byte(); // more flags
				Packet_Header_Length = Get_Byte();
				Packet_Length -= 9;
				if (code & 0x80)
				{
					code = Get_Byte();
					PES_PTS = (code & 0x0e) << 29;
					PES_PTS |= (Get_Short() & 0xfffe) << 14;
					PES_PTS |= (Get_Short()>>1) & 0x7fff;
					Packet_Length = Packet_Length - 5;
					SKIP_TRANSPORT_PACKET_BYTES(Packet_Header_Length-5)
						AudioPTS = (unsigned int) (PES_PTS & 0xffffffff);

					// Now we're at the start of the audio.
					// Find the audio header.
					code = Get_Byte();
					code = ((code & 0xff) << 8) | Get_Byte();
					Packet_Length -= 2;
					while ((code & 0xfff8) != 0xfff8 && Packet_Length > 0)
					{
						code = ((code & 0xff) << 8) | Get_Byte();
						Packet_Length--;
					}
					if ((code & 0xfff8) != 0xfff8)
					{
						SKIP_TRANSPORT_PACKET_BYTES(Packet_Length);
						continue;
					}
					// Found the audio header. Now check the layer field.
					// For MPEG, layer is 1, 2, or 3. For AAC, it is 0.
					// We demux the same for both; only the filename we create is different.
					if (((code & 6) >> 1) == 0x00)
					{
						// AAC audio.
						Channel[0] = AUDIO_TYPE_AAC;

						// Adjust the VideoPTS to account for frame reordering.
						if (!PTSAdjustDone && StartTemporalReference != -1 && StartTemporalReference < 18)
						{
							PTSAdjustDone = 1;
							picture_period = 1.0 / frame_rate;
							if (picture_structure != FRAME_PICTURE)
								picture_period /= 2;
							VideoPTS -= (int) (StartTemporalReference * picture_period * 90000);
						}

						/*
						if (MPEG2_Transport_VideoPID == 0 || PTSDifference(AudioPTS, VideoPTS, &PTSDiff))
						printf("%s AAC PID %03x.aac", szOutput, MPEG2_Transport_AudioPID);
						else
						printf("%s AAC PID %03x DELAY %dms.aac", szOutput, MPEG2_Transport_AudioPID, PTSDiff);*/
					}
					else
					{
						// MPEG audio.
						Channel[0] = AUDIO_TYPE_MPEG1;

						// Adjust the VideoPTS to account for frame reordering.
						if (!PTSAdjustDone && StartTemporalReference != -1 && StartTemporalReference < 18)
						{
							PTSAdjustDone = 1;
							picture_period = 1.0 / frame_rate;
							if (picture_structure != FRAME_PICTURE)
								picture_period /= 2;
							VideoPTS -= (int) (StartTemporalReference * picture_period * 90000);
						}

						/*
						if (MPEG2_Transport_VideoPID == 0 || PTSDifference(AudioPTS, VideoPTS, &PTSDiff))
						printf("%s PID %03x.mpa", szOutput, MPEG2_Transport_AudioPID);
						else
						printf("%s PID %03x DELAY %dms.mpa", szOutput, MPEG2_Transport_AudioPID, PTSDiff);
						*/
					}
					Packet_Length += 2;
					Rdptr -= 2;
					if (D2V_Flag)
					{
						mpafp = OpenAudio(szBuffer, "wb");
						if (mpafp == NULL)
						{
							// Cannot open the output file, Disable further audio processing.
							MPEG2_Transport_AudioType = 0xff;
							SKIP_TRANSPORT_PACKET_BYTES(Packet_Length);
							continue;
						}
						//DEMUX_MPA_AAC(mpafp);
					}
				}
			}
		}

		else if (tp.pid && tp.pid == MPEG2_Transport_AudioPID && (MPEG2_Transport_AudioType == 0x81)) 
		{
			// We are demuxing AC3 audio.
			// search for an MPEG-PES packet header
			if (tp.random_access_indicator || tp.payload_unit_start_indicator )
			{
				LOCATE

					code = Get_Short();
				code = (code & 0xffff)<<16 | Get_Short();
				Packet_Length = Packet_Length - 4; // remove these two bytes

				// Check for MPEG2-PES packet header. This may contains a PTS.
				if (code != PRIVATE_STREAM_1)
				{
					// No, move the buffer-pointer back.
					Rdptr -= 4; 
					Packet_Length = Packet_Length + 4;
				}
				else
				{
					// YES, pull out PTS 
					//Packet_Length = Get_Short();
					Get_Short(); // MPEG2-PES total Packet_Length
					Get_Byte(); // skip a byte

					code = Get_Byte();
					Packet_Header_Length = Get_Byte();
					Packet_Length = Packet_Length - 5; // compensate the bytes we extracted

					// get PTS, and skip rest of PES-header
					if (code >= 0x80 && Packet_Header_Length > 4 )
					{
						code = Get_Byte();
						PES_PTS = (code & 0x0e) << 29;
						PES_PTS |= (Get_Short() & 0xfffe) << 14;
						PES_PTS |= (Get_Short()>>1) & 0x7fff;
						Packet_Length = Packet_Length - 5;
						AudioPTS = (unsigned int) (PES_PTS & 0xffffffff);	
						SKIP_TRANSPORT_PACKET_BYTES( Packet_Header_Length-5 )
					}
					else
						SKIP_TRANSPORT_PACKET_BYTES( Packet_Header_Length )
				}
			}

			// Done processing the MPEG-2 PES header, now for the *real* audio-data

			LOCATE
				// if this is the *first* observation, we will seek to the
				// first valid AC3-frame, then decode its header.  
				// We tried checking for tp.payload_unit_start_indicator, but this
				// indicator isn't reliable on a lot of DTV-stations!
				// Instead, we'll manually search for an AC3-sync word.
				if (!ac3[0].rip &&
					(Start_Flag || MPEG2_Transport_VideoPID == 0) &&   // User sets video pid 0 for audio only.
					!Channel[0] && 
					(tp.random_access_indicator || tp.payload_unit_start_indicator) 
					&& Packet_Length > 5 )
				{

					code = Get_Byte();
					code = (code & 0xff)<<8 | Get_Byte();
					Packet_Length = Packet_Length - 2; // remove these two bytes
					i = 0;

					// search for an AC3-sync word.  We wouldn't have to do this if
					// DTV-stations made proper use of tp.payload_unit_start_indicator;
					while (code!=0xb77 && Packet_Length > 0 )
					{
						code = (code & 0xff)<<8 | Get_Byte();
						--Packet_Length;
						++i;
					}

					if ( code !=0xb77 )  // did we find the sync-header?
					{
						// no, we searched the *ENTIRE* transport-packet and came up empty!
						SKIP_TRANSPORT_PACKET_BYTES( Packet_Length )
							continue;  
					}

					// First time that we detected this particular channel? yes
					Channel[0] = AUDIO_TYPE_AC3;

					//Packet_Length = Packet_Length - 5; // remove five bytes
					Get_Short(); 
					ac3[0].rate = (Get_Byte()>>1) & 0x1f;
					Get_Byte();
					ac3[0].mode = (Get_Byte()>>5) & 0x07;
					//Packet_Length = Packet_Length + 5; // restore these five bytes
					Rdptr -= 5; // restore these five bytes

					// ok, now move "backward" by two more bytes, so we're back at the
					// start of the AC3-sync header

					Packet_Length += 2;
					Rdptr -= 2; 

					if (D2V_Flag || Decision_Flag)
					{
						// For transport streams, the audio is always track 1.
						if (Decision_Flag && (Track_Flag & 1))
						{
							//InitialAC3();

							//DECODE_AC3

							ac3[0].rip = 1;
						}
						else if (Method_Flag==AUDIO_DECODE && (Track_Flag & 1))
						{
							//InitialAC3();
							/*
							sprintf(szOutput, "PID %03x T%02d %sch %dKbps %s.wav", 
							MPEG2_Transport_AudioPID, 1, 
							AC3ModeDash[ac3[0].mode], 
							AC3Rate[ac3[0].rate], 
							FTType[SRC_Flag]);*/

							strcpy(pcm[0].filename, szBuffer);
							pcm[0].file = OpenAudio(szBuffer, "wb");
							if (pcm[0].file == NULL)
							{
								// Cannot open the output file, Disable further audio processing.
								MPEG2_Transport_AudioType = 0xff;
								SKIP_TRANSPORT_PACKET_BYTES(Packet_Length);
								continue;
							}

							//StartWAV(pcm[0].file, 0x01);	// 48K, 16bit, 2ch

							// Adjust the VideoPTS to account for frame reordering.
							if (!PTSAdjustDone && StartTemporalReference != -1 && StartTemporalReference < 18)
							{
								PTSAdjustDone = 1;
								picture_period = 1.0 / frame_rate;
								if (picture_structure != FRAME_PICTURE)
									picture_period /= 2;
								VideoPTS -= (int) (StartTemporalReference * picture_period * 90000);
							}

							if (PTSDifference(AudioPTS, VideoPTS, &PTSDiff))
								pcm[0].delay = 0;
							else
								pcm[0].delay = PTSDiff * 192;

							if (pcm[0].delay > 0)
							{
								for (i=0; i<pcm[0].delay; i++)
									fputc(0, pcm[0].file);

								pcm[0].size += pcm[0].delay;
								pcm[0].delay = 0;
							}

							//DECODE_AC3

							if (-pcm[0].delay > size)
								pcm[0].delay += size;
							else
							{
								/*
								if (SRC_Flag)
								Wavefs44(pcm[0].file, size+pcm[0].delay, AC3Dec_Buffer-pcm[0].delay);
								else
								fwrite(AC3Dec_Buffer-pcm[0].delay, size+pcm[0].delay, 1, pcm[0].file);*/

								pcm[0].size += size+pcm[0].delay;
								pcm[0].delay = 0;
							}

							ac3[0].rip = 1;
						}
						else if (Method_Flag == AUDIO_DEMUXALL || (Method_Flag==AUDIO_DEMUX && (Track_Flag & 1)))
						{
							// Adjust the VideoPTS to account for frame reordering.
							if (!PTSAdjustDone && StartTemporalReference != -1 && StartTemporalReference < 18)
							{
								PTSAdjustDone = 1;
								picture_period = 1.0 / frame_rate;
								if (picture_structure != FRAME_PICTURE)
									picture_period /= 2;
								VideoPTS -= (int) (StartTemporalReference * picture_period * 90000);
							}

							/*
							if (MPEG2_Transport_VideoPID == 0 || PTSDifference(AudioPTS, VideoPTS, &PTSDiff))
							sprintf(szBuffer, "%s PID %03x T%02d %sch %dKbps.ac3", szOutput, MPEG2_Transport_AudioPID, 1, 
							AC3ModeDash[ac3[0].mode], AC3Rate[ac3[0].rate]);
							else*/
							//sprintf(szBuffer, "%s PID %03x T%02d %sch %dKbps.ac3", MPEG2_Transport_AudioPID, 1, 
							//AC3ModeDash[ac3[0].mode], AC3Rate[ac3[0].rate]);



							audio_info.i_kbps = AC3Rate[ac3[0].rate];
							audio_info.i_khz = 48000;
							audio_info.i_num_channels = AC3Channels[ac3[0].mode];
							audio_info.uc_id = MPEG2_Transport_AudioPID;
							audio_info.valid = 1;

							/*

							sprintf(szBuffer, "%s_tmp_%d.audio", s_tmp_folder, 0); // ONLY 1 supported YET !!!

							ac3[0].file = OpenAudio(szBuffer, "wb");
							if (ac3[0].file == NULL)
							{
							// Cannot open the output file, Disable further audio processing.
							MPEG2_Transport_AudioType = 0xff;
							SKIP_TRANSPORT_PACKET_BYTES(Packet_Length);
							continue;
							}*/

							ac3[0].rip = 1;
						}
					}
				}
				else if (ac3[0].rip)
				{
				//if (Decision_Flag)
					//DECODE_AC3
					if (Method_Flag==AUDIO_DECODE)
					{
						//DECODE_AC3

						if (-pcm[0].delay > size)
							pcm[0].delay += size;
						else
						{
							/*
							if (SRC_Flag)
							Wavefs44(pcm[0].file, size+pcm[0].delay, AC3Dec_Buffer-pcm[0].delay);
							else
							fwrite(AC3Dec_Buffer-pcm[0].delay, size+pcm[0].delay, 1, pcm[0].file);*/

							pcm[0].size += size+pcm[0].delay;
							pcm[0].delay = 0;
						}
					}
					else
					{
						int y = 0;
						ac3_demux_count++;
						//DEMUX_AC3
					}
				}
		}

		if (ac3_demux_count > 1) {
			return;
		}

		// fallthrough case
		// skip remaining bytes in current packet
		SKIP_TRANSPORT_PACKET_BYTES( Packet_Length )
	}
}

// PVA transport stream parser.
void Next_PVA_Packet()
{
	unsigned int Packet_Length;
	pva_packet pva;
	unsigned int code, PTS, PES_PTS, Packet_Header_Length;
	double picture_period;

	//start = timeGetTime();
	for (;;)
	{
		// Don't loop forever. If we don't get data
		// in a reasonable time (1 secs) we exit.
		//time = timeGetTime();

		// Search for a good sync.
		for(;;)
		{
			// Sync word is 0x4156.
			if (Get_Byte() != 0x41) continue;
			if (Get_Byte() != 0x56)
			{
				// This byte might be a 0x41, so back up by one.
				Rdptr--;
				continue;
			}
			// To protect against emulation of the sync word,
			// also check that the stream says audio or video.
			pva.stream_id = Get_Byte();
			if (pva.stream_id != 0x01 && pva.stream_id != 0x02)
			{
				// This byte might be a 0x41, so back up by one.
				Rdptr--;
				continue;
			}
			break;
		}

		// Record the location of the start of the packet. This will be used
		// for indexing when an I frame is detected.
		if (D2V_Flag)
		{
			PackHeaderPosition = _telli64(Infile[CurrentFile])
				- (__int64)BUFFER_SIZE + (__int64)Rdptr - (__int64)Rdbfr - 3;
		}

		// Pick up the remaining packet header fields.
		pva.counter = Get_Byte();
		pva.reserved = Get_Byte();
		pva.flags = Get_Byte();
		pva.length = Get_Byte() << 8;
		pva.length |= Get_Byte();
		Packet_Length = pva.length;
		Rdmax = Rdptr + Packet_Length;

		// Any payload?
		if (Packet_Length == 0 || pva.reserved != 0x55)
			continue;  // No, try the next packet.

		// Check stream id for video.
		if (pva.stream_id == 1) 
		{
			// This is a video packet.
			// Extract the PTS if it exists.
			if (pva.flags & 0x10)
			{
				PTS = (int) ((Get_Byte() << 24) | (Get_Byte() << 16) | (Get_Byte() << 8) | Get_Byte());
				Packet_Length -= 4;
				if (pva.flags & 0x03)
				{
					// Suck up pre-bytes if any.
					int i;
					for (i = 0; i < (pva.flags & 0x03); i ++)
						Get_Byte();
					Packet_Length -= i;
				}
				if (!Start_Flag)
				{
					VideoPTS = PTS;
				}
			}

			// Deliver the video to the ES parsing layer. 
			Bitrate_Monitor += (Rdmax - Rdptr);
			return;
		}

		// Check stream id for audio.
		else if ((Method_Flag == AUDIO_DEMUXALL || Method_Flag == AUDIO_DEMUX) &&
			Start_Flag &&
			pva.stream_id == 2)
		{
			// This is an audio packet.
			if (mpafp)
			{
				// For audio, this flag bit means an embedded audio PES packet starts
				// immediately in this PVA packet.
				if (pva.flags & 0x10)
				{
					Get_Short(); // start code and stream id
					Get_Short();
					Get_Short(); // packet length
					Get_Byte(); // flags
					code = Get_Byte(); // more flags
					Packet_Header_Length = Get_Byte();
					Packet_Length -= 9;
					if (code & 0x80)
					{
						code = Get_Byte();
						PES_PTS = (code & 0x0e) << 29;
						PES_PTS |= (Get_Short() & 0xfffe) << 14;
						PES_PTS |= (Get_Short()>>1) & 0x7fff;
						Packet_Length = Packet_Length - 5;
						SKIP_TRANSPORT_PACKET_BYTES(Packet_Header_Length-5)
							AudioPTS = (unsigned int) (PES_PTS & 0xffffffff);
					}
				}
				//DEMUX_MPA_AAC(mpafp);
			}
			else if (pva.flags & 0x10)
			{
				Get_Short(); // start code
				Get_Short(); // rest of start code and stream id
				Get_Short(); // packet length
				Get_Byte(); // flags
				code = Get_Byte(); // more flags
				Packet_Header_Length = Get_Byte();
				Packet_Length -= 9;
				if (code & 0x80)
				{
					code = Get_Byte();
					PES_PTS = (code & 0x0e) << 29;
					PES_PTS |= (Get_Short() & 0xfffe) << 14;
					PES_PTS |= (Get_Short()>>1) & 0x7fff;
					Packet_Length = Packet_Length - 5;
					SKIP_TRANSPORT_PACKET_BYTES(Packet_Header_Length-5)
						AudioPTS = (unsigned int) (PES_PTS & 0xffffffff);
					// Now we're at the start of the audio.
					// Find the audio header.
					code = Get_Byte();
					code = ((code & 0xff) << 8) | Get_Byte();
					Packet_Length -= 2;
					while ((code & 0xfff8) != 0xfff8 && Packet_Length > 0)
					{
						code = ((code & 0xff) << 8) | Get_Byte();
						Packet_Length--;
					}
					if ((code & 0xfff8) != 0xfff8)
					{
						SKIP_TRANSPORT_PACKET_BYTES(Packet_Length);
						continue;
					}
					// Found the audio header. Now check the layer field.
					// For MPEG, layer is 1, 2, or 3. For AAC, it is 0.
					// We demux the same for both; only the file name we create is different.
					if (((code & 6) >> 1) == 0x00)
					{
						// AAC audio.
						Channel[0] = AUDIO_TYPE_AAC;

						// Adjust the VideoPTS to account for frame reordering.
						if (!PTSAdjustDone && StartTemporalReference != -1 && StartTemporalReference < 18)
						{
							PTSAdjustDone = 1;
							picture_period = 1.0 / frame_rate;
							if (picture_structure != FRAME_PICTURE)
								picture_period /= 2;
							VideoPTS -= (int) (StartTemporalReference * picture_period * 90000);
						}
						/*
						if (PTSDifference(AudioPTS, VideoPTS, &PTSDiff))
							printf("%s AAC.aac", szOutput);
						else
							printf("%s AAC DELAY %dms.aac", szOutput, PTSDiff);
							*/
					}
					else
					{
						// MPEG audio.
						Channel[0] = AUDIO_TYPE_MPEG1;

						// Adjust the VideoPTS to account for frame reordering.
						if (!PTSAdjustDone && StartTemporalReference != -1 && StartTemporalReference < 18)
						{
							PTSAdjustDone = 1;
							picture_period = 1.0 / frame_rate;
							if (picture_structure != FRAME_PICTURE)
								picture_period /= 2;
							VideoPTS -= (int) (StartTemporalReference * picture_period * 90000);
						}
						/*
						if (PTSDifference(AudioPTS, VideoPTS, &PTSDiff))
							printf("%s.mpa", szOutput);
						else
							printf("%s DELAY %dms.mpa", szOutput, PTSDiff);*/
					}
					// Unread the audio header bytes.
					Packet_Length += 2;
					Rdptr -= 2;
					if (D2V_Flag)
					{
						mpafp = OpenAudio(szBuffer, "wb");
						if (mpafp == NULL)
						{
							// Cannot open the output file.
							SKIP_TRANSPORT_PACKET_BYTES(Packet_Length);
							continue;
						}
						//DEMUX_MPA_AAC(mpafp);
					}
				}
			}	
		}

		// Not an video packet or an audio packet to be demultiplexed. Keep looking.
		SKIP_TRANSPORT_PACKET_BYTES(Packet_Length);
	}
}

// MPEG2 program stream parser.
void Next_Packet()
{
	static int i, Packet_Length, Packet_Header_Length, size;
	static unsigned int code, AUDIO_ID, VOBCELL_Count, This_Track, MPA_Track;
	static int stream_type;
	int PTSDiff;
	double picture_period;

	if (SystemStream_Flag == TRANSPORT_STREAM)
	{
		Next_Transport_Packet();
		return;
	}
	else if (SystemStream_Flag == PVA_STREAM)
	{
		Next_PVA_Packet();
		return;
	}

	for (;;)
	{
		code = Get_Short();
		code = (code << 16) | Get_Short();

		while ((code & 0xffffff00) != 0x00000100)
		{
			if (Stop_Flag)
				return;
			code = (code<<8) + Get_Byte();
		}

		switch (code)
		{
		case PACK_START_CODE:
			if (D2V_Flag)
			{
				PackHeaderPosition = _telli64(Infile[CurrentFile]);
				PackHeaderPosition = PackHeaderPosition - (__int64)BUFFER_SIZE + (__int64)Rdptr - 4 - (__int64)Rdbfr;
			}
			if ((Get_Byte() & 0xf0) == 0x20)
			{
				Rdptr += 7; // MPEG1 program stream
				stream_type = MPEG1_PROGRAM_STREAM;
			}
			else
			{
				Rdptr += 8; // MPEG2 program stream
				stream_type = MPEG2_PROGRAM_STREAM;
			}
			VOBCELL_Count = 0;
			break;

		case PRIVATE_STREAM_2:
			Packet_Length = Get_Short();

			if (++VOBCELL_Count==2)
			{
				Rdptr += 25;
				VOB_ID = Get_Short();
				Get_Byte();
				CELL_ID = Get_Byte();
				Rdptr += Packet_Length - 29;

				printf("%d", VOB_ID);
				////SetDlgItemText(hDlg, IDC_VOB_ID, szBuffer);

				printf("%d", CELL_ID);
				////SetDlgItemText(hDlg, IDC_CELL_ID, szBuffer);
			}
			else
				Rdptr += Packet_Length;
			break;

		case PRIVATE_STREAM_1:
			Packet_Length = Get_Short();

			Rdptr ++;	// +1
			code = Get_Byte();	// +1
			Packet_Header_Length = Get_Byte();	// +1

			if (code>=0x80)
			{
				__int64 PES_PTS;

				PES_PTS = (Get_Byte() & 0x0e) << 29;
				PES_PTS |= (Get_Short() & 0xfffe) << 14;
				PES_PTS |= (Get_Short()>>1) & 0x7fff;
				AudioPTS = (unsigned int) (PES_PTS & 0xffffffff);

				Rdptr += Packet_Header_Length-5;
			}
			else
				Rdptr += Packet_Header_Length;

			AUDIO_ID = Get_Byte();	// +1
			Packet_Length -= Packet_Header_Length+4;

			if (AUDIO_ID>=SUB_AC3 && AUDIO_ID<SUB_AC3+CHANNEL)
			{
				Rdptr += 3; Packet_Length -= 3; This_Track = AUDIO_ID-SUB_AC3;

				LOCATE

					if (!ac3[This_Track].rip && Start_Flag && !Channel[This_Track])
					{
						Channel[This_Track] = AUDIO_TYPE_AC3;

						code = Get_Byte();
						code = (code & 0xff)<<8 | Get_Byte();
						i = 0;

						while (code!=0xb77)
						{
							code = (code & 0xff)<<8 | Get_Byte();
							i++;
						}

						Get_Short();
						ac3[This_Track].rate = (Get_Byte()>>1) & 0x1f;
						Get_Byte();
						ac3[This_Track].mode = (Get_Byte()>>5) & 0x07;

						Rdptr -= 7; Packet_Length -= i;

						if (D2V_Flag || Decision_Flag)
						{
							if (Decision_Flag && (Track_Flag & (1 << This_Track)))
							{
								//InitialAC3();

								//DECODE_AC3

								ac3[This_Track].rip = true;
							}
							else if (Method_Flag==AUDIO_DECODE && (Track_Flag & (1 << This_Track)))
							{
								//InitialAC3();
								
								strcpy(pcm[This_Track].filename, szBuffer);
								pcm[This_Track].file = OpenAudio(szBuffer, "wb");

								//StartWAV(pcm[This_Track].file, 0x01);	// 48K, 16bit, 2ch

								// Adjust the VideoPTS to account for frame reordering.
								if (!PTSAdjustDone && StartTemporalReference != -1 && StartTemporalReference < 18)
								{
									PTSAdjustDone = 1;
									picture_period = 1.0 / frame_rate;
									if (picture_structure != FRAME_PICTURE)
										picture_period /= 2;
									VideoPTS -= (int) (StartTemporalReference * picture_period * 90000);
								}

								if (PTSDifference(AudioPTS, VideoPTS, &PTSDiff))
									pcm[This_Track].delay = 0;
								else
									pcm[This_Track].delay = PTSDiff * 192;

								if (pcm[This_Track].delay > 0)
								{
									//if (SRC_Flag)
									//	pcm[This_Track].delay = ((int)(0.91875*pcm[This_Track].delay)>>2)<<2;

									for (i=0; i<pcm[This_Track].delay; i++)
										fputc(0, pcm[This_Track].file);

									pcm[This_Track].size += pcm[This_Track].delay;
									pcm[This_Track].delay = 0;
								}

								//DECODE_AC3

								if (-pcm[This_Track].delay > size)
									pcm[This_Track].delay += size;
								else
								{
									pcm[This_Track].size += size+pcm[This_Track].delay;
									pcm[This_Track].delay = 0;
								}

								ac3[This_Track].rip = true;
							}
							else if (Method_Flag==AUDIO_DEMUXALL  ||  Method_Flag==AUDIO_DEMUX && (Track_Flag & (1 << This_Track)))
							{
								// Adjust the VideoPTS to account for frame reordering.
								if (!PTSAdjustDone && StartTemporalReference != -1 && StartTemporalReference < 18)
								{
									PTSAdjustDone = 1;
									picture_period = 1.0 / frame_rate;
									if (picture_structure != FRAME_PICTURE)
										picture_period /= 2;
									VideoPTS -= (int) (StartTemporalReference * picture_period * 90000);
								}

								//DEMUX_AC3

								ac3[This_Track].rip = true;
							}
						}
					}
					else if (ac3[This_Track].rip)
					{
						//if (Decision_Flag)
						//DECODE_AC3
						if (Method_Flag==AUDIO_DECODE)
						{
							//DECODE_AC3

							if (-pcm[This_Track].delay > size)
								pcm[This_Track].delay += size;
							else
							{
								pcm[This_Track].size += size+pcm[This_Track].delay;
								pcm[This_Track].delay = 0;
							}
						}
						else
						{
							//DEMUX_AC3
						}
					}
			}
			else if (AUDIO_ID>=SUB_PCM && AUDIO_ID<SUB_PCM+CHANNEL)
			{
				Rdptr += 6; Packet_Length -= 6; This_Track = AUDIO_ID-SUB_PCM;

				LOCATE

					if (!pcm[This_Track].rip && Start_Flag && !Channel[This_Track])
					{
						Channel[This_Track] = AUDIO_TYPE_LPCM;

						// Pick up the audio format byte.
						pcm[This_Track].format = Rdptr[-2];

						if (D2V_Flag)
						{
							if (Method_Flag==AUDIO_DEMUXALL || (Method_Flag == AUDIO_DEMUX && (Track_Flag & (1 << This_Track))))
							{
								// Adjust the VideoPTS to account for frame reordering.
								if (!PTSAdjustDone && StartTemporalReference != -1 && StartTemporalReference < 18)
								{
									PTSAdjustDone = 1;
									picture_period = 1.0 / frame_rate;
									if (picture_structure != FRAME_PICTURE)
										picture_period /= 2;
									VideoPTS -= (int) (StartTemporalReference * picture_period * 90000);
								}

								if (PTSDifference(AudioPTS, VideoPTS, &PTSDiff))
									pcm[This_Track].delay = 0;
								else
									pcm[This_Track].delay = PTSDiff * 192;


								printf("%s T%02d %s %s %dch.wav",
									szOutput,
									This_Track + 1,
									(pcm[This_Track].format & 0x30) == 0 ? "48K" : "96K",
									(pcm[This_Track].format & 0xc0) == 0 ? "16bit" : ((pcm[This_Track].format & 0xc0) == 0x40 ? "20bit" : "24bit"),
									(pcm[This_Track].format & 0x07) + 1);
								strcpy(pcm[This_Track].filename, szBuffer);

								pcm[This_Track].file = OpenAudio(szBuffer, "wb");
								//StartWAV(pcm[This_Track].file, pcm[This_Track].format);

								if (pcm[This_Track].delay > 0)
								{
									for (i=0; i<pcm[This_Track].delay; i++)
										fputc(0, pcm[This_Track].file);

									pcm[This_Track].size += pcm[This_Track].delay;
									pcm[This_Track].delay = 0;
								}

								if (-pcm[This_Track].delay > Packet_Length)
									pcm[This_Track].delay += Packet_Length;
								else
								{
									//DemuxLPCM(&size, &Packet_Length, PCM_Buffer, pcm[This_Track].format);
									//fwrite(PCM_Buffer-pcm[This_Track].delay, size+pcm[This_Track].delay, 1, pcm[This_Track].file);

									pcm[This_Track].size += size+pcm[This_Track].delay;
									pcm[This_Track].delay = 0;
								}

								pcm[This_Track].rip = true;
							}
						}
					}
					else if (pcm[This_Track].rip)
					{
						if (-pcm[This_Track].delay > Packet_Length)
							pcm[This_Track].delay += Packet_Length;
						else
						{
							pcm[This_Track].size += size+pcm[This_Track].delay;
							pcm[This_Track].delay = 0;
						}
					}
			}
			else if (AUDIO_ID>=SUB_DTS && AUDIO_ID<SUB_DTS+CHANNEL)
			{
				Rdptr += 3; Packet_Length -= 3; This_Track = AUDIO_ID-SUB_DTS;

				LOCATE

					if (!dts[This_Track].rip && Start_Flag && !Channel[This_Track])
					{
						Channel[This_Track] = AUDIO_TYPE_DTS;

						if (D2V_Flag)
						{
							if (Method_Flag==AUDIO_DEMUXALL  ||  Method_Flag==AUDIO_DEMUX && (Track_Flag & (1 << This_Track)))
							{
								// Adjust the VideoPTS to account for frame reordering.
								if (!PTSAdjustDone && StartTemporalReference != -1 && StartTemporalReference < 18)
								{
									PTSAdjustDone = 1;
									picture_period = 1.0 / frame_rate;
									if (picture_structure != FRAME_PICTURE)
										picture_period /= 2;
									VideoPTS -= (int) (StartTemporalReference * picture_period * 90000);
								}

								if (PTSDifference(AudioPTS, VideoPTS, &PTSDiff))
									printf("%s T%02d.dts", szOutput, This_Track+1);
								else
									printf("%s T%02d DELAY %dms.dts", szOutput, This_Track+1, PTSDiff);

								dts[This_Track].file = OpenAudio(szBuffer, "wb");

								//DEMUX_DTS

								dts[This_Track].rip = true;
							}
						}
					}
					else if (dts[This_Track].rip)
					{
						//DEMUX_DTS
					}
			}
			Rdptr += Packet_Length;
			break;

		case AUDIO_ELEMENTARY_STREAM_7:
			MPA_Track++;
		case AUDIO_ELEMENTARY_STREAM_6:
			MPA_Track++;
		case AUDIO_ELEMENTARY_STREAM_5:
			MPA_Track++;
		case AUDIO_ELEMENTARY_STREAM_4:
			MPA_Track++;
		case AUDIO_ELEMENTARY_STREAM_3:
			MPA_Track++;
		case AUDIO_ELEMENTARY_STREAM_2:
			MPA_Track++;
		case AUDIO_ELEMENTARY_STREAM_1:
			MPA_Track++;
		case AUDIO_ELEMENTARY_STREAM_0:
			if (stream_type == MPEG1_PROGRAM_STREAM)
			{
				// MPEG1 program stream.
				Packet_Length = Get_Short();

				Packet_Header_Length = 0;
				// Stuffing bytes.
				do 
				{
					code = Get_Byte();
					Packet_Header_Length += 1;
				} while (code == 0xff);
				if ((code & 0xc0) == 0x40)
				{
					// STD bytes.
					Get_Byte();
					code = Get_Byte();
					Packet_Header_Length += 2;
				}
				if ((code & 0xf0) == 0x20)
				{
					// PTS bytes.
					__int64 PES_PTS;

					PES_PTS = (code & 0x0e) << 29;
					PES_PTS |= (Get_Short() & 0xfffe) << 14;
					PES_PTS |= (Get_Short()>>1) & 0x7fff;
					AudioPTS = (unsigned int) (PES_PTS & 0xffffffff);
					Packet_Header_Length += 4;
				}
				else if ((code & 0xf0) == 0x30)
				{
					// PTS bytes.
					__int64 PES_PTS;

					PES_PTS = (code & 0x0e) << 29;
					PES_PTS |= (Get_Short() & 0xfffe) << 14;
					PES_PTS |= (Get_Short()>>1) & 0x7fff;
					AudioPTS = (unsigned int) (PES_PTS & 0xffffffff);
					Get_Short();
					Get_Short();
					Get_Byte();
					Packet_Header_Length += 9;
				}
				Packet_Length -= Packet_Header_Length;

				LOCATE

					if (!mpa[MPA_Track].rip && Start_Flag && !Channel[MPA_Track])
					{
						Channel[MPA_Track] = AUDIO_TYPE_MPEG1;

						code = Get_Byte();
						code = (code & 0xff)<<8 | Get_Byte();
						i = 0;

						while (code<0xfff0)
						{
							code = (code & 0xff)<<8 | Get_Byte();
							i++;
						}

						Rdptr -= 2; Packet_Length -= i;

						if (D2V_Flag)
						{
							if (Method_Flag==AUDIO_DEMUXALL  ||  Method_Flag==AUDIO_DEMUX && (Track_Flag & (1 << MPA_Track)))
							{
								// Adjust the VideoPTS to account for frame reordering.
								if (!PTSAdjustDone && StartTemporalReference != -1 && StartTemporalReference < 18)
								{
									PTSAdjustDone = 1;
									picture_period = 1.0 / frame_rate;
									if (picture_structure != FRAME_PICTURE)
										picture_period /= 2;
									VideoPTS -= (int) (StartTemporalReference * picture_period * 90000);
								}
								
								//DEMUX_MPA_AAC(mpa[MPA_Track].file);

								mpa[MPA_Track].rip = true;
							}
						}
					}
					else if (mpa[MPA_Track].rip)
						//DEMUX_MPA_AAC(mpa[MPA_Track].file);
						Rdptr += Packet_Length;
			}
			else
			{
				Packet_Length = Get_Short()-1;
				code = Get_Byte();

				if ((code & 0xc0)==0x80)
				{
					code = Get_Byte();	// +1
					Packet_Header_Length = Get_Byte();	// +1

					if (code>=0x80)
					{
						__int64 PES_PTS;

						PES_PTS = (Get_Byte() & 0x0e) << 29;
						PES_PTS |= (Get_Short() & 0xfffe) << 14;
						PES_PTS |= (Get_Short()>>1) & 0x7fff;
						AudioPTS = (unsigned int) (PES_PTS & 0xffffffff);

						Rdptr += Packet_Header_Length-5;
					}
					else
						Rdptr += Packet_Header_Length;

					Packet_Length -= Packet_Header_Length+2;

					LOCATE

						if (!mpa[MPA_Track].rip && Start_Flag && !Channel[MPA_Track])
						{
							Channel[MPA_Track] = AUDIO_TYPE_MPEG1;

							code = Get_Byte();
							code = (code & 0xff)<<8 | Get_Byte();
							i = 0;

							while (code<0xfff0)
							{
								code = (code & 0xff)<<8 | Get_Byte();
								i++;
							}

							Rdptr -= 2; Packet_Length -= i;

							if (D2V_Flag)
							{
								if (Method_Flag==AUDIO_DEMUXALL  ||  Method_Flag==AUDIO_DEMUX && (Track_Flag & (1 << MPA_Track)))
								{
									// Adjust the VideoPTS to account for frame reordering.
									if (!PTSAdjustDone && StartTemporalReference != -1 && StartTemporalReference < 18)
									{
										PTSAdjustDone = 1;
										picture_period = 1.0 / frame_rate;
										if (picture_structure != FRAME_PICTURE)
											picture_period /= 2;
										VideoPTS -= (int) (StartTemporalReference * picture_period * 90000);
									}
									//DEMUX_MPA_AAC(mpa[MPA_Track].file);

									mpa[MPA_Track].rip = true;
								}
							}
						}
						else if (mpa[MPA_Track].rip)
						{
							//DEMUX_MPA_AAC(mpa[MPA_Track].file);
						}
				}
				Rdptr += Packet_Length;
			}

			MPA_Track = 0;
			break;

		default:
			if ((code & 0xfffffff0) == VIDEO_ELEMENTARY_STREAM)
			{
				Packet_Length = Get_Short();
				Rdmax = Rdptr + Packet_Length;

				if (stream_type == MPEG1_PROGRAM_STREAM)
				{
					__int64 PES_PTS;
					unsigned int pts_stamp;

					// MPEG1 program stream.
					Packet_Header_Length = 0;
					// Stuffing bytes.
					do 
					{
						code = Get_Byte();
						Packet_Header_Length += 1;
					} while (code == 0xff);
					if ((code & 0xc0) == 0x40)
					{
						// STD bytes.
						Get_Byte();
						code = Get_Byte();
						Packet_Header_Length += 2;
					}
					if ((code & 0xf0) == 0x20)
					{
						// PTS bytes.
						PES_PTS = (code & 0x0e) << 29;
						PES_PTS |= (Get_Short() & 0xfffe) << 14;
						PES_PTS |= (Get_Short()>>1) & 0x7fff;
						pts_stamp = (unsigned int) (PES_PTS & 0xffffffff);
						Packet_Header_Length += 4;
					}
					else if ((code & 0xf0) == 0x30)
					{
						// PTS/DTS bytes.
						PES_PTS = (code & 0x0e) << 29;
						PES_PTS |= (Get_Short() & 0xfffe) << 14;
						PES_PTS |= (Get_Short()>>1) & 0x7fff;
						pts_stamp = (unsigned int) (PES_PTS & 0xffffffff);
						Get_Short();
						Get_Short();
						Get_Byte();
						Packet_Header_Length += 9;
					}
					else
					{
						// Just to kill a compiler warning.
						pts_stamp = 0;
					}
					if (!Start_Flag)
					{
						// Start_Flag becomes true after the first I frame.
						// So VideoPTS will be left at the value corresponding to
						// the first I frame.
						VideoPTS = pts_stamp;
					}
					Bitrate_Monitor += Rdmax - Rdptr;
					return;
				}
				else
				{
					// MPEG2 program stream.
					code = Get_Byte();
					if ((code & 0xc0) == 0x80)
					{
						__int64 PES_PTS, PES_DTS;
						unsigned int pts_stamp, dts_stamp;

						code = Get_Byte();
						Packet_Header_Length = Get_Byte();

						if (code >= 0x80)
						{
							PES_PTS = (Get_Byte() & 0x0e) << 29;
							PES_PTS |= (Get_Short() & 0xfffe) << 14;
							PES_PTS |= (Get_Short()>>1) & 0x7fff;
							pts_stamp = (unsigned int) (PES_PTS & 0xffffffff);
							// DTS is not used. The code is here for analysis and debugging.
							if ((code & 0xc0) == 0xc0)
							{
								PES_DTS = (Get_Byte() & 0x0e) << 29;
								PES_DTS |= (Get_Short() & 0xfffe) << 14;
								PES_DTS |= (Get_Short()>>1) & 0x7fff;
								dts_stamp = (unsigned int) (PES_DTS & 0xffffffff);
								//									dprintf("DGIndex: Video PTS = %d, Video DTS = %d, [diff=%d]\n",
								//											pts_stamp/90, dts_stamp/90, (pts_stamp-dts_stamp)/90);
								Rdptr += Packet_Header_Length - 10;
							}
							else
								Rdptr += Packet_Header_Length - 5;

							if (!Start_Flag)
							{
								// Start_Flag becomes true after the first I frame.
								// So VideoPTS will be left at the value corresponding to
								// the first I frame.
								VideoPTS = pts_stamp;
							}
						}
						else
						{
							Rdptr += Packet_Header_Length;
						}
						
						Bitrate_Monitor += Rdmax - Rdptr;
						return;
					}
					else
					{
						Rdptr += Packet_Length-1;
					}
				}
			}
			else if (code>=SYSTEM_START_CODE)
			{
				Packet_Length = Get_Short();
				Rdptr += Packet_Length;
			}
			break;
		}
	}
}

unsigned int Get_Bits_All(unsigned int N)
{
	N -= BitsLeft;
	Val = (CurrentBfr << (32 - BitsLeft)) >> (32 - BitsLeft);

	if (N)
		Val = (Val << N) + (NextBfr >> (32 - N));

	CurrentBfr = NextBfr;
	BitsLeft = 32 - N;
	Fill_Next();

	return Val;
}

void Flush_Buffer_All(unsigned int N)
{
	CurrentBfr = NextBfr;
	BitsLeft = BitsLeft + 32 - N;
	Fill_Next();
}

void Fill_Buffer()
{
	Read = _donread(Infile[CurrentFile], Rdbfr, BUFFER_SIZE);

	//	dprintf("DGIndex: Fill buffer\n");
	if (Read < BUFFER_SIZE)	Next_File();

	Rdptr = Rdbfr;

	if (SystemStream_Flag != ELEMENTARY_STREAM)
	{
		Rdmax -= BUFFER_SIZE;
	}
	else
		Bitrate_Monitor += Read;
}

void Next_File()
{
	int bytes;

	if (CurrentFile < NumLoadedFiles-1)
	{
		CurrentFile++;
		//process.run = 0;
		//for (i=0; i<CurrentFile; i++) process.run += Infilelength[i];
		_lseeki64(Infile[CurrentFile], 0, SEEK_SET);
		bytes = _donread(Infile[CurrentFile], Rdbfr + Read, BUFFER_SIZE - Read);
		//		dprintf("DGIndex: Next file at %d\n", Rdbfr + Read);
		if (Read + bytes == BUFFER_SIZE)
			// The whole buffer has valid data.
			buffer_invalid = (unsigned char *) 0xffffffff;
		else
			// Point to the first invalid buffer location.
			buffer_invalid = Rdbfr + Read + bytes;
	}
	else
	{
		buffer_invalid = Rdbfr + Read;
	}
}

/* decode sequence header */
void sequence_header()
{
	int constrained_parameters_flag;

	int vbv_buffer_size;
	int i;

	horizontal_size             = Get_Bits(12);
	vertical_size               = Get_Bits(12);
	aspect_ratio_information    = Get_Bits(4);
	frame_rate_code             = Get_Bits(4);
	if (mpeg_type == IS_MPEG1)
		frame_rate = (float) frame_rate_Table[frame_rate_code];
	bit_rate_value              = Get_Bits(18);
	Flush_Buffer(1);	// marker bit
	vbv_buffer_size             = Get_Bits(10);
	constrained_parameters_flag = Get_Bits(1);

	if (load_intra_quantizer_matrix = Get_Bits(1))
	{
		for (i=0; i<64; i++)
			intra_quantizer_matrix[scan[ZIG_ZAG][i]] = Get_Bits(8);
	}
	else
	{
		for (i=0; i<64; i++)
			intra_quantizer_matrix[i] = default_intra_quantizer_matrix[i];
	}

	if (load_non_intra_quantizer_matrix = Get_Bits(1))
	{
		for (i=0; i<64; i++)
			non_intra_quantizer_matrix[scan[ZIG_ZAG][i]] = Get_Bits(8);
	}
	else
	{
		for (i=0; i<64; i++)
			non_intra_quantizer_matrix[i] = 16;
	}

	/* copy luminance to chrominance matrices */
	for (i=0; i<64; i++)
	{
		chroma_intra_quantizer_matrix[i] = intra_quantizer_matrix[i];
		chroma_non_intra_quantizer_matrix[i] = non_intra_quantizer_matrix[i];
	}

	if (mpeg_type == IS_MPEG2)
		matrix_coefficients = 1;
	else
		matrix_coefficients = 5;


	// These are MPEG1 defaults. These will be overridden if we have MPEG2
	// when the sequence header extension is parsed.
	progressive_sequence = 1;
	chroma_format = CHROMA420;

	extension_and_user_data();
}

/* decode extension and user data */
/* ISO/IEC 13818-2 section 6.2.2.2 */
void extension_and_user_data()
{
	int code, ext_ID;

	if (Stop_Flag == true)
		return;
	next_start_code();

	while ((code = Show_Bits(32))==EXTENSION_START_CODE || code==USER_DATA_START_CODE)
	{
		if (code==EXTENSION_START_CODE)
		{
			Flush_Buffer(32);
			ext_ID = Get_Bits(4);

			switch (ext_ID)
			{
			case SEQUENCE_EXTENSION_ID:
				sequence_extension();
				break;
			case SEQUENCE_DISPLAY_EXTENSION_ID:
				sequence_display_extension();
				break;
			case QUANT_MATRIX_EXTENSION_ID:
				quant_matrix_extension();
				break;
			case PICTURE_DISPLAY_EXTENSION_ID:
				picture_display_extension();
				break;
			case PICTURE_CODING_EXTENSION_ID:
				picture_coding_extension();
				break;
			case COPYRIGHT_EXTENSION_ID:
				copyright_extension();
				break;
			}
			if (Stop_Flag == true)
				return;
			next_start_code();
		}
		else
		{
			Flush_Buffer(32);	// ISO/IEC 13818-2  sections 6.3.4.1 and 6.2.2.2.2
			if (Stop_Flag == true)
				return;
			next_start_code();	// skip user data
		}
	}
}

/* decode sequence extension */
/* ISO/IEC 13818-2 section 6.2.2.3 */
void sequence_extension()
{
	int low_delay;
	int frame_rate_extension_n;
	int frame_rate_extension_d;
	int horizontal_size_extension;
	int vertical_size_extension;
	int bit_rate_extension;
	int vbv_buffer_size_extension;
	static int pseq_prev = 0;

	profile_and_level_indication = Get_Bits(8);
	progressive_sequence         = Get_Bits(1);
	progressive_sequence_prev = pseq_prev;
	pseq_prev = progressive_sequence;
	chroma_format                = Get_Bits(2);
	horizontal_size_extension    = Get_Bits(2);
	vertical_size_extension      = Get_Bits(2);
	bit_rate_extension           = Get_Bits(12);
	Flush_Buffer(1);	// marker bit
	vbv_buffer_size_extension    = Get_Bits(8);
	low_delay                    = Get_Bits(1);

	frame_rate_extension_n       = Get_Bits(2);
	frame_rate_extension_d       = Get_Bits(5);
	frame_rate = (float)(frame_rate_Table[frame_rate_code] * (frame_rate_extension_n+1)/(frame_rate_extension_d+1));

	horizontal_size = (horizontal_size_extension<<12) | (horizontal_size&0x0fff);
	vertical_size = (vertical_size_extension<<12) | (vertical_size&0x0fff);
}

/* decode sequence display extension */
void sequence_display_extension()
{
	int video_format;  
	int color_description;
	int color_primaries;
	int transfer_characteristics;
	int display_horizontal_size;
	int display_vertical_size;

	video_format      = Get_Bits(3);
	color_description = Get_Bits(1);

	if (color_description)
	{
		color_primaries          = Get_Bits(8);
		transfer_characteristics = Get_Bits(8);
		matrix_coefficients      = Get_Bits(8);
	}

	display_horizontal_size = Get_Bits(14);
	Flush_Buffer(1);	// marker bit
	display_vertical_size   = Get_Bits(14);
}

/* decode quant matrix entension */
/* ISO/IEC 13818-2 section 6.2.3.2 */
void quant_matrix_extension()
{
	int i;

	if (load_intra_quantizer_matrix = Get_Bits(1))
		for (i=0; i<64; i++)
			chroma_intra_quantizer_matrix[scan[ZIG_ZAG][i]]
			= intra_quantizer_matrix[scan[ZIG_ZAG][i]] = Get_Bits(8);

			if (load_non_intra_quantizer_matrix = Get_Bits(1))
				for (i=0; i<64; i++)
					chroma_non_intra_quantizer_matrix[scan[ZIG_ZAG][i]]
					= non_intra_quantizer_matrix[scan[ZIG_ZAG][i]] = Get_Bits(8);

					if (load_chroma_intra_quantizer_matrix = Get_Bits(1))
						for (i=0; i<64; i++)
							chroma_intra_quantizer_matrix[scan[ZIG_ZAG][i]] = Get_Bits(8);

					if (load_chroma_non_intra_quantizer_matrix = Get_Bits(1))
						for (i=0; i<64; i++)
							chroma_non_intra_quantizer_matrix[scan[ZIG_ZAG][i]] = Get_Bits(8);


}

/* decode picture display extension */
/* ISO/IEC 13818-2 section 6.2.3.3. */
void picture_display_extension()
{
	int frame_center_horizontal_offset[3];
	int frame_center_vertical_offset[3];

	int i;
	int number_of_frame_center_offsets;

	/* based on ISO/IEC 13818-2 section 6.3.12 
	(November 1994) Picture display extensions */

	/* derive number_of_frame_center_offsets */
	if (progressive_sequence)
	{
		if (repeat_first_field)
		{
			if (top_field_first)
				number_of_frame_center_offsets = 3;
			else
				number_of_frame_center_offsets = 2;
		}
		else
			number_of_frame_center_offsets = 1;
	}
	else
	{
		if (picture_structure!=FRAME_PICTURE)
			number_of_frame_center_offsets = 1;
		else
		{
			if (repeat_first_field)
				number_of_frame_center_offsets = 3;
			else
				number_of_frame_center_offsets = 2;
		}
	}

	/* now parse */
	for (i=0; i<number_of_frame_center_offsets; i++)
	{
		frame_center_horizontal_offset[i] = Get_Bits(16);
		Flush_Buffer(1);	// marker bit

		frame_center_vertical_offset[i] = Get_Bits(16);
		Flush_Buffer(1);	// marker bit
	}
}

/* decode picture coding extension */
void picture_coding_extension()
{
	int chroma_420_type;
	int composite_display_flag;
	int v_axis;
	int field_sequence;
	int sub_carrier;
	int burst_amplitude;
	int sub_carrier_phase;

	f_code[0][0] = Get_Bits(4);
	f_code[0][1] = Get_Bits(4);
	f_code[1][0] = Get_Bits(4);
	f_code[1][1] = Get_Bits(4);

	intra_dc_precision			= Get_Bits(2);
	picture_structure			= Get_Bits(2);
	top_field_first				= Get_Bits(1);
	frame_pred_frame_dct		= Get_Bits(1);
	concealment_motion_vectors	= Get_Bits(1);
	q_scale_type				= Get_Bits(1);
	intra_vlc_format			= Get_Bits(1);
	alternate_scan				= Get_Bits(1);
	repeat_first_field			= Get_Bits(1);
	chroma_420_type				= Get_Bits(1);
	progressive_frame			= Get_Bits(1);
	composite_display_flag		= Get_Bits(1);

	if (composite_display_flag)
	{
		v_axis            = Get_Bits(1);
		field_sequence    = Get_Bits(3);
		sub_carrier       = Get_Bits(1);
		burst_amplitude   = Get_Bits(7);
		sub_carrier_phase = Get_Bits(8);
	}
}

/* decode extra bit information */
/* ISO/IEC 13818-2 section 6.2.3.4. */
int extra_bit_information()
{
	int Byte_Count = 0;

	while (Get_Bits(1))
	{
		Flush_Buffer(8);
		Byte_Count++;
	}

	return Byte_Count;
}

/* Copyright extension */
/* ISO/IEC 13818-2 section 6.2.3.6. */
/* (header added in November, 1994 to the IS document) */
void copyright_extension()
{
	int copyright_flag;
	int copyright_identifier;
	int original_or_copy;
	int copyright_number_1;
	int copyright_number_2;
	int copyright_number_3;

	int reserved_data;

	copyright_flag =       Get_Bits(1); 
	copyright_identifier = Get_Bits(8);
	original_or_copy =     Get_Bits(1);

	/* reserved */
	reserved_data = Get_Bits(7);

	Flush_Buffer(1); // marker bit
	copyright_number_1 =   Get_Bits(20);
	Flush_Buffer(1); // marker bit
	copyright_number_2 =   Get_Bits(22);
	Flush_Buffer(1); // marker bit
	copyright_number_3 =   Get_Bits(22);
}
int initial_parse(char *input_file, int *mpeg_type_p, int *is_program_stream_p)
{
	// Open the input file.
	if (input_file[0] == 0 || (file = _open(input_file, _O_RDONLY | _O_BINARY | _O_SEQUENTIAL)) == -1)
	{
		return -1;
	}

	// Determine the stream type: ES or program.
	determine_stream_type();

	// Re-open the input file.
	_close(file);
	file = _open(input_file, _O_RDONLY | _O_BINARY | _O_SEQUENTIAL);
	if (file == -1)
	{
		return -1;
	}

	// Determine MPEG type and find location of first video sequence header.
	EOF_reached = 0;
	video_parser(mpeg_type_p);
	_close(file);
	if (EOF_reached)
	{
		return -1;
	}
	*is_program_stream_p = stream_type;
	return 0;
}

static unsigned char get_byte(void)
{
	unsigned char val;

	if (stream_type != ELEMENTARY_STREAM)
	{
		if (buffer_ndx >= buffer_length)
		{
			// Need more data. Fill buffer with the next
			// packet's worth of elementary data.
			pack_parser();
		}
		if (EOF_reached == 1) 
		{
			return 0;
		}
		return buffer[buffer_ndx++];
	}
	else
	{
		// No program stream parsing needed. Just read from
		// the input file.
		if (_read(file, &val, 1) != 1)
		{
			EOF_reached = 1;
			return 0;
		}
		return val;
	}
}

static void video_parser(int *mpeg_type_p)
{
	unsigned char val;
	int sequence_header_active = 0;

	buffer_length = buffer_ndx = 0;

	// Inits.
	state = NEED_FIRST_0;
	found = 0;

	// Let's go! Start by assuming it's not MPEG at all.
	*mpeg_type_p = IS_NOT_MPEG;
	for (;;)
	{
		// Parse for start codes.
		val = get_byte();
		if (EOF_reached) return;
		switch (state)
		{
		case NEED_FIRST_0:
			if (val == 0)
				state = NEED_SECOND_0;
			break;
		case NEED_SECOND_0:
			if (val == 0)
				state = NEED_1;
			else
				state = NEED_FIRST_0;
			break;
		case NEED_1:
			if (val == 1)
			{
				found = 1;
				state = NEED_FIRST_0;
			}
			else if (val != 0)
				state = NEED_FIRST_0;
			break;
		}
		if (found == 1)
		{
			// Found a start code.
			found = 0;
			val = get_byte();
			if (EOF_reached) return;

			if (val == 0xB3)
			{
				// Found a sequence header, so it's at least MPEG1.
				*mpeg_type_p = IS_MPEG1;
				// Sequence header.
				if (sequence_header_active)
				{
					// Got another sequence header but didn't see a
					// sequence header extension, so it's MPEG1.
					return;
				}
				sequence_header_active = 1;
			}
			else if (val == 0xB5)
			{
				val = get_byte();
				if (EOF_reached) return;
				if ((val & 0xf0) == 0x10)
				{
					// Sequence extension.
					if (sequence_header_active)
					{
						// Must be MPEG2.
						*mpeg_type_p = IS_MPEG2;
						return;
					}
				}
				else if (sequence_header_active)
				{
					// Got some other extension. Must be MPEG1.
					*mpeg_type_p = IS_MPEG1;
					return;
				}
			}
			else
			{
				if (sequence_header_active)
				{
					// No sequence header extension. Must be MPEG1.
					*mpeg_type_p = IS_MPEG1;
					return;
				}
			}
		}
	}
	return;
}

static void pack_parser(void)
{
	unsigned char val, val2;
	int state, found = 0;
	int length;
	unsigned short pes_packet_length, system_header_length;
	unsigned char pes_packet_header_length, pack_stuffing_length;
	unsigned char code;


	// Look for start codes.
	state = NEED_FIRST_0;
	for (;;)
	{
		if (_read(file, &val, 1) != 1) { EOF_reached = 1; return; }
		switch (state)
		{
		case NEED_FIRST_0:
			if (val == 0)
				state = NEED_SECOND_0;
			break;
		case NEED_SECOND_0:
			if (val == 0)
				state = NEED_1;
			else
				state = NEED_FIRST_0;
			break;
		case NEED_1:
			if (val == 1)
			{
				found = 1;
				state = NEED_FIRST_0;
			}
			else if (val != 0)
				state = NEED_FIRST_0;
			break;
		}
		if (found == 1)
		{
			// Found a start code.
			found = 0;
			// Get the start code.
			if (_read(file, &val, 1) != 1) { EOF_reached = 1; return; }
//			dprintf("DGIndex: code = %x\n", val);
			if (val == 0xba)
			{
				// Pack header. Skip it.
				if (_read(file, &val, 1) != 1) { EOF_reached = 1; return; }
				if (stream_type == MPEG1_PROGRAM_STREAM)
				{
					// MPEG1 program stream.
					if (_read(file, buffer, 7) != 7) { EOF_reached = 1; return; }
				}
				else
				{
					// MPEG2 program stream.
					if (_read(file, buffer, 8) != 8) { EOF_reached = 1; return; }
					if (_read(file, &pack_stuffing_length, 1) != 1) { EOF_reached = 1; return; }
					pack_stuffing_length &= 0x03;
					if (_read(file, buffer, pack_stuffing_length) != pack_stuffing_length) { EOF_reached = 1; return; }
				}
			}
			if (val == 0xbb)
			{
				// System header. Skip it.
				if (_read(file, &val, 1) != 1)  { EOF_reached = 1; return; }
				system_header_length = (unsigned short) (val << 8);
				if (_read(file, &val2, 1) != 1)  { EOF_reached = 1; return; }
				system_header_length |= val2;
				if (_read(file, buffer, system_header_length) != system_header_length) { EOF_reached = 1; return; }
			}
			else if (val >= 0xE0 && val <= 0xEF)
			{
				// Video stream.
				// Packet length.
				if (_read(file, &val, 1) != 1)  { EOF_reached = 1; return; }
				pes_packet_length = (unsigned short) (val << 8);
				if (_read(file, &val2, 1) != 1)  { EOF_reached = 1; return; }
				pes_packet_length |= val2;
				if (stream_type == MPEG1_PROGRAM_STREAM)
				{
					// MPEG1 program stream.
					pes_packet_header_length = 0;
					// Stuffing bytes.
					do 
					{
						if (_read(file, &val, 1) != 1)  { EOF_reached = 1; return; }
						pes_packet_header_length += 1;
					} while (val == 0xff);
					if ((val & 0xc0) == 0x40)
					{
						// STD bytes.
						if (_read(file, &val, 1) != 1)  { EOF_reached = 1; return; }
						if (_read(file, &val, 1) != 1)  { EOF_reached = 1; return; }
						pes_packet_header_length += 2;
					}
					if ((val & 0xf0) == 0x20)
					{
						// PTS bytes.
						if (_read(file, buffer, 4) != 4)  { EOF_reached = 1; return; }
						pes_packet_header_length += 4;
					}
					else if ((val & 0xf0) == 0x30)
					{
						// PTS/DTS bytes.
						if (_read(file, buffer, 9) != 9)  { EOF_reached = 1; return; }
						pes_packet_header_length += 9;
					}
					// Send the video elementary data down the pipe to the video parser.
					buffer_length = pes_packet_length - pes_packet_header_length;
					if (buffer_length)
					{
						buffer_ndx = 0;
						if (_read(file, buffer, buffer_length) != buffer_length)  { EOF_reached = 1; return; }
						return;
					}
				}
				else
				{
					// MPEG2 program stream.
					// Flags.
					if (_read(file, &code, 1) != 1)  { EOF_reached = 1; return; }
					if ((code & 0xc0) == 0x80)
					{
						if (_read(file, &code, 1) != 1)  { EOF_reached = 1; return; }
						if (_read(file, &pes_packet_header_length, 1) != 1) { EOF_reached = 1; return; }
						// Skip the PES packet header.
						if (_read(file, buffer, pes_packet_header_length) != pes_packet_header_length)  { EOF_reached = 1; return; }
						// Send the video elementary data down the pipe to the video parser.
						buffer_length = pes_packet_length - pes_packet_header_length - 3;
						buffer_ndx = 0;
						if (_read(file, buffer, buffer_length) != buffer_length)  { EOF_reached = 1; return; }
						return;
					}
					else
					{
						// No video data here. Skip it.
						length = pes_packet_length - 1;
						if (_read(file, buffer, length) != length)  { EOF_reached = 1; return; }
					}
				}
			}
			else if (val > 0xbb)
			{
				// Not a stream that we are interested in. Skip it.
				if (_read(file, &val, 1) != 1)  { EOF_reached = 1; return; }
				pes_packet_length = (unsigned short) (val << 8);
				if (_read(file, &val2, 1) != 1)  { EOF_reached = 1; return; }
				pes_packet_length |= val2;
				length = pes_packet_length;
				if (_read(file, buffer, length) != length)  { EOF_reached = 1; return; }
			}
		}
	}
}

static void determine_stream_type(void)
{
	int i;
	unsigned char val;
	int state, found = 0;
	int num_sequence_header = 0;
	int num_picture_header = 0;

	// Start by assuming ES. Then look for a valid pack start. If one
	// is found declare a program stream.
	stream_type = ELEMENTARY_STREAM;

	// Look for start codes.
	state = NEED_FIRST_0;
	// Look for a pack start in the first part of the file.
	for (i = 0; i < 2500000; i++)
	{
		if (_read(file, &val, 1) != 1) { EOF_reached = 1; return; }
		switch (state)
		{
		case NEED_FIRST_0:
			if (val == 0)
				state = NEED_SECOND_0;
			break;
		case NEED_SECOND_0:
			if (val == 0)
				state = NEED_1;
			else
				state = NEED_FIRST_0;
			break;
		case NEED_1:
			if (val == 1)
			{
				found = 1;
				state = NEED_FIRST_0;
			}
			else if (val != 0)
				state = NEED_FIRST_0;
			break;
		}
		if (found == 1)
		{
			// Found a start code.
			found = 0;
			// Get the start code.
			if (_read(file, &val, 1) != 1) { EOF_reached = 1; return; }
			if (val == 0xba)
			{
				if (_read(file, &val, 1) != 1) { EOF_reached = 1; return; }
				if ((val & 0xf0) == 0x20)
				{
					// Check all the marker bits just to be sure.
					if (!(val & 1)) continue;
					if (_read(file, &val, 1) != 1) { EOF_reached = 1; return; }
					if (_read(file, &val, 1) != 1) { EOF_reached = 1; return; }
					if (!(val & 1)) continue;
					if (_read(file, &val, 1) != 1) { EOF_reached = 1; return; }
					if (_read(file, &val, 1) != 1) { EOF_reached = 1; return; }
					if (!(val & 1)) continue;
					if (_read(file, &val, 1) != 1) { EOF_reached = 1; return; }
					if (!(val & 0x80)) continue;
					if (_read(file, &val, 1) != 1) { EOF_reached = 1; return; }
					if (_read(file, &val, 1) != 1) { EOF_reached = 1; return; }
					if (!(val & 1)) continue;
					// MPEG1 program stream.
					stream_type = MPEG1_PROGRAM_STREAM;
					break;
				}
				else if ((val & 0xc0) == 0x40)
				{
					// Check all the marker bits just to be sure.
					if (!(val & 0x04)) continue;
					if (_read(file, &val, 1) != 1) { EOF_reached = 1; return; }
					if (_read(file, &val, 1) != 1) { EOF_reached = 1; return; }
					if (!(val & 0x04)) continue;
					if (_read(file, &val, 1) != 1) { EOF_reached = 1; return; }
					if (_read(file, &val, 1) != 1) { EOF_reached = 1; return; }
					if (!(val & 0x04)) continue;
					if (_read(file, &val, 1) != 1) { EOF_reached = 1; return; }
					if (!(val & 0x01)) continue;
					if (_read(file, &val, 1) != 1) { EOF_reached = 1; return; }
					if (_read(file, &val, 1) != 1) { EOF_reached = 1; return; }
					if (_read(file, &val, 1) != 1) { EOF_reached = 1; return; }
					if (!(val & 0x03)) continue;
					// MPEG2 program stream.
					stream_type = MPEG2_PROGRAM_STREAM;
					break;
				}
			}
			else if (val == 0xb3)
			{
				// Sequence header.
				num_sequence_header++;
			}
			else if (val == 0x00)
			{
				// Picture header.
				num_picture_header++;
			}
			if (num_sequence_header >= 2 && num_picture_header >= 2)
			{
				// We're seeing a lot of elementary stream data but we haven't seen
				// a pack header yet. Declare ES.
				break;
			}
		}
	}
}

void reset_vars()
{
	memset(&audio_info, 0, sizeof(TS_AUDIO_INFO));
	memset(&Channel, 0, CHANNEL * sizeof(int));
	memset(&ac3, 0, CHANNEL * sizeof(AC3Stream));
	memset(&mpa, 0, CHANNEL * sizeof(RAWStream));
	memset(&ac3, 0, CHANNEL * sizeof(RAWStream));
	memset(&pcm, 0, CHANNEL * sizeof(PCMStream));

	mpeg_type = 0;
	is_program_stream = 0;

	closed_gop = 0;
	stream_type = 0;
	file = 0;
	state = 0;
	found  = 0;

	memset(buffer, 0, sizeof(unsigned char) * 256000);
	buffer_length  = 0;
	buffer_ndx = 0;
	EOF_reached = 0;

	VideoPTS = 0;
	AudioPTS  = 0;

	ac3_demux_count = 0;

	buffer_invalid = NULL;
	profile_and_level_indication  = 0;
	MPEG2_Transport_VideoPID = 0;
	MPEG2_Transport_AudioPID  = 0;
	MPEG2_Transport_AudioType  = 0;

	load_intra_quantizer_matrix  = 0;
	load_non_intra_quantizer_matrix  = 0;
	load_chroma_intra_quantizer_matrix  = 0;
	load_chroma_non_intra_quantizer_matrix  = 0;
	frame_rate_code  = 0;

	D2V_Flag = true;
	CurrentFile = 0;
	NumLoadedFiles = 0;
	FO_Flag = 0;
	Info_Flag = 0;
	Pause_Flag = 0;
	Scale_Flag = 0;
	Start_Flag = 0;
	Stop_Flag = 0;
	SystemStream_Flag = 0;

	PackHeaderPosition = 0;

	LeadingBFrames = 0;
	ForceOpenGops = 0;



	Method_Flag = 0;
	Track_Flag = 0;
	DRC_Flag = 0;
	DSDown_Flag = 0;
	Decision_Flag = 0;
	PreScale_Ratio = 0;

	VOB_ID, CELL_ID = 0;

	memset(szOutput, 0, _MAX_PATH * sizeof(char));
	memset(szBuffer, 0, _MAX_PATH * sizeof(char));

	horizontal_size = 0;
	vertical_size = 0;
	bit_rate_value = 0;

	frame_rate = 0.0;
	Bitrate_Monitor = 0;

	/* TODO
	Infile[MAX_FILE_NUMBER];
	char *Infilename[MAX_FILE_NUMBER];
	__int64 Infilelength[MAX_FILE_NUMBER];*/

	Infiletotal = 0;

	intra_quantizer_matrix[64];
	intra_quantizer_matrix_log[64];
	non_intra_quantizer_matrix[64];
	non_intra_quantizer_matrix_log[64];
	chroma_intra_quantizer_matrix[64];
	chroma_intra_quantizer_matrix_log[64];
	chroma_non_intra_quantizer_matrix[64];
	chroma_non_intra_quantizer_matrix_log[64];

	full_pel_forward_vector = 0;
	full_pel_backward_vector = 0;
	forward_f_code = 0;
	backward_f_code = 0;

	q_scale_type = 0;
	alternate_scan = 0;
	quantizer_scale = 0;

	memset(block, 0, 8 * sizeof(short));

	/* ISO/IEC 13818-2 section 6.2.2.1:  sequence_header() */
	aspect_ratio_information = 0;

	progressive_sequence = 0;
	chroma_format = 0;

	temporal_reference = 0;
	picture_coding_type = 0;
	progressive_frame = 0;
	StartTemporalReference = 0;
	PTSAdjustDone = 0;

	matrix_coefficients = 0;
	closed_gop_prev = 0;
	progressive_sequence_prev = 0;

	memset(&f_code, 0 , sizeof(int) * 4);
	picture_structure = 0;
	frame_pred_frame_dct = 0;
	concealment_motion_vectors = 0;
	intra_dc_precision = 0;
	top_field_first = 0;
	repeat_first_field = 0;
	intra_vlc_format = 0;

	memset(Rdbfr, 0, sizeof(unsigned char) * BUFFER_SIZE);
	Rdptr = NULL;
	Rdmax = NULL;
	CurrentBfr = 0;
	NextBfr = 0;
	Val = 0;
	Read = 0;
	CurrentPackHeaderPosition  = 0;
}
