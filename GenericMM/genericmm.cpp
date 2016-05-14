//
// genericmm.cpp : Defines the entry point for the DLL application.
//
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "genericmm.h"
#include "AudioSourceAc3.h"
#include "vfw.h"

#define VERSION					"0.0.0.40"
#define KILO					(1024)
#define MEGA					(KILO*KILO)

int fill_bitrate_array(char *mp3, int *pa_values);

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

genericmm_API char *get_version(void)
{
	return VERSION;
}

int findstring(FILE *fp, char *str, int len)
{
	int location = 0;
	int i_found = 0;
	char c = 0;

	for (int i = 0 ; i < len; i++)
	{
		c = fgetc(fp);

		if (c == EOF)
		{
			return -1;
		}

		location++;
		if (c == str[i])
		{
			i_found++;
		}
		else
		{
			break;
		}
	}

	if (i_found == len)
	{
		return location;
	}
	else if (i_found > 0)
	{
		fseek(fp, -(i_found), SEEK_CUR);
	}

	return 0;
}

/*
** return 0 = OK
*/
genericmm_API int CorrectAC3HeaderInAvi(char* filename, int channels)
{
	int ret = 1;
	FILE *fp = NULL;
	char auds[] = {'a', 'u', 'd', 's'};
	char strf[] = {'s', 't', 'r', 'f'};
	char ac3[] = {0X00, 0X20};

	if (filename != NULL)
	{
		fp = fopen(filename, "r+b");

		if (fp)
		{
			bool found = false;
			int loc = 0XD0;
			char c = 0;

			fseek(fp, loc, SEEK_SET);

			int delta_loc = 0;

			delta_loc = 0;
			while (delta_loc <= 0)
			{
				delta_loc = findstring(fp, auds, sizeof(auds) / sizeof(char));
			}
			delta_loc = 0;
			while (delta_loc <= 0)
			{
				delta_loc = findstring(fp, strf, sizeof(strf) / sizeof(char));
			}
			delta_loc = 0;
			while (delta_loc <= 0)
			{
				delta_loc = findstring(fp, ac3, sizeof(ac3) / sizeof(char));
			}

			if (delta_loc > 0)
			{
				fseek(fp, 0, SEEK_CUR);
				fputc(channels, fp);
			}

			fclose(fp);
		}
	}

	return ret;
}

genericmm_API int get_avi_info(char *s_file, 
							   double *pf_fps, 
							   long *pl_frames, 
							   long *pl_seconds)
{
	int i_avi_status = 0;
	AVIFILEINFO fi;
	PAVIFILE pfile;

	AVIFileInit();

	i_avi_status = AVIFileOpen(&pfile, s_file, OF_READ, NULL);  
	if (i_avi_status == AVIERR_OK)
	{
		AVIFileInfo(pfile, &fi, sizeof(AVIFILEINFO));       
		AVIFileRelease(pfile );
	}

	if (i_avi_status == AVIERR_OK)
	{
		*pl_frames  = fi.dwLength;
		*pf_fps = (1.0 * fi.dwRate / fi.dwScale);
		*pl_seconds = (long) ((fi.dwLength) / *pf_fps);
	}
	else
	{
		*pf_fps = 1;
		*pl_seconds = 0;
		*pl_frames = 0;
	}

	AVIFileExit();

	return i_avi_status;
}

genericmm_API int get_nearest_keyframe(char *s_file, 
									   int i_seconds,
									   long *pl_frame,
									   double *f_fps)
{
	PAVISTREAM pavi_stream;
	int  i_avi_status = 0;
	long l_frames = 0;
	long l_seconds = 0;
	long l_sample = 0;
	long l_prev_sample = 0;
	long l_next_sample = 0;
	long l_prev_delta = 0;
	long l_next_delta = 0;

	*f_fps = 1.0;

	i_avi_status = get_avi_info(s_file, f_fps, &l_frames, &l_seconds);

	if (i_avi_status == AVIERR_OK)
	{
		AVIFileInit();
		AVIStreamOpenFromFile(&pavi_stream, s_file, streamtypeVIDEO, 0, OF_SHARE_DENY_WRITE,NULL);
	}

	if (i_seconds > 0)
	{
		l_sample = (long) (*f_fps * i_seconds);
	}
	else
	{
		//l_sample = i_frames;
	}

	l_prev_sample = AVIStreamPrevKeyFrame(pavi_stream, l_sample );
	l_next_sample = AVIStreamNextKeyFrame(pavi_stream, l_sample );

	l_next_delta = abs(l_next_sample - l_sample);
	l_prev_delta = abs(l_prev_sample - l_sample);

	if (l_next_delta > l_prev_delta)
	{
		*pl_frame = l_prev_sample;
	}
	else
	{
		*pl_frame = l_next_sample;
	}

	AVIStreamRelease(pavi_stream);

	AVIFileExit();

	return i_avi_status;
}

genericmm_API int filesize2frame(char *s_video_file, 
								 long l_size,
								 int i_audio_kbps,
								 long *pl_frame)
{
	return filesize2frame(s_video_file, NULL,  l_size,	 i_audio_kbps,	 pl_frame);
}

genericmm_API int search_for_split_points(char *s_video_file, 
										  int i_resolution,
										  int i_threshold,
										  int i_min_time_between_points,
										  int i_split_on_keyframe, SPLIT_DATA *pr_data)
{
	PAVISTREAM pavi_stream;
	long lPos = 0;
	long lFramePos = 0;
	long l_size_found = 0;
	unsigned long total_size = 0;
	int  i_avi_status = 0;
	long l_frames = 0;
	double f_fps = 1.0;
	long l_seconds = 0;

	if (i_resolution < 1)
	{
		i_resolution = 1;
	}

	if (i_threshold < 0)
	{
		i_threshold = 2;
	}

	if (i_min_time_between_points < 1)
	{
		i_min_time_between_points = 1;
	}

	if (i_split_on_keyframe < -1)
	{
		i_split_on_keyframe = -1;
	}

	if (i_split_on_keyframe > 1)
	{
		i_split_on_keyframe = 1;
	}

	i_avi_status = get_avi_info(s_video_file, &f_fps, &l_frames, &l_seconds);

	if (i_avi_status == AVIERR_OK)
	{
		AVIFileInit();
		i_avi_status = AVIStreamOpenFromFile(&pavi_stream, s_video_file, streamtypeVIDEO, 0, OF_SHARE_DENY_WRITE,NULL);
	}

	// search .........
	if (i_avi_status == AVIERR_OK)
	{
		int i_frame_no = 0;	

		int *ai_frame_size = NULL;

		ai_frame_size = (int*) calloc((l_frames / i_resolution) + 1, sizeof(int));

		for (lPos = 0 ; lPos < l_frames ; lPos += i_resolution)
		{
			total_size = 0;
			for (lFramePos = 0 ; (lFramePos < i_resolution) && (lPos + lFramePos < l_frames); lFramePos ++)
			{
				i_avi_status = AVIStreamSampleSize(pavi_stream, lPos + lFramePos, &l_size_found);
				total_size += l_size_found;
			}

			ai_frame_size[i_frame_no] = total_size / 1024;
			i_frame_no++;
		}

		int i_previous_frame = 0;
		int i_found_points = 0;

		for (int i = 0 ; i < i_frame_no; i++)
		{
			if (ai_frame_size[i] < i_threshold)
			{
				int i_max_frames_between_points = (int) (i_min_time_between_points * f_fps);

				long foundFrame = i * i_resolution;

				if (i_split_on_keyframe == -1)
				{
					foundFrame = AVIStreamPrevKeyFrame(pavi_stream, foundFrame);
				}
				else if (i_split_on_keyframe == 1)
				{
					foundFrame = AVIStreamNextKeyFrame(pavi_stream, foundFrame);
				}
				else
				{
					// Just leave it...
				}

				if (((foundFrame - i_previous_frame) > i_max_frames_between_points) &&
					((l_frames - foundFrame) > i_max_frames_between_points))
				{
					pr_data->ai_points[i_found_points] = (int) foundFrame;
					i_found_points++;
					i_previous_frame = foundFrame;
				}
			}
		}

		pr_data->i_num_points = i_found_points;

		free(ai_frame_size);

		AVIStreamRelease(pavi_stream);
	}

	AVIFileExit();

	return i_avi_status;
}

genericmm_API int filesize2frame(char *s_video_file, 
								 char* mp3_file, 
								 long l_size,
								 int i_audio_kbps,
								 long *pl_frame)
{
	PAVISTREAM pavi_stream;
	long lPos = 0;
	long l_size_found = 0;
	unsigned long total_size = 0;
	long lKF = 0;
	int  i_avi_status = 0;
	long l_frames = 0;
	double f_fps = 1.0;
	long l_seconds = 0;
	unsigned long i_audio_size = 0;
	bool b_get_audio_from_mp3 = false;
	int *bitrates = NULL;
	int secs = 0;

	if (l_size < 0)
	{
		i_avi_status = -1;
	}

	if (i_audio_kbps <0)
	{
		i_audio_kbps = 0;
	}
	else
	{
		i_audio_kbps /= 8;
	}

	// speed things up :
	long l_filesize = 0;
	i_avi_status = getfilesize(s_video_file, &l_filesize);

	if (l_size > l_filesize)
	{
		i_avi_status = -1;
	}

	if (i_avi_status == 0)
	{
		i_avi_status = get_avi_info(s_video_file, &f_fps, &l_frames, &l_seconds);
	}

	if (i_avi_status == AVIERR_OK)
	{
		AVIFileInit();
		i_avi_status = AVIStreamOpenFromFile(&pavi_stream,s_video_file, streamtypeVIDEO, 0, OF_SHARE_DENY_WRITE,NULL);
	}

	if (mp3_file)
	{
		b_get_audio_from_mp3 = true;
		secs = mp3_get_num_secs(mp3_file) / 1000;

		bitrates = (int*) calloc(secs + 1 , sizeof(int));

		fill_bitrate_array(mp3_file, bitrates);
	}

	// search .........
	if (i_avi_status == AVIERR_OK)
	{
		for (lPos = 0 ; lPos < l_frames ; lPos ++)
		{
			i_avi_status = AVIStreamSampleSize(pavi_stream, lPos, &l_size_found);

			if (b_get_audio_from_mp3)
			{
				int current_second = (int) (lPos / f_fps);

				if (current_second <= secs)
				{
					i_audio_size = (unsigned long) (1.0 * KILO * bitrates[current_second] / 8);
				}
				else
				{
					i_audio_size = 0;
				}
			}
			else
			{
				i_audio_size = (unsigned long) (1.0 * KILO * i_audio_kbps * lPos / f_fps);
			}

			total_size += l_size_found;

			if ((total_size + i_audio_size) > l_size)
			{
				long foundFrame = AVIStreamPrevKeyFrame(pavi_stream, lPos );
				if (foundFrame > *pl_frame)
				{
					*pl_frame = foundFrame;
					break;
				}  
			}
		}

		AVIStreamRelease(pavi_stream);
	}

	AVIFileExit();

	if (bitrates) free(bitrates);

	return i_avi_status;
}

genericmm_API int getfilesize(const char * s_file, long *pl_size)
{
	int i_status = -1;
	int fp = 0;

	*pl_size = 0;

	fp = _open(s_file,_O_RDONLY);

	if (fp > 0)
	{
		*pl_size = _lseek( fp, 0L, SEEK_END );

		_close(fp);
		if (*pl_size != -1L)
		{
			i_status = 0;
		}
	}

	return i_status;
}

genericmm_API int GetAc3Info(char *szFile, AC3_INFO *pr_info)
{
	WAVEFORMATEX *fmt, ac3WFmt;

	FILE *ac3File = NULL;

	pr_info->b_valid = false;
	pr_info->uc_id = 0;

	ac3File = fopen(szFile,"rb");

	if (ac3File) 
	{
		// check if it is an AC3 file...
		unsigned char first_bytes[16] = {0};

		fread(first_bytes, sizeof(first_bytes), 1, ac3File);

		pr_info->uc_id = first_bytes[0];

		/*

		if ((first_bytes[4] == 0x0B) && (first_bytes[5] == 0x77))
		{
		pr_info->b_valid = true;
		}
		else
		{
		fclose(ac3File);
		return false;
		}*/

		fseek(ac3File, 0, SEEK_SET);

		// extract WAVEFORMATEX from the AC3 file
		AudioSourceAC3 *ac3Src = new AudioSourceAC3(ac3File);

		if (!ac3Src->Parse(&ac3WFmt)) 
		{
			pr_info->i_kbps = 0;
			pr_info->i_num_channels = 0;
			pr_info->i_khz = 0;
			delete ac3Src;
			return FALSE;
		}

		if ((ac3Src->m_bsi->bsid == 0x8) || (ac3Src->m_bsi->bsid == 0x6) || (ac3Src->m_bsi->bsid == 0x4))
		{
			pr_info->b_valid = true;
		}

		if ((ac3Src->m_bsi->acmod == 2) && (ac3Src->m_bsi->dsurmod != 1))
		{
			//pr_info->b_valid = false;
		}

		// ac3Src->m_bsi->lfeon
		int lfe_on = ac3Src->m_bsi->lfeon;

		delete ac3Src->m_bsi;
		delete ac3Src;

		// allocate format structure
		if (!(fmt=(WAVEFORMATEX *) calloc(1, sizeof(WAVEFORMATEX)))) return FALSE;

		*fmt = ac3WFmt;

		pr_info->i_kbps = (fmt->nAvgBytesPerSec*8) / 1000;
		pr_info->i_num_channels = fmt->nChannels + lfe_on;
		pr_info->i_khz = fmt->nSamplesPerSec;

		free(fmt);

		fclose(ac3File);
	}

	return TRUE;
}

genericmm_API int get_endcredits_pos(char *s_divx_logfile, 
									 long *pl_frame,
									 long *pl_tot_frames,
									 int i_maxQ, 
									 int i_divx5_version)
{
	FILE *fp = NULL;
	int i_dummy = 0;
	char c_dummy = 0;
	int i_motion = 0;
	int i_complex = 0;
	char s_dummy[32] = {0};
	int i_frame = 0;
	int i_start_frame = 0;
	int i_found = 0;
	int i_not_found = 0;
	char ch = 0;
	int *pi_mot = NULL;

	int i_index = 0;
	int i = 0, j = 0;
	double av_mot = 0;

	int i_found_frame = 0;
	int i_step = 25;
	double f_step_motion = 0;
	int i_found_max = 3;

	if (i_maxQ <= 10)
	{
		i_found_max = 5;
	}
	else if (i_maxQ <= 10)
	{
		i_found_max = 4;
	}

	fp = fopen(s_divx_logfile, "rt");

	if (fp)
	{
		fseek(fp, -200, SEEK_END);
		while (ch != '\n')
		{
			ch = fgetc(fp);
		}
		// DIVX 5.02 and below...
		//Frame 163067: intra 0, quant 2, texture 10248, motion 1370, total 17320, complexity 25619


		// DIVX 5.05 :
		//##version 5
		//quality 5
		//bidirect 0
		//bidir_mult 2.000000
		//Frame 0: I, quant 7, texture 175792, motion 0, total 175800, complexity 1171946
		//Frame 1: P, quant 8, texture 26243, motion 4404, total 41168, complexity 216118
		while(!feof(fp))
		{
			if (i_divx5_version <= 502)
			{
				fscanf(fp, "%s%d: %s%d, %s%d, %s%d, %s%d, %s%d, %s%d",
					s_dummy, &i_frame, s_dummy, &i_dummy, s_dummy, &i_dummy, s_dummy, &i_dummy, 
					s_dummy, &i_motion, s_dummy, &i_dummy, s_dummy, &i_complex);
			}
			else
			{
				fscanf(fp, "%s %d: %c, %s %d, %s %d, %s %d, %s %d, %s %d",
					s_dummy, &i_frame, &c_dummy, s_dummy, &i_dummy, s_dummy, &i_dummy, 
					s_dummy, &i_motion, s_dummy, &i_dummy, s_dummy, &i_complex);
			}
		}

		*pl_tot_frames = i_frame;
		*pl_frame = *pl_tot_frames;
		fseek(fp, 0, SEEK_SET);

		pi_mot = (int*) calloc(((i_frame / i_step) + 10), sizeof(int));

		fscanf(fp ,"%s\n%s", s_dummy, s_dummy);
		fscanf(fp ,"%s\n%s", s_dummy, s_dummy);
		fscanf(fp ,"%s\n%s", s_dummy, s_dummy);
		fscanf(fp ,"%s\n%s", s_dummy, s_dummy);
		//fscanf(fp ,"%s\n%s", s_dummy, s_dummy);

		int p = 0;

		//Frame 67083: intra 0, quant 4, texture 52027, motion 3038, total 60685, complexity 650337
		while(!feof(fp))
		{
			if (i_divx5_version <= 502)
			{
				fscanf(fp, "%s%d: %s%d, %s%d, %s%d, %s%d, %s%d, %s%d",
					s_dummy, &i_frame, s_dummy, &i_dummy, s_dummy, &i_dummy, s_dummy, &i_dummy, 
					s_dummy, &i_motion, s_dummy, &i_dummy, s_dummy, &i_complex);
			}
			else
			{
				fscanf(fp, "%s %d: %c, %s %d, %s %d, %s %d, %s %d, %s %d",
					s_dummy, &i_frame, &c_dummy, s_dummy, &i_dummy, s_dummy, &i_dummy, 
					s_dummy, &i_motion, s_dummy, &i_dummy, s_dummy, &i_complex);
			}

			f_step_motion+=i_motion;
			//p++;
			//if (p == i_step)
			if (((i_frame % i_step) == 0) && (i_frame != 0))
			{
				f_step_motion /= i_step; 
				av_mot+=f_step_motion;
				pi_mot[i_index] = (int) (f_step_motion+0.5);
				//				fprintf(f, "%d\n", i_motion);
				p = 0;
				i_index++;
			}
		}

		//		fclose(f);

		av_mot/=i_index;

		for (i = (i_index - 60); i > 0 ; i--)
		{
			if (pi_mot[i] > (int) (av_mot * 0.75)) // haha
			{
				i_found++;
			}
			else
			{
				if (i_found > 0) i_found--;
			}

			if (i_found == i_found_max)
			{
				*pl_frame = (i + i_found + 1) * i_step + 750; // 30sec = safe
				if (*pl_frame > *pl_tot_frames) *pl_frame = *pl_tot_frames;
				break;
			}
		}

		free(pi_mot);

		fclose(fp);
	}

	return 0;
}								

genericmm_API void AddWaveHeaderToAc3(char *strAC3, char *strWAV, AC3_INFO *pr_info)
{
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
		short otherstuff1; 
		char dataheader[8]; /* the characters "data" */ 
		//unsigned long datalen; /* #bytes of actual data */ 
	} WAV_Header; 

	WAV_Header my_header;
	char *ch;
	long len = 0;
	long l_filesize = 0;

	FILE *fp_in;
	FILE *fp_out;
	AC3_INFO tmp_info;


	if (pr_info == NULL)
	{
		GetAc3Info(strAC3, &tmp_info);
		my_header.nchans = tmp_info.i_num_channels;
		my_header.bytesec = 1000 * (tmp_info.i_kbps / 8);
	}
	else
	{
		my_header.nchans = pr_info->i_num_channels;
		my_header.bytesec = 1000 * (pr_info->i_kbps / 8);
	}

	fp_in = fopen(strAC3, "rb");
	fp_out = fopen(strWAV, "wb");

	fseek(fp_in, 0, SEEK_END);

	l_filesize = ftell(fp_in);

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
	my_header.offset = 0x12;
	my_header.format = 0x2000; // ac3
	my_header.sampsec = 48000;
	my_header.bitsamp = 16;
	my_header.otherstuff = 0;
	my_header.otherstuff1 = 0x12;
	my_header.dataheader[0] = 'd'; 
	my_header.dataheader[1] = 'a';
	my_header.dataheader[2] = 't';
	my_header.dataheader[3] = 'a';

	l_filesize = l_filesize - 44;

	my_header.dataheader[7] = (char) ((l_filesize & 0xFF000000) >> (3*8));
	my_header.dataheader[6] = (char) ((l_filesize & 0x00FF0000) >> (2*8));
	my_header.dataheader[5] = (char) ((l_filesize & 0x0000FF00) >> (1*8));
	my_header.dataheader[4] = (char) ((l_filesize & 0x000000FF) >> (0*8));

	fseek(fp_in, 0 , SEEK_SET);

	fwrite(&my_header, sizeof(WAV_Header), 1, fp_out);

	ch = (char*) calloc(sizeof(char) , MEGA);

	len = fread(ch, sizeof(char), MEGA , fp_in);

	while (len > 0)
	{
		fwrite(ch, sizeof(char), len, fp_out);
		len = fread(ch, sizeof(char), MEGA , fp_in);
	}

	free(ch);

	fclose(fp_out);
	fclose(fp_in);

	return;
}

// NOT WORKING YET...
genericmm_API int get_endcredits_pos_xvid(char *s_xvid_logfile, 
										  long *pl_frame,
										  long *pl_tot_frames,
										  int i_found_max)
{
	return -1;
}