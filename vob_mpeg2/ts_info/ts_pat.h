/* 
 *	Copyright (C) 2004, Donald A Graft, All Rights Reserved
 *
 *  Command line PAT/PMT table parser for PID detection.
 *  Usage: table filename
 *	
 *  TABLE is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  TABLE is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with TABLE; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *
 */

#include <stdio.h>

#include "../vstrip/vobinfo.h"

#define TS_SYNC_BYTE 0x47
#define PAT_PID 0
#define LIMIT 10000000
#define MAX_PAT_ENTRIES 500

struct pat_entry
{
	unsigned int program;
	unsigned int pmtpid;
};

class PATParser
{
	unsigned int num_pat_entries;
	bool first_pat, first_pmt;
	struct pat_entry pat_entries[MAX_PAT_ENTRIES];
	
	
	int AnalyzeRaw(HWND hDialog, char *filename, unsigned int audio_pid, unsigned int *audio_type);
public:
	PATParser();
	int AnalyzePAT(char *filename, 
						  int* num_audio_streams, AUDIO_LIST* pr_audio_list,
						  int* num_streams, STREAM_LIST* pr_stream_list);
	int DumpPAT(HWND hDialog, char *filename);
	int DumpRaw(HWND hDialog, char *filename);
	int GetAudioType(char *filename, unsigned int audio_pid);
};

