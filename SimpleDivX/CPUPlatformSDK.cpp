#include "StdAfx.h"
#include "cpuplatformsdk.h"

#define HIDIGIT(d)	((BYTE)(d>>4))
#define LODIGIT(d)	((BYTE)(d&0x0F))

CPUPlatformSDK::CPUPlatformSDK(void)
{
	sCPUInfo.iSpeed = 0;
	sCPUInfo.iFamily = 0;
	sCPUInfo.iModel = 0;
	sCPUInfo.iStepping = 0;
}

CPUPlatformSDK::~CPUPlatformSDK(void)
{
}

bool CPUPlatformSDK::QueryCPUInfo()
{
	// determine CPU speed
	sCPUInfo.iSpeed = ICPUInfo::GetSpeed();

	// determine CPU info
	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	
	// CPU family
	switch(siSysInfo.wProcessorArchitecture)
	{
	case PROCESSOR_ARCHITECTURE_INTEL:
		sCPUInfo.iFamily = siSysInfo.wProcessorLevel;
		break;
	case PROCESSOR_ARCHITECTURE_IA64:
		sCPUInfo.iFamily = 1;
		break;
	default:
		ASSERT(0);
		return false;
	}

	// CPU model and stepping
	switch(sCPUInfo.iFamily)
	{
	// Intel 80386 and Intel 80486
	case 3: case 4:
		{
			if(HIBYTE(siSysInfo.wProcessorRevision) == 0xFF)
			{
				sCPUInfo.iModel = HIDIGIT(LOBYTE(siSysInfo.wProcessorRevision)) - 0x0A;
				sCPUInfo.iStepping = LODIGIT(LOBYTE(siSysInfo.wProcessorRevision));
			}
			else
			{
				sCPUInfo.iStepping = HIBYTE(siSysInfo.wProcessorRevision)+'A';
			}
		}
	// Intel Pentium, Cyrix, or NextGen 586
	case 5: case 6: case 15:
		{
			sCPUInfo.iModel = HIBYTE(siSysInfo.wProcessorRevision);
			sCPUInfo.iStepping = LOBYTE(siSysInfo.wProcessorRevision);
		}
	}

	return true;
}

unsigned int CPUPlatformSDK::GetSpeedMHz() const
{
	return (unsigned int)(sCPUInfo.iSpeed/(1024*1024));
}

std::string CPUPlatformSDK::GetCPUName() const
{
	return ICPUInfo::GetCPUName(ICPUInfo::GetVendorName(), 
		sCPUInfo.iFamily, sCPUInfo.iModel, 0, 0);
}

std::string CPUPlatformSDK::GetVendorName() const
{
	return ICPUInfo::GetVendorName();
}

short int CPUPlatformSDK::GetCPUFamily() const
{
	return sCPUInfo.iFamily;
}

short int CPUPlatformSDK::GetCPUModel() const
{
	return sCPUInfo.iModel;
}

short int CPUPlatformSDK::GetCPUStepping() const
{
	return sCPUInfo.iStepping;
}

bool CPUPlatformSDK::IsMMXSupported() const
{
	return (FALSE != IsProcessorFeaturePresent(PF_MMX_INSTRUCTIONS_AVAILABLE));
}

bool CPUPlatformSDK::IsSSESupported() const
{
	return (FALSE != IsProcessorFeaturePresent(PF_XMMI_INSTRUCTIONS_AVAILABLE));
}

bool CPUPlatformSDK::IsSSE2Supported() const
{
	return (FALSE != IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE));
}

bool CPUPlatformSDK::Is3DNowSupported() const
{
	return (FALSE != IsProcessorFeaturePresent(PF_3DNOW_INSTRUCTIONS_AVAILABLE));
}
