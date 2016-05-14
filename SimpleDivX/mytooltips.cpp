// MyRegKey.cpp: implementation of the CMyRegKey class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Mytooltips.h"
#include <afxcmn.h>
#include "resource.h"


// Static Data ??
#define  TOOLTIPDELAY  500
#define  TOOLTIPSTAY   30000
#define  TOOLTIPWIDTH  200

TOOLTIPS_INFO tooltip_table[] = 
{
	{PAGE_INPUT, IDC_FOLDER_EDIT, "Type here the location from the VOB/TS/IFO-files."},
	{PAGE_INPUT, IDC_VOBFOLDER, "Press this to select the location from the VOB/TS/IFO-files."},
	{PAGE_INPUT, IDC_CHECK_MAIN, "Press this to select all vob-files belonging to the main movie."},
	{PAGE_INPUT, IDC_CHECK_ALL, "Press this to select all vob-files."},
	{PAGE_INPUT, IDC_STATIC_VIDEO_INFORMATION, "Here you find information about the vob/ts/movie."},
	{PAGE_INPUT, IDC_LIST_VOBS, "Here you see all the vob files which can be selected."},
	{PAGE_INPUT, IDC_COMBO_TITLES, "Select the title you want to encode."},

	{PAGE_AUDIO, IDC_CREATE_AUDIO, "Check this if you want to create an avi file containing an audio stream."},
	{PAGE_AUDIO, IDC_CHECK_WAVEUP, "Check this if you want to amplify the audio stream with 3dB."},
	{PAGE_AUDIO, IDC_CHECK_NORM, "Check this if you want to Normalize the audio."},
	{PAGE_AUDIO, IDC_CHECK_NORM2, "Check this if you want to Normalize the audio and want to amplify the lower sound more."},
	{PAGE_AUDIO, IDC_CHECK_AUDIO, "Un-Check this if no audio stream is found by the program, and you want to override the check and select a stream-id by yourself. Normally you would turn this option on. ]"},
	{PAGE_AUDIO, IDC_COMBO_AUDIO_STREAMID, "Select here the audio stream you want to use the movie."},
	{PAGE_AUDIO, IDC_COMBO_AUDIO, "Select here the audio bitrate."},
	{PAGE_AUDIO, IDC_RADIO1, "Select 48000 hertz sample rate."},
	{PAGE_AUDIO, IDC_RADIO2, "Select 44100 hertz sample rate."},
	{PAGE_AUDIO, IDC_RADIO_MP3, "The audio in the movie will be compressed with MP3."},
	{PAGE_AUDIO, IDC_RADIO_AC3, "The audio in the movie will be the same as in the dvd. (AC3 audio)."},
	{PAGE_AUDIO, IDC_RADIO_OGG, "The audio in the movie will be compressed with OGG-vorbis."},

	{PAGE_VIDEO, IDC_COMBO_SOURCE_FORMAT, "Select here the source format.  (Movie : A normal movie from DVD, Video : A movie recorded on digital camera, Custom : Fill in your own values.)\0"},
	{PAGE_VIDEO, IDC_COMBO_FPS, "Select here the Frames Per Second.\0"},                                                                                                                                
	{PAGE_VIDEO, IDC_COMBO_OUTPUTFORMAT, "Select here the desired output format of the final avi. If you select 'custom', make sure you fill in valid values for Output Cropping and Resizing.\0"},
	{PAGE_VIDEO, IDC_COMBO_DEINTERLACE, "Select IVTC (inverse telecine) or de-interlace.\0"},
	
	{PAGE_VIDEO, IDC_VIDEOPREVIEW, "Press this for a preview.\0"},                                                                                                                                      
	{PAGE_VIDEO, IDC_CHECK_FRAMERANGE, "Check this if you want to decode only a range from the movie. [start and end values are seconds]\0"},
	{PAGE_VIDEO, IDC_CHECK_LBA, "Select this if you want to start in a dvd movie at a specific LBA.\0"},
	{PAGE_VIDEO, IDC_CHECK_AUTO, "Check this to do automatically cropping and resizing."},
	
	{PAGE_CODEC, IDC_COMBO_CODEC,"Select here the codec you want to use."},
	{PAGE_CODEC, IDC_EDIT_KEYFRAME,"Enter here the maximum keyframe-frame distance."},
	{PAGE_CODEC, IDC_RADIO1, "Select 1 or 2-pass mode."},
	{PAGE_CODEC, IDC_EDIT_DIVXLOG, "Enter here the log file you want to use. (Only needed by 2-pass mode.)"},
	{PAGE_CODEC, IDC_PASS_0, "Do only the first pass from the encoding session."},
	{PAGE_CODEC, IDC_PASS_1, "Do only the second pass from the encoding session. (This requires that the first pass was done first.)"},
	{PAGE_CODEC, IDC_PASS_2, "Do both passes."},
	{PAGE_CODEC, IDC_ADV_SET, "Select here codec-advanced settings."},

	{PAGE_COMPRESSION, IDC_GET_TIME,"Check this to retrieve information about the length from the movie."},
	{PAGE_COMPRESSION, IDC_EDIT_FRAMES,"Enter here the number of frames you want to convert."},
	{PAGE_COMPRESSION, IDC_EDIT_FileSize,"Enter here the total filesize (video+audio) from movie."},
	{PAGE_COMPRESSION, IDC_MINBITRATE, "Enter here the bitrate you want to use for the movie."},
	{PAGE_COMPRESSION, IDC_EDIT_HOUR,"Enter here the hour(s)."},
	{PAGE_COMPRESSION, IDC_EDIT_MIN,"Enter here the minute(s)."},
	{PAGE_COMPRESSION, IDC_EDIT_SEC,"Enter here the second(s)."},
	{PAGE_COMPRESSION, IDC_RADIO_END1, "End credits will be compressed with same bitrate as the whole movie."},
	{PAGE_COMPRESSION, IDC_RADIO_END2, "End credits will not be compressed, the movie is cut when the end-credits start. [ This option is only available if 2-pass mode is selected. ]"},
	{PAGE_COMPRESSION, IDC_RADIO_END3, "End credits will be blurred and recompressed with a lower bitrate. [ This option is only available if 2-pass mode is selected. ]"},

	{PAGE_OUTPUT, IDC_CHECK_SHUTDOWN, "Check this to shutdown the pc when conversion is done. It is recommended that no other applications are running."},
	{PAGE_OUTPUT, IDC_INFO, "Here you will see status info."},

	{PAGE_PROJECT, IDC_TEMPFOLDER, "Type here the location from the temp folder. (This folder will usually contain some temp files.)"},
	{PAGE_PROJECT, IDC_TEMP_LOCATION, "Press this to select the temp-folder."},
	{PAGE_PROJECT, IDC_OUTPUTAVIFOLDER, "Type here the location where the total created movie must be placed."},
	{PAGE_PROJECT, IDC_AVI_LOCATION, "Press this to select the output-folder."},
	{PAGE_PROJECT, IDC_CHECK_OVERWRITE_AVI, "Check this if you want to overwrite all (temp) files without warning."},
	{PAGE_PROJECT, IDC_CHECK_SPLIT, "Check this if you want to split the total movie."},
	{PAGE_PROJECT, IDC_CHECK_SPLIT_FADEOUT, "Check this if you want to automatically split when a fade-out occurs."},
	{PAGE_PROJECT, IDC_SPLITAT, "Enter here the size from the parts."},
	{PAGE_PROJECT, IDC_STATIC_SPLIT_IN_PARTS, "Split the output file in X parts."},
	{PAGE_PROJECT, IDC_CHECK_DEL_TEMP, "Check this if you want to delete all temp files when the movie is complete."},
	{PAGE_PROJECT, IDC_PROJNAME, "Type here the name from the project. (All temp files and the final file will be named after this name.)"},
	{PAGE_PROJECT, IDC_LIST_BATCH, "Here you see a list from all the batch entries. Click an entry and all the settings from that enrty will be loaded into the application."},
	{PAGE_PROJECT, IDC_ADD, "Click to Add a new Batch-Entry to the List."},
	{PAGE_PROJECT, IDC_REMOVE, "Click to Remove the current selected Entry to the List."},
	{PAGE_PROJECT, IDC_BUTTON_UP, "Click to Move the current selected Entry Up in the List."},
	{PAGE_PROJECT, IDC_BUTTON_DOWN, "Click to Move the current selected Entry Down in the List."},
	{PAGE_PROJECT, IDC_BUTTON_CLEAR, "Click to Clear all Batch-Entries in the List."},
	{PAGE_PROJECT, IDC_BUTTON_MODIFY, "Click to Modify the settings from the current selected Entry."},
	{PAGE_PROJECT, IDC_RADIO_NORMALMODE, "Do only 1 conversion. (Default)"},
	{PAGE_PROJECT, IDC_RADIO_BATCHMODE, "Do all conversions which are added to the Batch-List. (This enables you do multiple conversions.)"},

	{PAGE_SETUP, IDC_VIDEO_PRIO, "This defines the priority from Video Encoding. (This value is used when starting mencoder.)"},
	{PAGE_SETUP, IDC_DISK_PRIO, "This defines the priority from Disk IO (This value is used when starting virtualdub.)"},
	{PAGE_SETUP, IDC_EDIT_PROGRAM_LOCATIONS, "This box displays if the needed programs are found."},
	{PAGE_SETUP, IDC_BROWSE_PROGRAM, "Press this to select the location from the needed programs."},
	{PAGE_SETUP, IDC_LST_LANGUAGES, "Choose here the language."},
	{PAGE_SETUP, IDC_GET_DEF, "Press this to reset all settings to the default values."}
};   

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyToolTips::CMyToolTips()
{

}

CMyToolTips::~CMyToolTips()
{

}

//////////////////////////////////////////////////////////////////////
// Member funtions
//////////////////////////////////////////////////////////////////////

int CMyToolTips::PrepareTooltips(CWnd* p_window, 
								 int i_page, 
								 CToolTipCtrl *m_pTooltip)
{
	int i_return = 0;

	if (m_pTooltip->GetToolCount() == 0)
	{
		i_return = m_pTooltip->SetMaxTipWidth(TOOLTIPWIDTH);
		ASSERT(i_return!=0);

		m_pTooltip->SetDelayTime(TTDT_AUTOPOP, TOOLTIPSTAY);
		m_pTooltip->SetDelayTime(TTDT_INITIAL, TOOLTIPDELAY);

		for (int i = 0 ; i < sizeof(tooltip_table) / sizeof(TOOLTIPS_INFO) ; i++)
		{
			if (tooltip_table[i].i_page == i_page)
			{
				CWnd *w = p_window->GetDlgItem(tooltip_table[i].i_attribute);
				i_return = m_pTooltip->AddTool(w, tooltip_table[i].s_text,0,0);	
				ASSERT(i_return!=0);
			}
		}
	}

	return i_return;
}
