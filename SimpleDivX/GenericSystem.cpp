//
//
//
#include "stdafx.h"
#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include "GenericSystem.h"
#include "GenericInternal.h"
#include "CPUPlatformSDK.h"
#include "ICPUInfo.h"
#include "math.h"

#define GEN_VERSION "0.0.0.5"

/*
 * see also : http://www.sandpile.org/ia32/cpuid.htm
 * (SDK method used...)
 *
 * AMD 64 dual core
 * Athlon 64 (0.13 µm 939)						<webpage>
 * Family / Model / Stepping	15 / 11 / 1
 *
 * AMD Athlon(tm) 64 Processor 3700+			<registry mode>
 * Family / Model / Stepping	15 / 39 / 1
 *
 * AMD Athlon(TM) XP2400+						<registry mode>
 * AMD Athlon Thoroughbred						<sdk mode>
 * Family / Model / Stepping	 6 /  8 / 1
 *
 * AMD Sempron
 * Family / Model / Stepping	 6 / 10 / 0
 *
 * AMD Athlon(tm) - XP AMD Athlon Palamino 
 * Family / Model / Stepping	 6 /  6 / 2
 *
 * AMD Athlon(tm) - AMD Athlon Thoroughbred 1192MHz
 * Family / Model / Stepping	 6 /  8 / 1
 *
 *
 * Intel(R) Pentium(R) M processor 1.73GHz		<registry mode>
 * INTEL Itanium								<sdk mode>
 * Mobile pentium M (p4)						<webpage>
 * Family / Model / Stepping	 6 / 13 / 8
 *
 *
 */
int GetCpuInfo(CPU_info *info)
{
	ICPUInfo* cpuInfo = new CPUPlatformSDK;
	
	if(!cpuInfo->QueryCPUInfo())
		return 1;

	info->model = CPU_OTHER;

	if (cpuInfo->GetCPUName().find("AMD") != -1)
	{
		// AMD
		if (cpuInfo->GetCPUFamily() == 6)
		{
			// AMD XP
			/*
			if ((info->GetCPUModel() == 8)
			{
				info->model = CPU_A_XP;
			}*/
			info->model = CPU_A_XP;
		}
		else if (cpuInfo->GetCPUFamily() >= 15)
		{
			// EXTENDED
			if (cpuInfo->GetCPUModel() == 39)
			{
				// AMD Athlon(tm) 64 Processor 3700+
				info->model = CPU_A_64;
			}
			if (cpuInfo->GetCPUModel() == 43)
			{
				// AMD Athlon(tm) 64 X2 Dual Core Processor 3800+
				info->model = CPU_A_64;
			}
			info->model = CPU_A_64;
		}
		else
		{
			info->model = CPU_A_XP;
		}
	}
	else
	{
		// INTEL
		if (cpuInfo->GetCPUFamily() == 6)
		{
			if ((cpuInfo->GetCPUModel() >= 7) && (cpuInfo->GetCPUModel() < 12))
			{
				// INTEL P3
				info->model = CPU_I_P3;
			}
			else
			{
				// INTEL other (p2 / p3)
				info->model = CPU_I_P2;
			}
		}
		else if (cpuInfo->GetCPUFamily() == 15)
		{
			// EXTENDED
			info->model = CPU_I_P4;
		}
	}

	return 0;
}

//
// Search for all Drives on the system which include a specific folder
//
int SearchForFolderOnAllDrives(CString drives[MAX_HARD_DISKS], CString folder)
{
	int NumofValidDrives = 0;
	CString Path;
	
	for (int i = 0 ; i < MAX_HARD_DISKS ; i++)
	{
		Path.Format("%s\\%s", HardDiskLetters[i], folder);
		if (GetFileAttributes(Path) != -1)
		{
			// Found valid Drive + Folder
			drives[NumofValidDrives] = HardDiskLetters[i];
			NumofValidDrives++;
		}
	}

	return NumofValidDrives;
}

//
// Search for all CD/DVD Drives on the system which include a specific folder
//
int SearchForFolderOnCDDrives(CString drives[MAX_HARD_DISKS], CString folder)
{
	int NumofCDDrive = 0;
	int NumofValidCDDrive = 0;
	CString foundCDDrives[MAX_HARD_DISKS];
	CString Path;
	
	NumofCDDrive = SearchForCDDrives(foundCDDrives);

	for (int i = 0 ; i < NumofCDDrive ; i++)
	{
		Path = foundCDDrives[i] + '\\' + folder;
		if (GetFileAttributes(Path) != -1)
		{
			// Found valid Drive + Folder
			drives[NumofValidCDDrive] = foundCDDrives[i];
			NumofValidCDDrive++;
		}
	}

	return NumofValidCDDrive;
}

//
// Search for all CD/DVD Drives on the system.
//
int SearchForCDDrives(CString drives[MAX_HARD_DISKS])
{
	int NumofCDDrive = 0;
	
	for (int iCounter=0 ; iCounter < MAX_HARD_DISKS ; iCounter++)
	{
		if (GetDriveType(HardDiskLetters[iCounter]) == DRIVE_CDROM)
		{
			drives[NumofCDDrive] = HardDiskLetters[iCounter];
			NumofCDDrive++;
		}
	}

	return NumofCDDrive;
}

bool CreatePath(const char* PathToCreate)
{
	char Path[_MAX_PATH] = { 0 };
	
	// Copy path because we have to modify it
	strcpy(Path,PathToCreate);
	
	// First add an ending slash
	char* Pos = strrchr(Path,'\\');
	
	if (!Pos || Pos[1]!=0)
		strcat(Path,"\\");
	
	// From the first slash to the last check and create path
	// Exception : Jump initial duble slashed UNC host name and include first directory
	if (Path[0]=='\\' && Path[1]=='\\')
	{
		// Find slash ending the host name
		Pos = strchr(Path+2,'\\');
		
		// Find slash after the first directory (after the host name)
		if (Pos)
			
			Pos = strchr(Pos+1,'\\');
	}
	else
		Pos = strchr(Path,'\\');
	
	bool AllOk = true;
	
	while (Pos && AllOk)
	{
		// Cut the string
		*Pos = 0;
		
		// Check if exist this piece of path
		if (GetFileAttributes(Path)==-1)
			
			if (GetLastError()==ERROR_FILE_NOT_FOUND)
				
				AllOk = CreateDirectory(Path,NULL)==TRUE;
			
			else
				
				AllOk = false;
			
			// Recover the character deleted
			*Pos = '\\';
			
			// Find next slash
			Pos = strchr(Pos+1,'\\');
	}
	
	// Return true if finally the path was created or exist
	// (if not was created see LastError)
	
	return AllOk;
}

int GetFileSizeInt64(const char * s_file, __int64 *pl_size)
{
	int i_status = -1;
	int fp = 0;

	*pl_size = 0;
	
	fp = _open(s_file,_O_RDONLY);
		
	if (fp > 0)
	{
		(void) _lseeki64( fp, 0, SEEK_END );
			
		*pl_size = _telli64(fp);
		
		_close(fp);
		i_status = 0;
	}
	
	return i_status;
}

int GetFileSizeLong(const char * s_file, long *pl_size)
{
	int i_status = -1;
	int fp = 0;

	*pl_size = 0;
	
	fp = _open(s_file,_O_RDONLY);
		
	if (fp > 0)
	{
		*pl_size = _lseek(fp, 0L, SEEK_END );
			
		_close(fp);
		if (*pl_size != -1L)
		{
			i_status = 0;
		}
	}
	
	return i_status;
}