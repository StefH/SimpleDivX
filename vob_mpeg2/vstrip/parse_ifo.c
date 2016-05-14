/*************************************************************************
vStrip by [maven] (maven@maven.de)
parse_ifo.c: Parse the Cell-Position from the ProgramChain (PGC)
in an IFO-file. (tabsize 2)

  based on code originally written by Thomas Mirlacher (dent@cosy.sbg.ac.at)
  for the "free InFormatiOn project" (http://www.linuxvideo.org)
  
	Many Thanks!
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __UNIX__
#include <io.h>
#else
#include <unistd.h>
#endif
#include <fcntl.h>
#include "s_types.h"
#include "parse_ifo.h"
#include "file_io.h"
#include "vstrip.h"
#include "vobinfo.h"

#define get2bytes(b) be2me_16(*((word *)(b)))
#define get4bytes(b) be2me_32(*((dword *)(b)))

static struct
{
	char descr[3];
	char lang_long[20];
} iifo_lang_tbl[] = {
/* The ISO 639 language codes.
	*/
	{"  ", "Not Specified"},
	{"aa", "Afar"},
	{"ab", "Abkhazian"},
	{"af", "Afrikaans"},
	{"am", "Amharic"},
	{"ar", "Arabic"},
	{"as", "Assamese"},
	{"ay", "Aymara"},
	{"az", "Azerbaijani"},
	{"ba", "Bashkir"},
	{"be", "Byelorussian"},
	{"bg", "Bulgarian"},
	{"bh", "Bihari"},
	{"bi", "Bislama"},
	{"bn", "Bengali; Bangla"},
	{"bo", "Tibetan"},
	{"br", "Breton"},
	{"ca", "Catalan"},
	{"co", "Corsican"},
	{"cs", "Czech(Ceske)"},
	{"cy", "Welsh"},
	{"da", "Dansk"},
	{"de", "Deutsch"},
	{"dz", "Bhutani"},
	{"el", "Greek"},
	{"en", "English"},
	{"eo", "Esperanto"},
	{"es", "Espanol"},
	{"et", "Estonian"},
	{"eu", "Basque"},
	{"fa", "Persian"},
	{"fi", "Suomi"},
	{"fj", "Fiji"},
	{"fo", "Faroese"},
	{"fr", "Francais"},
	{"fy", "Frisian"},
	{"ga", "Irish"},
	{"gd", "Scots Gaelic"},
	{"gl", "Galician"},
	{"gn", "Guarani"},
	{"gu", "Gujarati"},
	{"ha", "Hausa"},
	{"he", "Hebrew"},				// formerly iw
	{"hi", "Hindi"},
	{"hr", "Hrvatski"},				// Croatian
	{"hu", "Magyar"},
	{"hy", "Armenian"},
	{"ia", "Interlingua"},
	{"id", "Indonesian"},				// formerly in
	{"ie", "Interlingue"},
	{"ik", "Inupiak"},
	{"in", "Indonesian"},				// replaced by id
	{"is", "Islenska"},
	{"it", "Italiano"},
	{"iu", "Inuktitut"},
	{"iw", "Hebrew"},				// replaced by he
	{"ja", "Japanese"},
	{"ji", "Yiddish"},				// replaced by yi
	{"jw", "Javanese"},
	{"ka", "Georgian"},
	{"kk", "Kazakh"},
	{"kl", "Greenlandic"},
	{"km", "Cambodian"},
	{"kn", "Kannada"},
	{"ko", "Korean"},
	{"ks", "Kashmiri"},
	{"ku", "Kurdish"},
	{"ky", "Kirghiz"},
	{"la", "Latin"},
	{"ln", "Lingala"},
	{"lo", "Laothian"},
	{"lt", "Lithuanian"},
	{"lv", "Latvian, Lettish"},
	{"mg", "Malagasy"},
	{"mi", "Maori"},
	{"mk", "Macedonian"},
	{"ml", "Malayalam"},
	{"mn", "Mongolian"},
	{"mo", "Moldavian"},
	{"mr", "Marathi"},
	{"ms", "Malay"},
	{"mt", "Maltese"},
	{"my", "Burmese"},
	{"na", "Nauru"},
	{"ne", "Nepali"},
	{"nl", "Nederlands"},
	{"no", "Norsk"},
	{"oc", "Occitan"},
	{"om", "(Afan) Oromo"},
	{"or", "Oriya"},
	{"pa", "Punjabi"},
	{"pl", "Polish"},
	{"ps", "Pashto, Pushto"},
	{"pt", "Portugues"},
	{"qu", "Quechua"},
	{"rm", "Rhaeto-Romance"},
	{"rn", "Kirundi"},
	{"ro", "Romanian"},
	{"ru", "Russian"},
	{"rw", "Kinyarwanda"},
	{"sa", "Sanskrit"},
	{"sd", "Sindhi"},
	{"sg", "Sangho"},
	{"sh", "Serbo-Croatian"},
	{"si", "Sinhalese"},
	{"sk", "Slovak"},
	{"sl", "Slovenian"},
	{"sm", "Samoan"},
	{"sn", "Shona"},
	{"so", "Somali"},
	{"sq", "Albanian"},
	{"sr", "Serbian"},
	{"ss", "Siswati"},
	{"st", "Sesotho"},
	{"su", "Sundanese"},
	{"sv", "Svenska"},
	{"sw", "Swahili"},
	{"ta", "Tamil"},
	{"te", "Telugu"},
	{"tg", "Tajik"},
	{"th", "Thai"},
	{"ti", "Tigrinya"},
	{"tk", "Turkmen"},
	{"tl", "Tagalog"},
	{"tn", "Setswana"},
	{"to", "Tonga"},
	{"tr", "Turkish"},
	{"ts", "Tsonga"},
	{"tt", "Tatar"},
	{"tw", "Twi"},
	{"ug", "Uighur"},
	{"uk", "Ukrainian"},
	{"ur", "Urdu"},
	{"uz", "Uzbek"},
	{"vi", "Vietnamese"},
	{"vo", "Volapuk"},
	{"wo", "Wolof"},
	{"xh", "Xhosa"},
	{"yi", "Yiddish"},				// formerly ji
	{"yo", "Yoruba"},
	{"za", "Zhuang"},
	{"zh", "Chinese"},
	{"zu", "Zulu"},
	{"\0", "???"}
};

static char *iifoDecodeLang(uint16_t descr)
{
	int i;
	
	for (i = 0; iifo_lang_tbl[i].lang_long[0]; i++)
		if (!memcmp(&descr, &iifo_lang_tbl[i].descr, 2))
			return iifo_lang_tbl[i].lang_long;
		return iifo_lang_tbl[i].lang_long;
}

static char *iifoDecodeAudioMode(uint16_t descr)
{
	static char audio_coding_mode[][9] = 
	{
		"AC3",
		"???",
		"MPEG1",
		"MPEG2ext",
		"LPCM",
		"???",
		"DTS"
	};
	
	return audio_coding_mode[descr & 0x07];
}

static char *iifoDecodeAudioStreamID(uint16_t descr, const int audio_idx)
{
	static dword stream_ofs[] = {0xbd80, 0x0000, 0xc000, 0xc800, 0xbda0, 0xbd88, 0xbd88};
	static char stream_id[10];
	
	if ((stream_ofs[descr & 7] & 0xff) != 0)
	{
		sprintf(stream_id, "0x%02X 0x%02X", stream_ofs[descr & 7] >> 8, audio_idx + (stream_ofs[descr & 7] & 0xff));
	}
	else
		sprintf(stream_id, "0x%02X", audio_idx + (stream_ofs[descr & 7] >> 8));
	return stream_id;
}

static int iifoDecodeAudioStreamID_as_int(uint16_t descr, const int audio_idx)
{
	static dword stream_ofs[] = {0xbd80, 0x0000, 0xc000, 0xc800, 0xbda0, 0xbd88, 0xbd88};
	static char stream_id[10];
	
	if ((stream_ofs[descr & 7] & 0xff) != 0)
	{
		return audio_idx + (stream_ofs[descr & 7] & 0xff);
	}
	
	return 0;
}

const pgc_program_map_t *iifoGetProgramMap(const pgc_t *pgc)
{
	const uint8_t *ptr = (uint8_t *)pgc;
	
	return (pgc_program_map_t *)(ptr + be2me_16(pgc->pgc_program_map_offset));
}

const ifo_pgci_caddr_t *iifoGetCellPlayInfo(const pgc_t *pgc)
{
	const uint8_t *ptr = (uint8_t *)pgc;
	
	return (ifo_pgci_caddr_t *)(ptr + be2me_16(pgc->cell_playback_tbl_offset));
}

const ifo_pgc_cpos_t *iifoGetCellPos(const pgc_t *pgc)
{
	const uint8_t *ptr = (uint8_t *)pgc;
	
	return (ifo_pgc_cpos_t *)(ptr + be2me_16(pgc->cell_position_tbl_offset));
}

uint8_t iifoGetNumCells(const pgc_t *pgc)
{
	return pgc->nr_of_cells;
}

uint8_t iifoGetNumPrograms(const pgc_t *pgc)
{
	return pgc->nr_of_programs;
}

const pgc_t *iifoGetPGCI(const ifo_t *ifo, const int title)
{
	pgci_sub_t *pgci_sub;
	ifo_hdr_t *hdr = NULL;
	uint8_t *ptr = NULL;
	
	hdr = (ifo_hdr_t *)ifo->title_pgci;
	
	if (hdr && title < be2me_16(hdr->num))
	{
		ptr = (uint8_t *)hdr;
		ptr += IFO_HDR_LEN;
		
		pgci_sub = (pgci_sub_t *)ptr + title;
		
		ptr = (uint8_t *)hdr + be2me_32(pgci_sub->start);
	}
	
	return (const pgc_t *)ptr;
}

#define OFF_TITLE_PGCI get4bytes (ifo->mat + 0xCC)

vstripbool iifoIsVTS(ifo_t *ifo)
{
	return strncmp (ifo->mat, "DVDVIDEO-VTS", 12) == 0;
}

int iifoReadLB(tp_fio_file fp, dword pos, dword count, byte *data)
{
	if ((pos = fio_direct_lseek(fp, pos, SEEK_SET)) == -1)
		return -1;
	return fio_direct_read(fp, data, count);
}

vstripbool iifoReadTBL(tp_fio_file fp, dword offset, byte **out_ptr)
{
	byte *data;
	ifo_hdr_t *hdr;
	dword len = 0;
	
	if (!offset)
		return FALSE;
	
	data =  (byte *)malloc(fio_SECTOR_SIZE);
	if (!data)
		return FALSE;
	
	if (iifoReadLB (fp, offset * fio_SECTOR_SIZE, fio_SECTOR_SIZE, data) <= 0)
		return FALSE;
	
	hdr = (ifo_hdr_t *)data;
	len = be2me_32(hdr->len) + 1;
	
	if (len > fio_SECTOR_SIZE)
	{
		if (!(data = (byte *)realloc((void *)data, len)))
			return FALSE;
		iifoReadLB (fp, offset * fio_SECTOR_SIZE, len, data);
	}
	*out_ptr = data;
	return TRUE;
}

// interface

ifo_t *ifoOpen(const char *name, const dword fio_flags)
{
	ifo_t *ret_ifo = NULL;
	tp_fio_file fd;
	
	fd = fio_open(name, fio_flags, 0);
	if (fd)
	{
		ifo_t *ifo = calloc(sizeof *ifo, 1);
		
		ifo->mat = calloc(fio_SECTOR_SIZE, 1);
		if (iifoReadLB(fd, 0, fio_SECTOR_SIZE, ifo->mat) < 0)
		{
			free(ifo->mat);
			free(ifo);
		}
		else 
		{ // looking ok so far
			if (iifoIsVTS(ifo) && iifoReadTBL(fd, OFF_TITLE_PGCI, &ifo->title_pgci))
				ret_ifo = ifo;
			else
			{
				if (ifo->title_pgci)
					free(ifo->title_pgci);
				free (ifo->mat);
				free (ifo);
			}
		}
		fio_close(fd);
	}
	return ret_ifo;
}

vstripbool ifoClose (ifo_t *ifo)
{
	if (ifo)
	{
		if (ifo->mat)
			free (ifo->mat);
		if (ifo->title_pgci)
			free (ifo->title_pgci);
		free (ifo);
	}
	return TRUE;
}

int ifoGetNumPGCI(const ifo_t *ifo)
{
	int num = 0;
	
	if (ifo && ifo->title_pgci)
		num = be2me_16(((ifo_hdr_t *)ifo->title_pgci)->num);
	return num;
}

// returns the number of cells and the length in time_out
int ifoGetPGCIInfo(const ifo_t *ifo, const dword title, unsigned long *l_seconds)
{
	int num_cells = 0;
	const pgc_t *pgc;
	
	if (ifo && (pgc = iifoGetPGCI(ifo, title)))
	{
		num_cells = pgc->nr_of_cells;
		*l_seconds = pgc->playback_time.hour * 3600 + pgc->playback_time.minute * 60 + pgc->playback_time.second;
	}

	return num_cells;
}

vstripbool ifoGetPGCICells(const ifo_t *ifo, const dword title, tp_vs_vobcellid cells)
{
	vstripbool success = FALSE;
	const pgc_t *pgc;
	const ifo_pgc_cpos_t *cpos;
	const ifo_pgci_caddr_t *cpi;
	
	if (ifo && cells && (pgc = iifoGetPGCI(ifo, title)) && (cpos = iifoGetCellPos(pgc)) && (cpi = iifoGetCellPlayInfo(pgc)))
	{
		dword i, j, k = 0, anum = 0;
		const pgc_program_map_t *ctm;
		
		success = TRUE;
		for (i = 0; i < pgc->nr_of_cells; i++)
		{
			cells[i].start_lba = be2me_32(cpi[i].vobu_start);
			cells[i].end_lba = be2me_32(cpi[i].vobu_last_end);
			cells[i].vob_id = be2me_16(cpos[i].vob_id);
			cells[i].cell_id = cpos[i].cell_id;
			
			cells[i].time[0] = cpi[i].playback_time.hour;
			cells[i].time[1] = cpi[i].playback_time.minute;
			cells[i].time[2] = cpi[i].playback_time.second;
			cells[i].time[3] = cpi[i].playback_time.frame_u & DVD_TIME_AND;
			
			for (j = 0; j < 4; j++)
				cells[i].time[j] = ((cells[i].time[j] & 0xf0) >> 4) * 10 + (cells[i].time[j] & 0x0f); // convert bcd to int
			cells[i].time[3] |= (cpi[i].playback_time.frame_u & (~DVD_TIME_AND)); // set the flags again
			
			cells[i].chapter = 0;
			
			switch (cpi[i].chain_info & 0xc0)
			{
			case 0x00: // 00
				cells[i].angle = 0x11; // 1 of 1
				k = i;
				break;
			case 0x40: // 01
				cells[i].angle = 0xf1; // 1 of ?
				k = i;
				anum = 1;
				break;
			case 0x80: // 10
				cells[i].angle = 0xf0 | (++anum & 0xf); // anum of ?
				break;
			case 0xc0: // 11
				cells[i].angle = 0xf0 | (++anum & 0xf); // anum of ?
				anum = (anum << 4) & 0xf0;
				for (j = k; j <= i; j++) // now set all the ones we've found
					if ((cells[j].angle & 0xf0) == 0xf0)
						cells[j].angle = (cells[j].angle & 0x0f) | anum;
					break;
			}
		}
		
		ctm = iifoGetProgramMap(pgc);
		if (ctm)
		{ // now fill in chapter info
			for (i = 1; i < pgc->nr_of_programs; i++)
			{
				k = min(pgc->nr_of_cells, ctm[i] - 1);
				for (j = ctm[i - 1] - 1; j < k; j++)
					cells[j].chapter = i - 1;
			}
			for (j = ctm[pgc->nr_of_programs - 1] - 1; j < pgc->nr_of_cells; j++)
				cells[j].chapter = pgc->nr_of_programs - 1; // the last one
		}
		
	}
	return success;
}

int ifoGetNumAudio(const ifo_t *ifo)
{
	int num = 0;
	
	if (ifo && ifo->mat)
	{
		audio_hdr_t *hdr = (audio_hdr_t *)(ifo->mat + IFO_OFFSET_AUDIO);
		
		num = be2me_16(hdr->num);
	}
	return num;
}

char *ifoGetAudioDesc(const ifo_t *ifo, const int audio_idx, int *pi_audio_id)
{
	static char out[128];
	
	if (ifo && ifo->mat)
	{
		audio_hdr_t *hdr = (audio_hdr_t *)(ifo->mat + IFO_OFFSET_AUDIO);
		
		if (audio_idx < be2me_16(hdr->num))
		{
			ifo_audio_t *audio = &hdr->audio[audio_idx];
						
			*pi_audio_id = iifoDecodeAudioStreamID_as_int(audio->coding_mode, audio_idx);
			
			sprintf(out, "%s [%d channels] %s", 
				iifoDecodeLang(audio->lang_code),
				audio->num_channels + 1,
				iifoDecodeAudioMode(audio->coding_mode));

			return out;
		}
	}
	return NULL;
}

int ifoGetNumSubPic(const ifo_t *ifo)
{
	int num = 0;
	
	if (ifo && ifo->mat)
	{
		spu_hdr_t *hdr = (spu_hdr_t *)(ifo->mat + IFO_OFFSET_SUBPIC);
		
		num = be2me_16(hdr->num);
	}
	return num;
}

uint16_t ifoGetSubPicCode(const ifo_t *ifo, const int subp_idx)
{
	static char out[256];
	
	if (ifo && ifo->mat)
	{
		spu_hdr_t *hdr = (spu_hdr_t *)(ifo->mat + IFO_OFFSET_SUBPIC);
		
		if (subp_idx < hdr->num)
		{
			ifo_spu_t *sub = &hdr->spu[subp_idx];
			//int i, j;
			//char *end;

			return sub->lang_code;
			
			//return iifoDecodeLang(*plang_code);
			
			//sprintf(out, "%s (0xBD 0x%02X) [", iifoDecodeLang(sub->lang_code), 0x20 + subp_idx);
			//sprintf(out, "%s (0xBD 0x%02X) [", iifoDecodeLang(sub->lang_code), 0x20 + subp_idx);
			
			/*
			j = ifoGetNumPGCI(ifo);
			for (i = 0; i < j; i++)
			{
			const pgc_t *pgc = iifoGetPGCI(ifo, i);
			
			  if (pgc && pgc->subp_status[subp_idx].available)
			  {
			  char buf[5];
			  
				sprintf(buf, "%i,", i);
				strcat(out, buf);
				}
				}
				end = strrchr(out, ',');
				if (end)
				*end = 0;
			strcat(out, "]");*/
			
			//return out;
		}
	}

	return 0;
}

char *ifoGetVideoDesc(const ifo_t *ifo)
{
	static char out[256];
	
	if (ifo && ifo->mat)
	{
		ifo_video_t *vid = (ifo_video_t *)(ifo->mat + IFO_OFFSET_VIDEO);
		const static char ar[4][5] = {"4:3", "???", "???", "16:9"};
		const static char vs[4][5] = {"NTSC", "PAL", "???", "???"};
		const static char cm[2][6] = {"MPEG1", "MPEG2"};
		const static char rs[2][4][8] = {{"720x480", "704x480", "352x480", "352x240"}, {"720x576", "704x576", "352x576", "352x288"}};
		// CD2
		//sprintf(out, "%s %s %s %s", cm[vid->coding_mode], rs[vid->video_standard][vid->resolution], vs[vid->video_standard], ar[vid->apect_ratio]);
		sprintf(out, "%s", vs[vid->video_standard]);
		return out;
	}
	return NULL;
}

vstripbool ifoParse(const char *ifo_name, 
					const t_ifo_flags flags, 
					tp_vs_vobcellid *cell_ids,
					MOVIE_INFO *pr_movie_info)
{
	if (ifo_name && *ifo_name)
	{
		ifo_t *ifo = NULL;
		t_fio_flags ffl = 0;
		int i, j;
		
		if ((flags & ifo_USE_ASPI) != 0)
			ffl |= fio_USE_ASPI;
		if ((flags & ifo_PREFER_ASPI) != 0)
			ffl |= fio_PREFER_ASPI;
		ifo = ifoOpen(ifo_name, ffl);
		
		if (ifo)
		{
			int num = ifoGetNumPGCI(ifo);
			int i_audio_id = 0;
			int i_lang_cnt = 0;
			
			if ((flags & ifo_PRINT_INFO) != 0)
			{
				strcpy(pr_movie_info->s_video_info, ifoGetVideoDesc(ifo));
				
				j = ifoGetNumAudio(ifo);
				pr_movie_info->i_audio_streams = j;
				
				for (i = 0; i < j; i++)
				{
					uint16_t lang_code = 0;
					strcpy(pr_movie_info->as_audio_languages[i], ifoGetAudioDesc(ifo, i, &i_audio_id));
					if (strlen(pr_movie_info->as_audio_languages[i]) < 1)
					{
						strcpy(pr_movie_info->as_audio_languages[i], "English");
					}
				
					pr_movie_info->audio_list[i] = i_audio_id;
				}
				
				j = ifoGetNumSubPic(ifo);
				
				for (i = 0; i < j; i++)
				{
					uint16_t lang_code = 0;
					pr_movie_info->sub_list[i] = 0x20 + i;

					lang_code = ifoGetSubPicCode(ifo, i);

					memset(pr_movie_info->as_subs_short[i], 0, 3);

					if (lang_code > 0)
					{
						strcpy(pr_movie_info->as_subs[i_lang_cnt], iifoDecodeLang(lang_code));

						memcpy(pr_movie_info->as_subs_short[i_lang_cnt], &lang_code, 2);
						i_lang_cnt++;
					}
				}

				pr_movie_info->i_num_subs = i_lang_cnt;
			}

				pr_movie_info->i_num_titles = num;
				
				for (i = 0; i < num; i++)
				{
					pr_movie_info->r_titles[i].i_num_chapters = ifoGetPGCIInfo(ifo, i,
						&(pr_movie_info->r_titles[i].l_length));

					//*cell_ids = calloc(pr_movie_info->r_titles[i].i_num_chapters, sizeof **cell_ids);
					//ifoGetPGCICells(ifo, i, *cell_ids);
				}

			ifoClose(ifo);
		}
		else
		{
			if ((flags & ifo_PRINT_INFO) != 0)
				//dummyfprintf(stderr, "* Unable to parse \"%s\" as IFO-File\n", ifo_name);
				return FALSE;
		}
	}

	return TRUE;
}
