#ifndef ___MYREG_H_INCLUDED___
#define ___MYREG_H_INCLUDED___

#include "SimpleDivX_Defines.h"
#include "EncodingInfo.h"
#include "ProgramInfo.h"

int OpenSimpleDivXRegKey(HKEY *phnd);
int CloseSimpleDivXRegKey(HKEY *phnd);

int Read_SimpleDivXSettings(CProgramInfo&, CEncodingInfo&);
int Write_SimpleDivXSettings(CProgramInfo&, CEncodingInfo&);

#endif