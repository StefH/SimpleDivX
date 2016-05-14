/*************************************************************************
	vStrip by [maven] (maven@maven.de)
	in_buffer.c: routines for input-buffer management (inb_ prefix)
							 (tabsize 2)
*************************************************************************/

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "s_types.h"
#include "file_io.h"
#include "in_buffer.h"

// flush the buffer and reset save-state
vstripbool inb_flush(tp_inb_buffer bb)
{
	vstripbool success = TRUE;
	dword i;

	for (i = 0; i < inb_MAX_OUTPUT; i++)
		if (bb->outfp[i] && ((bb->save & (1 << i)) != 0))
			if (fio_write(bb->bytes, bb->num_bytes, bb->outfp[i]) != bb->num_bytes)
				success = FALSE;
  bb->save = 0x00;
 	return success;
}

// gets the next block and writes the old data if neccessary
void inb_fresh(tp_inb_buffer bb)
{
	if (bb->save != 0x00 && !inb_flush(bb))
		bb->status = inb_CANT_WRITE;
	if (bb->status == inb_OK)
	{
		bb->encrypted = FALSE;
		bb->status = (bb->num_bytes < bb->buffer_size) ? inb_EMPTY : inb_OK; // are we empty?
	 	if (bb->status == inb_OK)
	  { // if not already empty, get the next block
	  	bb->lba++;
			if (bb->is_vob)
	 			bb->bytes = fio_get_block(bb->buffer_size, &bb->num_bytes, bb->infp);
			else
			{ // big hack to make block contiguous in mem (copying the right data "behind" the actual buffer, so it can be used/demuxed)
				byte *chunk = fio_get_block(bb->buffer_size, &bb->num_bytes, bb->infp);

				if (bb->even)
					memcpy(bb->bytes, bb->infp->file_buffer, fio_BUFFER_SIZE); // as we've read a full buffer
				else
				{
					memcpy(bb->bytes, chunk, bb->buffer_size);
					fio_refill(bb->infp); // get next block
					memcpy(bb->bytes + bb->buffer_size, bb->infp->file_buffer, bb->buffer_size);
				}

				bb->even = !bb->even;
			}
	 		if (bb->num_bytes == 0)
	 			bb->status = inb_EMPTY;
	 		else if (bb->is_vob && bb->num_bytes > 20)
		 		bb->encrypted = (bb->bytes[20] & inb_ENCRYPTED_MASK) == inb_ENCRYPTED_VALUE;
		 	bb->has_system = FALSE;
		 	bb->idx = 0;
		}
	}
}

// init an input-buffer with name as input-file, is_vob
tp_inb_buffer inb_init(char *name, dword fio_flags, vstripbool vob)
{
	tp_inb_buffer bb = NULL;
	
	if (name && *name)
	{
		int i;

		bb = malloc(sizeof *bb);
		bb->infp = fio_open(name, fio_flags  & ~(fio_APPEND | fio_WRITEABLE), 0);
		if (!bb->infp)
		{
  		free(bb);
  		return NULL;
		}
		for (i = 0; i < inb_MAX_OUTPUT; i++)
			bb->outfp[i] = NULL;
		bb->encrypted = FALSE;
		bb->save = 0x00;
		bb->status = inb_OK;
		bb->even = TRUE;
		bb->is_vob = vob;
		if (vob)
			bb->buffer_size = fio_SECTOR_SIZE;
		else
		{
			bb->buffer_size = fio_BUFFER_SIZE / 2;
			bb->bytes = malloc(bb->buffer_size * 2); // big overlap hack
		}
		bb->num_bytes = bb->buffer_size;
		inb_fresh(bb); // sets bb->status if empty
		bb->lba = 0; // was increased by inb_fresh
	}
  return bb;
}

// closes and input-buffer (flushes output-file and closes input-file)
void inb_done(tp_inb_buffer bb)
{
	if (bb)
	{
		if (bb->save != 0x00 && !inb_flush(bb))
			bb->status = inb_CANT_WRITE;
	  fio_close(bb->infp);
		free(bb);
	}
}

// gets num_bytes from the buffer and returns them if num_bytes <= 4
dword inb_get_bytes(dword num_bytes, tp_inb_buffer bb)
{
	dword out = 0;

	if (num_bytes <= 4)
	{
		dword i, j;

		j = min(num_bytes, bb->num_bytes - bb->idx);
		for (i = 0; i < j; i++)
		{
			out = (out << 8) + bb->bytes[bb->idx++];
			num_bytes--;
		}
		if (bb->idx >= bb->num_bytes)
		{
			inb_fresh(bb);
			if (num_bytes > 0)
				for (i = 0; i < num_bytes; i++)
					out = (out << 8) + bb->bytes[bb->idx++];
		}
	}
	else
	{
		bb->idx += num_bytes;
		while (bb->status == inb_OK && bb->idx >= bb->num_bytes)
		{
			dword ofs = bb->idx - bb->num_bytes;

			inb_fresh(bb);// sets idx to 0
			bb->idx = ofs;
		}
	}
	return out;
}

// writes one byte to the buffer at offset
vstripbool inb_poke_byte(int offset, byte b, tp_inb_buffer bb)
{
	int newofs = (int)bb->idx + offset;
	if ((newofs >= 0) && ((dword)newofs < bb->num_bytes))
	{
		bb->bytes[newofs] = b;
		return TRUE;
	}
	return FALSE;
}

// gets one bytes from the buffer at offset
byte inb_peek_byte(int offset, tp_inb_buffer bb)
{
	byte b = 0;
	int newofs = (int)bb->idx + offset;

	if ((newofs >= 0) && ((dword)newofs < bb->num_bytes))
		b = bb->bytes[newofs];
	return b;
}

// gets one or more bytes from the buffer at offset (doesn't make sense with num_bytes > 4)
dword inb_peek_bytes(int offset, dword num_bytes, tp_inb_buffer bb)
{
	dword out;
	int newofs = (int)bb->idx + offset;

	out = 0;
	if ((newofs >= 0) && ((dword)(newofs + num_bytes) <= bb->num_bytes))
	{
		dword i;

		for (i = newofs; i < newofs + num_bytes; i++)
			out = (out << 8) + bb->bytes[i];
	}
	return out;
}

// skip a certain number of sectors in the input-buffer
vstripbool inb_skip(dword secs, tp_inb_buffer bb)
{
	if (bb && bb->infp && secs > 0)
	{
		if (bb->save != 0x00 && !inb_flush(bb))
			bb->status = inb_CANT_WRITE;
		if (bb->status == inb_OK)
		{
			if (fio_skip(bb->infp, secs))
			{
				bb->lba += secs - 1;
				inb_fresh(bb);
			}
			else
				bb->status = inb_EMPTY;
		}
	}
	return bb->status == inb_OK;
}