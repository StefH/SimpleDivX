/*
** Header file.
*/
#ifndef _TC_H
#define _TC_H

#include <stdio.h>

#define MAX_LINE_LEN				512
#define MAX_PARAMS					 10

void printTestHeader(const char*, int, char s[MAX_PARAMS][MAX_LINE_LEN]);
void printTestFooter(const char* s_tc);

void readline(FILE *fp, char* s_line);
char * readparam(int id, char* s_line);
void readparams(FILE *fp, int, char s[MAX_PARAMS][MAX_LINE_LEN]);

#endif