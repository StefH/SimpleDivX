//
// mp3 and wave routines/functions
//

#include "stdafx.h"
#include "stdio.h"
#include "string.h"
#include <math.h>
#include "genericmm.h"

#define NUM_BITRATES 16
#define NUM_SAMPLE_RATES 4

typedef struct
{
	int i_num_channels;
	int i_khz;
	int i_kbps;
} AUDIO_INFO;

typedef struct  
{ 
	char riff[4]; /* the characters "RIFF" */ 
	unsigned long file_length; /* file length - 8 */ 
	char wave[8]; /* the characters "WAVEfmt " */ 
	unsigned long offset; /* position of "data"-20 (usually 16) */ 
	unsigned short format; /* 1 = PCM */ 
	unsigned short nchans; /* #channels (eg. 2=stereo) */ 
	unsigned long sampsec; /* #samples/sec (eg. 44100 for CD rate) */ 
	unsigned long bytesec; /* #bytes/sec (see note 1) */ 
	unsigned short bitsamp; /* #bits/sample (see note 1) */ 
	short otherstuff; /* N = offset-16 (see note 5) */ 
	char dataheader[4]; /* the characters "data" */ 
	unsigned long datalen; /* #bytes of actual data */ 
} WAV_Header; 

typedef struct
{
	int i_kbps;
} TABLE_BITRATES;

typedef struct
{
	int i_khz;
} TABLE_SAMPLE_RATE;

TABLE_BITRATES table_bitrates[NUM_BITRATES] = {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0};

TABLE_SAMPLE_RATE table_sample_rate[NUM_SAMPLE_RATES] = {44100, 48000, 32000, 22050};

static int get_audio_info(const char *list, AUDIO_INFO *pr_audio_info);

int getfilesize(char * s_file, long *pl_size);
int search_for_sync_bit(long,FILE *fp);
void write_empty(FILE *fp, long len);
int find_bitrate(FILE *fp);
long fill_bitrate_array(FILE *fp, int *pa_values);

int mp3_get_num_secs(char *mp3)
{
	long l_pos = 1;
	int frames = 0;
	bool firstbit = true;
	double ms = 1.0;
	AUDIO_INFO r_audio_info = {0,0,0};
	char list[4] = {0};
	FILE *fp;

	fp = fopen(mp3, "rb");
	
	if (fp)
	{
		while ((l_pos = search_for_sync_bit(l_pos+1, fp)) > 0)
		{
			fread( list, sizeof( char ), 4, fp);
			get_audio_info(list,  &r_audio_info);
			
			if (firstbit)
			{
				firstbit = false;
				ms = 1152.0 / r_audio_info.i_khz;
			}
			frames ++;
		}
		
	}

	return (int) (frames * ms * 1000);
}

int fill_bitrate_array(char *mp3, int *pa_values)
{
	double ms = 1.0; 
	bool firstbit = true;
	int khz = 44100;
	double cnt = 0;
	int sec = 0;
	int size = 0;
	int bit_cnt = 0;
	long l_pos = 1;
	AUDIO_INFO r_audio_info = {0,0,0};
	char list[4] = {0};
	
	FILE *fp;
	
	fp = fopen(mp3, "rb");
	
	if (fp)
	{
		
		fseek(fp, 0 , SEEK_SET);
		
		while ((l_pos = search_for_sync_bit(l_pos+1, fp)) > 0)
		{
			fread( list, sizeof( char ), 4, fp);
			get_audio_info(list,  &r_audio_info);
			
			if (firstbit)
			{
				firstbit = false;
				ms = 1152.0 / r_audio_info.i_khz;
			}
			
			cnt += ms;
			size += r_audio_info.i_kbps;
			bit_cnt++;
			
			if (cnt > 1.0)
			{
				sec++;
				pa_values[sec] = (int) (size / bit_cnt);

				cnt = cnt - 1.0;
				bit_cnt = 0;
			}
		}
		fclose(fp);
	}
		
	return 0;
}

int find_bitrate(FILE *fp)
{
	AUDIO_INFO r_audio_info = {0,0,0};
	char list[4] = {0};
	int khz = 0;
	int i = 0;
	WAV_Header my_header = {0};
	int num_frames = 0;
	long l_pos = 0;
	int i_total_kbps = 0;


	fseek(fp, 0 , SEEK_SET);
	fread(&my_header, sizeof(WAV_Header), 1, fp);
	
	while ((l_pos = search_for_sync_bit(l_pos+1, fp)) > 0)
	{
		fread( list, sizeof( char ), 4, fp);
		get_audio_info(list,  &r_audio_info);
								
		//printf("%d - pos=%ld\n", num_frames, l_pos);
		
		if (r_audio_info.i_khz == my_header.sampsec)
		{
			//printf("%d - pos=%ld\n", num_frames, l_pos);
			i_total_kbps+=r_audio_info.i_kbps;	
			num_frames++;
		}
	}
	
	int bitrate = int ((i_total_kbps / (1.0 * num_frames)) + 0.5);
	return bitrate;

	int newbitrate = table_bitrates[NUM_BITRATES-1].i_kbps;
	
	for (i = 0 ; i < NUM_BITRATES - 1; i++)
	{
		int below = 0;
		int above = 0;
		
		if (bitrate < table_bitrates[i].i_kbps)
		{
			below = abs(table_bitrates[i-1].i_kbps - bitrate);
			above = abs(bitrate - table_bitrates[i].i_kbps);
			
			if (below < above)
			{
				newbitrate = table_bitrates[i-1].i_kbps;
			}
			else
			{
				newbitrate = table_bitrates[i].i_kbps;
			}
			
			
			i = NUM_BITRATES;
		}
		
		
	}
	
	return newbitrate;
	
}

genericmm_API int CorrectWaveHeaderFromMp3(char *strMp3)
{
	FILE *fp = NULL;

	if (strMp3 != NULL)
	{
		fp = fopen(strMp3, "r+b");
		
		if (fp)
		{
			WAV_Header my_header = {0};
			
			fread(&my_header, sizeof(WAV_Header), 1, fp);
			fseek(fp, 0 , SEEK_SET);
			
			my_header.bytesec = (1000 * (find_bitrate(fp))) / 8;

				
			fseek(fp, 0 , SEEK_SET);
			fwrite(&my_header, sizeof(my_header), 1, fp);
			
			fclose(fp);
		}
	}

	return 0;
}

genericmm_API int AddWaveHeaderToMp3(char *strMp3)
{
	FILE *fp_in;
	char list[4] = {0};
	AUDIO_INFO r_audio_info = {0,0,0};
	long l_filesize = 0;
	long l_pos = 0;
	int i = 0;
	char s_basename[128] = {0};
	int br = 0;

	if (strMp3 != NULL)
	{
		fp_in = fopen(strMp3, "r+b");
		
		if (fp_in)
		{

			br = find_bitrate(fp_in);

			l_pos =  search_for_sync_bit(0,fp_in);

			//write_empty(fp_in,l_pos);
			
			if (l_pos != -1)
			{
				fread( list, sizeof( char ), 4, fp_in);
				
				get_audio_info(list,  &r_audio_info);
				
				getfilesize(strMp3, &l_filesize  );
				
				WAV_Header my_header;
				
				my_header.riff[0] = 'R';
				my_header.riff[1] = 'I';
				my_header.riff[2] = 'F';
				my_header.riff[3] = 'F';
				my_header.file_length = l_filesize;
				my_header.wave[0] = 'W';
				my_header.wave[1] = 'A';
				my_header.wave[2] = 'V';
				my_header.wave[3] = 'E';
				my_header.wave[4] = 'f';
				my_header.wave[5] = 'm';
				my_header.wave[6] = 't';
				my_header.wave[7] = ' ';
				my_header.offset = 16;
				my_header.format = 85; // mp3
				my_header.nchans = r_audio_info.i_num_channels;
				my_header.sampsec = r_audio_info.i_khz;
				my_header.bytesec = (1000 * br) / 8;
				my_header.bitsamp = 16;
				my_header.otherstuff = 16;
				my_header.dataheader[0] = 'd'; 
				my_header.dataheader[1] = 'a';
				my_header.dataheader[2] = 't';
				my_header.dataheader[3] = 'a';
				my_header.datalen = l_filesize - sizeof(my_header);
				
				fseek(fp_in, 0 , SEEK_SET);
				fwrite(&my_header, sizeof(WAV_Header), 1, fp_in);
			}
			fclose(fp_in);
			
		}
	}
	
	return 0;
}

int search_for_sync_bit(long start, FILE *fp)
{
	char c_1[1] = {0};
	char c_2[1] = {0};
	bool b_found = 0;
	long l_pos = 0;
	char *p_data = 0;

	fseek(fp, start , SEEK_SET);
	
	while (b_found == 0)
	{
		if (feof(fp)) return -1;
		fread(c_1, sizeof(char), 1 , fp);
		if ((c_1[0] & 0xFF) == 0xFF)
		{
			fread(c_2, sizeof(char), 1 , fp);
			if ((c_2[0] & 0xFF) == 0xFB)
			{
				b_found = 1;
				fseek(fp, -2 , SEEK_CUR);
				
				l_pos = ftell(fp);
			}
		}
	}
	
	return l_pos;
}

void write_empty(FILE *fp, long len)
{
	int i = 0;
	char c_1[1] = {0};

	c_1[0] = 0;
	c_1[1] = 0;
	
	// write 00
	fseek(fp, 0 , SEEK_SET);
	
	for (i = 0; i < len ; i++)
	{
		fwrite(c_1, sizeof(char), 1, fp);
	}
}

int getfilesize(char * s_file, long *pl_size)
{
	int i_status = -1;
	FILE *fp = NULL;
	
	fp = fopen(s_file, "rb");
	
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		
		*pl_size = ftell(fp);
		
		fclose(fp);
		i_status = 0;
	}
	
	return i_status;
}

int get_audio_info(const char *list, AUDIO_INFO *pr_audio_info)
{
	int i_num = 0;
	char c_tmp = 0;
	
	// get kbps
	c_tmp = (list[2] & 0xF0) >> 4;
	for (i_num = 0 ; i_num < NUM_BITRATES ; i_num ++)
	{
		if (i_num == c_tmp)
		{
			pr_audio_info->i_kbps = table_bitrates[i_num].i_kbps;
			break;
		}
	}
	
	// get sample rate
	c_tmp = (list[2] & 0x0C) >> 2;
	for (i_num = 0 ; i_num < NUM_SAMPLE_RATES ; i_num ++)
	{
		if (i_num == c_tmp)
		{
			pr_audio_info->i_khz = table_sample_rate[i_num].i_khz;
			break;
		}
	}
	
	// get num channels
	c_tmp = (list[3] & 0xC0) >> 8;
	
	if (c_tmp == 0x03)
	{
		pr_audio_info->i_num_channels = 1;
	}
	else
	{
		pr_audio_info->i_num_channels = 2;
	}
	
	
	return 0;
}
