#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "s_types.h"
#include "file_io.h"
#include "in_buffer.h"
#include "vstrip.h"
#include "parse_ifo.h"
#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include "vobinfo.h"

static int g_found_order = 0;

VSTRIP_DLL_API const char *vs_get_stream_name(const dword id)
{
	static char buffer[256];

	switch (id)
	{
		case PROGRAM_STREAM_MAP:
			return "Program Map";
		case PRIVATE_STREAM_1:
			return "Private 1";
		case PRIVATE_STREAM_2:
			return "Private 2";
		case ECM_STREAM:
			return "ECM";
		case EMM_STREAM:
			return "EMM";
		case PROGRAM_STREAM_DIRECTORY:
			return "Program Directory";
		case DSMCC_STREAM:
			return "DSMCC";
		case ITUTRECH222TYPEE_STREAM:
			return "ITU-T Rec. H.222.1 type E";
		case PADDING_STREAM:
			return "Padding";
		default:
			if ((id >= VIDEO_STREAM) && (id <= MAX_VIDEO_STREAM))
				sprintf(buffer, "Video %u", id - VIDEO_STREAM);
			else if ((id >= AUDIO_STREAM) && (id <= MAX_AUDIO_STREAM))
				sprintf(buffer, "MPEG1 Audio %u", id - AUDIO_STREAM);
			else
				strcpy(buffer, "other");
			return buffer;
	}
}

VSTRIP_DLL_API const double vs_get_framerate(const dword id)
{
	switch (id)
	{
		case 1:
			return 24000.0 / 1001.0; // 23.976
		case 2:
			return 24.000;
		case 3:
			return 25.000;
		case 4:
			return 30000.0 / 1001.0; // 29.97
		case 5:
			return 30.0;
		case 6:
			return 50.0;
		case 7:
			return 60000.0 / 1001.0; // (60000/1001)
		case 8:
			return 60.0;
		default:
			return 1.0;
	}
}

VSTRIP_DLL_API char *vs_get_aspectratio(const dword id, double *pf_ratio)
{
	switch (id)
	{
		case 1:
			*pf_ratio = 1.0; // square pixels
			return "1 : 1";
		case 2:
			*pf_ratio = 4.0 / 3.0;
			return "4 : 3";
		case 3:
			*pf_ratio = 16.0 / 9.0;
			return "16 : 9";
		case 4:
			*pf_ratio = 2.21 / 1.0;
			return "2.21 : 1";
		default:
			*pf_ratio = 1.0;
			return "? : ?";
	}
}

VSTRIP_DLL_API const char *vs_get_time(const double time)
{
	static char buffer[64];
	double mt = time + ((time < 0.0) ? -0.5 : 0.5);
	double amt = fabs(mt);

	if (mt > 0.0)
		sprintf(buffer, "%i:%02i:%02i.%03i", (int)(amt / 3600000.0), (int)(fmod(amt, 3600000.0) / 60000.0), (int)(fmod(amt, 60000.0) / 1000.0), (int)fmod(amt, 1000.0));
	else
		sprintf(buffer, "-%i:%02i:%02i.%03i", (int)(amt / 3600000.0), (int)(fmod(amt, 3600000.0) / 60000.0), (int)(fmod(amt, 60000.0) / 1000.0), (int)fmod(amt, 1000.0));
	return buffer;
}

static void print_streaminfo(const vstripbool print, 
							 const dword lba, 
							 const tp_vs_streamflags streams, 
							 const tp_vs_streamflags substreams,
							 const dword stream_id, 
							 const dword substream_id)
{
	if ((!streams[stream_id].found) || ((stream_id == PRIVATE_STREAM_1) && (!substreams[substream_id].found)))
	{
		if (print)
		{
			static const char vs_found_save[2][7] = {"Found", "Saving"};
			double pts = 0.0;

			if (stream_id == PRIVATE_STREAM_1)
			{
				dummyfprintf(vs_STDOUT, "%s 0x%02X = %s, sub 0x%02X", 
					vs_found_save[substreams[substream_id].save != 0x00], 
					stream_id, vs_get_stream_name(stream_id), 
					substream_id);

				substreams[substream_id].found = TRUE;

				substreams[substream_id].found_order = g_found_order;

				g_found_order++;

				if ((substreams[substream_id].save != 0x00) && (substreams[substream_id].remap_to != substream_id))
					dummyfprintf(vs_STDOUT, " -> 0x%02X", substreams[substream_id].remap_to);
				pts = substreams[substream_id].pts;
			}
			else
			{
				dummyfprintf(vs_STDOUT, "%s 0x%02X = %s", vs_found_save[streams[stream_id].save != 0x00], stream_id, vs_get_stream_name(stream_id));
				
				if ((streams[stream_id].save != 0x00) && (streams[stream_id].remap_to != stream_id))
					dummyfprintf(vs_STDOUT, " -> 0x%02X", streams[stream_id].remap_to);
				pts = streams[stream_id].pts;
			}

			dummyfprintf(vs_STDOUT, " [");
			if (pts > 0.0)
			{
				dummyfprintf(vs_STDOUT, "PTS %s ", vs_get_time(pts));
			}
			dummyfprintf(vs_STDOUT, "@LBA %u]\n", lba);
		}
		streams[stream_id].found = TRUE;
		//found_at
		
	}
}

static void vs_print_summary(const dword pack_packets, const dword system_packets, const tp_vs_streamflags streams, const tp_vs_streamflags substreams)
{
	dword i, j;

	fputs("\nSummary:\n", vs_STDOUT);
	dummyfprintf(vs_STDOUT, "MPEG Packs = %u\n", pack_packets);
	if (system_packets)
		dummyfprintf(vs_STDOUT, "System headers = %u\n", system_packets);
	for (i = 0; i < 256; i++)
	{
		if (streams[i].packets)
		{
			dummyfprintf(vs_STDOUT, "%s packets = %u, total bytes = %u\n", vs_get_stream_name(i), streams[i].packets, streams[i].bytes);
			if (i == PRIVATE_STREAM_1)
			{
				for (j = 0; j < 256; j++)
				{
					if (substreams[j].packets)
					{
						dummyfprintf(vs_STDOUT, "  Sub 0x%02X packets = %u, total bytes = %u", j, substreams[j].packets, substreams[j].bytes);
						if (streams[VIDEO_STREAM].pts != 0.0 && substreams[j].pts != 0.0 && streams[VIDEO_STREAM].pts != substreams[j].pts)
							dummyfprintf(vs_STDOUT, " (delay %s)\n", vs_get_time(substreams[j].pts - streams[VIDEO_STREAM].pts));
						else
							fputs("\n", vs_STDOUT);
					}
				}
			}
		}
	}
}

static vstripbool vs_change_vobcells(const dword vob_id, const dword cell_id, 
	const dword num_idl, const tp_vs_vobcellid idl, tp_vs_data vsd)
{ // returns save status (of this vob- and cell-id)
	vstripbool save_status = TRUE;
	dword i;

	// ---- check whether we want this one ----
	if (num_idl > 0 && idl)
	{
		for (i = 0; i < num_idl; i++)
			if ((idl[i].vob_id == vob_id && idl[i].cell_id == cell_id) || (idl[i].vob_id == vob_id && idl[i].cell_id == 0xff) || (idl[i].vob_id == 0xffff && idl[i].cell_id == cell_id))
				break;
		if (i >= num_idl)
			save_status = FALSE; // not found -> we don't want it
	}

	for (i = 0; i < vsd->num_outputs; i++)
	{
		if ((vsd->outputs[i].flags & (vso_SPLIT_VOBID | vso_SPLIT_CELLID)) != 0 && ((vsd->outputs[i].flags & vso_SPLIT_CELLID) != 0 || vsd->_in.sti.vob_id != vob_id))
		{
			if (vsd->_in.buffer->outfp[i])
			{ // close old-file
				fio_close(vsd->_in.buffer->outfp[i]);
				vsd->_in.buffer->outfp[i] = NULL;
			}

			if (save_status)
			{
				char fname[256];

				if ((vsd->outputs[i].flags & vso_SPLIT_CELLID) != 0)
				{
					//sprintf(fname, "%s_V%02uC%02u%s", vsd->_in.outfileprep[i], vob_id, cell_id, vsd->_in.outfileext[i]);
					// old cd2 : sprintf(fname, "%s%s", vsd->_in.outfileprep[i], ".ac3");
					//OK -> sprintf(fname, "%s_V%02uC%02u%s", vsd->_in.outfileprep[i], vob_id, cell_id, vsd->_in.outfileext[i]);
					//sprintf(fname, "%s_V%02uC%02u%u%s", vsd->_in.outfileprep[i], vob_id, cell_id, vsd->_in.buffer->lba, vsd->_in.outfileext[i]);

					sprintf(fname, "%s%s", vsd->_in.outfileprep[i], vsd->_in.outfileext[i]);
		
				}
				else
				{
					//sprintf(fname, "%s_V%02uC__%s", vsd->_in.outfileprep[i], vob_id, vsd->_in.outfileext[i]);
					//sprintf(fname, "%s_V%02uC__%s", vsd->_in.outfileprep[i], vob_id, ".ac3");
					sprintf(fname, "%s%s", vsd->_in.outfileprep[i], vsd->_in.outfileext[i]);
				}

				vsd->_in.buffer->outfp[i] = fio_open(fname, fio_WRITEABLE | ((((vsd->_in.opened[vob_id * MAX_CELLIDS + cell_id].opened & (1 << i)) == (1 << i)) || (vsd->outputs[i].flags & vso_APPEND) != 0) ? fio_APPEND : 0), vsd->outputs[i].split_output);
				vsd->_in.opened[vob_id * MAX_CELLIDS + cell_id].opened |= 1 << i;
			}
		} 
	}
	vsd->_in.sti.vob_id = vob_id;
	vsd->_in.sti.cell_id = cell_id;
	return save_status;
}

VSTRIP_DLL_API t_vs_errorcode vs_init(tp_vs_data vsd, t_vs_streamflags streams[256], t_vs_streamflags substreams[256])
{
	dword i;

	vsd->_in.init_ok = FALSE;
	vsd->_in.pack_packets = vsd->_in.system_packets = 0;
	vsd->_in.opened = NULL;
	vsd->_in.did_video_info = FALSE;
	vsd->_in.save_cell = (vsd->flags & vs_IGNORE_UNKNOWN_CELLS) == 0;
	//vsd->_in.save_cell = 1;

	// init some often used flags
	vsd->_in.framerate = vsd->_in.aspectratio = -1;
	if (vsd->framerate != -1)
		vsd->_in.framerate = vsd->framerate % MAX_FRAMERATE;
	if (vsd->aspectratio != -1)
		vsd->_in.aspectratio = (vsd->aspectratio % MAX_ASPECTRATIO) << 4;
	vsd->_in.did_video_info = (vsd->flags & vs_PRINT_INFO) == 0;

#ifdef vs_DECRYPT
	vsd->_in.vob_key = (vsd->flags & vs_CRACK_EACH_VOB_ID) != 0;
	vsd->_in.cell_key = (vsd->flags & vs_CRACK_EACH_CELL_ID) != 0;
	vsd->_in.valid_key = vsd->key[0] != 0 || vsd->key[1] != 0 || vsd->key[2] != 0 || vsd->key[3] != 0 || vsd->key[4] != 0;
	vs_init_css_tables(); // so we have the tables ready for the decrypting
#endif


	// init stream-info
	vsd->_in.sti.stream_id = vsd->_in.sti.substream_id = 0;
	vsd->_in.sti.cell_id = vsd->_in.sti.vob_id = -1;
	vsd->_in.sti.encrypted = FALSE;
	for (i = 0; i < 256; i++)
	{
		streams[i].packets = substreams[i].packets = 0;
		streams[i].bytes = substreams[i].bytes = 0;
		streams[i].found = substreams[i].found = FALSE;
		streams[i].pts = substreams[i].pts = 0.0;
	}
	i = 0;

	vsd->_in.buffer = inb_init(vsd->infile, i, (vstripbool)((vsd->flags & vs_NO_VOB) == 0));
	if (!vsd->_in.buffer || (vsd->_in.buffer && vsd->_in.buffer->status != inb_OK))
	{
		if (vsd->_in.buffer)
		{
			if ((vsd->flags & vs_PRINT_ERROR) != 0)

			dummyfprintf(vs_STDERR, "* Unable to read from \"%s\"\n", vsd->infile);

			inb_done(vsd->_in.buffer);
		}
		else if ((vsd->flags & vs_PRINT_ERROR) != 0)
			dummyfprintf(vs_STDERR, "* Unable to open \"%s\" for input\n", vsd->infile);
		return vse_CANT_OPEN_INPUT;
	}
	
	vsd->_in.only_gop_mask = 0xff; // assume KeepOnlyGOP is false for all
	// init output-files
	for (i = 0; i < vsd->num_outputs; i++)
		if (vsd->outputs[i].outfile[0])
		{
			tp_fio_file outfp = NULL;
			char *outman;
			char outname[256];

			if ((vsd->outputs[i].flags & vso_ONLY_KEEP_GOPS) != 0)
				vsd->_in.only_gop_mask &= ~(1 << i);
			if ((vsd->outputs[i].flags & (vso_SPLIT_CELLID | vso_SPLIT_VOBID)) == 0) // don't split by cell- or vob-ids
				outman = vsd->outputs[i].outfile;
			else
			{ // prepare the filename parts for allocation by vob-/cell-ids
				strcpy(vsd->_in.outfileprep[i], vsd->outputs[i].outfile);
				strcpy(vsd->_in.outfileext[i], ".");
				outman = strrchr(vsd->_in.outfileprep[i], '.');
				if (outman)
				{
					strcpy(vsd->_in.outfileext[i], outman);
					*outman = 0;
				}
				// cd2
				//sprintf(outname, "%s_V__C__%s", vsd->_in.outfileprep[i], vsd->_in.outfileext[i]);
				//cd old : sprintf(outname, "%s_.ac3", vsd->_in.outfileprep[i]);
				sprintf(outname, "%s%s", vsd->_in.outfileprep[i], vsd->_in.outfileext[i]);
				outman = outname;
			}
			outfp = fio_open(outman, fio_WRITEABLE | (((vsd->outputs[i].flags & vso_APPEND) != 0) ? fio_APPEND : 0), vsd->outputs[i].split_output);
			if (!outfp)
			{
				int j;

				for (j = i - 1; j >= 0; j--)
					if (vsd->_in.buffer->outfp[j])
					{
						fio_close(vsd->_in.buffer->outfp[j]);
						vsd->_in.buffer->outfp[j] = NULL;
					}
				if ((vsd->flags & vs_PRINT_ERROR) != 0)
					dummyfprintf(vs_STDERR, "* Unable to open \"%s\" for output\n", outman);
				if (vsd->_in.buffer)
					inb_done(vsd->_in.buffer);
				return vse_CANT_CREATE_OUTPUT;
			}
			else
				vsd->_in.buffer->outfp[i] = outfp;
		}
	vsd->_in.opened = calloc(MAX_VOBIDS, MAX_CELLIDS * sizeof *vsd->_in.opened);
	if (vsd->start_lba > 0)
		inb_skip(vsd->start_lba, vsd->_in.buffer);
	vsd->_in.init_ok = TRUE;
	return vse_OK;
}

static void vs_close_all(tp_vs_data vsd)
{
	if (vsd->_in.init_ok)
	{
		if (vsd->_in.buffer)
		{
			dword i;

			for (i = 0; i < vsd->num_outputs; i++)
			{
				fio_close(vsd->_in.buffer->outfp[i]);
				vsd->_in.buffer->outfp[i] = NULL;
			}
		}
		if (vsd->_in.opened)
		{
			free(vsd->_in.opened);
			vsd->_in.opened = NULL;
		}
		inb_done(vsd->_in.buffer);
		vsd->_in.buffer = NULL;
		vsd->_in.init_ok = FALSE;
	}
}

VSTRIP_DLL_API t_vs_errorcode vs_done(tp_vs_data vsd, t_vs_streamflags streams[256], t_vs_streamflags substreams[256])
{
	if (vsd->_in.init_ok)
	{
		if ((vsd->flags & vs_PRINT_SUMMARY) != 0)
			vs_print_summary(vsd->_in.pack_packets, vsd->_in.system_packets, streams, substreams);
		vs_close_all(vsd);
	}

	return vse_OK;
}

VSTRIP_DLL_API t_vs_errorcode vs_strip_one_block(tp_vs_data vsd, 
												 t_vs_streamflags streams[256], 
												 t_vs_streamflags substreams[256], 
												 dword num_idl, 
												 tp_vs_vobcellid idl, 
												 vstripbool end_or_start, 
												 VOBINFO *pr_vobinfo)
{
	dword i, j, packet_length, header_length;
	vstripbool	user_func_stay = TRUE;
	byte save_this_one = 0x00;
	double ratio = 0.0;

	if (!vsd->_in.init_ok)
		return vse_INIT_FAILED;
	if (vsd->_in.buffer->idx >= CSS_OFFSET && vsd->_in.buffer->encrypted)
		inb_fresh(vsd->_in.buffer); // skip the rest of this one because it's encrypted (only encrypted if vob)

	i = inb_get_bytes(4, vsd->_in.buffer);
	switch (i)
	{
		case PACK_START_CODE:
			vsd->_in.pack_packets++;
			inb_get_bytes(9, vsd->_in.buffer);
			j = inb_get_bytes(1, vsd->_in.buffer) & 7;
			inb_get_bytes(j, vsd->_in.buffer);
		break;
		case SYSTEM_START_CODE:
			vsd->_in.system_packets++;
			vsd->_in.buffer->save = 0x00;
			for (j = 0; j < vsd->num_outputs; j++)
				vsd->_in.buffer->save |= ((vsd->outputs[j].flags & vso_DEMUX) == 0 && vsd->_in.save_cell) << j;
			vsd->_in.buffer->has_system = TRUE;
			inb_get_bytes(8, vsd->_in.buffer);
			while ((inb_peek_byte(0, vsd->_in.buffer) & 0xf0) != 0)
				inb_get_bytes(3, vsd->_in.buffer);
		break;
		case SYSTEM_END_CODE:
		break;
		default:
			if ((i >> 8) != PACKET_START_CODE_PREFIX)
			{
				dword ctr;

				if (vsd->max_sync_bytes > 0)
				{
					i = inb_get_bytes(3, vsd->_in.buffer);
					for (ctr = 0; ctr < vsd->max_sync_bytes && i != PACKET_START_CODE_PREFIX; ctr++)
						i = ((i << 8) & 0xffffff) | inb_get_bytes(1, vsd->_in.buffer);
				}
				if (ctr >= vsd->max_sync_bytes)
				{ // didn't make it
					if ((vsd->flags & vs_PRINT_ERROR) != 0)
					{
					//	dummyfprintf(vs_STDERR, "\n* Lost sync [@LBA %u], was expecting PACKET_START_CODE_PREFIX!\n", vsd->_in.buffer->lba);
					}
					vs_close_all(vsd);
					return vse_LOST_SYNC;
				}
				else
				{
					i = ((i << 8) & 0xffffff) | inb_get_bytes(1, vsd->_in.buffer);
		//			if ((vsd->flags & vs_PRINT_ERROR) != 0)
		//				dummyfprintf(vs_STDERR, "Skipped %u bytes for resync [@LBA %u]\n", ctr, vsd->_in.buffer->lba);
				}
			}
			// we are sync'ed
			vsd->_in.sti.stream_id = i & 0x000000FF;
			vsd->_in.sti.substream_id = 0;
			if (vsd->_in.sti.stream_id == PRIVATE_STREAM_1)
			{ // let's have a peek at the substream-id
				byte header_skip;

				header_skip = inb_peek_byte(4, vsd->_in.buffer);
				vsd->_in.sti.substream_id = inb_peek_byte(5 + header_skip, vsd->_in.buffer);
				streams[vsd->_in.sti.stream_id].packets++;
				vsd->_in.cur_stream = &substreams[vsd->_in.sti.substream_id];
			}
			else
			{
				vsd->_in.cur_stream = &streams[vsd->_in.sti.stream_id];
				inb_poke_byte(-1, vsd->_in.cur_stream->remap_to, vsd->_in.buffer);
				save_this_one |= vsd->_in.cur_stream->save;
			}
			packet_length = inb_get_bytes(2, vsd->_in.buffer);
			vsd->_in.sti.lba = vsd->_in.buffer->lba;
			vsd->_in.sti.idx = vsd->_in.buffer->idx;
			vsd->_in.sti.length = packet_length;
			vsd->_in.cur_stream->packets++;
			vsd->_in.sti.encrypted = vsd->_in.buffer->encrypted;

			switch (vsd->_in.sti.stream_id)
			{
				case PRIVATE_STREAM_2:
				{
					// get the VOB- and CELL-ID tags/remove macrovision
					dword vob_id, cell_id;
					byte macro_flag;

					switch (inb_peek_byte(0, vsd->_in.buffer))
					{
					case 0: // PCI-packet (remove macrovision)
						if ((vsd->flags & vs_DEMACRO) != 0 && ((macro_flag = inb_peek_byte(5, vsd->_in.buffer)) & 0x80) != 0)
							inb_poke_byte(5, (byte)(macro_flag & (~vs_MACROVISION_BITS)), vsd->_in.buffer);
						break;
					case 1: // DSI-packet (get cell/vob ids)
						vob_id = inb_peek_bytes(25, 2, vsd->_in.buffer);
						cell_id = inb_peek_bytes(28, 1, vsd->_in.buffer);
						if (vsd->_in.sti.vob_id != vob_id || vsd->_in.sti.cell_id != cell_id)
						{
							vsd->_in.save_cell = vs_change_vobcells(vob_id, cell_id, num_idl, idl, vsd);
							if (!vsd->_in.save_cell)
								vsd->_in.buffer->save = 0x00; // save none
							else
								for (j = 0; j < vsd->num_outputs; j++)
									vsd->_in.buffer->save |= (vsd->_in.buffer->has_system && (vsd->outputs[j].flags & vso_DEMUX) == 0) << j;
						}
						break;
					}
					/*						if ((vsd->flags & vs_NO_VOB) == 0 && inb_peek_bytes(packet_length, 3, vsd->_in.buffer) != PACKET_START_CODE_PREFIX)
					inb_get_bytes((fio_SECTOR_SIZE - (vsd->_in.buffer->idx + packet_length)) & (fio_SECTOR_SIZE / 2 - 1), vsd->_in.buffer); // skip rest of this chunk in the program-stream*/
				}
				case PROGRAM_STREAM_MAP:
				case ECM_STREAM:
				case EMM_STREAM:
				case PROGRAM_STREAM_DIRECTORY:
				case DSMCC_STREAM:
				case ITUTRECH222TYPEE_STREAM:
				case PADDING_STREAM:
					vsd->_in.cur_stream->bytes += packet_length;
					//print_streaminfo((vstripbool)((vsd->flags & vs_PRINT_INFO) != 0), vsd->_in.buffer->lba, streams, substreams, vsd->_in.sti.stream_id, 0);
					if (save_this_one != 0x00 && vsd->_in.save_cell)
					{
						if (vsd->_in.cur_stream->user_func)
							user_func_stay = vsd->_in.cur_stream->user_func(&vsd->_in.buffer->bytes[vsd->_in.buffer->idx], &vsd->_in.sti, vsd->_in.cur_stream->user_data);
						for (j = 0; j < vsd->num_outputs; j++)
							if ((save_this_one & (1 << j)) != 0 && (vsd->outputs[j].flags & vso_DEMUX) != 0)
							{
								if (fio_write(&vsd->_in.buffer->bytes[vsd->_in.buffer->idx], packet_length, vsd->_in.buffer->outfp[j]) != packet_length)
									vsd->_in.buffer->status = inb_CANT_WRITE;
							}
							else
								vsd->_in.buffer->save |= save_this_one & (1 << j);
					}
					inb_get_bytes(packet_length, vsd->_in.buffer);
					/*
					//if (vsd->_in.cur_stream->pts > 0.0)
					{
						// only used for start ??
						//_fcloseall();
						//vsd->end_lba = 0xffffffff;
						//return vse_DONE; // CD2
						//streams[VIDEO_STREAM].pts
						if (pr_vobinfo->l_time_start > 0)
						{
						}
						else
						{
							//pr_vobinfo->l_time_start = (long) (vsd->_in.cur_stream->pts);
						}
					}*/
					break;
				default:
					inb_get_bytes(1, vsd->_in.buffer); // skip flags
					i = inb_get_bytes(1, vsd->_in.buffer);
					header_length = inb_get_bytes(1, vsd->_in.buffer);
					if (i >= 0x80 && vsd->_in.cur_stream->pts == 0.0)
					{ // PTS
						dword pts;

						pts = (inb_peek_byte(0, vsd->_in.buffer) & 0x0e) >> 1;
						if (pts & 0x04)
							vsd->_in.cur_stream->pts = 4294967296.0;
						else
							vsd->_in.cur_stream->pts = 0.0;
						pts = pts << 30;
						pts |= (inb_peek_bytes(1, 2, vsd->_in.buffer) & 0xfffe) << 14;
						pts |= (inb_peek_bytes(3, 2, vsd->_in.buffer) >> 1) & 0x7fff;
						vsd->_in.cur_stream->pts += (double)pts;
						vsd->_in.cur_stream->pts /= 90.0;
					}

					if (vsd->_in.buffer->lba > 0)
					{
						int rrr = 9;
					}

					inb_get_bytes(header_length + 1, vsd->_in.buffer); // +1 = skip substream_id, already got that
					print_streaminfo((vstripbool)((vsd->flags & vs_PRINT_INFO) != 0), vsd->_in.buffer->lba, streams, substreams, vsd->_in.sti.stream_id, vsd->_in.sti.substream_id);
					if (vsd->_in.cur_stream->pts > 0.0)
					{
						//*pl_milliseconds = (long) (vsd->_in.cur_stream->pts);
						if (end_or_start == 1) 
						{
							pr_vobinfo->l_time_end = (long) (vsd->_in.cur_stream->pts);
							//vsd->end_lba = vsd->start_lba + 1;
							//return vse_DONE; // CD2
						}
						else
						{
							// start
							pr_vobinfo->l_time_start = (long) (vsd->_in.cur_stream->pts);
						}
						
						// CD2
					}
					streams[vsd->_in.sti.stream_id].bytes += packet_length - header_length - 3;
					if (vsd->_in.sti.stream_id == PRIVATE_STREAM_1)
					{
						vsd->_in.cur_stream->bytes += packet_length - header_length - 3;
						inb_poke_byte(-1, vsd->_in.cur_stream->remap_to, vsd->_in.buffer);
						save_this_one |= vsd->_in.cur_stream->save;
					}
					else if ((vsd->_in.sti.stream_id >= VIDEO_STREAM) && (vsd->_in.sti.stream_id <= MAX_VIDEO_STREAM))
					{ 
						if (inb_peek_bytes(-1, 4, vsd->_in.buffer) == SEQUENCE_HEADER_CODE)
						{ // change framerate, aspectratio
							byte frameaspect;

							frameaspect = inb_peek_byte(6, vsd->_in.buffer);
							if (!vsd->_in.did_video_info)
							{
								if (end_or_start == 0)
								{
									pr_vobinfo->i_width = inb_peek_bytes(3, 3, vsd->_in.buffer) >> 12;
									pr_vobinfo->i_height = inb_peek_bytes(3, 3, vsd->_in.buffer) & 0xfff;

									//dummyfprintf(vs_STDOUT, "  Width = %u\n", inb_peek_bytes(3, 3, vsd->_in.buffer) >> 12);
									//dummyfprintf(vs_STDOUT, "  Height = %u\n", inb_peek_bytes(3, 3, vsd->_in.buffer) & 0xfff);
									
									pr_vobinfo->f_framerate = vs_get_framerate(frameaspect & 0xf);
									pr_vobinfo->s_aspect_ratio = vs_get_aspectratio((frameaspect >> 4), &ratio);
									pr_vobinfo->f_aspect_ratio = ratio;
									
									//dummyfprintf(vs_STDOUT, "  Aspect-ratio = %s\n", vs_get_aspectratio(frameaspect >> 4));
									//dummyfprintf(vs_STDOUT, "  Frame-rate = %f\n", vs_get_framerate(frameaspect & 0xf));
								}
								vsd->_in.did_video_info = TRUE;
							}
							if (vsd->_in.framerate != -1 || vsd->_in.aspectratio != -1)
							{
								if (vsd->_in.framerate != -1)
									frameaspect = (frameaspect & 0xf0) | vsd->_in.framerate;
								if (vsd->_in.aspectratio != -1)
									frameaspect = (frameaspect & 0xf) | vsd->_in.aspectratio;
								inb_poke_byte(6, frameaspect, vsd->_in.buffer);
							}
							if (vsd->_in.cur_stream->pts > 0.0)
							{
								//return vse_DONE; // CD2
							}
						}
						else
						{
							save_this_one &= vsd->_in.only_gop_mask;
						}
					}

					if (save_this_one != 0x00 && vsd->_in.save_cell)
					{
						vsd->_in.sti.idx = vsd->_in.buffer->idx - 1;
						vsd->_in.sti.length = packet_length - header_length - 3;
						vsd->_in.sti.lba = vsd->_in.buffer->lba;

						if (vsd->_in.cur_stream->user_func)
							user_func_stay = vsd->_in.cur_stream->user_func(&vsd->_in.buffer->bytes[vsd->_in.sti.idx], &vsd->_in.sti, vsd->_in.cur_stream->user_data);
						
						for (j = 0; j < vsd->num_outputs; j++)
							if ((save_this_one & (1 << j)) != 0 && (vsd->outputs[j].flags & vso_DEMUX) != 0)
							{
								dword w_idx = vsd->_in.buffer->idx - 1;
								dword w_len = packet_length - header_length - 3;

								if ((vsd->outputs[j].flags & vso_KEEP_AC3_IDENT_BYTES) == 0 && (vsd->_in.sti.stream_id == PRIVATE_STREAM_1) && (vsd->_in.sti.substream_id >= SUBSTREAM_AC3) && (vsd->_in.sti.substream_id <= MAX_SUBSTREAM_AC3))
								{ // skip the AC3 identifier bytes
									w_idx += 4;
									w_len -= 4;
								}
								else if ((vsd->outputs[j].flags & vso_KEEP_PCM_IDENT_BYTES) == 0 && (vsd->_in.sti.stream_id == PRIVATE_STREAM_1) && (vsd->_in.sti.substream_id >= SUBSTREAM_PCM) && (vsd->_in.sti.substream_id <= MAX_SUBSTREAM_PCM))
								{ // skip the PCM identifier bytes (and 3 extra???)
									w_idx += 7;
									w_len -= 7;
								}
								if (w_len > vsd->_in.buffer->num_bytes - w_idx)
									j = 0;
								if (fio_write(&vsd->_in.buffer->bytes[w_idx], w_len, vsd->_in.buffer->outfp[j]) != w_len)
									vsd->_in.buffer->status = inb_CANT_WRITE;
							}
							else
								vsd->_in.buffer->save |= save_this_one & (1 << j);
					}
					inb_get_bytes(packet_length - header_length - 4, vsd->_in.buffer);
				break;
			}
		break;
	}
	if (user_func_stay == FALSE)
	{
		vs_done(vsd, streams, substreams);
		return vse_USER_FUNC_EXIT;
	}
	if (vsd->_in.buffer->status == inb_OK && vsd->_in.buffer->lba > vsd->end_lba)
		vsd->_in.buffer->status = inb_EMPTY;
	if (vsd->_in.buffer->status == inb_OK)
		return vse_OK;
	else if (vsd->_in.buffer->status == inb_EMPTY)
	{
		vs_done(vsd, streams, substreams);
		return vse_DONE;
	}
	else
	{ // inb_CANT_WRITE
		if ((vsd->flags & vs_PRINT_ERROR) != 0)
			dummyfprintf(vs_STDERR, "Could not write to output-file(s). Maybe disk full?\n");
		vs_close_all(vsd);
		return vse_CANT_WRITE_OUTPUT;
	}
}

VSTRIP_DLL_API t_vs_errorcode vs_strip(tp_vs_data vsd, 
									   t_vs_streamflags streams[256], 
									   t_vs_streamflags substreams[256], 
									   dword num_idl, 
									   tp_vs_vobcellid idl, 
									   vstripbool end_or_start,
									   VOBINFO *pr_vobinfo)
{
	t_vs_errorcode ec = vse_INIT_FAILED;

	ec = vs_init(vsd, streams, substreams);
	if (ec == vse_OK)
		for (; ec == vse_OK; ec = vs_strip_one_block(vsd, streams, substreams, num_idl, idl, end_or_start, pr_vobinfo)) ;
	return ec; // vs_done not neccessary as we call vs_strip_one_block until it was done
}

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

				res = malloc((numchars + 2) * sizeof(*res));
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


int vstrip_getfilesize(const char * s_file, long *pl_size)
{
	int i_status = -1;
	int fp = 0;

	*pl_size = 0;
		
	fp = _open(s_file,_O_RDONLY);
		
	if (fp)
	{
		*pl_size = _lseek(fp, 0L, SEEK_END);
		
		_close(fp);
		if (*pl_size != -1L)
		{
			i_status = 0;
		}
	}
	
	return i_status;
}


int get_vob_info(char *s_inputfile, 
				 int b_search_for_audio, 
				 int b_search_for_end,
				 int b_search_for_cropping,
				 VOBINFO* pr_vobinfo,
				 char *s_tmp_folder)
{
	t_vs_data vsd = {0};
	t_vs_streamflags streams[256] = {0};
	t_vs_streamflags substreams[256] = {0};
	tp_vs_vobcellid	ifo_cellids = NULL;

	VOBINFO r_tmp = {0};

	dword ifo_num_cellids = 0;
	
	unsigned char AudioStreams[MAX_AUDIO] = {0};
	
	long l_filesize = 0;
	
	long max_search_start = 0;
	long end = 0;
	long small_end = 0;
	
	double time = 0;
	double pts = 0;
	
	int j = 0;
	int i = 0;	
	int indexed = 0;
	int end_found = 0;
	int end_try = 0;
	int __start = 32;

	g_found_order = 0; // :-)

	pr_vobinfo->i_audio_streams = 0;
	pr_vobinfo->i_video_streams = 0;

	for (i = 0 ; i < MAX_AUDIO; i++)
	{
		pr_vobinfo->audio_list[i].id = 0;
		pr_vobinfo->audio_list[i].type = AUDIO_TYPE_INVALID;
	}

	for (i = 0 ; i < MAX_STRMS; i++)
	{
		pr_vobinfo->video_list[i].id = 0;
		pr_vobinfo->video_list[i].type = STREAM_TYPE_INVALID;
	}

	if (b_search_for_audio)
	{
		max_search_start = (2048 * 4) + 1;
	}
	else
	{
		max_search_start = 512;
	}

	memset(&vsd, 0, sizeof(t_vs_data));
	
	vsd.flags = (t_vs_flags) (vs_PRINT_SUMMARY | vs_PRINT_INFO | vso_DEMUX);
	vsd.max_sync_bytes = 2048; 
	vsd.aspectratio = vsd.framerate = -1;
	vsd.outputs[0].flags = 0;
	vsd.outputs[0].split_output = 0;
	vsd.num_outputs = 0;
	vsd.start_lba = 0;
	vsd.end_lba = max_search_start;
			
	for (i = 0; i < 256; i++)
	{
		streams[i].remap_to = i;
		streams[i].user_func = NULL;
		streams[i].save = 0x00;
		substreams[i].remap_to = i;
		substreams[i].user_func = NULL;
		substreams[i].save = 0x00;
	}
	
	vsd.infile = parse_listfile(s_inputfile);

	// Get Start info + start audio info:
	vs_strip(&vsd, streams, substreams, ifo_num_cellids, ifo_cellids, 0, pr_vobinfo);
	//pr_vobinfo->l_time_start = (long) streams[VIDEO_STREAM].pts;

	// check streams
	for (i = 0; i < 256; i++)
	{
		//fprintf(fp, "i*=%d\n", i);
		streams[i].remap_to = i;
		streams[i].user_func = NULL;
		streams[i].save = 0x00;
		substreams[i].remap_to = i;
		substreams[i].user_func = NULL;
		substreams[i].save = 0x00;
		if (streams[i].found)
		{
			if (i == VIDEO_STREAM)
			{
				pr_vobinfo->video_list[0].id = i;
				pr_vobinfo->video_list[0].type = STREAM_TYPE_MPEG2_VIDEO;
				pr_vobinfo->i_video_streams = 1;
			}

			if ((i >= SUBSTREAM_AC3) && (i <= MAX_AUDIO_STREAM))
			{
				substreams[i].found = 1;
			}
		}
		
		if (substreams[i].found)
		{
//			fprintf(fp, "found=%d\n", i);

			if (b_search_for_audio)
			{
				AUDIO_TYPE audio_type = AUDIO_TYPE_INVALID;

				//fprintf(fp, "sear=%d\n", i);
				if ((i >= SUBSTREAM_AC3) && (i <= MAX_SUBSTREAM_AC3))
				{
					audio_type = AUDIO_TYPE_AC3;
				}

				if ((i >= SUBSTREAM_PCM) && (i <= MAX_SUBSTREAM_PCM))
				{
					audio_type = AUDIO_TYPE_LPCM;
				}

				if ((i >= AUDIO_STREAM) && (i <= MAX_AUDIO_STREAM))
				{
					audio_type = AUDIO_TYPE_MP2;
				}

				if (audio_type != AUDIO_TYPE_INVALID)
				{
					AudioStreams[j] = i;
					pr_vobinfo->audio_list[j].id = i;
					pr_vobinfo->audio_list[j].type = audio_type;
					j++;
				}
			}
		}
	}

	vs_done(&vsd, streams, substreams);

	if (j == 0)
	{
		// try alternate
		/*
		VOBINFO r_tsinfo = {0};

		get_ts_info(s_inputfile, 
				0,
				0,
				&r_tsinfo);*/
	}
	else
	{
		pr_vobinfo->i_audio_streams = j;
	}

	if (b_search_for_audio)
	{
		vsd.outputs[0].flags = (t_vs_flags) (vso_SPLIT_VOBID | vso_SPLIT_CELLID | vso_KEEP_AC3_IDENT_BYTES | vso_DEMUX);
		//vsd.outputs[0].flags = (t_vs_flags) (vso_KEEP_AC3_IDENT_BYTES | vso_KEEP_PCM_IDENT_BYTES | vso_DEMUX);
		vsd.outputs[0].split_output = 0;

		vsd.num_outputs = 1;
		vsd.start_lba = 1;
		vsd.end_lba = max_search_start;

		//if ((vsd->outputs[i].flags & (vso_SPLIT_VOBID | vso_SPLIT_CELLID)) != 0 && ((vsd->outputs[i].flags & vso_SPLIT_CELLID) != 0 || vsd->_in.sti.vob_id != vob_id))
		
		for (i = 0 ; i < pr_vobinfo->i_audio_streams ; i++)
		{
			if (AudioStreams[i])
			{
				sprintf(vsd.outputs[0].outfile, "%s_tmp_%d.audio", s_tmp_folder, i);
				
				if ((pr_vobinfo->audio_list[i].type == AUDIO_TYPE_AC3) ||
					(pr_vobinfo->audio_list[i].type == AUDIO_TYPE_LPCM))
				{
					substreams[AudioStreams[i]].save = 0x01;
				}
				else
				{
					// mp2
					streams[AudioStreams[i]].save = 0x01;
				}
				
				vs_strip(&vsd, streams, substreams, ifo_num_cellids, ifo_cellids, 0, &r_tmp);

				if ((pr_vobinfo->audio_list[i].type == AUDIO_TYPE_AC3) ||
					(pr_vobinfo->audio_list[i].type == AUDIO_TYPE_LPCM))
				{
					time = substreams[AudioStreams[i]].pts - streams[VIDEO_STREAM].pts;
				}
				else
				{
					// mp2
					time = streams[AudioStreams[i]].pts - streams[VIDEO_STREAM].pts;
				}

				pr_vobinfo->audio_delay[i] = time;

				streams[AudioStreams[i]].save = 0x00;
				substreams[AudioStreams[i]].save = 0x00;
			}
		}
		
		for (i = 0; i < 256; i++)
		{
			streams[i].remap_to = i;
			streams[i].user_func = NULL;
			streams[i].save = 0x00;
			substreams[i].remap_to = i;
			substreams[i].user_func = NULL;
			substreams[i].save = 0x00;
		}

		vs_done(&vsd, streams, substreams);
	}
	

	// end info ::::
	if (b_search_for_end)
	{
		vstrip_getfilesize(s_inputfile, &l_filesize);

		for (i = 0; i < 256; i++)
		{
			streams[i].remap_to = i;
			streams[i].user_func = NULL;
			streams[i].save = 0x00;
			substreams[i].remap_to = i;
			substreams[i].user_func = NULL;
			substreams[i].save = 0x00;
		}

		pr_vobinfo->l_filesize = (long) (l_filesize / 1024);
		vsd.end_lba = (int) ( (l_filesize / 2048) - 1);

		do
		{
			vsd.outputs[0].flags = (t_vs_outputflags) (0);
			//vsd.outputs[0].flags = (t_vs_flags) (vs_PRINT_SUMMARY | vs_PRINT_INFO | vso_DEMUX);
			vsd.num_outputs = 0;

			vsd.start_lba = (int) ((l_filesize / 2048) - (64 + 32 * end_try));

			vs_strip(&vsd, streams, substreams, ifo_num_cellids, ifo_cellids, 1, &r_tmp);
			
			for (i = 0; i < 256; i++)
			{
				streams[i].remap_to = i;
				streams[i].user_func = NULL;
				streams[i].save = 0x00;
				substreams[i].remap_to = i;
				substreams[i].user_func = NULL;
				substreams[i].save = 0x00;
				
				if ((streams[i].found > 0) && (streams[i].pts > 0))
				{
					pts = streams[i].pts;
					end_found = 1;
					break;
				}

				if ((substreams[i].found > 0) && (substreams[i].pts > 0))
				{
					pts = substreams[i].pts;
					end_found = 1;
					break;
				}

				streams[i].pts = 0;
				streams[i].found = 0;
				substreams[i].found = 0;
				substreams[i].pts = 0;
			}

			end_try++;
			//__start *= 2;
			vs_done(&vsd, streams, substreams);

		} while ((end_found != 1) && (end_try < 32));

	//	vs_done(&vsd, streams, substreams);

		pr_vobinfo->l_time_end = (long) pts;

		// CLEAN UP
		vs_done(&vsd, streams, substreams);
	}
	
	if (b_search_for_cropping)
	{
		(void) mpeg2dec_lib_get_info(s_inputfile, pr_vobinfo);
	}
	
	free(vsd.infile);

	return 0;
}

VSTRIP_DLL_API void dummyfprintf(FILE *fp, const char *in, ...)
{

}

int parse_movie_ifo(char *s_ifofile, MOVIE_INFO *pr_movie_info)
{
	t_ifo_flags fl = ifo_PRINT_INFO;// | print_cell_pos;
	dword ifo_num_cellids = 0;
	tp_vs_vobcellid	ifo_cellids = NULL;
	
	int ret = ifoParse(s_ifofile, 
		fl, 
		&ifo_cellids, 
		pr_movie_info);
	
	return ret;
}

VSTRIP_DLL_API int auto_calc_size(int i_new_width, 
				 int i_original_height, 
				 int i_original_width,
				 int i_cropping_height,
				 float f_aspect_ratio,
				 int *pi_cx, 
				 int *pi_cy, 
				 int *pi_dx, 
				 int *pi_dy)
{
	float f_special = 1.0;
	int i_extra = 0;

	i_extra = (int) ((16 * (i_new_width / 720.0)) + 1);
	i_extra = (i_extra / 8 ) * 8;

	i_new_width += i_extra;

	if (i_original_height == 480)
	{
		if ((i_original_width > 640) && (fabs(f_aspect_ratio - (4.0 / 3.0)) < 0.01))
		{
			i_original_width = 640 + i_extra;
		}
	}
		
	*pi_cx = (int) i_new_width - i_extra;
	*pi_dx = (int) i_new_width;

	if (i_cropping_height < (int) (i_original_height * 0.80))
	{
		*pi_dy = (int) ((i_new_width / f_aspect_ratio) * f_special);

		if (fabs(f_aspect_ratio - (16.0 / 9.0)) < 0.01)
		{
			*pi_cy = (int) ((i_new_width / 2.35) * 1.0);
		}
		else if ((fabs(f_aspect_ratio - (4.0 / 3.0)) < 0.01))
		{
			*pi_cy = (int) ((i_new_width / (16.0/9.0)) * 1.0);
		}
		else
		{
			// ?
		}
	}
	else
	{
		*pi_cy = (int) ((i_new_width / f_aspect_ratio) * f_special) - i_extra;
		*pi_dy = (int) ((i_new_width / f_aspect_ratio) * 1.0);
	}

	*pi_dy = ((*pi_dy) >> 4) << 4;
	*pi_cy = ((*pi_cy) >> 4) << 4;

	return 0;
}

#define VERSION "0.0.0.41"

char * get_vobinfo_version()
{
	return VERSION;
}