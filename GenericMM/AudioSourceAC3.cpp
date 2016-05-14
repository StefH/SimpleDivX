//
//
//

#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <vfw.h>

#include "AudioSourceAC3.h"

#define MAX_SEARCH_SIZE (512*1024)

// byte swapping routine
#define SWAP_ENDIAN32(x)  ((((uint_8*)&x)[0] << 24) |  \
	(((uint_8*)&x)[1] << 16) |  \
	(((uint_8*)&x)[2] << 8)  |  \
((uint_8*)&x)[3])           

AC3_Bitstream::AC3_Bitstream(FILE *f) 
{
	file            = f;
	current_word    = 0;
	bits_left       = 0;
	total_bits_read = 0;
	done            = FALSE;
	
	// preload data
	this->Load();
}

AC3_Bitstream::~AC3_Bitstream() 
{
}

// Load data from file - does byte swapping
int AC3_Bitstream::Load()
{
	int bytes_read = 0;
	
	if (file!=NULL){
		bytes_read = fread(&current_word,4,1,file);
	}
	
	bytes_read = ( bytes_read << 2 ); /* multiply by 4 */
	
	current_word = SWAP_ENDIAN32(current_word);
	bits_left = bytes_read * 8;
	
	//FIXME finishing up the stream isn't done too gracefully
	
	if (bytes_read < 4){  // check to see if end of file
		done = TRUE;
		file = NULL;
	}
	
	return bytes_read;
}

// Fetches 1-32 bits from the file opened in bitstream_open
int AC3_Bitstream::GetBitSequence(uint_32 num_bits)
{
	uint_32 result;
	uint_32 bits_read;
	uint_32 bits_to_go;
	
	if(num_bits == 0)
		return 0;
	
	bits_read = num_bits > bits_left ? bits_left : num_bits; 
	
	result = current_word  >> (32 - bits_read);
	current_word <<= bits_read;
	bits_left -= bits_read;
	
	if(bits_left == 0){
		long loaded = Load();
	}
	
	if (bits_read < num_bits) {
		bits_to_go = num_bits - bits_read;
		result <<= bits_to_go;
		result |= current_word  >> (32 - bits_to_go);
		current_word <<= bits_to_go;
		bits_left -= bits_to_go;
	}
	
	total_bits_read += num_bits;
	// crc_process(result,num_bits);
	
	return result;
}


// Positions the stream pointer to a sync word
int AC3_Bitstream::Resync() {
	
	int i = 0;
	uint_16 sync_word;
	
	if (!file) return -1;
	
	sync_word = GetBitSequence(16);
	
	/* Make sure we sync'ed */
	while(1) 
	{
		if(sync_word == 0x0b77)
			break;
		sync_word = ( sync_word << 1 );
		sync_word |= GetBitSequence(1);
		i++;
		if (i > 32) 
		{  
			//if ( ftell(file)==-1 || feof(file) || i > MAX_SEARCH_SIZE)  // if not found after 1024 bytes -> stop it :-)
			if (!file) return -1;
			if ( feof(file) || (ftell(file)==-1) || i > MAX_SEARCH_SIZE)  // if not found after 1024 bytes -> stop it :-))
			{
				i = -1;
				break;
			}
		}
	}
	
	total_bits_read = 16;
	// crc_init();
	
	return i; /* return # bits skipped */
}

///////////////////////////

const uint_16 nfchans[] = {2,1,2,3,3,4,4,5};

struct frmsize_s
{
	uint_16 bit_rate;
	uint_16 frm_size[3];
};

const struct frmsize_s frmsizecod_tbl[] = {
	{ 32  ,{64   ,69   ,96   } },
	{ 32  ,{64   ,70   ,96   } },
	{ 40  ,{80   ,87   ,120  } },
	{ 40  ,{80   ,88   ,120  } },
	{ 48  ,{96   ,104  ,144  } },
	{ 48  ,{96   ,105  ,144  } },
	{ 56  ,{112  ,121  ,168  } },
	{ 56  ,{112  ,122  ,168  } },
	{ 64  ,{128  ,139  ,192  } },
	{ 64  ,{128  ,140  ,192  } },
	{ 80  ,{160  ,174  ,240  } },
	{ 80  ,{160  ,175  ,240  } },
	{ 96  ,{192  ,208  ,288  } },
	{ 96  ,{192  ,209  ,288  } },
	{ 112 ,{224  ,243  ,336  } },
	{ 112 ,{224  ,244  ,336  } },
	{ 128 ,{256  ,278  ,384  } },
	{ 128 ,{256  ,279  ,384  } },
	{ 160 ,{320  ,348  ,480  } },
	{ 160 ,{320  ,349  ,480  } },
	{ 192 ,{384  ,417  ,576  } },
	{ 192 ,{384  ,418  ,576  } },
	{ 224 ,{448  ,487  ,672  } },
	{ 224 ,{448  ,488  ,672  } },
	{ 256 ,{512  ,557  ,768  } },
	{ 256 ,{512  ,558  ,768  } },
	{ 320 ,{640  ,696  ,960  } },
	{ 320 ,{640  ,697  ,960  } },
	{ 384 ,{768  ,835  ,1152 } },
	{ 384 ,{768  ,836  ,1152 } },
	{ 448 ,{896  ,975  ,1344 } },
	{ 448 ,{896  ,976  ,1344 } },
	{ 512 ,{1024 ,1114 ,1536 } },
	{ 512 ,{1024 ,1115 ,1536 } },
	{ 576 ,{1152 ,1253 ,1728 } },
	{ 576 ,{1152 ,1254 ,1728 } },
	{ 640 ,{1280 ,1393 ,1920 } },
	{ 640 ,{1280 ,1394 ,1920 } }
};

AudioSourceAC3::AudioSourceAC3(FILE *ac3File) 
{
	// set the file ptr
	m_ac3File = ac3File;
}

AudioSourceAC3::~AudioSourceAC3() 
{
	// nothing
}

BOOL AudioSourceAC3::Parse(LPWAVEFORMATEX fmt)
{
	BOOL b_sync_found = FALSE;

	if (m_ac3File == NULL) 
	{
		return FALSE;
	}
	
	// create a bitstream
	m_ac3Bitstream = new AC3_Bitstream(m_ac3File);
	
	for (int i = 0; i < 5 ; i++)
	{
		// sync to the start of an AC3 frame
		if (m_ac3Bitstream->Resync() < 0) 
		{
			
		}
		else
		{
			b_sync_found = TRUE;
			break;
		}
	}
	
	if (!b_sync_found)
	{
		// unable to resync - clean up and return
		delete m_ac3Bitstream;
		return FALSE;
	}

	// create sync info structure
	m_sync_info = new AC3_SYNCINFO();
	
	// and parse the sync_info
	ParseSyncInfo();
	
	// create the BSI
	m_bsi = new AC3_BSI();
	ParseBSI();

	fmt->wFormatTag = WAVE_FORMAT_DOLBY_AC3;
	fmt->nChannels  = m_bsi->nfchans;
	
	switch (m_sync_info->fscod) 
	{
	case 2:
		fmt->nSamplesPerSec = 32000;
		break;
	case 1:
		fmt->nSamplesPerSec = 44100;
		break;
	case 0:
		fmt->nSamplesPerSec = 48000;
		break;
	default:
		fmt->nSamplesPerSec = 0;
		break;
	}
	
	fmt->nAvgBytesPerSec = (1000 * m_sync_info->bit_rate) / 8;
	fmt->nBlockAlign = 1;
	fmt->wBitsPerSample = 0;
	fmt->cbSize = sizeof(WAVEFORMATEX);
	
	//delete m_bsi;
	delete m_sync_info;
	delete m_ac3Bitstream;
	
	return TRUE;
}

// Parse a syncinfo structure, minus the sync word
void AudioSourceAC3::ParseSyncInfo()
{
	/* Get crc1 - we don't actually use this data though */
	m_ac3Bitstream->GetBitSequence(16);
	
	/* Get the sampling rate */
	m_sync_info->fscod  = m_ac3Bitstream->GetBitSequence(2);
	
	/* Get the frame size code */
	m_sync_info->frmsizecod = m_ac3Bitstream->GetBitSequence(6);
	
	m_sync_info->bit_rate = frmsizecod_tbl[m_sync_info->frmsizecod].bit_rate;
	m_sync_info->frame_size = frmsizecod_tbl[m_sync_info->frmsizecod].frm_size[m_sync_info->fscod];
	
	// stats_printf_syncinfo(syncinfo);
}

void AudioSourceAC3::ParseBSI()
{
	uint_32 i = 0;
	
	/* Check the AC-3 version number */
	m_bsi->bsid = m_ac3Bitstream->GetBitSequence(5);
	
	/* Get the audio service provided by the steram */
	m_bsi->bsmod = m_ac3Bitstream->GetBitSequence(3);
	
	/* Get the audio coding mode (ie how many channels)*/
	m_bsi->acmod = m_ac3Bitstream->GetBitSequence(3);

	/* Predecode the number of full bandwidth channels as we use this
	* number a lot */
	m_bsi->nfchans = nfchans[m_bsi->acmod];
	
	/* If it is in use, get the centre channel mix level */
	if ((m_bsi->acmod & 0x1) && (m_bsi->acmod != 0x1))
		m_bsi->cmixlev = m_ac3Bitstream->GetBitSequence(2);
	
	/* If it is in use, get the surround channel mix level */
	if (m_bsi->acmod & 0x4)
		m_bsi->surmixlev = m_ac3Bitstream->GetBitSequence(2);
	
	/* Get the dolby surround mode if in 2/0 mode */
	if(m_bsi->acmod == 0x2)
		m_bsi->dsurmod= m_ac3Bitstream->GetBitSequence(2);
	
	/* Is the low frequency effects channel on? */
	m_bsi->lfeon = m_ac3Bitstream->GetBitSequence(1);
	
	/* Get the dialogue normalization level */
	m_bsi->dialnorm = m_ac3Bitstream->GetBitSequence(5);
	
	/* Does compression gain exist? */
	m_bsi->compre = m_ac3Bitstream->GetBitSequence(1);
	if (m_bsi->compre) {
		/* Get compression gain */
		m_bsi->compr = m_ac3Bitstream->GetBitSequence(8);
	}
	
	/* Does language code exist? */
	m_bsi->langcode = m_ac3Bitstream->GetBitSequence(1);
	if (m_bsi->langcode) {
		/* Get langauge code */
		m_bsi->langcod = m_ac3Bitstream->GetBitSequence(8);
	}
	
	/* Does audio production info exist? */
	m_bsi->audprodie = m_ac3Bitstream->GetBitSequence(1);
	if (m_bsi->audprodie) {
		/* Get mix level */
		m_bsi->mixlevel = m_ac3Bitstream->GetBitSequence(5);
		
		/* Get room type */
		m_bsi->roomtyp = m_ac3Bitstream->GetBitSequence(2);
	}
	
	/* If we're in dual mono mode then get some extra info */
	if (m_bsi->acmod ==0) {
		/* Get the dialogue normalization level two */
		m_bsi->dialnorm2 = m_ac3Bitstream->GetBitSequence(5);
		
		/* Does compression gain two exist? */
		m_bsi->compr2e = m_ac3Bitstream->GetBitSequence(1);
		if (m_bsi->compr2e) {
			/* Get compression gain two */
			m_bsi->compr2 = m_ac3Bitstream->GetBitSequence(8);
		}
		
		/* Does language code two exist? */
		m_bsi->langcod2e = m_ac3Bitstream->GetBitSequence(1);
		if (m_bsi->langcod2e) {
			/* Get langauge code two */
			m_bsi->langcod2 = m_ac3Bitstream->GetBitSequence(8);
		}
		
		/* Does audio production info two exist? */
		m_bsi->audprodi2e = m_ac3Bitstream->GetBitSequence(1);
		if (m_bsi->audprodi2e) {
			/* Get mix level two */
			m_bsi->mixlevel2 = m_ac3Bitstream->GetBitSequence(5);
			
			/* Get room type two */
			m_bsi->roomtyp2 = m_ac3Bitstream->GetBitSequence(2);
		}
	}
	
	/* Get the copyright bit */
	m_bsi->copyrightb = m_ac3Bitstream->GetBitSequence(1);
	
	/* Get the original bit */
	m_bsi->origbs = m_ac3Bitstream->GetBitSequence(1);
	
	/* Does timecode one exist? */
	m_bsi->timecod1e = m_ac3Bitstream->GetBitSequence(1);
	
	if(m_bsi->timecod1e) {
		m_bsi->timecod1 = m_ac3Bitstream->GetBitSequence(14);
	}
	
	/* Does timecode two exist? */
	m_bsi->timecod2e = m_ac3Bitstream->GetBitSequence(1);
	
	if(m_bsi->timecod2e) {
		m_bsi->timecod2 = m_ac3Bitstream->GetBitSequence(14);
	}
	/* Does addition info exist? */
	m_bsi->addbsie = m_ac3Bitstream->GetBitSequence(1);
	
	if(m_bsi->addbsie) {
		/* Get how much info is there */
		m_bsi->addbsil = m_ac3Bitstream->GetBitSequence(6);
		
		/* Get the additional info */
		for(i = 0 ; i < (uint_32) (m_bsi->addbsil + 1) ; i++)
		{
			m_bsi->addbsi[i] = m_ac3Bitstream->GetBitSequence(8);
		}
	}
}