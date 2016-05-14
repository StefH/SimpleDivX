// FileFindEx.cpp : implementation file
//
#include "stdafx.h"
#include "FileFindEx.h"

// Revision History
// 
// September 19, 2005 - Version 1 - Initial release
// September 22, 2005 - Fixed problem with recalling FindFile() after traversing 
//						list.
// September 23, 2005 - Fixed so the class and sample will compile either MBCS or
//						Unicode.

// CFileFindEx
CFileFindEx::CFileFindEx()
{
	m_nIncludeFilters = 0;
	m_nExcludeFilters = 0;
	m_RegExParseError = REPARSE_ERROR_OK;
	m_nCount = 1;
}

CFileFindEx::~CFileFindEx()
{
	m_csIncludeFiltersParsed.RemoveAll();
	m_csExcludeFiltersParsed.RemoveAll();
}

// CFileFindEx member functions

// FindFile(LPCTSTR csFilePath, LPCTSTR csIncludeFilters, LPCTSTR csExcludeFilters, BOOL bRegularExpression)
// See if there are any files that match our criteria. The filespec passed in
// will be either a normal DOS fspec:
//    drive:\path\*.cpp 
// or
//    drive:\path\[filespec not used]
//    csIncludeOrRegExFilters passed in (format: *.cpp|*.h|*.rc or [regex])
//    csExcludeFilters passed in (format: xyz*.cpp|xyz*.h|xyz*.rc)
//
// This function calls the base function to see if there are any files to check
// then checks the return against our patterns and returns true if the file matches.
//
BOOL CFileFindEx::FindFile(LPCTSTR csFilePath, LPCTSTR csIncludeFilters, LPCTSTR csExcludeFilters, BOOL bRegularExpression)
{
	CString csSearchPath, csFilters, csOriginalName;

	// Set initially to one in case they recall the function
	m_nCount = 1;

	csOriginalName = csFilePath; // Convert to CString
	if(csOriginalName.IsEmpty())
		return false; // No path passed in

	m_csIncludeFilters = csIncludeFilters;
	m_csExcludeFilters = csExcludeFilters;
	m_bRegularExpression = bRegularExpression;

	if(m_csIncludeFilters.IsEmpty() && m_csExcludeFilters.IsEmpty()) {
		// No filters passed in so use path as is
		csSearchPath = csOriginalName;
		m_bUsingFilters = false;
	}
	else {
		CFileStatus fFileInfo;
		m_bUsingFilters = true;
		if(!::PathIsDirectory(csOriginalName)) {
			// They passed in a file so don't filter
			csSearchPath = csOriginalName;
			m_bUsingFilters = false;
		}
		if(m_bUsingFilters) {
			// Split the path and the fspec into two pieces if they passed in file spec.
			if(csOriginalName.FindOneOf(_T("?*")) != -1)
		        csOriginalName = GetFolderOnly(csOriginalName);
			// Add *.* to the path because we want to sift through all the files
			csSearchPath = AddPathAndFile(csOriginalName,_T("*.*"));
		}
	}

	// Get the filters to use from the two strings passed in.
	ParseFilters();

    // Loop through files in the current folder until we find one that matches
	// If anything matches in the search path
	if(CFileFind::FindFile(csSearchPath)) {
		m_Finder.FindFile(csSearchPath); // Look ahead version
		m_bWorked = m_Finder.FindNextFile();
		if(CheckFileForMatch())
			return true; // The file is going to match so return

		// Otherwise let's keep looking if there are more files
		while(m_bWorked) {
			CFileFind::FindNextFile(); // Stay in sync
			m_bWorked = m_Finder.FindNextFile();
			if(CheckFileForMatch())
				return true; // The file is going to match so return
		}
	}
	return false; // Didn't find a name that matches or a folder
}

// FindNextFile()
// See if there are any more files to match
// This function calls the base function to see if there are more files to check
// then checks the return against our patterns and returns true if the file matches.
BOOL CFileFindEx::FindNextFile( )
{
	// We kept count of how many files we had to skip to get to one that
	// actually matches are criteria.  Jump past them in our buffer version.
	while(m_nCount) {
		// Skip file files that didn't match
		CFileFind::FindNextFile();
		--m_nCount;
	}

	// No more files after the one we just found so no sense it checking
	// again even if they call the routine again... just remind them.
	if(!m_bWorked)
		return false; // No more files to check

	// Now see if there will be any subsequent matches.  Keep track of
	// the count so we can catch up when/if the user actually wants more
	// files.
	while(m_bWorked) {
		++m_nCount;
		m_bWorked = m_Finder.FindNextFile();
		if(CheckFileForMatch())
			return true; // The file is going to match so return
	}
	return false; // No more matches after the current one
}

// CheckFileForMatch()
// Check the passed in file to see if it matches any of our filters.  Loops through the
// filters that were on the original path.  This routine uses ATL regular expressions which
// are very limited, but OK for what we're doing here.  Another engine like boost could be 
// used instead with minor changes.
BOOL CFileFindEx::CheckFileForMatch()
{
	// If not using filters or this file is a folder return true without
	// checking patterns
	// WHY ALLOW IsDirectory ????
	/*
	if(!m_bUsingFilters || m_Finder.IsDirectory())
		return true; // Use them all*/

	if(!m_bUsingFilters)
		return true; // Use them all

	CString csFile = m_Finder.GetFileName();

	// Using ATL Regex stuff do the parse and match to see if it works
	CAtlREMatchContext<> matchContext; // match context for parse

	// Look through Include list
	BOOL bMatch = m_nIncludeFilters == 0; // True if no include filters to include everything
	for(UINT i=0;i < m_nIncludeFilters; ++i) {
		CString cs = m_csIncludeFiltersParsed.GetAt(i);
		m_RegExParseError = Parse(cs.GetBuffer(),false);
		if(m_RegExParseError == REPARSE_ERROR_OK) {
			if(Match(csFile, &matchContext, NULL)) {
				bMatch = true; // Found it
				break;
			}
		}
		else {
			CString csError;
			csError.Format(_T("Error on regex %s:%s"),cs,GetParseError());
			AfxMessageBox(csError);
		}
	}

	// If it matches make sure it's not in the exclude list
	for(UINT i=0; i < m_nExcludeFilters; ++i) {
		CString cs = m_csExcludeFiltersParsed.GetAt(i);
		m_RegExParseError = Parse(cs.GetBuffer(),false);
		if(m_RegExParseError == REPARSE_ERROR_OK) {
			if(Match(csFile, &matchContext, NULL)) {
				bMatch = false; // Oops matches exclude list
				break;
			}
		}
		else {
			CString csError;
			csError.Format(_T("Error on regex %s:%s"),cs,GetParseError());
			AfxMessageBox(csError);
		}
	}
	return bMatch;
}

// ConvertToRegex(LPCTSTR csPattern)
// Convert a filespec to a regular expression.  For example *.cpp becomes ^.*\.cpp$
CString CFileFindEx::ConvertToRegex(LPCTSTR csPattern)
{
	CString csRegex;
	CString cs = csPattern;

	csRegex += _T("^"); // Match at beginning of line
	for(int i=0; i < cs.GetLength(); ++i) {
		if(cs[i] == _TCHAR('*'))
			csRegex += _T(".*?"); // 0 or more chars in this position
		else if(cs[i] == _TCHAR('.'))
			csRegex += _T("\\."); // extension dot
		else if(cs[i] == _TCHAR('?'))
			csRegex += _T("."); // One char in this position
		else
			csRegex += cs[i];
	}
	csRegex += _T("$"); // Match at end of line as well making this a whole word
	return csRegex;
}

// ParseFilters()
// Split apart the filters and convert each to a regular expression (unless they already are)
void CFileFindEx::ParseFilters()
{
	CString cs;
	UINT nFilters = 0;

	// Clear current buffers (just in case the same class is reused.
	m_csIncludeFiltersParsed.RemoveAll();
	m_csExcludeFiltersParsed.RemoveAll();
	m_nIncludeFilters = 0;
	m_nExcludeFilters = 0;

	// Parse Include Filters (if any passed in)
	if(!m_csIncludeFilters.IsEmpty()) {
		// Parse 
		cs = FirstFilter(m_csIncludeFilters);
		while(!cs.IsEmpty()) {
			// Split up the filters and convert based on match type can be both INCLUDE and EXCLUDE
			if(!m_bRegularExpression)
				cs = ConvertToRegex(cs);
			m_csIncludeFiltersParsed.Add(cs);
			++m_nIncludeFilters;
			cs = NextFilter();
		}
	}

	// Parse Exclude Filters (if any passed in).  If they are using regular expressions then
	// exclude filters are not needed (they can be done in the regex string).
	if(!m_csExcludeFilters.IsEmpty()) {
		// Parse 
		cs = FirstFilter(m_csExcludeFilters);
		while(!cs.IsEmpty()) {
			if(!m_bRegularExpression)
				cs = ConvertToRegex(cs);
			m_csExcludeFiltersParsed.Add(cs);
			++m_nExcludeFilters;
			cs = NextFilter();
		}
	}
}

// GetErrorString(REParseError err)
// Get string name of ATL REParseError error code.
LPCTSTR CFileFindEx::GetErrorString(REParseError err)
{
	static struct {
		REParseError err;
		LPCTSTR name;
	} ATLErrs[] = {
		{REPARSE_ERROR_OK,				_T("Worked OK")},
		{REPARSE_ERROR_OUTOFMEMORY,		_T("Out of memory")},
		{REPARSE_ERROR_BRACE_EXPECTED,	_T("Brace expected")},
		{REPARSE_ERROR_PAREN_EXPECTED,	_T("Paren expected")},
		{REPARSE_ERROR_BRACKET_EXPECTED,_T("Bracket expected")},
		{REPARSE_ERROR_UNEXPECTED,		_T("Unexpected syntax")},
		{REPARSE_ERROR_EMPTY_RANGE,		_T("Empty range")},
		{REPARSE_ERROR_INVALID_GROUP,	_T("Invalid group")},
		{REPARSE_ERROR_INVALID_RANGE,	_T("Invalid range")},
		{REPARSE_ERROR_EMPTY_REPEATOP,	_T("Invalid repeator")},
		{REPARSE_ERROR_INVALID_INPUT,	_T("Invalid input")},
		{REPARSE_ERROR_OK, NULL}
	};
	for (int i=0; ATLErrs[i].name; i++) {
		if (err == ATLErrs[i].err)
			return ATLErrs[i].name;
	}
	return _T("Unknown error");
}

// FirstFilter(CString csFilter)
// Parse through filter list.  This function returns the first filter in the list.
CString CFileFindEx::FirstFilter(CString csFilter)
{
	CString cs;
	
	m_csFilter = csFilter;
	if(m_csFilter.IsEmpty()) {
		cs = _T("*.*"); // Need all files
		m_nIndex = -1;
	}
	else {
		m_nIndex = m_csFilter.FindOneOf(_T("|"));
		if(m_nIndex == -1)
			cs = m_csFilter; // Only one filter
		else {
			cs = m_csFilter.Left(m_nIndex);
			++m_nIndex; // Past the token
		}
	}
	// Get first filter in the list
	return cs;
}

// NextFilter()
// Continue working through list if there are more filters.
CString CFileFindEx::NextFilter()
{
	// Get next filter in list or return NULL if no more filters
	if(m_nIndex == -1)
		return _T(""); // No more to find

	CString cs;
	int nIndex;

	cs = m_csFilter.Mid(m_nIndex);
	nIndex = cs.FindOneOf(_T("|"));
	if(nIndex != -1) {
		cs = cs.Left(nIndex);
		m_nIndex += nIndex + 1;
	}
	else
		m_nIndex = -1; // Last one found
	return cs;
}

//  GetFolderOnly(LPCTSTR csPath)
//	Strip off the file name so we can direct the file scanning dialog to go
//	back to the same directory as before.
CString CFileFindEx::GetFolderOnly(LPCTSTR csPath)
{
	CString temp = csPath;
	::PathRemoveFileSpec(temp.GetBuffer(0));
	temp.ReleaseBuffer(-1);
	return temp;
}

//  AddPathAndFile(LPCTSTR csPath, LPCTSTR csFile)
//	Strip off the file name so we can direct the file scanning dialog to go
//	back to the same directory as before.
CString CFileFindEx::AddPathAndFile(LPCTSTR csPath, LPCTSTR csFile)
{
	CString cs = csPath;
	::PathAddBackslash(cs.GetBuffer(_MAX_PATH));
	::PathAppend(cs.GetBuffer(_MAX_PATH),csFile);
	cs.ReleaseBuffer(-1);
	return cs;
}

