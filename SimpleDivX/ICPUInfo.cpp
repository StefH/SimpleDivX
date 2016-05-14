#include "StdAfx.h"
#include "icpuinfo.h"

#include <complex>

unsigned __int64 ICPUInfo::GetSpeed() const 
{
	unsigned __int64 start, stop;
	unsigned __int64 nCtr, nFreq, nCtrStop;
			  
	QueryPerformanceFrequency((LARGE_INTEGER *)&nFreq);
	_asm _emit 0x0F
	_asm _emit 0x31
	_asm mov DWORD PTR start, eax
	_asm mov DWORD PTR [start+4], edx
	QueryPerformanceCounter((LARGE_INTEGER *)&nCtrStop);
	nCtrStop += nFreq;
	do
	{
		QueryPerformanceCounter((LARGE_INTEGER *)&nCtr);
	}while (nCtr < nCtrStop);

	_asm _emit 0x0F
	_asm _emit 0x31
	_asm mov DWORD PTR stop, eax
	_asm mov DWORD PTR [stop+4], edx

	return (stop-start);
}

std::string ICPUInfo::FormatSpeed(unsigned __int64 speed) const 
{
	char *msr[]={"KB","MB","GB"};
	int i=0;
	std::string sSpeed("");
	std::stringstream ssSpeed(sSpeed);
	__int64 iSpeed = GetSpeed();

	for(i=3; i>0; i--)
	{
		double fpow = pow(1024,i);
		if(iSpeed>=fpow)
		{
			double fSpeed = iSpeed/fpow;
			ssSpeed << fSpeed << msr[i-1];
			break;
		}
	}

	sSpeed = ssSpeed.str();
	return sSpeed;
}

std::string ICPUInfo::GetVendorName() const 
{
	DWORD dwMax = 0;
	union {
		char cBuffer[12+1];
		struct {
			DWORD dw0;
			DWORD dw1;
			DWORD dw2;
		} stc;
	} Vendor;

	_asm {
		push ebx
			push ecx
			push edx

			// get the vendor string
			xor eax, eax
			cpuid
			mov dwMax, eax
			mov Vendor.stc.dw0, ebx
			mov Vendor.stc.dw1, edx
			mov Vendor.stc.dw2, ecx

			pop ecx
			pop ebx
			pop edx
	}

	Vendor.cBuffer[12] = 0;
	std::string sVendor(Vendor.cBuffer);

	return sVendor;
}

std::string ICPUInfo::GetCPUName(std::string vendor, short int family, short int model, short int family_ex, short int model_ex) const
{
	if(vendor.compare("GenuineIntel") == 0)
	{
		switch(family)
		{
		case 4:
			{
				switch(model)
				{
				case 0: return "INTEL 486 DX-25/33";
				case 1: return "INTEL 486 DX-50";
				case 2: return "INTEL 486 SX";
				case 3: return "INTEL 486 DX2";
				case 4: return "INTEL 486 SL";
				case 5: return "INTEL 486 SX2";
				case 7: return "INTEL 486 DX2-WB";
				case 8: return "INTEL 486 DX4";
				case 9: return "INTEL 486 DX4-WB";
				}
			}
			break;
		case 5:
			{
				switch(model)
				{
				case 0: return "INTEL Pentium 60/66 A-step";
				case 1: return "INTEL Pentium 60/66";
				case 2: return "INTEL Pentium 75-200";
				case 3: return "INTEL OverDrive PODP5V83";
				case 4: return "INTEL Pentium MMX";
				case 7: return "INTEL Mobile Pentium 75-200";
				case 8: return "INTEL Mobile Pentium MMX";
				}
				break;
			}
		case 6:
			{
				switch(model)
				{
					case 1: return "INTEL  Pentium Pro A-Step";
					case 2: return "INTEL  Pentium Pro";
					case 3: return "INTEL  Pentium II Klamath";
					case 5: return "INTEL  Pentium II Deschutes";
					case 6: return "INTEL  Celeron Mendocino";
					case 7: return "INTEL  Pentium III Katmai";
					case 8: return "INTEL  Pentium III Coppermine";
					case 9: return "INTEL  Mobile Pentium III";
					case 10: return "INTEL  Pentium III (0.18um)";
					case 11: return "INTEL  Pentium III (0.13um)";
				}
			}
		case 7:	return "INTEL Itanium";
		case 15:
			{
				switch(family_ex)
				{
				case 0:
					{
						switch(model_ex)
						{
						case 0: case 1: return "INTEL Pentium IV (0.18 µm)";
						case 2: return "INTEL Pentium IV (0.13 µm)";
						case 3: return "INTEL Pentium IV (0.09 µm)";
						}
					}
					break;
				case 1: return "INTEL Itanium 2 (IA-64)";
				}
			}
			break;			
		}
	}
	else if(vendor.compare("AuthenticAMD") == 0)
	{
		switch(family)
		{
		case 4:
			{
				switch(model)
				{
				case 3:return "AMD 486 DX2";
				case 7:return "AMD 486 DX2-WB";
				case 8:return "AMD 486 DX4";
				case 9:return "AMD 486 DX4-WB";
				case 14:return "AMD Am5x86-WT";
				case 15:return "AMD Am5x86-WB";
				}
			}
			break;
		case 5:
			{
				switch(model)
				{
				case 0: return "AMD K5 SSA5";
				case 1: case 2: case 3: return "AMD K5";
				case 6: case 7: return "AMD K6";
				case 8: return "AMD K6-2";
				case 9: return "AMD K6-3";
				case 13: return "AMD K6-3+";
				}
			}
			break;
		case 6:
			{
				switch(model)
				{
				case 0: case 1: return "AMD Athlon (25um)";
				case 2: return "AMD Athlon (18um)";
				case 3: return "AMD Duron";
				case 4: return "AMD Athlon Thunderbird";
				case 6: return "AMD Athlon Palamino";
				case 7: return "AMD Duron Morgan";
				case 8: return "AMD Athlon Thoroughbred";
				case 10: return "AMD Athlon Barton";
				}
			}
			break;
		case 15:
			{
				switch(family_ex)
				{
				case 0:
					{
						switch(model_ex)
						{
						case 4: return "AMD Athlon 64";
						case 5: return "AMD Athlon 64FX Operon";
						}
					}
				}
			}
		}
	}
	else if(vendor.compare("CyrixInstead") == 0)
		return "Cyrix";
	else if(vendor.compare("CentaurHauls") == 0)
		return "Centaur";

	return "unknown";
}
