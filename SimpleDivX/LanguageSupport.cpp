#include "stdafx.h"
#include "resource.h"
#include "SimpleDivX_Defines.h"
#include "Utils.h"
#include "Mytooltips.h"
#include "LanguageSupport.h"
#include ".\allcontrolssheet.h"

#define NOLANGDEFINED	"not defined"

// For general use :
#define FRAMES_KEY "FRAMES"
#define FRAMES_VALUE "Frames"
#define WIDTH_KEY "WIDTH"
#define WIDTH_VALUE "width ="
#define HEIGHT_KEY "HEIGHT"
#define HEIGHT_VALUE "height ="
#define TWO_CHANNELS_KEY "TWO_CHANNELS"
#define TWO_CHANNELS_VALUE "(2 channels)"
#define MOVIE_LENGTH_KEY "MOVIE_LENGTH"
#define MOVIE_LENGTH_VALUE "Movie Length"
#define LANGUAGE_KEY "LANGUAGE"
#define LANGUAGE_VALUE "Language"

LANG_ATTRIBUTE ar_tab_names[] = {
	{PAGE_INPUT, 0, "TAB_INPUT", "Input", ""},
	{PAGE_VIDEO, 0, "TAB_VIDEO", "Video", ""},
	{PAGE_AUDIO, 0, "TAB_AUDIO", "Audio", ""},
	{PAGE_CODEC, 0, "TAB_CODEC", "Codec", ""},
	{PAGE_COMPRESSION, 0, "TAB_COMPRESSION", "Compression", ""},
	{PAGE_MISC, 0, "TAB_MISC", "Misc", ""},
	{PAGE_PROJECT, 0, "TAB_PROJECT", "Project", ""},
	{PAGE_OUTPUT, 0, "TAB_OUTPUT", "Output", ""},
	{PAGE_SETUP, 0, "TAB_SETUP", "Setup", ""},
	{PAGE_GENERAL, 0, "QUIT", "Quit", ""}
};

LANG_ATTRIBUTE ar_lang_data[] = {
		{PAGE_INPUT, IDC_STATIC_INPUT_FILES, "INPUT_FILES", "Input Files", ""},
		{PAGE_INPUT, IDC_STATIC_FOLDER, "FOLDER", "Folder", ""},
		{PAGE_INPUT, IDC_STATIC_VOB_FILES, "VOB_FILES", "VOB / TS / IFO Files", ""},
		{PAGE_INPUT, IDC_STATIC_CHECK_MAIN, "CHECK_MAIN", "Select Main Movie", ""},
		{PAGE_INPUT, IDC_STATIC_CHECK_ALL, "CHECK_ALL", "Select All Files", ""},
		{PAGE_INPUT, IDC_STATIC_VIDEO_INFORMATION, "VIDEO_INFORMATION", "Video Information", ""},
		{PAGE_INPUT, IDC_STATIC_RES, "RESOLUTION", "Resolution" , ""},
		{PAGE_INPUT, IDC_STATIC_MOVIE_LENGTH, MOVIE_LENGTH_KEY, MOVIE_LENGTH_VALUE, ""},
		{PAGE_INPUT, IDC_STATIC_FRAMERATE, "FRAME_RATE", "Framerate", ""},
		{PAGE_INPUT, IDC_STATIC_ASPECT, "ASPECT_RATIO", "Aspect Ratio", ""},
		{PAGE_INPUT, IDC_STATIC_VF, "VIDEO_FORMAT", "Video Format", ""},
		{PAGE_INPUT, IDC_STATIC_CHAP, "CHAPTERS", "Chapters", ""},
		{PAGE_INPUT, IDC_STATIC_TITLES, "TITLE", "Choose Title", ""},

		{PAGE_VIDEO, IDC_STATIC_FRAMES_PER_SECOND, "FRAMES_PER_SECOND", "Frames per second", ""},
		{PAGE_VIDEO, IDC_BUTTON_ADVANCED, "ADVANCED", "Advanced", ""},
		{PAGE_VIDEO, IDC_CHECK_AUTO, "CHECK_AUTO", "Auto cropping and resizing" , ""},
		{PAGE_VIDEO, IDC_STATIC_W1, WIDTH_KEY, WIDTH_VALUE, ""},
		{PAGE_VIDEO, IDC_STATIC_W2, WIDTH_KEY, WIDTH_VALUE, ""},
		{PAGE_VIDEO, IDC_STATIC_W3, WIDTH_KEY, WIDTH_VALUE, ""},
		{PAGE_VIDEO, IDC_STATIC_H1, HEIGHT_KEY, HEIGHT_VALUE, ""},
		{PAGE_VIDEO, IDC_STATIC_H2, HEIGHT_KEY, HEIGHT_VALUE, ""},
		{PAGE_VIDEO, IDC_VIDEOPREVIEW, "PREVIEW", "Preview", ""},
		{PAGE_VIDEO, IDC_STATIC_RESIZING, "RESIZING", "Resizing", ""},
		{PAGE_VIDEO, IDC_STATIC_CROPPING, "CROPPING", "Cropping", ""},
		{PAGE_VIDEO, IDC_STATIC_CUSTOM_OUTPUT_FORMAT, "CUSTOM_OUTPUT_FORMAT", "Custom Output Format", ""},
		{PAGE_VIDEO, IDC_STATIC_VIDEO_OPTIONS, "VIDEO_OPTIONS", "Video Options", ""},
		{PAGE_VIDEO, IDC_STATIC_FRAME_OPTIONS, "FRAME_OPTIONS", "Frame Options", ""},
		{PAGE_VIDEO, IDC_CHECK_LBA, "OPEN_VOB_LBA", "Open VOB bitstream at LBA", ""},
		{PAGE_VIDEO, IDC_CHECK_FRAMERANGE, "FRAME_RANGE", "Range from", ""},
		{PAGE_VIDEO, IDC_STATIC_TO, "TO", "to", ""},
		{PAGE_VIDEO, IDC_STATIC_SOURCE_FORMAT, "SOURCE_FORMAT", "Source Format", ""},
		{PAGE_VIDEO, IDC_STATIC_DEINTERLACE, "DEINTERLACE", "Inverse telecine / De-interlace", ""},

		{PAGE_AUDIO, IDC_STATIC_GENERAL_AUDIO, "GENERAL_AUDIO", "General Audio", ""},
		{PAGE_AUDIO, IDC_CREATE_AUDIO, "CREATE_AUDIO", "Create Audio", ""},
		{PAGE_AUDIO, IDC_STATIC_AUDIO_STREAMS_LANGUAGES, "AUDIO_STREAMS_LANGUAGES", "Audio Streams and Languages", ""},
		{PAGE_AUDIO, IDC_CHECK_AUDIO, "CHECK_AUDIO", "Check Audio", ""},
		
		{PAGE_AUDIO, IDC_STATIC_AUDIOSTRID, LANGUAGE_KEY, LANGUAGE_VALUE, ""},
		{PAGE_AUDIO, IDC_STATIC_AUDIO_FORMAT, "AUDIO_FORMAT", "Audio Format", ""},
		{PAGE_AUDIO, IDC_STATIC_MP3_CHANNELS, TWO_CHANNELS_KEY, TWO_CHANNELS_VALUE, ""},
		{PAGE_AUDIO, IDC_STATIC_OGG_CHANNELS, TWO_CHANNELS_KEY, TWO_CHANNELS_VALUE, ""},
		{PAGE_AUDIO, IDC_RADIO_RECOMPRESSAC3, TWO_CHANNELS_KEY, TWO_CHANNELS_VALUE, ""},
		{PAGE_AUDIO, IDC_RADIO_NORECOMPRESSAC3, "SIX_CHANNELS", "(6 channels)", ""},
		{PAGE_AUDIO, IDC_STATIC_AUDIO_BITRATE_AND_CONVERSION, "AUDIO_BITRATE_AND_CONVERSION", "AudioBitrate and Conversion", ""},
		{PAGE_AUDIO, IDC_STATIC_AUDIO_KBPS, "AUDIO_KBPS", "Bitrate (kbps)", ""},
		{PAGE_AUDIO, IDC_STATIC_CONV, "SAMPLE_RATE_CONVERSION", "Samplerate Conversion", ""},
		{PAGE_AUDIO, IDC_CHECK_WAVEUP, "AMPLIFY", "Amplify", ""},
		{PAGE_AUDIO, IDC_CHECK_NORM, "NORMALIZE", "Peak level normalize.", ""},
		{PAGE_AUDIO, IDC_CHECK_NORM2, "NORMALIZE_MIDNIGHT", "Peak level normalize. (Midnight Mode)", ""},

		{PAGE_CODEC, IDC_STATIC_CODEC_OPTIONS, "CODEC_OPTIONS", "Codec Options", ""},
		{PAGE_CODEC, IDC_STATIC_DIVX_OPTIONS, "DIVX_OPTIONS", "DivX Options", ""},
		{PAGE_CODEC, IDC_STATIC_CHOOSE_CODEC, "CHOOSE_CODEC", "Choose the codec you want to use", ""},
		{PAGE_CODEC, IDC_ADV_SET, "ADVANCED_SETTINGS", "Advanced Settings", ""},
		{PAGE_CODEC, IDC_STATIC_MAX_KF, "MAX_KEYFRAME", "Maximum Key-Frame", ""},
		{PAGE_CODEC, IDC_STATIC_FIRST_PASS_LOCATION, "FIRST_PASS_LOCATION", "First-Pass log file location", ""},
		{PAGE_CODEC, IDC_RADIO_AUTO_LOGFILENAMING, "AUTO_LOGFILENAMING", "Automatic (Logfile will be named after the Project Name.)", ""},
		{PAGE_CODEC, IDC_RADIO_MANUAL_LOGFILENAMING, "MANUAL_LOGFILENAMING", "Manual", ""},
		{PAGE_CODEC, IDC_STATIC_PASS_OPTIONS, "PASS_OPTIONS", "Pass Options", ""},
		{PAGE_CODEC, IDC_PASS_0, "ONLY_FIRST_PASS", "Do only first pass.", ""},
		{PAGE_CODEC, IDC_PASS_1, "ONLY_SECOND_PASS", "Do only second pass.", ""},
		{PAGE_CODEC, IDC_PASS_2, "BOTH_PASSES", "Do both passes.", ""},
		{PAGE_CODEC, IDC_STATIC_FRAMES, FRAMES_KEY, FRAMES_VALUE, ""},

		{PAGE_COMPRESSION, IDC_STATIC_BITRATE_FILE_OPTIONS, "BITRATE_FILE_OPTIONS", "Bitrate / File Options", ""},
		{PAGE_COMPRESSION, IDC_STATIC_END_CREDITS_OPTIONS, "END_CREDITS_OPTIONS", "End-Credits Options", ""},
		{PAGE_COMPRESSION, IDC_RADIO20, "ENTER_BITRATE", "Enter bitrate", ""},
		{PAGE_COMPRESSION, IDC_RADIO21, "ENTER_FINAL_FILESIZE", "Enter final movie size", ""},
		{PAGE_COMPRESSION, IDC_GET_TIME, "GET_TIME", "Get Time from Movie.", ""},
		{PAGE_COMPRESSION, IDC_STATIC_MOVIE_LENGTH_CPAGE, MOVIE_LENGTH_KEY, MOVIE_LENGTH_VALUE, ""},
		{PAGE_COMPRESSION, IDC_STATIC_FILESIZE, "FILESIZE", "File size", ""},
		{PAGE_COMPRESSION, IDC_STATIC_VIDEO_BITRATE, "VIDEO_BITRATE", "Bitrate", ""},
		{PAGE_COMPRESSION, IDC_STATIC_DESCRIPTION, "DESCRIPTION", "Description", ""},
		{PAGE_COMPRESSION, IDC_STATIC_HOURS, "HOURS", "Hour(s)", ""},
		{PAGE_COMPRESSION, IDC_STATIC_MINUTES, "MINUTES", "Minute(s)", ""},
		{PAGE_COMPRESSION, IDC_STATIC_SECONDS, "SECONDS", "Second(s)", ""},
		{PAGE_COMPRESSION, IDC_RADIO_END1, "ENDCREDITS_NORMAL", "Normal", ""},
		{PAGE_COMPRESSION, IDC_RADIO_END2, "ENDCREDITS_REMOVE", "Do not include end-credits in movie.", ""},
		{PAGE_COMPRESSION, IDC_RADIO_END3, "ENDCREDITS_RECOMPRESS", "Recompress end-credits at", ""},
		{PAGE_COMPRESSION, IDC_STATIC_RECOMPRESS_PCT, "RECOMPRESS_PCT", "% of the bitrate from main movie.", ""},
		{PAGE_COMPRESSION, IDC_RADIO_AUTO, "ENDCREDITS_AUTOMATIC", "Automatic detection.", ""},
		{PAGE_COMPRESSION, IDC_RADIO_MANUAL, "ENDCREDITS_MANUAL", "Manual, end-credits start at", ""},
		{PAGE_COMPRESSION, IDC_STATIC_FRAMES_CPAGE, FRAMES_KEY, FRAMES_VALUE, ""},
		{PAGE_COMPRESSION, IDC_STATIC_FRAMES_CPAGE2, FRAMES_KEY, FRAMES_VALUE, ""},

		{PAGE_MISC, IDC_STATIC_GENERAL_SUBTITLE, "GENERAL_SUBTITLES", "General Subtitle Options"},
		{PAGE_MISC, IDC_CREATE_SUBTITLES, "CREATE_SUBTITLES", "Create Subtitles using VobSub"},
		{PAGE_MISC, IDC_STATIC_SUBTITLESTRID, LANGUAGE_KEY, LANGUAGE_VALUE, ""},
		{PAGE_MISC, IDC_CHECK_ALL_LANG, "SELECT_ALL_LANGUAGES", "Select all languages", ""},
		{PAGE_MISC, IDC_STATIC_CONTAINER, "CONTAINER_FORMAT", "Movie Container Format", ""},

		{PAGE_PROJECT, IDC_STATIC_PROJECT_OPTIONS, "PROJECT_OPTIONS", "Project Options", ""},
		{PAGE_PROJECT, IDC_STATIC_BATCH_SETTINGS, "BATCH_SETTINGS", "Batch Settings", ""},
		{PAGE_PROJECT, IDC_STATIC_PROJECT_NAME, "PROJECT_NAME", "Project Name", ""},
		{PAGE_PROJECT, IDC_STATIC_TEMP_FOLDER, "TEMP_FOLDER", "Temp Folder", ""},
		{PAGE_PROJECT, IDC_STATIC_OUTPUT_FOLDER, "OUTPUT_FOLDER", "Output Folder", ""},
		{PAGE_PROJECT, IDC_CHECK_OVERWRITE_AVI, "OVERWRITE_FILES", "Overwrite Files (without warning)", ""},
		{PAGE_PROJECT, IDC_CHECK_DEL_TEMP, "DELETE_TEMP", "Delete Temp Files when finished.", ""},
		{PAGE_PROJECT, IDC_CHECK_SPLIT, "SPLIT_FILE", "Split total file in", ""},
		{PAGE_PROJECT, IDC_CHECK_SPLIT_FADEOUT, "SPLIT_FADEOUT", "Split on Fade-Out", ""},
		{PAGE_PROJECT, IDC_STATIC_SPLIT_MB, "SPLIT_MB", "MB. each", ""},
		{PAGE_PROJECT, IDC_STATIC_SPLIT_IN_PARTS, "SPLIT_PARTS", "parts, of", ""},
		{PAGE_PROJECT, IDC_RADIO_NORMALMODE, "NORMAL_MODE", "Normal", ""},
		{PAGE_PROJECT, IDC_RADIO_BATCHMODE, "BATCH_MODE", "Batch Mode", ""},
		{PAGE_PROJECT, IDC_BUTTON_UP, "BUTTON_UP", "Up", ""},
		{PAGE_PROJECT, IDC_BUTTON_DOWN, "BUTTON_DOWN", "Down", ""},
		{PAGE_PROJECT, IDC_ADD, "BUTTON_ADD", "Add", ""},
		{PAGE_PROJECT, IDC_REMOVE, "BUTTON_REMOVE", "Remove", ""},
		{PAGE_PROJECT, IDC_BUTTON_MODIFY, "BUTTON_MODIFY", "Modify", ""},
		{PAGE_PROJECT, IDC_BUTTON_CLEAR, "BUTTON_CLEAR", "Clear List", ""},
		{PAGE_PROJECT, IDC_CHECK_DELETE_BATCH, "DELETE_BATCH", "Delete batch entry when ready.", ""},

		{PAGE_OUTPUT, IDC_STATIC_SYSTEM_SETTINGS, "SYSTEM_SETTINGS", "System Settings", ""},
		{PAGE_OUTPUT, IDC_STATIC_STATUS_INFORMATION, "STATUS_INFORMATION", "Status Information", ""},
		{PAGE_OUTPUT, IDC_STATIC_PROGRESS, "PROGRESS", "Progress", ""},
		{PAGE_OUTPUT, IDC_CHECK_SHUTDOWN, "SHUTDOWN", "Shutdown Windows when finished.", ""},

		{PAGE_SETUP, IDC_STATIC_EXTERNAL_PROGRAMS, "EXTERNAL_PROGRAMS", "External Programs", ""},
		{PAGE_SETUP, IDC_STATIC_PRIORITY_SETTINGS, "PRIORITY_SETTINGS", "Priority Settings", ""},
		{PAGE_SETUP, IDC_STATIC_VIDEO_PRIORITY, "VIDEO_PRIORITY", "Video Encoding Priority", ""},
		{PAGE_SETUP, IDC_STATIC_DISK_PRIORITY, "DISK_PRIORITY", "Disk read/write Priority", ""},
		{PAGE_SETUP, IDC_STATIC_SETTING_WARNING, "SETTING_WARNING", "(*) Setting to Highest is not recommended!", ""},
		{PAGE_SETUP, IDC_GET_DEF, "GET_DEFAULT_VALUES", "Reset to Default Settings", ""},
		{PAGE_SETUP, IDC_STATIC_LANGUAGE_SETTINGS, "LANGUAGE_SETTINGS", "Language Settings", ""},
		{PAGE_SETUP, IDC_STATIC_LANGUAGE, "LANGUAGE", "Language", ""},

		{PAGE_PROGRAM_LOCATIONS, IDC_STATIC_PROGRAM_LOCATIONS, "PROGRAM_LOCATIONS", "Program Locations", ""},
		{PAGE_PROGRAM_LOCATIONS, IDOK, "SAVE", "Save", ""},
		{PAGE_PROGRAM_LOCATIONS, IDCANCEL, "CANCEL", "Cancel", ""}
};

CLanguageSupport::CLanguageSupport()
{
	i_num_attribs = 0;
	i_num_languages = 0;
	i_num_tabs = 0;
		
	for (int i = 0 ; i < _PAGE_LAST ; i++)
	{
		as_page_language[i] = strdup(NOLANGDEFINED);
	}

	currentLanguage = strdup(NOLANGDEFINED);

	//pLanguages = new CSortStringArray();
}

CLanguageSupport::~CLanguageSupport()
{
	for (int i = 0 ; i < _PAGE_LAST ; i++)
	{
		free(as_page_language[i]);
	}

	//delete(pLanguages);
}

CList<CString,CString&>& CLanguageSupport::GetLanguageNames()
{
	return myList;
}

bool CLanguageSupport::Init(CWinApp *pApplication, CString s_language)
{
	char s_file_name[STRLEN] = {0};

	this->pApplication = pApplication;

	GetCurrentDirectory(STRLEN, s_current_dir);

	CFileFind finder;

	sprintf(s_file_name, "%s\\%s\\*%s", s_current_dir, LANGUAGE_FOLDER, LANG);

	BOOL bWorking = finder.FindFile(s_file_name);
	
	CString strFileName;
	
	myList.AddTail(CString(DEF_GUI_LANG));

	i_num_languages = 1;
	
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		strFileName = finder.GetFileTitle ();

		if (strFileName != DEF_GUI_LANG)
		{
			if (strFileName != APP_NAME) // hack
			{
				myList.AddTail(strFileName);
			}
		}
	}

	finder.Close();

	Utils::SortList(myList);

	i_num_languages = myList.GetSize() + 1;

	i_num_attribs = sizeof(ar_lang_data) / sizeof(LANG_ATTRIBUTE);

	i_num_tabs = sizeof(ar_tab_names) / sizeof(LANG_ATTRIBUTE);

	if (i_num_languages > 1)
	{
		b_language_change_allowed = true;
		ChangeLanguage(s_language);
	}
	else
	{
		ChangeLanguage(DEF_GUI_LANG);
	}

	return b_language_change_allowed;
}

void CLanguageSupport::InitAttribs(CWnd* p_window, int page)
{
	bool b_page_changed = false;

	if ((b_language_change_allowed) || (strcmp(currentLanguage, DEF_GUI_LANG) == 0))
	{
		int num_attribs = 0;

		for (num_attribs = 0 ; num_attribs < i_num_attribs ; num_attribs++)
		{
			if (ar_lang_data[num_attribs].page == page)
			{
				if (strcmp(as_page_language[page], currentLanguage) != 0)
				{
					free(as_page_language[page]);
					as_page_language[page] = strdup(currentLanguage);
					b_page_changed = true;
				}

				if (b_page_changed)
				{
					(p_window->GetDlgItem(ar_lang_data[num_attribs].enum_value))->SetWindowText(ar_lang_data[num_attribs].s_value);
				}
			}
		}

		if (b_page_changed)
		{
			int nPage = 0;
			CAllControlsSheet *pControl = (CAllControlsSheet*) p_window;
			for (nPage = 0 ; nPage < PAGE_SETUP+1 ; nPage++)
			{
				CString newString;
				newString.Format("%d. %s", nPage + 1, ar_tab_names[nPage].s_value);
				pControl->SetPageTitle(nPage, newString);
			}

			for (nPage = 0 ; nPage < i_num_tabs ; nPage++)
			{
				if (ar_tab_names[nPage].page == PAGE_GENERAL)
				{
					p_window->GetParent()->GetDlgItem(IDCANCEL)->SetWindowText(ar_tab_names[nPage].s_value);
				}
			}
		}
	}
}

int CLanguageSupport::ChangeInternalLanguageStrings(CString language)
{
	INTERNAL_LANG_ATTRIBUTE ar_internal_lang_data[] = {
		{strStatusBusy, "STATUS_BUSY", "Status : Busy..."},
		{strStatusReady, "STATUS_READY", "Status : Ready..."},
		{strVideoAndAudio, "VIDEO_AND_AUDIO", "(Video and Audio)"},
		{strAudioFormat, "WITH_AUDIO", "with XXX audio."},
		{strNoAudioFormat, "WITH_NO_AUDIO", "with no audio."},
		{str1PassVideo, "CREATE_ONE_PASS_MOVIE", "Create a 1-pass video file"},
		{str2PassVideo, "CREATE_TWO_PASS_MOVIE", "Create a 2-pass video file"},
		{strNoAudioFound, "NO_AUDIO_FOUND", "No Audio found"}
	};

	int i_num_internal_attribs = sizeof(ar_internal_lang_data) / sizeof(INTERNAL_LANG_ATTRIBUTE);
	int num_attribs = 0;

	for (num_attribs = 0 ; num_attribs < i_num_internal_attribs ; num_attribs++)
	{
		if (language.CompareNoCase(DEF_GUI_LANG) == 0)
		{
			ar_internal_lang_data[num_attribs].theString = ar_internal_lang_data[num_attribs].s_default_value;
		}
		else
		{
			// read from file
			ar_internal_lang_data[num_attribs].theString = pApplication->GetProfileString(APP_NAME,
				ar_internal_lang_data[num_attribs].s_attribute_name,
				ar_lang_data[num_attribs].s_default_value);
		}
	}

	return 0;
}

int CLanguageSupport::ChangeLanguage(CString language)
{
	if ((b_language_change_allowed) || (language.CompareNoCase(DEF_GUI_LANG) == 0))
	{
		char s_filename[STRLEN] = {0};
		sprintf(s_filename, "%s\\%s\\%s%s", s_current_dir, LANGUAGE_FOLDER, language, LANG);

		//First free the string allocated by MFC at CWinApp startup.
		//The string is allocated before InitInstance is called.
		free((void*)pApplication->m_pszProfileName);
		//Change the name of the .INI file.
		//The CWinApp destructor will free the memory.
		pApplication->m_pszProfileName=_tcsdup(_T(s_filename));

		if (language.Compare(currentLanguage) != 0)
		{
			currentLanguage = language;

			ChangeInternalLanguageStrings(language);

			int num_attribs = 0;

			for (num_attribs = 0 ; num_attribs < i_num_attribs ; num_attribs++)
			{
				if (language.CompareNoCase(DEF_GUI_LANG) == 0)
				{
					ar_lang_data[num_attribs].s_value = ar_lang_data[num_attribs].s_default_value;
				}
				else
				{
					// read from file
					CString strValue;
					
					strValue = pApplication->GetProfileString(APP_NAME,
						ar_lang_data[num_attribs].s_attribute_name,
						ar_lang_data[num_attribs].s_default_value);

					ar_lang_data[num_attribs].s_value =strValue.GetBuffer();
				}
			}

			for (num_attribs = 0 ; num_attribs < i_num_tabs ; num_attribs++)
			{
				if (language.CompareNoCase(DEF_GUI_LANG) == 0)
				{
					ar_tab_names[num_attribs].s_value = ar_tab_names[num_attribs].s_default_value;
				}
				else
				{
					// read from file
					CString strValue;

					strValue = pApplication->GetProfileString(APP_NAME,
						ar_tab_names[num_attribs].s_attribute_name,
						ar_tab_names[num_attribs].s_default_value);

					ar_tab_names[num_attribs].s_value = strValue.GetBuffer();
				}
			}
		}
	}

	return 0;
}