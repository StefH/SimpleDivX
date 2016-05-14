#include "stdafx.h"
#include "MyReg.h"
#include "EncodingInfo.h"
#include "ProgramInfo.h"

#define REG_READ  0
#define REG_WRITE 1
#define REG_SIMPLEDIVX_MAIN	"Software\\SimpleDivX"
#define REG_SIMPLEDIVX	"Software\\SimpleDivX\\Settings"

typedef struct _regentry
{
	char* s_entry;
	unsigned long r_type;
} REGENTRY;

char* SimpleDivX_Entries[] = 
{
		"MEncoder",       // 0
		"StartAtLBA",	  // 1
		"VirtualDub",	  // 2
		"OutputTemp",     // 3
		"OutputTotal",    // 4
		"VobFolder",      // 5
		"CodecVersion",    // 6
		"NormalizeWave",      // 7        
		"VideoOutputFormat", // 8
		"RecompressAc3",  // 9
		"OutputMode",     // 10
		"CPU_ID",        // 11
		"SelectedTitleId", // 12
		"Framerate",      // 13
		"mkvmerge",   // 14
		"AudioStream",   // 15
		"Amplify", // 16
		"EnableAudiocheck",   // 17          
		"StartAtLBA",   // 18
		"HalfResolution", // 19
		"SelectRange",    // 20
		"Filesize",         // 21
		"CreateAudio",   // 22
		"AudioMode",     // 23
		"LanguageID",    // 24
		"VideoKbps",     // 25        
		"CreateAviOptions",  // 26
		"EnterFilesize", // 27
		"HertzId",          // 28
		"StartFrame",    // 29
		"EndFrame",      // 30
		"CropX",				// 31
		"CropY",				// 32
		"ResizeX",				// 33
		"ResizeY",				// 34
		"Split",            // 35
		"SplitAt",          // 36
		"MaxKeyFrame",			// 37
		"GetTimeFromVideo",   // 38
		"Frames",			// 39
		"EnableVideoCheck",   // 40
		"MPlayer", // 41
		"Reserved", // 42 TODO
		"EndCreditOption",	 // 43
		"BitrateAdjust",     // 44
		"LogFile",       // 45
		"PassMode",          // 46
		"AdvancedCodecSettings",    // 47
		"AudioKbps",       // 48
		"BitrateRatio",     // 49
		"ProjectName", // 50
		"MaxQuantizer", //51
		"MinQuantizer", //52
		"EndCreditsManualEnd", //53
		"EndCreditsMode", // 54
		"EndCreditsRecompressionPCT",
		"AutoLogFileNaming", // 56
		"AutoSizeWidthID", // 57
		"Ac3Preload", // 58
		"Ac3Interleave", // 59
		"VideoPriority", // 60
		"AudioPriority", // 61
		"DiskPriority", // 62
		"AutoResizeAndCrop", // 63
		"Deinterlace", // 64
		"AmplifyDB", // 65
		"AudioVBR", // 66
		"OverwriteFiles", // 67
		"DeleteTempFiles", // 68
		"BatchMode", // 69
		"SourceFormatID", // 70
		"InstallDir", // 71 readonly
		"Reserved", // 72 TODO
		"Reserved", // 73	TODO
		"Version", // 74
		"CreateSubTitles", // 75
		"SelectAllSubs", // 76
		"ContainerFormat", // 77
		"IFO_ID", // 78
		"NumberSplitParts" // 79
};

#define NUM_ITEMS 79


static int ChangeRegSZ(HKEY hnd, char *, char *);
static int ChangeRegDW(HKEY hnd, char *, DWORD );

static int HandleSimpleDivXSettings(int, CProgramInfo&, CEncodingInfo&);

CString ReadStringFromRegistry(HKEY *phnd, CString strEntryName, CString s_default);
CString ReadStringFromRegistry(HKEY *phnd, CString strEntryName);

CString ConvertBoolToString(BOOL b_value);
CString ConvertIntToString(int i_value);
CString ConvertLongToString(long l_value);
CString ConvertFloatToString(double f_value);
long ConvertStringToLong(CString strValue , long l_default);
long ConvertStringToLong(CString strValue);
bool ConvertStringToBool(CString strValue , bool b_default);
bool ConvertStringToBool(CString strValue);
int ConvertStringToInt(CString strValue , int i_default);
int ConvertStringToInt(CString strValue);
int ConvertStringToInt(CString strValue, int defaultValue, int min, int max);
double ConvertStringToFloat(CString strValue , double f_default);
double ConvertStringToFloat(CString strValue);

CString ReadStringFromRegistry(HKEY *phnd, CString strEntryName, CString s_default)
{
	DWORD l_type = REG_SZ;
	DWORD dw_len = _MAX_PATH;
	char s_data[_MAX_PATH];
	CString returnValue = s_default;

	if (s_default.IsEmpty())
	{
		s_default = "";
	}

	if (RegQueryValueEx(*phnd, 
		TEXT(strEntryName), 0, &l_type, (BYTE*) s_data, &dw_len) == ERROR_SUCCESS)
	{
		returnValue = s_data;
	}

	return returnValue;
}

CString ReadStringFromRegistry(HKEY *phnd, CString strEntryName)
{
	return ReadStringFromRegistry(phnd, strEntryName, "");
}

CString ConvertBoolToString(BOOL b_value)
{
	CString s_dummy;
	s_dummy.Format("%d", b_value);
	return s_dummy;
}

CString ConvertIntToString(int i_value)
{
	CString s_dummy;
	s_dummy.Format("%d", i_value);
	return s_dummy;
}

CString ConvertLongToString(long l_value)
{
	CString s_dummy;
	s_dummy.Format("%ld", l_value);
	return s_dummy;
}

CString ConvertFloatToString(double f_value)
{
	CString s_dummy;
	s_dummy.Format("%.2f", f_value);
	return s_dummy;
}


long ConvertStringToLong(CString strValue , long l_default)
{
	long returnValue = l_default;

	if (!strValue.IsEmpty())
	{
		returnValue = atol(strValue);
	}

	return returnValue;
}

long ConvertStringToLong(CString strValue)
{
	return ConvertStringToLong(strValue, 0);
}

bool ConvertStringToBool(CString strValue , bool b_default)
{
	bool returnValue = b_default;

	if (!strValue.IsEmpty())
	{
		returnValue = (atoi(strValue)) > 0;
	}

	return returnValue;
}

bool ConvertStringToBool(CString strValue)
{
	return ConvertStringToBool(strValue, false);
}

int ConvertStringToInt(CString strValue , int i_default)
{
	int returnValue = i_default;

	if (!strValue.IsEmpty())
	{
		returnValue = atoi(strValue);
	}

	return returnValue;
}

int ConvertStringToInt(CString strValue)
{
	return ConvertStringToInt(strValue, 0);
}

int ConvertStringToInt(CString strValue, int defaultValue, int min, int max)
{
	int value = ConvertStringToInt(strValue, defaultValue);
	if ((value < min) || (value > max))
	{
		value = defaultValue;
	}

	return value;
}

double ConvertStringToFloat(CString strValue , double f_default)
{
	double returnValue = f_default;

	if (!strValue.IsEmpty())
	{
		returnValue = atof(strValue);
	}

	return returnValue;
}

double ConvertStringToFloat(CString strValue)
{
	return ConvertStringToFloat(strValue, 0.0);
}

static int ChangeRegSZ(HKEY hnd, char *KeyName, char *Data)
{
	RegSetValueEx(hnd, KeyName, 0, REG_SZ,(const unsigned char *)Data, strlen(Data)) ;
	return 0;
}

static int ChangeRegDW(HKEY hnd, char *KeyName, DWORD Data)
{
	RegSetValueEx(hnd, KeyName, 0, REG_DWORD,( const BYTE *)&Data, sizeof(Data)) ;
	return 0;
}

int Read_SimpleDivXSettings(CProgramInfo& Prog_Info, CEncodingInfo& enc_info)
{
	return HandleSimpleDivXSettings(REG_READ, Prog_Info, enc_info);
}

int Write_SimpleDivXSettings(CProgramInfo& Prog_Info, CEncodingInfo& enc_info)
{
	return HandleSimpleDivXSettings(REG_WRITE, Prog_Info, enc_info);
}

int OpenSimpleDivXRegKey(HKEY *phnd)
{
	RegOpenKeyEx(HKEY_CURRENT_USER, REG_SIMPLEDIVX, 0,KEY_READ | KEY_WRITE ,phnd);
	return 0;
}

int CloseSimpleDivXRegKey(HKEY *phnd)
{
	RegCloseKey(*phnd);
	return 0;
}

static int HandleSimpleDivXSettings(int option, 
									CProgramInfo& Prog_Info,
									CEncodingInfo& Enc_Info)
{
	int i = 0, res = 0;
	int val = 0;
	CString strValue;
	DWORD l_type = REG_SZ;
	DWORD dw_len = _MAX_PATH;
	HKEY phnd = 0;
	CString s_version = "0.0";

	DWORD dwDisposition = 0;

	res =  RegCreateKeyEx(
		HKEY_CURRENT_USER, // handle to open key
		REG_SIMPLEDIVX, // subkey name
		0, // reserved
		NULL, // class string
		REG_OPTION_NON_VOLATILE, // special options
		KEY_ALL_ACCESS, // desired security access
		NULL, // inheritance
		&phnd, // key handle 
		&dwDisposition // disposition value buffer
		);

	if (option == REG_READ) // read
	{
		if (res != ERROR_SUCCESS)
		{
			return 0;
		}

		if (dwDisposition == REG_OPENED_EXISTING_KEY)
		{
			for (i = 0 ; i <= NUM_ITEMS ; i ++)
			{
				strValue = ReadStringFromRegistry(&phnd, SimpleDivX_Entries[i]);

				if (!strValue.IsEmpty())
				{
					switch(i)
					{
					case 0:
						Prog_Info.strMEncoderFolder = strValue;
						break;
					case 1:
						Enc_Info.i_start_at_lba = ConvertStringToInt(strValue);
						break;
					case 2:
						Prog_Info.strVirtualDubFolder = strValue;
						break;
					case 3:
						Enc_Info.s_TempFolder = strValue;
						break;
					case 4:
						Enc_Info.s_OutputFolder = strValue;
						break;
					case 5:
						Enc_Info.sVobFolder = strValue;
						break;
					case 6:
						Enc_Info.i_codec_version = ConvertStringToInt(strValue);
						break;
					case 7:
						Enc_Info.i_norm_wave = ConvertStringToInt(strValue);
						break;
					case 8:
						Enc_Info.i_video_output_format_id = ConvertStringToInt(strValue);
						break;
					case 9:
						Enc_Info.b_convert_ac3_to_2channels = ConvertStringToBool(strValue);
						break;
					case 10:
						Enc_Info.i_outputmode_id = ConvertStringToInt(strValue);
						break;
					case 11:
						Prog_Info.cpu_model = ConvertStringToInt(strValue);
						break;
					case 12:
						Enc_Info.i_selected_title_id = ConvertStringToInt(strValue);
						break;
					case 13:
						Enc_Info.i_framerate_id = ConvertStringToInt(strValue);
						break;
					case 14:
						Prog_Info.strMkvMergeFolder = strValue;
						break;
					case 15:
						Enc_Info.i_audio_stream_id = ConvertStringToInt(strValue);
						break;
					case 16:
						Enc_Info.b_amplify_audio = ConvertStringToBool(strValue);
						break;
					case 17:
						Enc_Info.b_enable_audiocheck = ConvertStringToBool(strValue);
						break;
					case 18:
						Enc_Info.b_start_at_lba = ConvertStringToBool(strValue);
						break;
					case 19:
						Enc_Info.b_halfresolution = ConvertStringToBool(strValue);
						break;
					case 20:
						Enc_Info.b_selectrange = ConvertStringToBool(strValue);
						break;
					case 21:
						Enc_Info.i_filesize = ConvertStringToInt(strValue);
						break;
					case 22:
						Enc_Info.b_create_audio = ConvertStringToBool(strValue);
						break;
					case 23:
						Enc_Info.i_audio_format = ConvertStringToInt(strValue);
						break;
					case 24:
						Prog_Info.currentLanguage = strdup(strValue.GetBuffer());
						break;
					case 25:
						Enc_Info.i_video_bitrate = ConvertStringToInt(strValue, 1000, 10, MAXDIVX5KBPS);
						break;
					case 26:
						Enc_Info.i_create_avi_options = ConvertStringToInt(strValue);
						break;
					case 27:
						Enc_Info.b_enter_filesize = ConvertStringToBool(strValue);
						break;
					case 28:
						Enc_Info.i_hertz_id = ConvertStringToInt(strValue);
						break;
					case 29:
						Enc_Info.i_start_time = ConvertStringToInt(strValue);
						break;
					case 30:
						Enc_Info.i_end_time = ConvertStringToInt(strValue);
						break;
					case 31:
						Enc_Info.i_cx = ConvertStringToInt(strValue);
						break;
					case 32:
						Enc_Info.i_cy = ConvertStringToInt(strValue);
						break;
					case 33:
						Enc_Info.i_dx = ConvertStringToInt(strValue);
						break;
					case 34:
						Enc_Info.i_dy = ConvertStringToInt(strValue);
						break;
					case 35:
						Enc_Info.i_split = ConvertStringToInt(strValue);
						break;
					case 36:
						Enc_Info.l_split_at = ConvertStringToInt(strValue);
						break;
					case 37:
						Enc_Info.i_max_keyframe_interval = ConvertStringToInt(strValue);
						break;
					case 38:
						Enc_Info.b_gettimefromvob = ConvertStringToBool(strValue);
						break;
					case 39:
						Enc_Info.l_frames_chosen = ConvertStringToLong(strValue);
						break;
					case 40:
						Enc_Info.b_enable_videocheck = ConvertStringToBool(strValue);
						break;
					case 41:
						Prog_Info.strMPlayerFolder = strValue;
						break;
					case 42:
						// TODO
						break;
					case 43:
						Enc_Info.i_endcredits_option = ConvertStringToInt(strValue);
						break;
					case 44:
						//Enc_Info.i_bitrate_adjust = ConvertStringToInt(strValue);
						break;
					case 45:
						Enc_Info.sTwoPassLogFile = strValue;
						break;
					case 46:
						Enc_Info.i_pass_mode = ConvertStringToInt(strValue);
						if ((Enc_Info.i_pass_mode < 0) || (Enc_Info.i_pass_mode > 2))
						{
							Enc_Info.i_pass_mode = 0;
						}
						break;
					case 47:
						val = ConvertStringToInt(strValue);
						if (val & 1)  Enc_Info.b_adv_gmc = 1; 
						if (val & 2)  Enc_Info.b_adv_qpel = 1;
						if (val & 4)  Enc_Info.b_adv_bi = 1;
						if (val & 8)  Enc_Info.b_adv_chroma_motion = 1;
						if (val & 16) Enc_Info.b_adv_lumi_mask = 1; 
						if (val & 32) Enc_Info.b_adv_vhq = 1;
						if (val & 64) Enc_Info.b_adv_trellis = 1;
						if (val & 128) Enc_Info.b_adv_dark_mask = 1;
						break;
					case 48:
						Enc_Info.i_audio_kbps = ConvertStringToInt(strValue, 128);
						break;
					case 49:
						Enc_Info.d_ratio = ConvertStringToFloat(strValue);
						break;
					case 50:
						Enc_Info.s_ProjName = strValue;
						break;
					case 51:
						Enc_Info.i_max_qant= ConvertStringToInt(strValue);
						break;
					case 52:
						Enc_Info.i_min_qant = ConvertStringToInt(strValue);
						break;
					case 53:
						Enc_Info.l_manual_end = ConvertStringToInt(strValue);
						break;
					case 54:
						Enc_Info.i_endcredits_auto_manual = ConvertStringToInt(strValue);
						break;
					case 55:
						Enc_Info.i_end_credits_pct = ConvertStringToInt(strValue);
						break;
					case 56:
						Enc_Info.b_auto_logfile_naming = ConvertStringToBool(strValue);
						break;
					case 57:
						Enc_Info.i_auto_size_width_id = ConvertStringToInt(strValue, 4);
						break;
					case 58:
						//Prog_Info.i_ac3_preload = ConvertStringToInt(strValue);
						break;
					case 59:
						//Prog_Info.i_ac3_interleave = ConvertStringToInt(strValue);
						break;
					case 60:
						Prog_Info.i_video_prio = ConvertStringToInt(strValue, 3);
						break;
					case 61:
						//Prog_Info.i_audio_prio = ConvertStringToInt(strValue, 3);
						break;
					case 62:
						Prog_Info.i_disk_prio = ConvertStringToInt(strValue, 3);
						break;
					case 63:
						Enc_Info.b_auto_size = ConvertStringToBool(strValue);
						break;
					case 64:
						Enc_Info.i_deinterlace = ConvertStringToInt(strValue);
						break;
					case 65:
						Enc_Info.i_audio_amplify = ConvertStringToInt(strValue, 3);
						break;
					case 66:
						Enc_Info.b_audio_vbr = ConvertStringToBool(strValue);
						break;
					case 67:
						Enc_Info.b_overwrite = ConvertStringToBool(strValue);
						break;
					case 68:
						Enc_Info.b_delete_temp = ConvertStringToBool(strValue);
						break;
					case 69:
						Prog_Info.b_batch_enabled = ConvertStringToBool(strValue);
						break;
					case 70:
						Enc_Info.i_source_format_id = ConvertStringToInt(strValue, 0);
						break;
					case 71:
						Prog_Info.strInstallDir = strValue;
						break;
					case 72:
						//
						break;
					case 73:
						//
						break;
					case 74:
						s_version = strValue;
						break;
					case 75:
						Enc_Info.b_create_subtitles = ConvertStringToBool(strValue);
						break;
					case 76:
						Enc_Info.b_extract_all_subtitles = ConvertStringToBool(strValue);
						break;
					case 77:
						Enc_Info.i_container_format = ConvertStringToInt(strValue, 0);
						break;
					case 78:
						Enc_Info.i_selected_ifo_id = ConvertStringToInt(strValue, 0);
						break;
					case 79:
						Enc_Info.i_num_parts_requested = ConvertStringToInt(strValue, 0);
						break;
					default:
						break;
					}
				}
			}
		}
		else
		{
			res = 2;
		}

	}
	else // write
	{
		res = RegOpenKeyEx(HKEY_CURRENT_USER, REG_SIMPLEDIVX, 0, KEY_WRITE , &phnd);

		if (res == ERROR_SUCCESS)
		{
			for (i = 0 ; i <= NUM_ITEMS ; i ++)
			{
				switch(i)
				{		
				case 0:
					strValue = Prog_Info.strMEncoderFolder;
					break;
				case 1:
					strValue.Format("%d", Enc_Info.i_start_at_lba);
					break;
				case 2:
					strValue = Prog_Info.strVirtualDubFolder;
					break;
				case 3:
					strValue = Enc_Info.s_TempFolder;				
					break;
				case 4:
					strValue = Enc_Info.s_OutputFolder;
					break;
				case 5:
					strValue = Enc_Info.sVobFolder;
					break;
				case 6:
					strValue.Format("%d",Enc_Info.i_codec_version);
					break;
				case 7:
					strValue.Format("%d",Enc_Info.i_norm_wave);
					break;
				case 8:
					strValue.Format("%d",Enc_Info.i_video_output_format_id);
					break;
				case 9:
					strValue.Format("%d", Enc_Info.b_convert_ac3_to_2channels);
					break;
				case 10:
					strValue.Format("%d",Enc_Info.i_outputmode_id);
					break;
				case 11:
					strValue.Format("%d",Prog_Info.cpu_model);
					break;
				case 12:
					strValue.Format("%d",Enc_Info.i_selected_title_id);
					break;
				case 13:
					strValue.Format("%d",Enc_Info.i_framerate_id);
					break;
				case 14:
					strValue = Prog_Info.strMkvMergeFolder;
					break;
				case 15:
					strValue.Format("%d",Enc_Info.i_audio_stream_id);
					break;
				case 16:
					strValue.Format("%d",Enc_Info.b_amplify_audio);
					break;
				case 17:
					strValue.Format("%d",Enc_Info.b_enable_audiocheck);
					break;
				case 18:
					strValue.Format("%d",Enc_Info.b_start_at_lba);
					break;
				case 19:
					strValue.Format("%d",Enc_Info.b_halfresolution);
					break;
				case 20:
					strValue.Format("%d",Enc_Info.b_selectrange);
					break;
				case 21:
					strValue.Format("%d",Enc_Info.i_filesize);
					break;
				case 22:
					strValue.Format("%d",Enc_Info.b_create_audio);
					break;
				case 23:
					strValue.Format("%d",Enc_Info.i_audio_format);
					break;
				case 24:
					strValue = Prog_Info.currentLanguage;
					break;
				case 25:
					strValue.Format("%d",Enc_Info.i_video_bitrate);
					break;
				case 26:
					strValue.Format("%d",Enc_Info.i_create_avi_options);
					break;
				case 27:
					strValue.Format("%d",Enc_Info.b_enter_filesize);
					break;
				case 28:
					strValue.Format("%d",Enc_Info.i_hertz_id);
					break;
				case 29:
					strValue.Format("%d",Enc_Info.i_start_time);
					break;
				case 30:
					strValue.Format("%d",Enc_Info.i_end_time);
					break;
				case 31:
					strValue.Format("%d",Enc_Info.i_cx);
					break;
				case 32:
					strValue.Format("%d",Enc_Info.i_cy);
					break;
				case 33:
					strValue.Format("%d",Enc_Info.i_dx);
					break;
				case 34:
					strValue.Format("%d",Enc_Info.i_dy);
					break;
				case 35:
					strValue.Format("%d",Enc_Info.i_split);
					break;
				case 36:
					strValue.Format("%ld",Enc_Info.l_split_at);
					break;
				case 37:
					strValue.Format("%d",Enc_Info.i_max_keyframe_interval);
					break;
				case 38:
					strValue.Format("%d",Enc_Info.b_gettimefromvob);
					break;
				case 39:
					strValue.Format("%ld",Enc_Info.l_frames_chosen);
					break;
				case 40:
					strValue.Format("%d", Enc_Info.b_enable_videocheck);
					break;
				case 41:
					strValue = Prog_Info.strMPlayerFolder;
					break;
				case 42:
					break;
				case 43:
					strValue.Format("%d", Enc_Info.i_endcredits_option);
					break;
				case 44:
					//strValue.Format("%d", Enc_Info.i_bitrate_adjust);
					break;
				case 45:
					strValue = Enc_Info.sTwoPassLogFile;
					break;
				case 46:
					strValue.Format("%d", Enc_Info.i_pass_mode);
					break;
				case 47:
					val = Enc_Info.b_adv_gmc * 1 + 
						Enc_Info.b_adv_qpel * 2 + 
						Enc_Info.b_adv_bi * 4 + 
						Enc_Info.b_adv_chroma_motion * 8 +
						Enc_Info.b_adv_lumi_mask * 16 +
						Enc_Info.b_adv_vhq * 32 +
						Enc_Info.b_adv_trellis * 64 +
						Enc_Info.b_adv_dark_mask * 128;
					strValue.Format("%d", val);
					break;
				case 48:
					strValue.Format("%d", Enc_Info.i_audio_kbps);
					break;
				case 49:
					strValue.Format("%.2f", Enc_Info.d_ratio);
					break;
				case 50:
					strValue = Enc_Info.s_ProjName;
					break;
				case 51:
					strValue.Format("%d", Enc_Info.i_max_qant);
					break;
				case 52:
					strValue.Format("%d", Enc_Info.i_min_qant);
					break;
				case 53:
					strValue.Format("%d", Enc_Info.l_manual_end);
					break;
				case 54:
					strValue.Format("%d", Enc_Info.i_endcredits_auto_manual);
					break;
				case 55:
					strValue.Format("%d", Enc_Info.i_end_credits_pct);
					break;
				case 56:
					strValue.Format("%d", Enc_Info.b_auto_logfile_naming);
					break;
				case 57:
					strValue.Format("%d", Enc_Info.i_auto_size_width_id);
					break;
				case 58:
					//strValue.Format("%d", Prog_Info.i_ac3_preload);
					break;
				case 59:
					//strValue.Format("%d", Prog_Info.i_ac3_interleave);
					break;
				case 60:
					strValue.Format("%d", Prog_Info.i_video_prio);
					break;
				case 61:
					//strValue.Format("%d", Prog_Info.i_audio_prio);
					break;
				case 62:
					strValue.Format("%d", Prog_Info.i_disk_prio);
					break;
				case 63:
					strValue.Format("%d", Enc_Info.b_auto_size);
					break;
				case 64:
					strValue.Format("%d", Enc_Info.i_deinterlace);
					break;
				case 65:
					strValue.Format("%d", Enc_Info.i_audio_amplify);
					break;
				case 66:
					strValue.Format("%d", Enc_Info.b_audio_vbr);
					break;
				case 67:
					strValue.Format("%d", Enc_Info.b_overwrite);
					break;
				case 68:
					strValue.Format("%d", Enc_Info.b_delete_temp);
					break;
				case 69:
					strValue.Format("%d", Prog_Info.b_batch_enabled);
					break;
				case 70:
					strValue.Format("%d", Enc_Info.i_source_format_id);
					break;
				case 71:
					// Read only value...
					break;
				case 72:
					//
					break;
				case 73:
					//
					break;
				case 74:
					strValue = APP_VERSION;
					break;
				case 75:
					strValue = ConvertBoolToString(Enc_Info.b_create_subtitles);
					break;
				case 76:
					strValue = ConvertBoolToString(Enc_Info.b_extract_all_subtitles);
					break;
				case 77:
					strValue = ConvertIntToString(Enc_Info.i_container_format);
					break;
				case 78:
					strValue = ConvertIntToString(Enc_Info.i_selected_ifo_id);
					break;
				case 79:
					strValue = ConvertIntToString(Enc_Info.i_num_parts_requested);
					break;
				default:
					break;
				}

				if (strlen(strValue) > 0)
				{
					(void) ChangeRegSZ(phnd, SimpleDivX_Entries[i], strValue.GetBuffer(0));
					strValue.Empty();
				}
			}
		}
	}


	if (phnd)
	{
		RegCloseKey(phnd);
	}

	if ((option == REG_READ) && (dwDisposition == REG_OPENED_EXISTING_KEY))
	{
		int i_ver = (int) (atof(s_version) * 100);

		// special :
		if (i_ver < 121)
		{
			// delete reg key
			(void) RegDeleteKey(HKEY_CURRENT_USER, REG_SIMPLEDIVX);
			(void) RegDeleteKey(HKEY_CURRENT_USER, REG_SIMPLEDIVX_MAIN);
			res = 1;
		}
	}

	// 0 = ok
	return res;
}
