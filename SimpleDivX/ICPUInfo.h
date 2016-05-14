#pragma once

#include <WTYPES.h>
#include <string>
#include <sstream>

class ICPUInfo
{
public:
	virtual bool QueryCPUInfo() = 0;	// called to retrieve CPU information

	virtual unsigned __int64 GetSpeed() const ;			// determine CPU speed
	std::string FormatSpeed(unsigned __int64 speed) const ;// CPU speed in short format; eg: 1.39GHz
	virtual unsigned int GetSpeedMHz() const = 0;

	virtual std::string GetCPUName() const = 0;			// processor name
	virtual std::string GetVendorName() const ;			// vendor name
	std::string GetCPUName(std::string vendor, short int family, short int model, short int family_ex, short int model_ex) const;	// processor name from vendor, family and model
	
	virtual short int GetCPUFamily() const = 0;			// family number
	virtual short int GetCPUModel() const = 0;			// model number
	virtual short int GetCPUStepping() const = 0;	    // stepping number

	virtual bool IsMMXSupported() const = 0;			// true if MMX feature is supported
	virtual bool IsSSESupported() const = 0;			// true if SSE feature is supported
	virtual bool IsSSE2Supported() const = 0;			// true if SSE2 feature is supported
	virtual bool Is3DNowSupported() const = 0;			// true if 3DNOW feature is supported
};
