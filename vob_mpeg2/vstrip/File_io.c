/*************************************************************************
  vStrip by [maven] (maven@maven.de)
	file_io.c: routines for buffered io-managament and listfile-support
						 (tabsize 2)
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "s_types.h"
#include "file_io.h"

void fio_open_for_writing(tp_fio_file f, const char *name)
{
	strcpy(f->fname, name);
	f->num_buffers = 0;
	if ((f->flags & fio_APPEND) != 0)
	{
		int fp;

		f->written_to = TRUE;
		fp = _open(name, _O_BINARY | _O_RDONLY); // check size
		if (fp != -1)
		{
			long int fposition;

			fposition = _lseek(fp, 0, SEEK_END);
			f->written_to = fposition > 0;
			if (fposition > 0)
				f->num_buffers = fposition / fio_BUFFER_SIZE;
			_close(fp);
		}
		f->normal_file = _open(name, _O_BINARY | _O_WRONLY | _O_CREAT | _O_SEQUENTIAL | _O_APPEND, _S_IREAD | _S_IWRITE);
	}
	else
	{
		f->normal_file = _open(f->fname, _O_BINARY | _O_WRONLY | _O_CREAT | _O_SEQUENTIAL | _O_TRUNC, _S_IREAD | _S_IWRITE);
		
		//f->normal_file = open(cs, O_BINARY | O_WRONLY | O_CREAT, 0);
		f->written_to = FALSE;
	}
}

vstripbool fio_flush(tp_fio_file f)
{
	if (f && (f->flags & fio_WRITEABLE) != 0 && (f->buffer_idx > 0))
	{
		vstripbool  success;

		f->num_buffers += f->buffer_idx / fio_BUFFER_SIZE; // only increases on a full buffer
		if (f->split_size > 0 && f->num_buffers > f->split_size)
		{
			char name[256], ext[256] = "", buf[256];
			char *dot;

			f->split_idx++;
			strcpy(name, f->real_name);
			dot = strrchr(name, '.');
			if (dot)
			{
				*dot = 0;
				strcpy(ext, dot + 1);
			}
			sprintf(buf, "%s_%u.%s", name, f->split_idx, ext);
			if (f->normal_file != -1)
			{
				_close(f->normal_file);
				f->normal_file = -1;
			}
			fio_open_for_writing(f, buf);
			f->num_buffers++; // because the upcoming write is going in there
		}
		success = (_write(f->normal_file, f->file_buffer, f->buffer_idx) == (int)f->buffer_idx);
		f->buffer_left = sizeof f->file_buffer;
		f->buffer_idx = 0;
		return success;
	}
	else
		return FALSE;
}

static vstripbool fio_read_open_try_aspi(const char *name, tp_fio_file f)
{
	vstripbool success = FALSE;
#ifdef aspi_USE_ASPI
	dword address;

	if (aspi_GetDriveAddress(name[0], &address))
	{
		tp_udf_file ff = udf_find_file(address, 0, (vstripbool)((f->flags & fio_SUPPORT_1GB) != 0), name + 2); // skip drive letter and ':'

		if (ff)
		{
			dword start_lba, end_lba;

			if (udf_get_lba(address, ff, &start_lba, &end_lba))
			{
				tp_fio_udf_file fuf = malloc(sizeof *fuf);

				// maybe set timeout?
				fuf->address = address;
				fuf->cur_lba = fuf->start_lba = start_lba;
				fuf->end_lba = end_lba;
				fuf->sec_size = ff->sec_size;

				fuf->async_read.max_lba = fuf->end_lba;
				fuf->async_read.handle = NULL;

				f->real_file = fuf;
				f->size = (fuf->end_lba - fuf->start_lba + 1) * fuf->sec_size;
				success = TRUE;
			}
			udf_free(ff);
		}
	}
#endif
	return success;
}

static vstripbool fio_read_open(const char *name, tp_fio_file f)
{ // opens file read-only according to flags and sets file-size
	vstripbool success = FALSE;

	f->real_file = NULL;
	f->normal_file = -1;
	if ((f->flags & fio_PREFER_ASPI) != 0 && (f->flags & fio_USE_ASPI) != 0)
		success = fio_read_open_try_aspi(name, f);

	if (!success)
	{
		int fp = _open(name, _O_BINARY | _O_RDONLY | _O_SEQUENTIAL);

		if (fp != -1)
		{
			success = TRUE;
			f->normal_file = fp;
			f->size = _lseek(fp, 0, SEEK_END);
			_lseek(fp, 0, SEEK_SET);
		}
	}

	if (!success && (f->flags & fio_PREFER_ASPI) == 0 && (f->flags & fio_USE_ASPI) != 0)
		success = fio_read_open_try_aspi(name, f);
	return success;
}

long fio_direct_lseek(const tp_fio_file f, long offset, int origin)
{
	if (f->normal_file != -1)
		return _lseek(f->normal_file, offset, origin);
#ifdef aspi_USE_ASPI
	else if (f->real_file)
	{
		if (origin == SEEK_SET)
		{
			f->real_file->cur_lba = f->real_file->start_lba + offset / f->real_file->sec_size;
			return (f->real_file->cur_lba - f->real_file->start_lba) * f->real_file->sec_size;
		}
		else if (origin == SEEK_CUR)
		{
			f->real_file->cur_lba += offset / f->real_file->sec_size;
			return (f->real_file->cur_lba - f->real_file->start_lba) * f->real_file->sec_size;
		}
		else if (origin == SEEK_END)
		{
			f->real_file->cur_lba = f->real_file->end_lba + 1 + offset / f->real_file->sec_size;
			return (f->real_file->cur_lba - f->real_file->start_lba) * f->real_file->sec_size;
		}
	}
#endif
	return -1;
}

int fio_direct_read(const tp_fio_file f, void *buffer, unsigned int count)
{
	if (f->normal_file != -1)
		return _read(f->normal_file, buffer, count);
#ifdef aspi_USE_ASPI
	else if (f->real_file)
	{
		if (f->real_file->cur_lba > f->real_file->end_lba)
			return 0;
		else 
		{
			dword max_sec = min(f->real_file->end_lba - f->real_file->cur_lba + 1, count / f->real_file->sec_size);
			dword res = aspi_ReadSectors(f->real_file->address, f->real_file->cur_lba, max_sec, f->real_file->sec_size, buffer, &f->real_file->async_read);

			f->real_file->cur_lba += max_sec;

			if (res == SS_COMP)
				return max_sec * f->real_file->sec_size;
		}
	}
#endif
	return -1;
}

vstripbool fio_refill(tp_fio_file f)
{
	if (f && (f->flags & fio_WRITEABLE) == 0)
	{
		if ((f->normal_file == -1 && !f->real_file) && f->list_idx < f->list_num)
		{
			for (; f->list_idx < f->list_num; f->list_idx++)
			{ // try until we can open one
				if (fio_read_open(f->list_names + f->list_idx * fio_MAX_FNAME, f))
				{
					f->list_idx++;
					f->buffer_left = sizeof f->file_buffer;
					break;
				}
			}
		}
		if ((f->normal_file != -1 || f->real_file) && f->buffer_left == fio_BUFFER_SIZE)
		{
			f->buffer_left = fio_direct_read(f, f->file_buffer, fio_BUFFER_SIZE);
			f->buffer_idx = 0;
			f->end_of_file = (f->buffer_left <= 0);
		}
		else
			f->end_of_file = TRUE;
		if (f->end_of_file && f->list_idx < f->list_num)
		{
			if (f->normal_file != -1)
			{
				_close(f->normal_file);
				f->normal_file = -1;
			}
			if (f->real_file)
			{
				free(f->real_file);
				f->real_file = NULL;
			}
			fio_refill(f);
		}
		return TRUE;
	}
	else
		return FALSE;
}

vstripbool fio_skip(const tp_fio_file f, const dword blocks)
{
	dword secs = blocks;

	while (secs > 0 && !f->end_of_file)
	{
		dword left;

		left = (f->size - (fio_direct_lseek(f, 0, SEEK_CUR) - f->buffer_left)) / fio_SECTOR_SIZE;
		if (secs >=	left)
		{ // try go to the next file
			if (f->list_idx < f->list_num)
			{
				if (f->normal_file != -1)
				{
					_close(f->normal_file);
					f->normal_file = -1;
				}
				if (f->real_file)
				{
					free(f->real_file);
					f->real_file = NULL;
				}
				fio_refill(f);
			}
			else
			{
				fio_direct_lseek(f, 0, SEEK_END);
				f->end_of_file = TRUE;
			}
			secs -= left;
		}
		else if (secs >= fio_BUFFER_SIZE / fio_SECTOR_SIZE)
		{
			fio_direct_lseek(f, secs * fio_SECTOR_SIZE - f->buffer_left, SEEK_CUR);
			fio_refill(f); // and into the buffer
			secs = 0;
		}
		else
		{
			dword num_read = fio_SECTOR_SIZE;

			while (secs > 0 && !f->end_of_file)
			{
				fio_get_block(fio_SECTOR_SIZE, &num_read, f);
				secs--;
			}
		}
	}
	return secs == 0;
}

tp_fio_file fio_open(const char *name, const t_fio_flags flags, const dword split_size)
{
	tp_fio_file f = NULL;

	f = malloc(sizeof *f);
	if (f)
	{
		strcpy(f->real_name, name);
		f->real_file = NULL;
		f->normal_file = -1;
		f->end_of_file = FALSE;
		f->flags = flags;
		f->buffer_idx = 0;
		f->buffer_left = sizeof f->file_buffer;
		f->list_num = -1;
		f->list_idx = 0;
		f->list_names = NULL;
		if ((f->flags & fio_WRITEABLE) != 0)
		{
			f->size = 0;
			f->num_buffers = 0;
			f->split_size = split_size;
			f->split_idx = 0;
			fio_open_for_writing(f, name);
		}
		else
		{
			if ((f->flags & fio_USE_ASPI) == 0)
				f->flags &= ~fio_PREFER_ASPI; 
			if (name[0] == ':')
			{ // parse & copy filenames
				const char *cn = name + 1;
				int i, num;

				for (num = 0; *cn; cn += strlen(cn) + 1)
					num++;
				if (num > 0)
				{
					char *dest;

					f->list_num = num;
					f->list_names = dest = malloc(f->list_num * fio_MAX_FNAME * sizeof *f->list_names);
					for (i = 0, cn = name + 1; *cn; cn += strlen(cn) + 1)
					{
						strcpy(dest, cn);
						dest += fio_MAX_FNAME;
					}
				}
			}
			else
				fio_read_open(name, f);
		}
		if ((f->normal_file == -1 && f->real_file == NULL) && f->list_idx >= f->list_num)
		{
			free(f);
			return NULL;
		}
		if ((f->flags & fio_WRITEABLE) != 0)
			f->end_of_file = TRUE;
		else
			f->buffer_idx = f->buffer_left;
	}
	return f;
}

// block-size must be aligned and a divisor of fio_BUFFER_SIZE
void *fio_get_block(dword block_size, dword *num_bytes, tp_fio_file f)
{
	void *block = NULL;
	dword num = 0;

	if (f->buffer_idx >= f->buffer_left)
		fio_refill(f);
	if (block_size <= (f->buffer_left - f->buffer_idx))
	{
		block = &f->file_buffer[f->buffer_idx];
		num = block_size;
		f->buffer_idx += num;
	}
	else
	{
		block = &f->file_buffer[f->buffer_idx];
		num = f->buffer_left - f->buffer_idx;
		f->buffer_idx += num;
		f->end_of_file = TRUE;
	}
	*num_bytes = num;
	return block;
}

dword fio_write(void *buf, dword num_bytes, tp_fio_file f)
{
	dword num_written = 0;

	if (buf && (num_bytes > 0) && f && (f->flags & fio_WRITEABLE) != 0)
	{
		dword num_copy_left = num_bytes;
		byte *bb = (byte *)buf;

		f->written_to = TRUE;
		while ((num_copy_left > 0))
		{
			dword num_copy;

			num_copy = (num_copy_left <= f->buffer_left) ? num_copy_left : f->buffer_left;
			memcpy(&f->file_buffer[f->buffer_idx], bb, num_copy);
			bb += num_copy;
			f->buffer_idx += num_copy;
			f->buffer_left -= num_copy;
			num_written += num_copy;
			num_copy_left -= num_copy;
			if (f->buffer_left <= 0)
			{
				if (!fio_flush(f))
				{
					num_written = 0;
					break;
				}
			}
		}
	}
	return num_written;
}

void fio_close(tp_fio_file f)
{
	if (f)
	{
		if ((f->flags & fio_WRITEABLE) != 0)
		{
			fio_flush(f);
			_close(f->normal_file);
			f->normal_file = -1;
			if (!f->written_to)
				remove(f->fname);
		}
		else 
		{
			if (f->normal_file != -1)
				_close(f->normal_file);
			if (f->real_file)
				free(f->real_file);
		}
		if (f->list_names)
			free(f->list_names);
		free(f);
	}
}
