#ifndef _LNG_H
#define _LNG_H

#include "SimpleDivX_Defines.h"

typedef struct _langattribute
{
	int page; // PAGE_INPUT
	int enum_value; // IDC_STATIC_INPUT_FILES
	CString s_attribute_name; // "INPUT_FILES"
	CString s_default_value; // ENGLISH
	CString s_value; // the text
} LANG_ATTRIBUTE;

typedef struct _internal_langattribute
{
	CString &theString; //
	CString s_attribute_name; // "INPUT_FILES"
	CString s_default_value; // ENGLISH
} INTERNAL_LANG_ATTRIBUTE;

class CLanguageSupport 
{
public:
	CList<CString,CString&>& GetLanguageNames();
	int ChangeLanguage(CString);
	void InitAttribs(CWnd* p_window, int page);
	CLanguageSupport();
	
	bool Init(CWinApp *pApplication, CString);
	virtual ~CLanguageSupport();

	/* Global string which are translated ... */
	CString strStatusBusy;
	CString strStatusReady;
	CString strVideoOnly;
	CString strVideoAndAudio;
	CString strAudioFormat;
	CString strNoAudioFormat;
	CString str1PassVideo;
	CString str2PassVideo;
	CString strNoAudioFound;

private:
	
	void FreeAttribs(void);
	int i_num_attribs;
	int i_num_languages;
	int i_num_tabs;
	bool b_language_change_allowed;
	CString currentLanguage;
	char *as_page_language[_PAGE_LAST];
	//CSortStringArray *pLanguages;
	char s_current_dir[STRLEN];

	CList<CString,CString&> myList;
	
	int ChangeInternalLanguageStrings(CString language);

	CWinApp *pApplication;
};

#endif