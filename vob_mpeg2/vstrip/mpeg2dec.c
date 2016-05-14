//
//
//

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "inttypes.h"

#include "vstrip.h"
#include "mpeg2.h"
#include "video_out.h"
#include "convert.h"


#define BUFFER_SIZE 4096
static uint8_t buffer[BUFFER_SIZE];
static FILE * in_file;
static int demux_track = 0;
static int demux_pid = 0;
static int demux_pva = 0;
static mpeg2dec_t * mpeg2dec;
static vo_open_t * output_open = NULL;
static vo_instance_t * output;

int fnd = 0;
int framenum = 0;
int crop_h = 0;

#define BUFFER_SIZE 4096
#define MAX_CHECK_POINTS 5
#define MAX_LONG 4294967296
#define MAX_CHECK_FRAMES 10

/*
#ifdef HAVE_GETTIMEOFDAY

//static void print_fps (int final);

static RETSIGTYPE signal_handler (int sig)
{
print_fps (1);
signal (sig, SIG_DFL);
raise (sig);
}

static void print_fps (int final)
{
static uint32_t frame_counter = 0;
static struct timeval tv_beg, tv_start;
static int total_elapsed;
static int last_count = 0;
struct timeval tv_end;
float fps, tfps;
int frames, elapsed;

gettimeofday (&tv_end, NULL);

if (!frame_counter) {
tv_start = tv_beg = tv_end;
signal (SIGINT, signal_handler);
}

elapsed = (tv_end.tv_sec - tv_beg.tv_sec) * 100 +
(tv_end.tv_usec - tv_beg.tv_usec) / 10000;
total_elapsed = (tv_end.tv_sec - tv_start.tv_sec) * 100 +
(tv_end.tv_usec - tv_start.tv_usec) / 10000;

if (final) {
if (total_elapsed)
tfps = frame_counter * 100.0 / total_elapsed;
else
tfps = 0;

fprintf (stderr,"\n%d frames decoded in %.2f seconds (%.2f fps)\n",
frame_counter, total_elapsed / 100.0, tfps);

return;
}

frame_counter++;

if (elapsed < 50)	
return;

tv_beg = tv_end;
frames = frame_counter - last_count;

fps = frames * 100.0 / elapsed;
tfps = frame_counter * 100.0 / total_elapsed;

fprintf (stderr, "%d frames in %.2f sec (%.2f fps), "
"%d last %.2f sec (%.2f fps)\033[K\r", frame_counter,
total_elapsed / 100.0, tfps, frames, elapsed / 100.0, fps);

last_count = frame_counter;
}

#else 

static void print_fps (int final)
{
}

#endif
*/
static void save_ppm (int width, int height, uint8_t * buf, int num, int *crop_h)
{
	int i = 0, j = 0, found = 0;
	double black = 0.0;
	unsigned char rgb[3];
	double f_x = 0;
	/*char filename[100];
	FILE * ppmfile;

	sprintf (filename, "e:\\%d.ppm", num);
	ppmfile = fopen (filename, "wb");
	if (!ppmfile)
	return;
	fprintf (ppmfile, "P6\n%d %d\n255\n", width, height);
	fwrite (buf, 3 * width, height, ppmfile);
	fclose (ppmfile);*/

	for (j = 0 ; j < (height * width * 3) && !found; j += 3)
	{
		rgb[0] = buf[j];
		i++;
		rgb[1] = buf[j + 1];
		i++;
		rgb[2] = buf[j + 2];
		i++;

		f_x += ((rgb[0] + rgb[1] + rgb[2]) / 3.0);

		if (i==(width))
		{
			f_x /= (width);

			if (f_x > 2.0)//|| ((top) && (c < 0x10)) && j > (0.5 * i_tot_height))
			{
				*crop_h = height - 2 * (j / (width * 4));
				found = 1;
				break;
			}
			f_x = 0;
			i=0;
		}
	}
}


static void decode_mpeg2 (uint8_t * current, uint8_t * end)
{
	const mpeg2_info_t * info;
	int state;
	vo_setup_result_t setup_result;

	mpeg2_buffer (mpeg2dec, current, end);

	info = mpeg2_info (mpeg2dec);
	while (1) {
		state = mpeg2_parse (mpeg2dec);
		switch (state) {
	case -1:
		return;
	case STATE_SEQUENCE:
		/* might set nb fbuf, convert format, stride */
		/* might set fbufs */
		if (output->setup (output, info->sequence->width,
			info->sequence->height, &setup_result)) {
				fprintf (stderr, "display setup failed\n");
				exit (1);
			}
			if (setup_result.convert)
				mpeg2_convert (mpeg2dec, setup_result.convert, NULL);
			if (output->set_fbuf) {
				uint8_t * buf[3];
				void * id;

				mpeg2_custom_fbuf (mpeg2dec, 1);
				output->set_fbuf (output, buf, &id);
				mpeg2_set_buf (mpeg2dec, buf, id);
				output->set_fbuf (output, buf, &id);
				mpeg2_set_buf (mpeg2dec, buf, id);
			} else if (output->setup_fbuf) {
				uint8_t * buf[3];
				void * id;

				output->setup_fbuf (output, buf, &id);
				mpeg2_set_buf (mpeg2dec, buf, id);
				output->setup_fbuf (output, buf, &id);
				mpeg2_set_buf (mpeg2dec, buf, id);
				output->setup_fbuf (output, buf, &id);
				mpeg2_set_buf (mpeg2dec, buf, id);
			}
			break;
	case STATE_PICTURE:
		/* might skip */
		/* might set fbuf */
		if (output->set_fbuf) {
			uint8_t * buf[3];
			void * id;

			output->set_fbuf (output, buf, &id);
			mpeg2_set_buf (mpeg2dec, buf, id);
		}
		if (output->start_fbuf)
			output->start_fbuf (output, info->current_fbuf->buf,
			info->current_fbuf->id);
		break;
	case STATE_PICTURE_2ND:
		/* should not do anything */
		break;
	case STATE_SLICE:
	case STATE_END:
		/* draw current picture */
		/* might free frame buffer */
		if (info->display_fbuf) 
		{
			/*
			output->draw (output, info->display_fbuf->buf,
			info->display_fbuf->id);*/
			if (fnd == MAX_CHECK_FRAMES)
			{
				save_ppm (info->sequence->width, info->sequence->height, 
					info->display_fbuf->buf[0], framenum++, &crop_h);
			}
			fnd += 1;
			//print_fps (0);
		}
		if (output->discard && info->discard_fbuf)
		{
			output->discard (output, info->discard_fbuf->buf,
				info->discard_fbuf->id);
		}
		break;
		}
	}
}

#define DEMUX_PAYLOAD_START 1
static int demux (uint8_t * buf, uint8_t * end, int flags)
{
	static int mpeg1_skip_table[16] = {
		0, 0, 4, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	/*
	* the demuxer keeps some state between calls:
	* if "state" = DEMUX_HEADER, then "head_buf" contains the first
	*     "bytes" bytes from some header.
	* if "state" == DEMUX_DATA, then we need to copy "bytes" bytes
	*     of ES data before the next header.
	* if "state" == DEMUX_SKIP, then we need to skip "bytes" bytes
	*     of data before the next header.
	*
	* NEEDBYTES makes sure we have the requested number of bytes for a
	* header. If we dont, it copies what we have into head_buf and returns,
	* so that when we come back with more data we finish decoding this header.
	*
	* DONEBYTES updates "buf" to point after the header we just parsed.
	*/

#define DEMUX_HEADER 0
#define DEMUX_DATA 1
#define DEMUX_SKIP 2
	static int state = DEMUX_SKIP;
	static int state_bytes = 0;
	static uint8_t head_buf[264];

	uint8_t * header;
	int bytes;
	int len;

	if (fnd > MAX_CHECK_FRAMES)
	{
		fnd = 0;
		return 1;
	}

#define NEEDBYTES(x)						\
	do {							\
	int missing;						\
	\
	missing = (x) - bytes;					\
	if (missing > 0) {					\
	if (header == head_buf) {				\
	if (missing <= end - buf) {			\
	memcpy (header + bytes, buf, missing);	\
	buf += missing;				\
	bytes = (x);				\
	} else {					\
	memcpy (header + bytes, buf, end - buf);	\
	state_bytes = bytes + end - buf;		\
	return 0;					\
	}						\
	} else {						\
	memcpy (head_buf, header, bytes);		\
	state = DEMUX_HEADER;				\
	state_bytes = bytes;				\
	return 0;					\
	}							\
	}							\
	} while (0)

#define DONEBYTES(x)		\
	do {			\
	if (header != head_buf)	\
	buf = header + (x);	\
	} while (0)

	if (flags & DEMUX_PAYLOAD_START)
		goto payload_start;
	switch (state) {
	case DEMUX_HEADER:
		if (state_bytes > 0) {
			header = head_buf;
			bytes = state_bytes;
			goto continue_header;
		}
		break;
	case DEMUX_DATA:
		if (demux_pid || (state_bytes > end - buf)) {
			decode_mpeg2 (buf, end);
			state_bytes -= end - buf;
			return 0;
		}
		decode_mpeg2 (buf, buf + state_bytes);
		buf += state_bytes;
		break;
	case DEMUX_SKIP:
		if (demux_pid || (state_bytes > end - buf)) {
			state_bytes -= end - buf;
			return 0;
		}
		buf += state_bytes;
		break;
	}

	while (1) {
		if (demux_pid) {
			state = DEMUX_SKIP;
			return 0;
		}
payload_start:
		header = buf;
		bytes = end - buf;
continue_header:
		NEEDBYTES (4);
		if (header[0] || header[1] || (header[2] != 1)) {
			if (demux_pid) {
				state = DEMUX_SKIP;
				return 0;
			} else if (header != head_buf) {
				buf++;
				goto payload_start;
			} else {
				header[0] = header[1];
				header[1] = header[2];
				header[2] = header[3];
				bytes = 3;
				goto continue_header;
			}
		}
		if (demux_pid) {
			if ((header[3] >= 0xe0) && (header[3] <= 0xef))
				goto pes;
			fprintf (stderr, "bad stream id %x\n", header[3]);
			exit (1);
		}
		switch (header[3]) {
	case 0xb9:	/* program end code */
		/* DONEBYTES (4); */
		/* break;         */
		return 1;
	case 0xba:	/* pack header */
		NEEDBYTES (12);
		if ((header[4] & 0xc0) == 0x40) {	/* mpeg2 */
			NEEDBYTES (14);
			len = 14 + (header[13] & 7);
			NEEDBYTES (len);
			DONEBYTES (len);
			/* header points to the mpeg2 pack header */
		} else if ((header[4] & 0xf0) == 0x20) {	/* mpeg1 */
			DONEBYTES (12);
			/* header points to the mpeg1 pack header */
		} else {
			fprintf (stderr, "weird pack header\n");
			exit (1);
		}
		break;
	default:
		if (header[3] == demux_track) {
pes:
			NEEDBYTES (7);
			if ((header[6] & 0xc0) == 0x80) {	/* mpeg2 */
				NEEDBYTES (9);
				len = 9 + header[8];
				NEEDBYTES (len);
				/* header points to the mpeg2 pes header */
				if (header[7] & 0x80) {
					uint32_t pts;

					pts = (((buf[9] >> 1) << 30) |
						(buf[10] << 22) | ((buf[11] >> 1) << 15) |
						(buf[12] << 7) | (buf[13] >> 1));
					mpeg2_pts (mpeg2dec, pts);
				}
			} else {	/* mpeg1 */
				int len_skip;
				uint8_t * ptsbuf;

				len = 7;
				while (header[len - 1] == 0xff) {
					len++;
					NEEDBYTES (len);
					if (len > 23) {
						fprintf (stderr, "too much stuffing\n");
						break;
					}
				}
				if ((header[len - 1] & 0xc0) == 0x40) {
					len += 2;
					NEEDBYTES (len);
				}
				len_skip = len;
				len += mpeg1_skip_table[header[len - 1] >> 4];
				NEEDBYTES (len);
				/* header points to the mpeg1 pes header */
				ptsbuf = header + len_skip;
				if (ptsbuf[-1] & 0x20) {
					uint32_t pts;

					pts = (((ptsbuf[-1] >> 1) << 30) |
						(ptsbuf[0] << 22) | ((ptsbuf[1] >> 1) << 15) |
						(ptsbuf[2] << 7) | (ptsbuf[3] >> 1));
					mpeg2_pts (mpeg2dec, pts);
				}
			}
			DONEBYTES (len);
			bytes = 6 + (header[4] << 8) + header[5] - len;
			if (demux_pid || (bytes > end - buf)) {
				decode_mpeg2 (buf, end);
				state = DEMUX_DATA;
				state_bytes = bytes - (end - buf);
				return 0;
			} else if (bytes > 0) {
				decode_mpeg2 (buf, buf + bytes);
				buf += bytes;
			}
		} else if (header[3] < 0xb9) {
			fprintf (stderr,
				"looks like a video stream, not system stream\n");
			DONEBYTES (4);
		} else {
			NEEDBYTES (6);
			DONEBYTES (6);
			bytes = (header[4] << 8) + header[5];
			if (bytes > end - buf) {
				state = DEMUX_SKIP;
				state_bytes = bytes - (end - buf);
				return 0;
			}
			buf += bytes;
		}
		}
	}
}

static void ps_loop (int start)
{
	uint8_t * end = NULL;

	fseek(in_file, start * BUFFER_SIZE, SEEK_SET);

	do {
		end = buffer + fread (buffer, 1, BUFFER_SIZE, in_file);
		if (demux (buffer, end, 0))
			break;	/* hit program_end_code */
	} while (end == buffer + BUFFER_SIZE);
}

/*
static int pva_demux (uint8_t * buf, uint8_t * end)
{
static int state = DEMUX_SKIP;
static int state_bytes = 0;
static uint8_t head_buf[12];

uint8_t * header;
int bytes;
int len;

switch (state) {
case DEMUX_HEADER:
if (state_bytes > 0) {
header = head_buf;
bytes = state_bytes;
goto continue_header;
}
break;
case DEMUX_DATA:
if (state_bytes > end - buf) {
decode_mpeg2 (buf, end);
state_bytes -= end - buf;
return 0;
}
decode_mpeg2 (buf, buf + state_bytes);
buf += state_bytes;
break;
case DEMUX_SKIP:
if (state_bytes > end - buf) {
state_bytes -= end - buf;
return 0;
}
buf += state_bytes;
break;
}

while (1) {
payload_start:
header = buf;
bytes = end - buf;
continue_header:
NEEDBYTES (2);
if (header[0] != 0x41 || header[1] != 0x56) {
if (header != head_buf) {
buf++;
goto payload_start;
} else {
header[0] = header[1];
bytes = 1;
goto continue_header;
}
}
NEEDBYTES (8);
if (header[2] != 1) {
DONEBYTES (8);
bytes = (header[6] << 8) + header[7];
if (bytes > end - buf) {
state = DEMUX_SKIP;
state_bytes = bytes - (end - buf);
return 0;
} 
buf += bytes; 
} else {
len = 8;
if (header[5] & 0x10) {
len = 12 + (header[5] & 3);
NEEDBYTES (len);
decode_mpeg2 (header + 12, header + len);
mpeg2_pts (mpeg2dec, ((header[8] << 24) | (header[9] << 16) |
(header[10] << 8) | header[11]));
}
DONEBYTES (len);
bytes = (header[6] << 8) + header[7] + 8 - len;
if (bytes > end - buf) {
decode_mpeg2 (buf, end);
state = DEMUX_DATA;
state_bytes = bytes - (end - buf);
return 0;
} else if (bytes > 0) {
decode_mpeg2 (buf, buf + bytes);
buf += bytes;
}
}
}
}

static void pva_loop (void)
{
uint8_t * end;

do {
end = buffer + fread (buffer, 1, BUFFER_SIZE, in_file);
pva_demux (buffer, end);
} while (end == buffer + BUFFER_SIZE);
}

static void ts_loop (void)
{
#define PACKETS (BUFFER_SIZE / 188)
uint8_t * buf;
uint8_t * data;
uint8_t * end;
int packets;
int i;
int pid;

do {
packets = fread (buffer, 188, PACKETS, in_file);
for (i = 0; i < packets; i++) {
buf = buffer + i * 188;
end = buf + 188;
if (buf[0] != 0x47) {
fprintf (stderr, "bad sync byte\n");
exit (1);
}
pid = ((buf[1] << 8) + buf[2]) & 0x1fff;
if (pid != demux_pid)
continue;
data = buf + 4;
if (buf[3] & 0x20) {	
data = buf + 5 + buf[4];
if (data > end)
continue;
}
if (buf[3] & 0x10)
demux (data, end, (buf[1] & 0x40) ? DEMUX_PAYLOAD_START : 0);
}
} while (packets == PACKETS);
}
*/

/*
static void es_loop (void)
{
uint8_t * end;

do {
end = buffer + fread (buffer, 1, BUFFER_SIZE, in_file);
decode_mpeg2 (buffer, end);
} while (end == buffer + BUFFER_SIZE);
}*/


int _mpeg2dec_lib_get_info(char *s_filename, VOBINFO *pr_vobinfo)
{
	vo_driver_t * drivers;
	drivers = vo_drivers ();

	output_open = drivers[5].open;

	output = output_open ();

	in_file = fopen (s_filename, "rb");

	mpeg2dec = mpeg2_init ();

	/*
	if (demux_pva)
	pva_loop ();
	else if (demux_pid)
	ts_loop ();
	else if (demux_track)
	ps_loop ();
	else
	es_loop ();*/
	demux_track = 0xe0;
	//ps_loop(0);

	mpeg2_close (mpeg2dec);
	if (output->close)
		output->close (output);
	//print_fps (1);
	//getch();
	return 0;
}

int mpeg2dec_lib_get_info(char *s_filename, VOBINFO *pr_vobinfo)
{
	vo_driver_t * drivers = NULL;
	vo_open_t * output_open = NULL;
	int i = 0;
	int j = 0;
	FILE *fp = NULL;
	int max = 0;
	int ai_tmp[MAX_CHECK_POINTS] = {0};
	double afwijking = 0.05;
	long l_filesize = 0;
	int ai_checksizes[MAX_CHECK_POINTS] = {0};
	int ai_cropping_height[MAX_CHECK_POINTS];
	int k = BUFFER_SIZE;
	double x = 0;
	int i_counter = 0;

	for (i = 0 ; i < MAX_CHECK_POINTS; i ++)
	{
		ai_cropping_height[i] = -1;
	}

	i_counter = 0;

	in_file = fopen (s_filename, "rb");

	if (in_file)
	{
		vstrip_getfilesize(s_filename, &l_filesize);

		if (l_filesize > MAX_LONG)
		{
			l_filesize = MAX_LONG - 1;
		}

		ai_checksizes[0] = k; 
		ai_checksizes[MAX_CHECK_POINTS - 1] = (long) ((l_filesize / BUFFER_SIZE) - k);

		for (i = 1; i < MAX_CHECK_POINTS - 1; i++)
		{
			x = 1.0 * i / ((MAX_CHECK_POINTS - 2) + 1);
			ai_checksizes[i] = (long) ((l_filesize * x) / BUFFER_SIZE);
		}

		// doit
		drivers = vo_drivers ();
		output_open = drivers[5].open;
		output = output_open ();

		mpeg2dec = mpeg2_init ();
		demux_track = 0xe0;

		for (i = 0 ; i < MAX_CHECK_POINTS ; i++)
		{
			ps_loop(ai_checksizes[i]);
			ai_cropping_height[i] = crop_h;
		}

		mpeg2_close (mpeg2dec);
		if (output->close)
			output->close (output);

		fclose(in_file);

		for (i = 0 ; i < MAX_CHECK_POINTS; i ++)
		{
			for (j = 0 ; j < MAX_CHECK_POINTS ; j++)
			{
				if (abs(ai_cropping_height[i] - ai_cropping_height[j]) < (afwijking * ai_cropping_height[i]))
				{
					ai_tmp[i]++;
				}
			}
		}


		for (i = 0 ; i < MAX_CHECK_POINTS; i ++)
		{
			if (ai_tmp[i] > max)
			{
				max = ai_tmp[i];
				j = i;
			}
		}

		pr_vobinfo->i_cropping_height = ai_cropping_height[j];

	}

	return 0;
}
