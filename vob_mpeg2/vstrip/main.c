/*************************************************************************
  vStrip: Parses VOB- and IFO-Files and extracts data from them.
  Copyright (C) 1999-2001 [maven] (maven@maven.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  main.c: application example for vstrip.c, basically just parses flags,
          tries to parse an IFO-file (using parse_ifo.c)
          and calls vs_strip (tabsize 2)...

  Watcom C 11 Project-File and linux-makefile included...

  inspired by
    Brent Beyeler's bbDMUX (beyeler@home.com)

  thanx!

  [see vstrip.c for more information]
*************************************************************************/

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "vstrip.h"
#include "aspi.h"
#include "parse_ifo.h"

char *parse_listfile(char *fname)
{
	char *res = NULL;
	char *ext;

	ext = strrchr(fname, '.');
	if (fname[0] == '@' || (ext && stricmp(ext, ".LST") == 0))
	{ // read in stream-list
		int num = 0;
		int numchars = 0;
		char *name;
		FILE *string_list;

		for (name = fname; *name && *name == '@'; name++) ;

		string_list = fopen(name, "rt");
		if (string_list)
		{
			char buf[256];

			while (!feof(string_list))
			{
				if (fgets(buf, 256, string_list) != NULL)
				{
					char *nstart;
					int i;

					for (nstart = buf; *nstart && (*nstart == ' '); nstart++) ;
					for (i = strlen(nstart) - 1; i >= 0 && (nstart[i] == ' ' || nstart[i] == 13 || nstart[i] == 10); i--) ;
					if (i > 0)
					{
						numchars += i + 2; // (one extra for 0)
						num++;
					}
				}
			}
			fclose(string_list);

			if (num > 0)
			{
				char *dest;

				res = malloc((numchars + 2) * sizeof *res);
				strcpy(res, ":"); // marker for multi-string for vstrip
				dest = res + 1;
				string_list = fopen(name, "rt");
				while (!feof(string_list))
				{
					if (fgets(buf, 256, string_list) != NULL)
					{
						char *nstart;
						int i;

						for (nstart = buf; *nstart && (*nstart == ' '); nstart++) ;
						for (i = strlen(nstart) - 1; i >= 0 && (nstart[i] == ' ' || nstart[i] == 13 || nstart[i] == 10); i--) ;
						if (i > 0)
						{
							nstart[i + 1] = 0;
							strcpy(dest, nstart);
							dest += i + 2;
						}
					}
				}
				strcpy(dest, ""); // terminating 0
				fclose(string_list);
			}
		}
	}
	else
	{
		res = malloc((strlen(fname) + 1) * sizeof *res);
		strcpy(res, fname);
	}
	return res;
}

// used: !$abcdefgijklmnopqrstuv
static const char usage[] =
"Usage:\n"
"  vStrip [@]In [-Options] [[=]IDs]\n"
"  In  denotes the input-file and has to be the first argument. If a '@'\n"
"      is prepended to the name, it is read as a streamlist (textfile).\n"
"  Options:\n"
"  -!  defaults to strip everything,\n"
"  -b  disables macrovision removal,\n"
"  -d  enables demux mode (not producing a VOB anymore),\n"
"  -e  disables VOB-assumptions (no 2048 byte sectors), enables demux,\n"
"  -g  only keeps GOP-Headers of the Video-Stream(s),\n"
"  -l  lists the CELL-Positions parsed from the IFO-file,\n"
#ifdef aspi_USE_ASPI
"  -u  tries to parse (and output) UDF-filesystem on In (e.g. vStrip M:\\ -u)\n"
#endif
"  -c  splits the VOB by CELL-ID,\n"
"  -v  splits the VOB by VOB-ID,\n"
"  -oM sets the output filename to M (add '+' in front to append),\n"
"  -iN sets the IFO-file filename to N,\n"
"  -jO sets default angle for IFO-Parsing to O (=1, 0 disables removal),\n"
"  -pP selects program-chain (PGC) P (from IFO-file) and enables IFO-Parsing,\n"
"  -$Q splits output-file(s) after Q megabytes (1024KB chunks),\n"
"  -aR changes the aspect-ratio to R,\n"
"  -fS changes the frame-rate to S,\n"
"  -sT seeks to LBA T (2048 byte sectors),\n"
#ifdef aspi_USE_ASPI
"  -tU file IO: clib then ASPI (U=1), ASPI then clib (U=2), clib only (U=3),\n"
#endif
#ifdef vs_DECRYPT
"  -kV sets the key to V (5 hex bytes, no spaces),\n"
"  -mW a key deduced from a padding-stream is worth W normal guesses (=2),\n"
"  -nX sets the number of times a key needs to repeat to X (=2),\n"
"  -qY sets percentage a candidate key has to have of all keys to Y (=75),\n"
"  -rZ key-search per VOB-ID (Z=1), CELL-ID (Z=2) or VOB- & CELL-ID (Z=3),\n"
#endif
"  IDs hex stream-ids (e.g. 0xe0), seperated by a space. If a '=' is\n"
"      prepended to the id, the last stream is remapped to this id.\n"
"Hints:\n"
"  - Demux mode can split, too (but \"c\" and \"v\" are exclusive),\n"
"  - All the options w/o an argument can be combined (e.g. \"-!dc\")\n"
"  - Try a test-run on the VOB first (only filename as parameter),\n"
"  - Frame-rates:\n";

static const char examples[] =
"Examples:\n"
"  To get a list of the streams in a VOB:\n"
"    vStrip in.vob\n"
"  To keep video and AC3 stream 0x81 mapped to 0x80 using PGC 0 and 1GB chunks:\n"
"    vStrip @in.lst -iin.ifo -p0 -$1024 -!oout.vob 0xe0 0xbd 0x81 =0x80\n"
"  To demux the video-stream:\n"
"    vStrip in.vob -!doout.m2v 0xe0\n";


#include "aspi.h"
#include "udf.h"

#ifdef aspi_USE_ASPI

void udf_traverse(const dword address, tp_udf_file f, const dword depth)
{
	char buf[512];

	while (udf_get_next(address, f))
	{
		dword i, j;

		for (i = 0; i < depth * 2; i++)
			buf[i] = ' ';
		buf[i] = '\0';

		if (f->is_parent)
			strcat(buf, "..");
		else
			strcat(buf, f->name);
		if (f->is_dir)
			strcat(buf, "/");

		if (udf_get_lba(address, f, &i, &j))
		{
			char buf2[256];

			sprintf(buf2, " [@LBA: %d - %d]", i, j);
			strcat(buf, buf2);
		}

		if (!f->is_parent)
			puts(buf);

		if (f->is_dir && !f->is_parent)
		{
			tp_udf_file fs = udf_get_sub(address, f);

			if (fs)
			{
				udf_traverse(address, fs, depth + 1);
				udf_free(fs);
			}
		}
	}
}

#endif // aspi_USE_ASPI

int main(int argc, char* argv[])
{
  char							ifo_name[256] = "";
  int								ifo_pgc_idx = -1;
	int								ifo_angle = 1;
  dword							ifo_num_cellids = 0;
  tp_vs_vobcellid		ifo_cellids = NULL;
	t_vs_data					vsd;
  t_vs_streamflags	streams[256], substreams[256];
	int								i, j;
	t_ifo_flags				print_cell_pos = 0;
#ifdef aspi_USE_ASPI
	vstripbool							print_udf = FALSE;
#endif

	memset(&vsd, 0, sizeof vsd);
  vsd.flags = vs_PRINT_SUMMARY | vs_PRINT_INFO | vs_PRINT_ERROR | vs_DEMACRO;
#ifdef aspi_USE_ASPI
  vsd.flags |= vs_USE_ASPI | vs_SUPPORT_1GB;
#else
#endif
  vsd.max_sync_bytes = 2048;
  vsd.aspectratio = vsd.framerate = -1;
  vsd.start_lba = 1048000;
	vsd.end_lba = 1048576;
	vsd.outputs[0].flags = 0;
  vsd.outputs[0].split_output = 0;
#ifdef vs_DECRYPT
  vsd.same_guess = vsd.pad_guess = 2;
  vsd.pc_guess = 75; // 75 percent
#endif
  for (i = 0; i < 256; i++)
  {
  	streams[i].remap_to = i;
  	streams[i].user_func = NULL;
  	streams[i].save = 0x01;
  	substreams[i].remap_to = i;
  	substreams[i].user_func = NULL;
  	substreams[i].save = 0x01;
  }
  dummyfprintf(vs_STDOUT, "vStrip - version %s, by %s.\n\n", vs_VERSION, vs_AUTHOR);
  if (argc < 2)
  {
		fputs(usage, vs_STDERR);
		for (i = 1; i <= MAX_FRAMERATE/2; i++)
			dummyfprintf(vs_STDERR, "    %-30s %s\n", vs_get_framerate(i),vs_get_framerate(i+MAX_FRAMERATE/2));   
    fputs("  - Aspect-ratios:\n", vs_STDERR);
    for (i = 1; i <= MAX_ASPECTRATIO/2; i++)
      dummyfprintf(vs_STDERR, "    %-30s %s\n", vs_get_aspectratio(i),vs_get_aspectratio(i+MAX_ASPECTRATIO/2));
		fputs(examples, vs_STDERR);
		exit(1);
  }

  if (argc > 2)
  {
  	int	 last_stream = -1;
  	vstripbool first_negate = TRUE;
  	vstripbool last_was_substream = FALSE;
  	vstripbool waiting_for_remap = FALSE;
  	char *outname = NULL;

  	for (i = 2; i < argc; i++)
  	{
  		char *parse_char = argv[i];

  		while (*parse_char && (*parse_char != '0') && (*parse_char != '=') && (*parse_char == '-' || *parse_char == '/'))
  		{
  			{
	  			char lower;

  				++parse_char;
  				while (*parse_char)
  				{
  					lower = tolower(*parse_char);
  					switch (lower)
  					{
  						case '!':
				  			if (first_negate)
				  			{
					  			for (j = 0; j < 256; j++)
					  			{
					  				streams[j].save = 0x00;
					  				substreams[j].save = 0x00;
					  			}
					  			first_negate = FALSE;
					  		}
				  			parse_char++;
					  	break;
					  	case 'b':
				  			vsd.flags &= ~vs_DEMACRO;
				  			parse_char++;
					  	break;
					  	case 'c':
				  			vsd.outputs[0].flags |= vso_SPLIT_CELLID;
				  			parse_char++;
					  	break;
					  	case 'd':
				  			vsd.outputs[0].flags |= vso_DEMUX;
				  			parse_char++;
					  	break;
					  	case 'e':
				  			vsd.flags |= vs_NO_VOB;
				  			vsd.outputs[0].flags |= vso_DEMUX;
				  			parse_char++;
					  	break;
					  	case 'g':
				  			vsd.outputs[0].flags |= vso_ONLY_KEEP_GOPS;
				  			parse_char++;
					  	break;
					  	case 'i':
                outname = ++parse_char;
                if (outname[0] == 0)
                {
                  if (++i == argc)
                  {
                    fputs("* Expecting name after -i\n", vs_STDERR);
                    exit(1);
                  }
                  outname = argv[i]; // take the next arg
                }
                strcpy(ifo_name, outname);
					  		parse_char += strlen(parse_char); // end this arg
					  	break;
					  	case 'l':
					  		print_cell_pos = ifo_PRINT_CELL_POS;
					  		parse_char++;
					  	break;
#ifdef aspi_USE_ASPI
					  	case 'u':
								print_udf = TRUE;
				  			parse_char++;
					  	break;
#endif
					  	case 'v':
				  			vsd.outputs[0].flags |= vso_SPLIT_VOBID;
				  			parse_char++;
					  	break;
					  	case 'o':
								outname = ++parse_char;
								if (outname[0] == 0)
								{
                  if (++i == argc)
									{
				  					fputs("* Expecting name after -o\n", vs_STDERR);
				  					exit(1);
									}
                  outname = argv[i];
				  			}
						    if (outname[0] == '+')
						    {
			  					vsd.outputs[0].flags |= vso_APPEND;
						    	outname++;
						  	}
						    strcpy(vsd.outputs[0].outfile, outname);
					  		parse_char += strlen(parse_char);
					  	break;
					  	case '$':
					  	case 'a':
					  	case 'f':
					  	case 'j':
					  	case 'k':
					  	case 'm':
					  	case 'n':
					  	case 'p':
					  	case 'q':
					  	case 'r':
					  	case 's':
					  	case 't':
					  	{ // numerical argument
				  			vstripbool found_hex = FALSE;
				  			char num[256];
				  			char *cur = num;
				  			char c;

				  			parse_char++,	c = tolower(*parse_char);
				  			while ((c >= '0' && c <= '9') || c == 'x' || (c >= 'a' && c <= 'f'))
				  			{
				  				if (c == 'x' || (c >= 'a' && c <= 'f'))
				  					found_hex = TRUE;
				  				*cur = c;
				  				cur++;
				  				parse_char++,	c = tolower(*parse_char);
				  			}
				  			if (cur != num && (lower == 'k' || (!found_hex && (lower == '$' || lower == 'a' || lower == 'f' || lower == 'm'
				  				|| lower == 'j' || lower == 'n' || lower == 'p' || lower == 'q' || lower == 'r' || lower == 's' || lower == 't'))))
				  			{
					  			*cur = 0;
					  			switch(lower)
					  			{
					  				case '$':
					  					vsd.outputs[0].split_output = atoi(num) * ((1 << 20) / fio_BUFFER_SIZE);
					  				break;
					  				case 'a':
					  					vsd.aspectratio = atoi(num);
					  				break;
					  				case 'f':
					  					vsd.framerate = atoi(num);
					  				break;
					  				case 'j':
					  					ifo_angle = atoi(num);
					  				break;
#ifdef vs_DECRYPT
					  				case 'k':
					  					if (num[0] == '0' && tolower(num[1]) == 'x')
					  						cur = num + 2;
					  					else
					  						cur = num;
					  					if (strlen(cur) == 10)
					  					{
					  						dword k1, k2;

					  						sscanf(cur, "%02x%08x", &k1, &k2);
					  						vsd.key[0] = k1;
					  						vsd.key[1] = k2 >> 24;
					  						vsd.key[2] = (k2 >> 16) & 0xff;
					  						vsd.key[3] = (k2 >> 8) & 0xff;
					  						vsd.key[4] = k2 & 0xff;
					  					}
						  				else
						  				{
							  				fputs("* Wrong number of Key digits (need 5 hex-bytes)\n", vs_STDERR);
							  				exit(1);
							  			}
					  				break;
					  				case 'm':
					  					vsd.pad_guess = atoi(num);
					  				break;
					  				case 'n':
					  					vsd.same_guess = atoi(num);
					  				break;
					  				case 'q':
					  					vsd.pc_guess = min(100, atoi(num));
					  				break;
					  				case 'r':
											vsd.flags &= ~(vs_CRACK_EACH_VOB_ID | vs_CRACK_EACH_CELL_ID);
					  					j = atoi(num);
					  					switch (j)
					  					{
					  						case 1:
										    	vsd.flags |= vs_CRACK_EACH_VOB_ID;
					  						break;
					  						case 2:
										    	vsd.flags |= vs_CRACK_EACH_CELL_ID;
					  						break;
					  						case 3:
										    	vsd.flags |= vs_CRACK_EACH_VOB_ID | vs_CRACK_EACH_CELL_ID;
					  						break;
					  						default:
					  							fputs("* Expecting [1,2,3] after -r\n", vs_STDERR);
					  							exit(1);
					  					}
					  				break;
#endif
					  				case 'p':
					  					ifo_pgc_idx = atoi(num);
					  				break;
					  				case 's':
					  					vsd.start_lba = atoi(num);
					  				break;
#ifdef aspi_USE_ASPI
					  				case 't':
											vsd.flags &= ~(vs_USE_ASPI | vs_PREFER_ASPI);
					  					j = atoi(num);
					  					switch (j)
					  					{
					  						case 1:
										    	vsd.flags |= vs_USE_ASPI;
					  						break;
					  						case 2:
										    	vsd.flags |= vs_USE_ASPI | vs_PREFER_ASPI;
					  						break;
					  						case 3:
					  						break;
					  						default:
					  							fputs("* Expecting [1,2,3] after -t\n", vs_STDERR);
					  							exit(1);
					  					}
					  				break;
#endif
					  			}
					  		}
					  		else
					  		{
				  				dummyfprintf(vs_STDERR, "* Expecting number after \"%c\"\n", lower);
				  				exit(1);
					  		}
					  	}
					  	break;
					  	default:
			  				dummyfprintf(vs_STDERR, "* Unknown option \"%c\"\n", lower);
			  				exit(1);
  					}
  				}
  			}
	  	}
  		if (*parse_char == '=')
  		{
  			if (last_stream < 0)
  			{
  				fputs("* The argument before = sign was not a Stream ID\n", vs_STDERR);
  				exit(1);
  			}
  			waiting_for_remap = TRUE;
	  		parse_char++;
  		}
  		if (*parse_char == '0')
  		{
		    sscanf(parse_char, "0x%x", &j);
		    if (j > 255)
		    {
		      fputs("* Stream ID must be in the range 0x00-0xFF\n", vs_STDERR);
		      exit(1);
		    }
		    if (last_was_substream)
		    {
		    	substreams[j].save = !first_negate;
		    	last_stream = j | 256;
		    	last_was_substream = FALSE;
		    }
		    else
		    {
		    	if (waiting_for_remap)
		    	{
		    		if ((last_stream & 256) != 0)
		    			substreams[last_stream & 255].remap_to = j;
		    		else
		    			streams[last_stream].remap_to = j;
		    		last_stream = -1;
		    		waiting_for_remap = FALSE;
		    	}
		    	else
		    	{
			    	if (j != PRIVATE_STREAM_1)
			    	{
			    		streams[j].save = !first_negate;
				    	last_stream = j;
			    	}
			    	else
			    	{
			    		streams[j].save = !first_negate;
			    		last_was_substream = TRUE;
			    	}
			    }
			  }
			}
	    else if (*parse_char)
	    {
				dummyfprintf(vs_STDERR, "* Expecting Stream ID (and not \"%c\")\n", *parse_char);
				exit(1);
	    }
  	}
  	if (last_was_substream)
    {
      fputs("* Missing SubStream ID\n", vs_STDERR);
      exit(1);
    }
    if (waiting_for_remap)
    {
      fputs("* Missing Remap-value after = sign\n", vs_STDERR);
      exit(1);
    }
    if (vsd.outputs[0].split_output > 0)
    	dummyfprintf(vs_STDOUT, "Splitting output-file(s) after %u megabytes (1024KB chunks).\n", vsd.outputs[0].split_output / ((1 << 20) / fio_BUFFER_SIZE));
    if ((vsd.outputs[0].flags & vso_ONLY_KEEP_GOPS) != 0)
    	fputs("Only keeping GOPs.\n", vs_STDOUT);
    if ((vsd.outputs[0].flags & vso_DEMUX) != 0)
    {
    	fputs("Demux-Mode enabled.\nAll remapping disabled.\n", vs_STDOUT);
	    for (i = 0; i < 256; i++)
	    {
	    	streams[i].remap_to = i;
	    	substreams[i].remap_to = i;
	    }
    }
    if ((vsd.outputs[0].flags & vso_SPLIT_VOBID) != 0)
    	fputs("Splitting output by VOB-ID\n", vs_STDOUT);
    if ((vsd.outputs[0].flags & vso_SPLIT_CELLID) != 0)
    	fputs("Splitting output by CELL-ID\n", vs_STDOUT);
  }

#ifdef aspi_USE_ASPI
	if (print_udf && aspi_Init())
	{
		dword address;
		tp_udf_file file;
		vstripbool res;
		
		res = aspi_GetDriveAddress(argv[1][0], &address);
		if (res)
		{
			aspi_SetTimeout(address, 15);

			file = udf_get_root(address, 0, TRUE);
			if (file)
			{
				udf_traverse(address, file, 0);
				udf_free(file);
			}
		}
		aspi_Done();
	}
#endif

	if (ifo_name[0])
	{
		t_ifo_flags fl = ifo_PRINT_INFO | print_cell_pos;
#ifdef aspi_USE_ASPI
		vstripbool aspi_ok = FALSE;

		if ((vsd.flags & vs_USE_ASPI) != 0)
		{
			aspi_ok = aspi_Init();
			if (aspi_ok)
			{
				fl |= ifo_USE_ASPI;
				if ((vsd.flags & vs_PREFER_ASPI) != 0)
					fl |= ifo_PREFER_ASPI;
			}
		}
#endif

		if (!ifoParse(ifo_name, ifo_pgc_idx, fl, &ifo_num_cellids, &ifo_cellids))
		{
#ifdef aspi_USE_ASPI
			if (aspi_ok)
				aspi_Done();
#endif
			exit(1);
		}
		else if (ifo_angle > 0)
		{ // set the vob/cell_ids for unwanted angles to 0
			for (i = 0; i < (int)ifo_num_cellids; i++)
				if ((ifo_cellids[i].angle >> 4) >= ifo_angle && (ifo_cellids[i].angle & 0xf) != ifo_angle)
				{ // so we skip them
					ifo_cellids[i].cell_id = 0;
					ifo_cellids[i].vob_id = 0;
				}
		}
#ifdef aspi_USE_ASPI
		if (aspi_ok)
			aspi_Done();
#endif
	}

	vsd.infile = parse_listfile(argv[1]);

  if (!vsd.outputs[0].outfile[0])
  {
		vsd.num_outputs = 0;
    fputs("Scanning for stream id's, press control-c to quit...\n", vs_STDOUT);
	  for (i = 0; i < 256; i++)
	  {
	  	streams[i].save = 0x00;
	  	substreams[i].save = 0x00;
	  }
	}
	else
		vsd.num_outputs = 1;

	vs_strip(&vsd, streams, substreams, ifo_num_cellids, ifo_cellids);
 	if (ifo_cellids)
 		free(ifo_cellids);
 	free(vsd.infile);
  return 0;
}