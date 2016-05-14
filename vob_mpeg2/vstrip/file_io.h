#ifndef _FILE_IO_H_INCLUDED

/*************************************************************************
  vStrip by [maven] (maven@maven.de)
*************************************************************************/

#define _FILE_IO_H_INCLUDED

#include <stdio.h>
#include "s_types.h"
//#include "aspi.h"

#define fio_BUFFER_SIZE 65536
// DVD-Sector Size
#define fio_SECTOR_SIZE 2048
// Maximum Length of a Filename
#define fio_MAX_FNAME 256

#ifndef _MSC_VER

#define _open open
#define _close close
#define _read read
#define _write write
#define _lseek lseek

#ifndef _S_IREAD
#define _S_IREAD      S_IREAD
#endif
#ifndef _S_IWRITE
#define _S_IWRITE     S_IWRITE
#endif
#ifndef _O_RDONLY
#define _O_RDONLY     O_RDONLY
#endif
#ifndef _O_WRONLY
#define _O_WRONLY     O_WRONLY
#endif
#ifndef _O_CREAT
#define _O_CREAT      O_CREAT
#endif
#ifndef _O_APPEND
#define _O_APPEND     O_APPEND
#endif
#ifndef _O_TRUNC
#define _O_TRUNC      O_TRUNC
#endif
#ifndef _O_SEQUENTIAL
#define _O_SEQUENTIAL (0)
#endif
#ifndef _O_BINARY
#define _O_BINARY     (0)
#endif

#endif

typedef enum
{
	fio_APPEND = 0x01, fio_WRITEABLE = 0x02, fio_USE_ASPI = 0x04,
	fio_PREFER_ASPI = 0x08, fio_SUPPORT_1GB = 0x10, fio_FORCE_DWORD = -1
} t_fio_flags;

typedef struct
{
	dword	address;
	dword	start_lba, end_lba;
	dword	sec_size;
	dword	cur_lba;
#ifdef aspi_USE_ASPI
	t_aspi_async async_read;
#endif
} t_fio_udf_file, *tp_fio_udf_file;

typedef struct
{
	// ---- public ----
	vstripbool	end_of_file;
	// ---- private ----
	char	fname[fio_MAX_FNAME], real_name[fio_MAX_FNAME]; // realname saves the original filename (fname is changed by output-file-splitting)
	byte	file_buffer[fio_BUFFER_SIZE];
	dword	buffer_idx, buffer_left, size, num_buffers, split_size, split_idx;
	tp_fio_udf_file real_file;
	int		normal_file;
	int		list_num, list_idx;
	char	*list_names;
	t_fio_flags flags;
	vstripbool	written_to;
} t_fio_file, *tp_fio_file;

// void fio_open_for_writing(tp_fio_file f, const char *name);
tp_fio_file fio_open(const char *name, const t_fio_flags flags, const dword split_size);
vstripbool fio_refill(tp_fio_file f);
long fio_direct_lseek(const tp_fio_file f, long offset, int origin);
int fio_direct_read(const tp_fio_file f, void *buffer, unsigned int count);
void *fio_get_block(dword block_size, dword *num_bytes, tp_fio_file f);
dword fio_write(void *buf, dword num_bytes, tp_fio_file f);
void fio_close(tp_fio_file f);
vstripbool fio_skip(const tp_fio_file f, const dword blocks);

#endif