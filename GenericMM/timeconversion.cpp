//
//
//

#include "stdafx.h"
#include "genericmm.h"
#include "math.h"
#include <io.h>
#include <string.h>

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#define TMP_DIGIT_OVERFLOW "E!"

void digit2string2(long digit, char *str, char*);
void digit2string3(long digit, char *str, char*);

void timeformat2timestring_hmss(long hour, long min, long sec, long msec, char *string, bool ms);

void seconds2timeformat(long seconds, long *p_hour, long *p_min, long *p_sec)
{
	long msec = 0;
	milliseconds2timeformat(1000 * seconds, p_hour, p_min, p_sec, &msec);
}

void seconds2timeformat(long seconds, char *string)
{
	milliseconds2timeformat(seconds * 1000, string);
}

void seconds2timeformat_hms(long seconds, char *string)
{
	long h,m,s;
	seconds2timeformat(seconds, &h, &m, &s);
	timeformat2timestring_hmss(h,m,s,0, string, false);
}

void seconds2timeformat_hmss(long mseconds, char *string)
{
	long h,m,s,ms;
	milliseconds2timeformat(mseconds, &h, &m, &s, &ms);
	timeformat2timestring_hmss(h,m,s,ms,string, true);
}

void milliseconds2timeformat(long mseconds, char *string)
{
	long h,m,s,ms;
	milliseconds2timeformat(mseconds, &h, &m, &s, &ms);
	timeformat2timestring(h, m, s, ms, string);
}

void milliseconds2timeformatVobSub(long mseconds, char *string)
{
	long h,m,s,ms;
	milliseconds2timeformat(mseconds, &h, &m, &s, &ms);
//	timeformat2timestring(h, m, s, ms, string);
}

void milliseconds2timeformat(long mseconds, long *p_hour, long *p_min, long *p_sec, long *p_msec)
{
	long seconds = mseconds / 1000;
	*p_hour = seconds / 3600;
	*p_min  = (-*p_hour * 3600 + seconds) / 60;
	*p_sec  = (-*p_hour * 3600 - *p_min * 60 + seconds);
	*p_msec = mseconds - seconds * 1000;
}

void timeformat2timestring_hmss(long hour, long min, long sec, long msec , char *string, bool ms)
{
	char digit[16] = {0};
	
	strcpy(string, "");
	
	digit2string2(hour, digit, "h \0");
	strcat(string, digit);
	digit2string2(min, digit, "m \0");
	strcat(string, digit);
	digit2string2(sec, digit, "s \0");
	strcat(string, digit);

	if (ms)
	{
		digit2string3(msec, digit, "ms\0");
		strcat(string, digit);
	}
}

void timeformat2timestring_hms(long hour, long min, long sec, char *string)
{
	timeformat2timestring_hmss(hour, min, sec, 0 , string, false);
}

void timeformat2timestring(long hour, long min, long sec, long msec, char *string)
{
	char digit[16] = {0};
	
	strcpy(string, "");
	
	digit2string2(hour, digit, ":\0");
	strcat(string, digit);
	digit2string2(min, digit, ":\0");
	strcat(string, digit);
	digit2string2(sec, digit, ".\0");
	strcat(string, digit);
	digit2string3(msec, digit, "\0");
	strcat(string, digit);
	
}

void digit2string2(long digit, char *str, char *s)
{
	if (digit < 10)
	{
		
		sprintf(str, "0%ld%s", digit, s);
		
	}
	else if (digit < 60)
	{
		
		sprintf(str, "%ld%s", digit, s);
		
	}
	else
	{
		strcpy(str, TMP_DIGIT_OVERFLOW);
	}
}

void digit2string3(long digit, char *str, char *s)
{
	if (digit < 10)
	{
		sprintf(str, "00%ld%s", digit, s);
		
	}
	else if (digit < 100)
	{
		
		sprintf(str, "%0ld%s", digit, s);
		
	}
	else if (digit < 1000)
	{
		
		sprintf(str, "%ld%s", digit, s);
		
	}
	else
	{
		strcpy(str, TMP_DIGIT_OVERFLOW);
	}
}