#pragma once

#include "SimpleDivX_defines.h"
#include "EncodingInfo.h"

class CSimpleDivXBatch : public CObject
{
	DECLARE_SERIAL( CSimpleDivXBatch )

public:
	CSimpleDivXBatch(void);
	~CSimpleDivXBatch(void);

	void Serialize(CArchive& ar);

	void CopyIt(const CSimpleDivXBatch& e);

	CSimpleDivXBatch( const CSimpleDivXBatch& e )
	{
		CopyIt(e);
	}

	const CSimpleDivXBatch& operator=( const CSimpleDivXBatch& e )
    {
		CopyIt(e);
        	 
		return *this;
    }

	int Swap(int idx_1, int idx_2);
	int GetNum();
	
	
	CEncodingInfo& GetEncInfo(void);
	CEncodingInfo& GetEncInfoAt(int idx);
	int AddEncInfo(CEncodingInfo& Enc);
	int RemoveEncInfo(int idx);
	int ClearEncList(void);
	int SetEncInfo(CEncodingInfo& Enc);
	int SetEncInfoAt(int idx, CEncodingInfo& Enc);
	

private:
	CEncodingInfo Enc_Info; // used for normal mode
	CList<CEncodingInfo,CEncodingInfo&> MyList;

protected:
};
