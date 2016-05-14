#include "stdafx.h"
#include <afx.h>
#include <afxtempl.h>
#include ".\utils.h"

void Utils::SortList(CList<CString,CString&>& mylist)
{
	if (mylist.IsEmpty()) return;

	bool bChanged = true;

	while (bChanged)
	{
		bChanged = false;

		POSITION pos = mylist.GetHeadPosition();
		POSITION pos_save;

		while (pos != NULL)
		{
			pos_save = pos;
			CString i1 = mylist.GetNext(pos);
			if (pos != NULL)
			{
				CString i2 = mylist.GetAt(pos);
				if (i2 < i1) // need to switch
				{
					mylist.SetAt(pos_save,i2);
					mylist.SetAt(pos,i1);
					bChanged = true;
				}
			}
		}
	}
}