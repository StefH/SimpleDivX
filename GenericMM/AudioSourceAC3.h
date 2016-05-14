#ifndef f_AUDIOSOURCEAC3_H
#define f_AUDIOSOURCEAC3_H

#include <stdio.h>
#include <windows.h>
#include <vfw.h>

/*  AC3 audio held in a WAV file

    wFormatTag          WAVE_FORMAT_DOLBY_AC3
    nChannels           1 -6 channels valid
    nSamplesPerSec      48000, 44100, 32000
    nAvgByesPerSec      4000 to 80000
    nBlockAlign         128 - 3840
    wBitsPerSample      Up to 24 bits - (in the original)

*/

#define WAVE_FORMAT_DOLBY_AC3 0x2000


typedef unsigned long  uint_32;
typedef unsigned short uint_16;
typedef unsigned char  uint_8;

typedef signed long    sint_32;
typedef signed short   sint_16;
typedef signed char    sint_8;

class AC3_SYNCINFO {
public:
  uint_32 magic;
  /* Sync word == 0x0B77 */
  /* uint_16 syncword; */
  /* crc for the first 5/8 of the sync block */
  /* uint_16 crc1; */
  /* Stream Sampling Rate (kHz) 0 = 48, 1 = 44.1, 2 = 32, 3 = reserved */
  uint_16 fscod;	
  /* Frame size code */
  uint_16 frmsizecod;
  
  /* Information not in the AC-3 bitstream, but derived */
  /* Frame size in 16 bit words */
  uint_16 frame_size;
  /* Bit rate in kilobits */
  uint_16 bit_rate;
};

class AC3_BSI {
public:
  uint_32 magic;
  /* Bit stream identification == 0x8 */
  uint_16 bsid;	
  /* Bit stream mode */
  uint_16 bsmod;
  /* Audio coding mode */
  uint_16 acmod;
  /* If we're using the centre channel then */
  /* centre mix level */
  uint_16 cmixlev;
  /* If we're using the surround channel then */
  /* surround mix level */
  uint_16 surmixlev;
  /* If we're in 2/0 mode then */
  /* Dolby surround mix level - NOT USED - */
  uint_16 dsurmod;
  /* Low frequency effects on */
  uint_16 lfeon;
  /* Dialogue Normalization level */
  uint_16 dialnorm;
  /* Compression exists */
  uint_16 compre;
  /* Compression level */
  uint_16 compr;
  /* Language code exists */
  uint_16 langcode;
  /* Language code */
  uint_16 langcod;
  /* Audio production info exists*/
  uint_16 audprodie;
  uint_16 mixlevel;
  uint_16 roomtyp;
  /* If we're in dual mono mode (acmod == 0) then extra stuff */
  uint_16 dialnorm2;
  uint_16 compr2e;
  uint_16 compr2;
  uint_16 langcod2e;
  uint_16 langcod2;
  uint_16 audprodi2e;
  uint_16 mixlevel2;
  uint_16 roomtyp2;
  /* Copyright bit */
  uint_16 copyrightb;
  /* Original bit */
  uint_16 origbs;
  /* Timecode 1 exists */
  uint_16 timecod1e;
  /* Timecode 1 */
  uint_16 timecod1;
  /* Timecode 2 exists */
  uint_16 timecod2e;
  /* Timecode 2 */
  uint_16 timecod2;
  /* Additional bit stream info exists */
  uint_16 addbsie;
  /* Additional bit stream length - 1 (in bytes) */
  uint_16 addbsil;
  /* Additional bit stream information (max 64 bytes) */
  uint_8	addbsi[64];
  
  /* Information not in the AC-3 bitstream, but derived */
  /* Number of channels (excluding LFE)
   * Derived from acmod */
  uint_16 nfchans;
};

class AC3_AUDBLK {
public:
  uint_32 magic1;
  /* block switch bit indexed by channel num */
  uint_16 blksw[5];
  /* dither enable bit indexed by channel num */
  uint_16 dithflag[5];
  /* dynamic range gain exists */
  uint_16 dynrnge;
  /* dynamic range gain */
  uint_16 dynrng;
  /* if acmod==0 then */
  /* dynamic range 2 gain exists */
  uint_16 dynrng2e;
  /* dynamic range 2 gain */
  uint_16 dynrng2;
  /* coupling strategy exists */
  uint_16 cplstre;
  /* coupling in use */
  uint_16 cplinu;
  /* channel coupled */
  uint_16 chincpl[5];
  /* if acmod==2 then */
  /* Phase flags in use */
  uint_16 phsflginu;
  /* coupling begin frequency code */
  uint_16 cplbegf;
  /* coupling end frequency code */
  uint_16 cplendf;
  /* coupling band structure bits */
  uint_16 cplbndstrc[18];
  /* Do coupling co-ords exist for this channel? */
  uint_16 cplcoe[5];
  /* Master coupling co-ordinate */
  uint_16 mstrcplco[5];
  /* Per coupling band coupling co-ordinates */
  uint_16 cplcoexp[5][18];
  uint_16 cplcomant[5][18];
  /* Phase flags for dual mono */
  uint_16 phsflg[18];
  /* Is there a rematrixing strategy */
  uint_16 rematstr;
  /* Rematrixing bits */
  uint_16 rematflg[4];
  /* Coupling exponent strategy */
  uint_16 cplexpstr;
  /* Exponent strategy for full bandwidth channels */
  uint_16 chexpstr[5];
  /* Exponent strategy for lfe channel */
  uint_16 lfeexpstr;
  /* Channel bandwidth for independent channels */
  uint_16 chbwcod[5];
  /* The absolute coupling exponent */
  uint_16 cplabsexp;
  /* Coupling channel exponents (D15 mode gives 18 * 12 /3  encoded exponents */
  uint_16 cplexps[18 * 12 / 3];
  /* Sanity checking constant */
  uint_32	magic2;
  /* fbw channel exponents */
  uint_16 exps[5][252 / 3];
  /* channel gain range */
  uint_16 gainrng[5];
  /* low frequency exponents */
  uint_16 lfeexps[3];
  
  /* Bit allocation info */
  uint_16 baie;
  /* Slow decay code */
  uint_16 sdcycod;
  /* Fast decay code */
  uint_16 fdcycod;
  /* Slow gain code */
  uint_16 sgaincod;
  /* dB per bit code */
  uint_16 dbpbcod;
  /* masking floor code */
  uint_16 floorcod;
  
  /* SNR offset info */
  uint_16 snroffste;
  /* coarse SNR offset */
  uint_16 csnroffst;
  /* coupling fine SNR offset */
  uint_16 cplfsnroffst;
  /* coupling fast gain code */
  uint_16 cplfgaincod;
  /* fbw fine SNR offset */
  uint_16 fsnroffst[5];
  /* fbw fast gain code */
  uint_16 fgaincod[5];
  /* lfe fine SNR offset */
  uint_16 lfefsnroffst;
  /* lfe fast gain code */
  uint_16 lfefgaincod;
  
  /* Coupling leak info */
  uint_16 cplleake;
  /* coupling fast leak initialization */
  uint_16 cplfleak;
  /* coupling slow leak initialization */
  uint_16 cplsleak;
  
  /* delta bit allocation info */
  uint_16 deltbaie;
  /* coupling delta bit allocation exists */
  uint_16 cpldeltbae;
  /* fbw delta bit allocation exists */
  uint_16 deltbae[5];
  /* number of cpl delta bit segments */
  uint_16 cpldeltnseg;
  /* coupling delta bit allocation offset */
  uint_16 cpldeltoffst[8];
  /* coupling delta bit allocation length */
  uint_16 cpldeltlen[8];
  /* coupling delta bit allocation length */
  uint_16 cpldeltba[8];
  /* number of delta bit segments */
  uint_16 deltnseg[5];
  /* fbw delta bit allocation offset */
  uint_16 deltoffst[5][8];
  /* fbw delta bit allocation length */
  uint_16 deltlen[5][8];
  /* fbw delta bit allocation length */
  uint_16 deltba[5][8];
  
  /* skip length exists */
  uint_16 skiple;
  /* skip length */
  uint_16 skipl;
  
  /* channel mantissas */
  uint_16 chmant[5][256];
  
  /* coupling mantissas */
  uint_16 cplmant[256];
  
  /* coupling mantissas */
  uint_16 lfemant[7];
  
  /*  -- Information not in the bitstream, but derived thereof  -- */
  
  /* Number of coupling sub-bands */
  uint_16 ncplsubnd;
  
  /* Number of combined coupling sub-bands
   * Derived from ncplsubnd and cplbndstrc */
  uint_16 ncplbnd;
  
  /* Number of exponent groups by channel
   * Derived from strmant, endmant */
  uint_16 nchgrps[5];
  
  /* Number of coupling exponent groups
   * Derived from cplbegf, cplendf, cplexpstr */
  uint_16 ncplgrps;
  
  /* End mantissa numbers of fbw channels */
  uint_16 endmant[5];
  
  /* Start and end mantissa numbers for the coupling channel */
  uint_16 cplstrtmant;
  uint_16 cplendmant;
  
  /* Decoded exponent info */
  uint_16 fbw_exp[5][256];
  uint_16 cpl_exp[256];
  uint_16 lfe_exp[7];
  
  /* Bit allocation pointer results */
  uint_16 fbw_bap[5][256];
  /* FIXME figure out exactly how many entries there should be (253-37?) */
  uint_16 cpl_bap[256];
  uint_16 lfe_bap[7];
  
  uint_32 magic3;
};

class AC3_Bitstream {
public:
  FILE    *file;
  uint_32 current_word;
  uint_32 bits_left;
  uint_32 total_bits_read;
  BOOL    done;
  long    total_bytes_read;

  AC3_Bitstream(FILE *file);
  ~AC3_Bitstream();

  int GetBitSequence(uint_32 num_bits);
  int Load();
  int Resync();
};

class AudioSourceAC3 {
public:
  FILE          *m_ac3File;
  AC3_Bitstream *m_ac3Bitstream;
  AC3_SYNCINFO  *m_sync_info;
  AC3_BSI       *m_bsi;
  AC3_AUDBLK    *m_audblk;

public:
  AudioSourceAC3(FILE *f);
  ~AudioSourceAC3();

  BOOL Parse(LPWAVEFORMATEX fmt);

private:
  void ParseBSI();
  void ParseSyncInfo();
};

#endif
