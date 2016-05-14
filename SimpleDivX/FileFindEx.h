#pragma once
#pragma warning(push)
#pragma warning(disable : 4018)
#pragma warning(disable : 4389)
#include "atlrx.h"			// ATL regex
#pragma warning(pop)

// CFileFindEx command target
class CFileFindEx : public CFileFind, CAtlRegExp<>
{
public:
	CFileFindEx();
	virtual ~CFileFindEx();

	virtual BOOL FindFile(LPCTSTR csFilePath = _T(""), LPCTSTR csIncludeFilters = _T(""),
		LPCTSTR csExcludeFilters = _T(""), BOOL bRegularExpression = false);
	virtual BOOL FindNextFile();
	static LPCTSTR GetErrorString(REParseError err);
	CString GetParseError() {return GetErrorString(m_RegExParseError);};
	CString FirstFilter(CString csFilter);
	CString NextFilter();
	CString GetFolderOnly(LPCTSTR csPath);
	CString AddPathAndFile(LPCTSTR csPath, LPCTSTR csFile);

protected:
	CString m_csIncludeFilters;
	CString m_csExcludeFilters;
	CString m_csFilter;
	CFileFind m_Finder;
	int m_nCount;
	int m_nIndex;
	BOOL m_bRegularExpression;
	BOOL m_bUsingFilters;
	BOOL m_bWorked;
	BOOL CheckFileForMatch();
	REParseError m_RegExParseError;	
	CString ConvertToRegex(LPCTSTR csPattern);
	void ParseFilters();
	CStringArray m_csIncludeFiltersParsed;
	CStringArray m_csExcludeFiltersParsed;
	UINT m_nIncludeFilters;
	UINT m_nExcludeFilters;
};
