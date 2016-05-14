#include "stdafx.h"
#include ".\SimpleDivX_Defines.h"
#include ".\simpledivxbatch.h"

IMPLEMENT_SERIAL( CSimpleDivXBatch, CObject, 1 )

CSimpleDivXBatch::CSimpleDivXBatch(void)
{
	MyList.RemoveAll();
}

CSimpleDivXBatch::~CSimpleDivXBatch(void)
{
}

void CSimpleDivXBatch::CopyIt(const CSimpleDivXBatch& e)
{
	int i = 0;
	Enc_Info = e.Enc_Info;
	POSITION pos;

	MyList.RemoveAll();
	for (i = 0 ; i < e.MyList.GetCount(); i++)
	{
		pos = e.MyList.FindIndex(i);
		MyList.SetAt(pos, (CEncodingInfo) e.MyList.GetAt(pos));
	}			
}

int CSimpleDivXBatch::ClearEncList(void)
{
	MyList.RemoveAll();
	return 0;
}

int CSimpleDivXBatch::GetNum()
{
	return MyList.GetCount();
}

int CSimpleDivXBatch::RemoveEncInfo(int idx)
{
	MyList.RemoveAt(MyList.FindIndex(idx));
	return 0;
}

int CSimpleDivXBatch::AddEncInfo(CEncodingInfo& Enc)
{
	MyList.AddTail(Enc);
	return 0;
}

CEncodingInfo& CSimpleDivXBatch::GetEncInfoAt(int idx)
{
	return MyList.GetAt(MyList.FindIndex(idx));
}

CEncodingInfo& CSimpleDivXBatch::GetEncInfo(void)
{
	return Enc_Info;
}

int CSimpleDivXBatch::SetEncInfoAt(int idx, CEncodingInfo& Enc)
{
	MyList.SetAt(MyList.FindIndex(idx), Enc);
	return 0;
}

int CSimpleDivXBatch::SetEncInfo(CEncodingInfo& Enc)
{
	Enc_Info = Enc;
	return 0;
}

int CSimpleDivXBatch::Swap(int idx_1, int idx_2)
{
	CEncodingInfo Temp = (CEncodingInfo) MyList.GetAt(MyList.FindIndex(idx_1));

	MyList.SetAt(MyList.FindIndex(idx_1), 
		(CEncodingInfo) MyList.GetAt(MyList.FindIndex(idx_2)));

	MyList.SetAt(MyList.FindIndex(idx_2), Temp);

	return 0;
}

void CSimpleDivXBatch::Serialize(CArchive& ar)
{
	int fileValid = 0;
	int i = 0;

	CObject::Serialize( ar );

	CString AppVersion = APP_VERSION;
	
	if( ar.IsStoring() )
	{
		if (MyList.GetCount() > 0)
		{
			ar << AppVersion;
		
			MyList.Serialize(ar);
		}
	}
	else
	{
		TRY
		{
			ar >> AppVersion;
			
			MyList.Serialize(ar);
		}
		CATCH(CArchiveException, pEx) 
		{ 
			// invalid;
		}
		END_CATCH
	}
}

template <> void AFXAPI SerializeElements <CEncodingInfo> ( CArchive& ar, 
    CEncodingInfo* pNewEncInfos, INT_PTR nCount )
{
    for ( int i = 0; i < nCount; i++, pNewEncInfos++ )
    {
        // Serialize each CEncodingInfo object
        pNewEncInfos->Serialize( ar );
    }
}