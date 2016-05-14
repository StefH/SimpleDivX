#ifndef _SIMPLE_TYPES_H_INCLUDED

/*************************************************************************
  vStrip by [maven] (maven@maven.de)
*************************************************************************/

#define _SIMPLE_TYPES_H_INCLUDED

#ifndef TRUE
#define TRUE (1 == 1)
#define FALSE (1 == 0)
#endif

typedef unsigned char 			vstripbool;
typedef unsigned char 			byte;
typedef unsigned short int 	word;
typedef unsigned int				dword;

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b)) 
#endif

void vs_log(char *str);
#define _log(s) vs_log(s)

#ifdef VSTRIP_DLL
#define VSTRIP_DLL_API __declspec(dllexport)
#else
#define VSTRIP_DLL_API
#endif

#endif