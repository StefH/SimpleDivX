#include "TestCase.h"
#include "string.h"


#define PARAM						"param"
#define MAX_PARAM_ID_LEN				16

void printTestHeader(const char* s_tc,
					 int i_num,
					 char s_param[MAX_PARAMS][MAX_LINE_LEN])
{
	printf("Testcase : '%s' is started.\n", s_tc);

	for (int i = 0 ; i < i_num ; i++)
	{
		if (s_param[i] != NULL)
		{
			printf("param%d = %s\n", i, s_param[i]);
		}
	}
}

void printTestFooter(const char* s_tc)
{
	printf("Testcase : '%s' is finished.\n\n", s_tc);
}

void readline(FILE *fp, char *s_line)
{
	int i_char_pos = 0;
	char c_char = 0;

	while ((c_char != '\n') && (c_char != EOF))
	{
		c_char = fgetc(fp);
		if ((c_char != '\n') && (c_char != EOF))
		{
			s_line[i_char_pos++] = c_char;
		}
	}

	s_line[i_char_pos] = 0;
}

void readparams(FILE *fp,
				int i_num,
				char s_param[MAX_PARAMS][MAX_LINE_LEN])
{
	char s_line[MAX_LINE_LEN] = {0};

	for (int i = 0 ; i < i_num ; i++)
	{
		memset(s_param[i], MAX_LINE_LEN, sizeof(char));
		readline(fp, s_line);
		if (!((s_line[0] == '\n') || (s_line[0] == '#')))
		{
			strcpy(s_param[i], readparam(i, s_line));
		}
	}
}

char * readparam(int id, char* s_line)
{
	char s_param[MAX_PARAM_ID_LEN] = {0};

	sprintf(s_param, "%s%d", PARAM, id);

	return &s_line[strlen(s_param) + 1];
}