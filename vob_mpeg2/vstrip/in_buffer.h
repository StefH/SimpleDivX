#ifndef _IN_BUFFER_H_INCLUDED

/*************************************************************************
  vStrip by [maven] (maven@maven.de)
*************************************************************************/

#define _IN_BUFFER_H_INCLUDED

#include "s_types.h"
#include "file_io.h"

// was 0x30
#define inb_ENCRYPTED_MASK 0x30
#define inb_ENCRYPTED_VALUE 0x10
#define inb_MAX_OUTPUT 8

typedef enum
{
	inb_OK, inb_EMPTY, inb_CANT_WRITE
} t_inb_status;

typedef struct
{
	tp_fio_file		infp, outfp[inb_MAX_OUTPUT];
	byte					*bytes;
	dword					idx, num_bytes, buffer_size, lba;
	vstripbool					encrypted, has_system, is_vob, even;
	byte					save;
	t_inb_status	status;
} t_inb_buffer, *tp_inb_buffer;

// gets the next block and writes the old data if neccessary
void inb_fresh(tp_inb_buffer bb);
// init an input-buffer with name as input-file and out (can be NULL) as outfile
tp_inb_buffer inb_init(char *name, dword fio_flags, vstripbool vob);
// closes and input-buffer (flushes output-file and closes input-file)
void inb_done(tp_inb_buffer bb);
// gets num_bytes from the buffer and returns them if num_bytes <= 4
dword inb_get_bytes(dword num_bytes, tp_inb_buffer bb);
// writes one byte to the buffer at offset
vstripbool inb_poke_byte(int offset, byte b, tp_inb_buffer bb);
// gets one bytes from the buffer at offset
byte inb_peek_byte(int offset, tp_inb_buffer bb);
// gets one or more bytes from the buffer at offset (doesn't make sense with num_bytes > 4)
dword inb_peek_bytes(int offset, dword num_bytes, tp_inb_buffer bb);
// skip a certain number of sectors in the input-buffer
vstripbool inb_skip(dword secs, tp_inb_buffer bb);

#endif