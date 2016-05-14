#include "stdafx.h"
#include ".\configdata.h"
#include "SimpleDivX_Defines.h"

#include "SimpleDivXBatch.h"
#include "EncodingInfo.h"
#include "ProgramInfo.h"

IMPLEMENT_SERIAL( CConfigData, CObject, 1 )

#define  SIMPLEDIVXDATA		"SimpleDivX.dat"

CConfigData::CConfigData(void)
{
	status = 0;
}

CConfigData::~CConfigData(void)
{
}

CSimpleDivXBatch* CConfigData::GetBatchInfo()
{
	return &Batch_Info;
}

void CConfigData::Serialize(CArchive& ar)
{
	CObject::Serialize( ar );

	if( ar.IsStoring() )
	{
		TRY
		{
			Batch_Info.Serialize(ar);
		}
		CATCH(CArchiveException, pEx) 
		{ 
			// invalid;
		}
		END_CATCH
	}
	else
	{
		TRY
		{
			//Prog_Info.Serialize(ar);
			Batch_Info.Serialize(ar);
		}
		CATCH(CArchiveException, pEx) 
		{ 
			status = 1;
		}
		END_CATCH
	}
}


void CConfigData::Write_Settings(void)
{   
	CFile theFile;
	CFileException e;

	theFile.Open(SIMPLEDIVXDATA, CFile::modeCreate | CFile::modeWrite, &e);
		
	CArchive ar(&theFile, CArchive::store);

	Serialize(ar);
	    
	ar.Close();
	theFile.Close();
}

void CConfigData::Read_Settings(void)
{
	CFile theFile;
	BOOL fileStatus = theFile.Open(SIMPLEDIVXDATA, CFile::modeRead);

	if (fileStatus)
	{
		CArchive ar(&theFile, CArchive::load);

		TRY
		{
			Serialize(ar);
		}
		CATCH(CFileException, pEx) 
		{ 
			// invalid;
			status = -1;
		}
		END_CATCH
		    
		ar.Close();
		theFile.Close();
	}
	else
	{
		status = 2;
	}
}