#ifndef _GEN_SYS_H_
#define _GEN_SYS_H_

//
// Main header for generic-system
//
#define MAX_HARD_DISKS	24

typedef enum _cpu_model_
{
	CPU_I_P2,
	CPU_I_P3,
	CPU_I_P4,
	CPU_A_XP,
	CPU_A_64,
	CPU_OTHER
} CPU_MODEL;

typedef struct
{
	CPU_MODEL model;
} CPU_info;

#if defined(_STDCALL_SUPPORTED)
#define STDCALL __stdcall    // Declare our calling convention.
#else
#define STDCALL
#endif // STDCALL_SUPPORTED

#ifdef __cplusplus
extern "C" {     // Don’t use C++ decorations for our API names
#endif

STDCALL bool CreatePath(const char* PathToCreate);
STDCALL int GetFileSizeInt64(const char* s_file, __int64 *pl_size);
STDCALL int GetFileSizeLong(const char * s_file, long *pl_size);
STDCALL int SearchForFolderOnAllDrives(CString drives[MAX_HARD_DISKS], CString folder);
STDCALL int SearchForFolderOnCDDrives(CString drives[MAX_HARD_DISKS], CString folder);
STDCALL int SearchForCDDrives(CString drives[MAX_HARD_DISKS]);
STDCALL int GetCpuInfo(CPU_info *info);

#ifdef __cplusplus
}                // Close the extern C.
#endif

#endif