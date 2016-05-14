#pragma once
#include "icpuinfo.h"

class CPUPlatformSDK :
	public ICPUInfo
{
	struct _CPU_WSI {
		unsigned __int64	iSpeed;
		short int			iFamily;
		short int			iModel;
		short int			iStepping;
	};

	struct _CPU_WSI sCPUInfo;
public:
	CPUPlatformSDK(void);
	virtual ~CPUPlatformSDK(void);

	virtual bool QueryCPUInfo();
	virtual unsigned int GetSpeedMHz() const;

	virtual std::string GetCPUName() const;
	virtual std::string GetVendorName() const;

	virtual short int GetCPUFamily() const;
	virtual short int GetCPUModel() const;
	virtual short int GetCPUStepping() const;

	virtual bool IsMMXSupported() const;
	virtual bool IsSSESupported() const;
	virtual bool IsSSE2Supported() const;
	virtual bool Is3DNowSupported() const;
};
