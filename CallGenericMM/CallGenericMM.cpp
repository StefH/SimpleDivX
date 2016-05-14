// CallGenericMM.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TestCase.h"
#include "GenericMM.h"

// Test file
#define TEST_CASE_FILE				"Tests.txt"

// Defined Tests
#define TC_GETAVIINFO				"getaviinfo"
#define TC_FILESIZE2FRAME			"filesize2frame"
#define TC_SEARCHSPLITPOINTS		"searchsplitpoints"
#define TC_LISTCODECS				"listcodecs"

static int Test_GetAviInfo(int, char [MAX_PARAMS][MAX_LINE_LEN]);
static int Test_Filesize2frame(int, char [MAX_PARAMS][MAX_LINE_LEN]);
static int Test_SearchSplitPoints(int, char [MAX_PARAMS][MAX_LINE_LEN]);
static int Test_ListCodecs();

int _tmain(int argc, _TCHAR* argv[])
{
	bool b_test_found = false;
	int i_tests = 0;
	FILE *fp;

	fp = fopen(TEST_CASE_FILE, "rt");

	if (fp)
	{
		char s_line[MAX_LINE_LEN] = {0};
		char s_param[MAX_PARAMS][MAX_LINE_LEN] = {0};
		int i_num_param = 0;
		
		while (!feof(fp))
		{
			b_test_found = false;

			readline(fp, s_line);

			if ((s_line[0] == '\n') || (s_line[0] == '#'))
			{
				continue;
			}

			if (strcmp(s_line, TC_FILESIZE2FRAME) == 0)
			{
				i_num_param = 2;
				readparams(fp, i_num_param, s_param);
				Test_Filesize2frame(i_num_param, s_param);
				b_test_found = true;
			}

			if (strcmp(s_line, TC_SEARCHSPLITPOINTS) == 0)
			{
				i_num_param = 5;
				readparams(fp, i_num_param, s_param);
				Test_SearchSplitPoints(i_num_param, s_param);
				b_test_found = true;
			}

			if (strcmp(s_line, TC_GETAVIINFO) == 0)
			{
				i_num_param = 1;
				readparams(fp, i_num_param, s_param);
				Test_GetAviInfo(i_num_param, s_param);
				b_test_found = true;
			}

			if (strcmp(s_line, TC_LISTCODECS) == 0)
			{
				i_num_param = 0;
				Test_ListCodecs();
				b_test_found = true;
			}

			if (b_test_found)
			{
				i_tests++;
				b_test_found = false;
			}
		}

		fclose(fp);

		printf("Done. %d tests executed.\n", i_tests);
	}
	else
	{
		printf("Error : file %s not found, no tests executed.\n");
	}

	return 0;
}

static int Test_GetAviInfo(int i_num_params, char s_param[MAX_PARAMS][MAX_LINE_LEN])
{
	int result = 0;
	double f_fps = 0;
	long l_frames = 0;
	long l_seconds = 0; 
	
	printTestHeader(TC_GETAVIINFO, i_num_params, s_param);

	result = get_avi_info((char*) s_param[0], &f_fps, &l_frames, &l_seconds);

	if (result == 0)
	{
		printf("Frames per second       : %2.3f\n", f_fps);
		printf("Total number of frames  : %ld\n", l_frames);
		printf("Total number of seconds : %ld\n", l_seconds);
	}
	else
	{
		printf("Error reading file.\n");
	}

	printTestFooter(TC_GETAVIINFO);

	return result;
}

static int Test_SearchSplitPoints(int i_num_params, char s_param[MAX_PARAMS][MAX_LINE_LEN])
{
	int result = 0;
	int i_res = atoi(s_param[1]);
	int i_threshold = atoi(s_param[2]);
	int i_min_time_between_points = atoi(s_param[3]);
	int i_split_on_keyframe = atoi(s_param[4]);
		
	printTestHeader(TC_SEARCHSPLITPOINTS, i_num_params, s_param);

	SPLIT_DATA r_data = {0};

	// TODO
	// Maybe too much memory allocated, but its safe now.
	r_data.ai_points = (int*) calloc(1024, sizeof(int));

	result = search_for_split_points((char*) s_param[0], 
		i_res, i_threshold, i_min_time_between_points, i_split_on_keyframe, &r_data);

	FILE *fp;
	char s_filename[32];

	sprintf(s_filename, "Points_%d_%d_%d_%d.txt", 
			i_res,
			i_threshold,
			i_min_time_between_points,
			i_split_on_keyframe);

	fp = fopen(s_filename, "wt");

	for (int i = 0 ; i < r_data.i_num_points ; i++)
	{
		fprintf(fp, "%ld\n", r_data.ai_points[i]);
	}

	fclose(fp);

	free(r_data.ai_points);

	printTestFooter(TC_SEARCHSPLITPOINTS);

	return result;
}

static int Test_Filesize2frame(int i_num_params, char s_param[MAX_PARAMS][MAX_LINE_LEN])
{
	int  result = 0; 
	long l_frame = 0;
	int  i_split_size = 0;

	printTestHeader(TC_FILESIZE2FRAME, i_num_params, s_param);

	i_split_size = atoi(s_param[1]);

	result = filesize2frame((char*) s_param[0], i_split_size*1024*1024, 0, &l_frame);

	printf("Frame found at position : %ld\n", l_frame);

	printTestFooter(TC_FILESIZE2FRAME);

	return result;
}

static int Test_ListCodecs()
{
	int  result = 0;
	char **as_codecs = NULL;
	int i_num_codecs = 0;
	
	printTestHeader(TC_LISTCODECS, 0, NULL);

	i_num_codecs = GetCodecs(&as_codecs);

	printf("Number of installed codecs found : %d\n", i_num_codecs);

	for (int i = 0 ; i < i_num_codecs ; i++)
	{
		printf("%s\n", as_codecs[i]);
	}

	FreeCodecs(i_num_codecs, &as_codecs);

	printTestFooter(TC_LISTCODECS);

	return result;
}