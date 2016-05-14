#pragma once
#include "afx.h"
#include "ProgramInfo.h"
#include "SimpleDivXBatch.h"

class CConfigData : public CObject
{
	DECLARE_SERIAL( CConfigData )

private:
	CSimpleDivXBatch Batch_Info;

public:
	~CConfigData(void);
	CConfigData(void);
	void Serialize(CArchive& ar);

	CSimpleDivXBatch* GetBatchInfo();

	void Write_Settings(void);
	void Read_Settings(void);
	int status;
};
