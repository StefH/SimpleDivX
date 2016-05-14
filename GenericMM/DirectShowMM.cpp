//
// DirectShow code
//

#include "genericmm.h"
#include <comdef.h>

#include <windows.h>
#include <mmsystem.h>
#include <streams.h>
#include <stdio.h>
#include <oleauto.h>
#include <stdio.h>
#include <string.h>
#include <Vfw.h>

#include <initguid.h>
#include "OggDS.h"

struct CCInfo 
{
	ICINFO		*pCompInfo;
	int			nComp;
	COMPVARS	*pCV;
	//BITMAPINFOHEADER *bih;
	//char		tbuf[128];
	//char		s_name[128];
	
	HIC			hic;
	FOURCC		fccSelect;
	ICINFO		*piiCurrent;
	
	void		*pState;
	int			cbState;
	//char		szCurrentCompression[256];
};

typedef struct 
{
	char **ps_filter;
	wchar_t **pws_filter;
	int i_num;
} DS_FILTER_INFO;

#define HELPER_RELEASE(x) { if (x) x->Release(); x = NULL; }
#define MAX_WAIT_TIME (30 * 60 * 1000) // 30mins
#define TIMEFORMAT (10000.0)

static void rename_file(char *s_old, int i_part);
void RenderFile(char* Filename, char* Filtername, char* Property, char* Value, IGraphBuilder *Graph, IBaseFilter *OggMuxer);

int	UsedPins = 0;

genericmm_API int GetCodecInfo(char **sa_codecs)
{
    HRESULT hr;
    ICreateDevEnum *pSysDevEnum = NULL;
    IEnumMoniker *pEnum = NULL;
    IMoniker *pMoniker = NULL;
	int i_num = 0;
	char s[128] = "";
	int b = 0;
	
	CoInitialize(NULL);
	
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, 
        CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, 
        (void**)&pSysDevEnum);
    hr = pSysDevEnum->CreateClassEnumerator(
		CLSID_VideoCompressorCategory, &pEnum, 0);
    
    while (S_OK == pEnum->Next(1, &pMoniker, NULL))
    {
        IPropertyBag *pPropBag = NULL;
        pMoniker->BindToStorage(0, 0, IID_IPropertyBag, 
			(void **)&pPropBag);
        VARIANT var;
        VariantInit(&var);
        hr = pPropBag->Read(L"FriendlyName", &var, 0);
        if (SUCCEEDED(hr))
        {
			
            WideCharToMultiByte( CP_ACP, 0, var.bstrVal, -1, s, MAX_PATH, NULL, &b);
			sa_codecs[i_num] = _strdup(s);
			i_num++;
        }   
        VariantClear(&var); 
        pPropBag->Release();
        pMoniker->Release();
    }
	
    pSysDevEnum->Release();
    pEnum->Release();
	
	CoUninitialize();
	
	return i_num;
}

// Not my version !
genericmm_API int combine_avi_ogg2(char *m_movie, 
								   char *s_ogg_file,
								   char *s_out_file,
								   long i_split_at_milli_sec,
								   char *s_video_name,
								   char *s_audio_name)
{
	HRESULT					hr;
	IGraphBuilder			*pGraph = NULL;
	IBaseFilter				*pOggMux = NULL;
	IMediaEvent				*pMediaEvent = NULL;
	IMediaControl			*pMediaControl = NULL;
	IMediaSeeking			*pMediaSeeking = NULL;
	IBaseFilter				*pTargetFile = NULL;
	IFileSinkFilter2		*FileDest = NULL;
	IMediaSeeking			*pSeek = NULL;
	IMediaEventEx			*pimex = NULL;
	
	WCHAR					transformed[MAX_PATH];
	
	IPin					*inPin = NULL;
	IPin					*outPin = NULL;
	IPropertyBag			*pPropBag = NULL;
	
	LONGLONG duration = 0;
	LONGLONG Current = 0;
	LONGLONG Stop = 0;
	LONGLONG lstart = 0;
	LONGLONG lend = 0;
	GUID timeFormat = TIME_FORMAT_NONE;
	
	FILE *fp;
	int status = 0;
	long EvCode = 0;
	UsedPins = 0;
	
	fp = fopen("c:\\combine2.txt", "wt");
	
	CoInitialize(NULL);
	
	
	////////////////////////////////////////////////////////////
	// Create a filter graph
	////////////////////////////////////////////////////////////
	
	// Create instance of the FilterGraphBuilder
	hr = CoCreateInstance(CLSID_FilterGraph,	// CLSID of object
		NULL,									// outer unknown
		CLSCTX_INPROC_SERVER,					// type of server
		IID_IGraphBuilder,						// interface wanted
		(void **) &pGraph);						// pointer to IGraphBuilder
	if (FAILED(hr))
		fprintf(fp, "Can't create a filtergraph!", "Error!", MB_OK);
	
	// Register FilterGraph globally (for connection via GraphEdit)
#ifdef REGISTER_FILTERGRAPH
	AddToRot(pGraph, &RegRot);
#endif
	
	////////////////////////////////////////////////////////////
	// Create Output file filter
	////////////////////////////////////////////////////////////
	
	// convert Filename to WCHAR
	mbstowcs(transformed, s_out_file, strlen(s_out_file)+1);
	// Create instance of the FileWriter Filter
	hr = CoCreateInstance(CLSID_FileWriter,		// CLSID of object
		NULL,									// outer unknown
		CLSCTX_INPROC_SERVER,					// type of server
		IID_IBaseFilter,						// interface wanted
		(void **) &pTargetFile);				// pointer to FileWriter
	if (FAILED(hr))
		fprintf(fp, "Can't create FileWriter!", "Error!", MB_OK);
	// Get the FileSink interface for setting the filename
	hr = pTargetFile->QueryInterface(IID_IFileSinkFilter2, (void**)&FileDest);
	if (FAILED(hr))
		fprintf(fp, "Can't queryinterface FileDest on TargetFile!", "Error!", MB_OK);
	
	pGraph->QueryInterface(IID_IMediaSeeking, (void**)&pSeek);
	
	pGraph->QueryInterface(IID_IMediaEventEx, (void **)&pimex);
	
	
	// Set the Filename
	hr = FileDest->SetFileName(transformed,NULL);
	if (FAILED(hr))
		fprintf(fp, "Can't SetFileName at FileDest!", "Error!", MB_OK);
	// Set File to overwrite existing by deleting them
	hr = FileDest->SetMode(AM_FILE_OVERWRITE);
	// Add Filter to the Filtergraph
	hr = pGraph->AddFilter(pTargetFile, L"File Writer");
	if (FAILED(hr))
		fprintf(fp, "Can't add FileWriter to Graph!", "Error!", MB_OK);
	FileDest->Release();
	
	////////////////////////////////////////////////////////////
	// Enumerate pins to connect the FileDest
	////////////////////////////////////////////////////////////
	hr = pTargetFile->FindPin(L"in", &inPin);
	if (FAILED(hr))
		fprintf(fp, "Can't find FileWriter input pin!", "Error!", MB_OK);
	
	////////////////////////////////////////////////////////////
	// Create Muxer file filter
	////////////////////////////////////////////////////////////
	// Create an instance of the OggMuxer Filter
	hr = CoCreateInstance(CLSID_OggMux,		// CLSID of object
		NULL,								// outer unknown
		CLSCTX_INPROC_SERVER,				// type of server
		IID_IBaseFilter,					// interface wanted
		(void **) &pOggMux);				// pointer to Ogg Muxer
	if (FAILED(hr))
		fprintf(fp, "Can't create OggMuxer!", "Error!", MB_OK);
	// Add OggMuxer Filter to the Filtergraph
	hr = pGraph->AddFilter(pOggMux, L"Ogg Multiplexer");
	if (FAILED(hr))
		fprintf(fp, "Can't add OggMuxer to Graph!", "Error!", MB_OK);
	
	////////////////////////////////////////////////////////////
	// Enumerate pins to connect the OggMuxer
	////////////////////////////////////////////////////////////
	hr = pOggMux->FindPin(L"Ogg Stream", &outPin);
	if (FAILED(hr))
		fprintf(fp, "Can't find oggmux output pin!", "Error!", MB_OK);
	
	////////////////////////////////////////////////////////////
	// Connect the OggMuxer with the FileWriter
	////////////////////////////////////////////////////////////
	hr = pGraph->Connect(outPin, inPin);
	if (FAILED(hr))
		fprintf(fp, "Can't connect OggMuxer output with FileWriter input!", "Error!", MB_OK);
	
	inPin->Release();
	outPin->Release();
	
	////////////////////////////////////////////////////////////
	// Add input sources
	////////////////////////////////////////////////////////////
	// Add source for Movie
	if (m_movie != "") {
		RenderFile(m_movie, "Moviesource", "Title", s_video_name, pGraph, pOggMux);
	}
	/*
	// Add sources for Soundtracks
	if (m_soundtracklist.GetCount() > 0)
	{
	for (int c=0; c<m_soundtracklist.GetCount(); c++) {
				set_soundtrack(c);
				(m_soundtrack1.Right(4)==".wav")?EncodeFile(m_soundtrack1, m_quality/100, "Soundtracksource", "Language", m_language1, pGraph, pOggMux):
				
				  }
				  m_soundtrack1 = "";
				  m_language1 = "(not specified)";
				  m_soundtrack_delay = 0;
				  m_quality = 1;
				  UpdateData(false);
		} else*/
	
	RenderFile(s_ogg_file, "Soundtracksource", "Language", s_audio_name, pGraph, pOggMux);
	
	
	////////////////////////////////////////////////////////////
	// run the graph
	////////////////////////////////////////////////////////////
	// Get the interface of the MediaControl
	hr = pGraph->QueryInterface(IID_IMediaControl, (void **) &pMediaControl);
	if (FAILED(hr))
		fprintf(fp, "Can't query interface the media control!", "Error!", MB_OK);
	
	// Query MediaEvent Interface
	hr = pGraph->QueryInterface(IID_IMediaEvent, (void **) &pMediaEvent);
	if (FAILED(hr))
		fprintf(fp, "Can't query MediaEvent interface!", "Error!", MB_OK);
	
	// ..and run, run, run the FilterGraph :)
	//hr = pMediaControl->Run();
	
	lstart = 0;
	lend = (LONGLONG) (TIMEFORMAT * i_split_at_milli_sec);
	
	hr = pSeek->GetTimeFormat(&timeFormat);
	hr = pSeek->GetDuration(&duration);
	hr = pSeek->GetPositions(&Current,&Stop);
	
	if ((lend >= Stop) || (Current >= lend))
	{
		// do not split !!!
		hr = pMediaControl->Run();
		fprintf(fp, "hr = pimc->Run();\n");
		fclose(fp);
		do
		{
			hr = pimex->WaitForCompletion(MAX_WAIT_TIME, &EvCode);
		}while (EvCode != EC_COMPLETE);
		hr = pMediaControl->Stop();
		status = 0;
	}
	else
	{
		// make 1st part
		hr = pSeek->SetPositions(&lstart,AM_SEEKING_AbsolutePositioning,
			&lend,AM_SEEKING_AbsolutePositioning);
		hr = pMediaControl->Run();
		do
		{
			hr = pimex->WaitForCompletion(MAX_WAIT_TIME, &EvCode);
		}while (EvCode != EC_COMPLETE);
		hr = pMediaControl->Stop();
		
		rename_file(s_out_file, 0);
		
		// make 2nd part
		hr = pSeek->SetPositions(&lend,AM_SEEKING_AbsolutePositioning,
			&Stop,AM_SEEKING_AbsolutePositioning);
		hr = pMediaControl->Run();
		do
		{
			hr = pimex->WaitForCompletion(MAX_WAIT_TIME, &EvCode);
		}while (EvCode != EC_COMPLETE);
		hr = pMediaControl->Stop();
		
		rename_file(s_out_file, 1);
		status = 1;
	}
/*
	HELPER_RELEASE(pGraph);
	HELPER_RELEASE(pOggMux);
	HELPER_RELEASE(pMediaEvent);
	HELPER_RELEASE(pMediaControl);
	HELPER_RELEASE(pMediaSeeking);
	HELPER_RELEASE(pTargetFile);
	HELPER_RELEASE(FileDest);
	HELPER_RELEASE(pSeek);
	HELPER_RELEASE(pimex);
	
	HELPER_RELEASE(inPin);
	HELPER_RELEASE(outPin);
	HELPER_RELEASE(pPropBag);*/

	CoUninitialize();
	
	return status;
}

////////////////////////////////////////////////////////////
// Add File Render Filter to the FilterGraph
////////////////////////////////////////////////////////////
void RenderFile(char* Filename, char* Filtername, char* Property, char* Value, IGraphBuilder *Graph, IBaseFilter *OggMuxer)
{
	IPin					*inPin = NULL;
	IPropertyBag			*pPropBag = NULL;
	WCHAR					transformed[MAX_PATH];
	WCHAR					transfilter[MAX_PATH];
	char					tempstring[400] = {0};
	char					FindPin[400] = {0};
	HRESULT					hr;
	
	// Convert File-&Filtername to WCHAR
	mbstowcs(transformed, Filename, strlen(Filename)+1); 
	mbstowcs(transfilter, Filtername, strlen(Filtername)+1);
	// Let the FilterGraphBuilder render the file
	hr = Graph->RenderFile(transformed, transfilter);
	if (FAILED(hr))
		//MessageBox(ErrMsg, "Render File Error!", MB_OK);
		// Find the input pin where added source to OggMuxer
		
		sprintf(tempstring,"hoi");
	sprintf(tempstring, "Stream %d", UsedPins);
	strcpy(FindPin,tempstring);
	mbstowcs(transformed, FindPin, strlen(FindPin)+1); 
	hr = OggMuxer->FindPin(transformed, &inPin);
	if (FAILED(hr))
		//MessageBox("Can't find OggMuxer input pin!", "Render File Error!", MB_OK);
		
		// Convert language string
		mbstowcs(transfilter, Property, strlen(Property)+1);
	mbstowcs(transformed, Value, strlen(Value)+1); 
	VARIANT	V;
	V.vt = VT_BSTR;
	V.bstrVal = SysAllocString(transformed);
	
	// Get the OggMuxer property interface of the pin
	inPin->QueryInterface(IID_IPropertyBag, (void**)&pPropBag);
	
	// Finally write the language of the pin
	
	pPropBag->Write(transfilter, &V);
	
	// Clean up the stuff
	VariantClear(&V);
	inPin->Release();
	pPropBag->Release();

	UsedPins++;
}

genericmm_API int get_media_info(char *s_media_file, MEDIA_INFO *media_info)
{
	IGraphBuilder *pigb  = NULL;
	IMediaControl *pimc  = NULL;
	IMediaEventEx *pimex = NULL;
	IVideoWindow  *pivw  = NULL;
	IFilterGraph  *pifg  = NULL;
	IMediaSeeking *pSeek = NULL;
		
	IFileSinkFilter2 *pSink= NULL;
	
	IBaseFilter *pFileWriteFilter = NULL;
	//IBaseFilter *BaseFilter = NULL;
	//IBaseFilter *pCodecFilter = NULL;
	IBaseFilter *pAVISplitterFilter = NULL;
	IBaseFilter *pVideoRenderer = NULL;
	IBaseFilter *pOggMultiFilter = NULL;
	IBaseFilter *pVobSub = NULL;
	IBaseFilter *pffdShow = NULL;
	
	IEnumFilters *EnumFilters = NULL;
	
	IEnumPins *pEnumPins = NULL;
	IPin *pPinAVISplitterFilter = NULL;
	IPin *pPinAVISplitterFilter_out = NULL;
	IPin *pPinsOggMultiFilter[10] = {0};
	IPin *pPinVideoRenderer = NULL;
	IPin *pInFileWriteFilter = NULL;
	IPin *pStreamNamePin = NULL;
	
	int i_NumberOfFilters = 0;
	int i_NumberOfPins = 0;
	int i = 0;
	int i_input_oggfilter = 0;
	int i_output_oggfilter = 0;
	
	ULONG Fetched = 0;
	HRESULT hr = 0;
	long EvCode = 0;
	long seconds = 0;
	
	WCHAR ws_media_file[MAX_PATH];
	
	WCHAR ws_divxcodec[] = L"DivX Decoder Filter";
	WCHAR ws_ffdshow[] = L"ffdshow MPEG-4 Video Decoder";
	WCHAR ws_divx3codec[] = L"DivX MPEG-4 DVD Video Decompressor ";
	WCHAR ws_avisplitter[] = L"AVI Splitter";
	WCHAR ws_videorenderer[] = L"Video Renderer";
	WCHAR ws_vobsub[] = L"DirectVobSub (auto-loading version)";
	
	LONGLONG duration = 0;
	LONGLONG Current = 0;
	LONGLONG Stop = 0;
	LONGLONG lstart = 0;
	LONGLONG lend = 0;
	GUID timeFormat = TIME_FORMAT_NONE;
	
	MultiByteToWideChar( CP_ACP, 0, s_media_file, -1, ws_media_file, MAX_PATH );
	
	HELPER_RELEASE(pSink);	
	HELPER_RELEASE(pFileWriteFilter);
	HELPER_RELEASE(pOggMultiFilter);
	HELPER_RELEASE(pAVISplitterFilter);
	HELPER_RELEASE(pVobSub);
	HELPER_RELEASE(pffdShow);
	HELPER_RELEASE(pVideoRenderer);
	HELPER_RELEASE(pifg);
	HELPER_RELEASE(pSeek);
	HELPER_RELEASE(pivw);
	HELPER_RELEASE(pimex);
	HELPER_RELEASE(pimc);
	HELPER_RELEASE(pigb);
	
	CoInitialize(NULL);
	
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pigb);
	
	if (!SUCCEEDED(hr)) goto END;
	
	// QueryInterface for some basic interfaces
	//pigb->QueryInterface(IID_IMediaControl, (void **)&pimc);
	//pigb->QueryInterface(IID_IMediaEventEx, (void **)&pimex);
	//pigb->QueryInterface(IID_IVideoWindow, (void **)&pivw);
	pigb->QueryInterface(IID_IMediaSeeking, (void**)&pSeek);
	
	// Have the graph construct its the appropriate graph automatically
	hr = pigb->RenderFile(ws_media_file, NULL);
	if (!SUCCEEDED(hr)) goto END;
	
	hr = pSeek->GetTimeFormat(&timeFormat);
	hr = pSeek->GetDuration(&duration);
	hr = pSeek->GetPositions(&Current,&Stop);
	//hr = pSeek->GetRate(&(media_info->f_fps));
	
	hr = pSeek->SetPositions(&lstart,AM_SEEKING_AbsolutePositioning,
	&lend,AM_SEEKING_AbsolutePositioning);

	
	media_info->l_milliseconds = (long) (Stop / TIMEFORMAT);
	seconds = media_info->l_milliseconds / 1000;
	seconds2timeformat_hmss(media_info->l_milliseconds, (media_info->s_time_hmss));


END:
	
	HELPER_RELEASE(pSink);
	HELPER_RELEASE(pFileWriteFilter);
	HELPER_RELEASE(pOggMultiFilter);
	HELPER_RELEASE(pAVISplitterFilter);
	HELPER_RELEASE(pVobSub);
	HELPER_RELEASE(pffdShow);
	HELPER_RELEASE(pVideoRenderer);
	HELPER_RELEASE(pifg);
	HELPER_RELEASE(pSeek);
	HELPER_RELEASE(pivw);
	HELPER_RELEASE(pimex);
	HELPER_RELEASE(pimc);
	HELPER_RELEASE(pigb);
	
	CoUninitialize();
	
	return hr;
}


static HRESULT FilterInfoCreate(DS_FILTER_INFO **fi, int i_num)
{
	HRESULT hr = S_OK;
	*fi = (DS_FILTER_INFO*) malloc(sizeof(DS_FILTER_INFO));
	(*fi)->i_num = i_num;
	(*fi)->ps_filter = (char **) malloc(sizeof(char*) * i_num);
	(*fi)->pws_filter = (wchar_t **) malloc(sizeof(wchar_t *) * i_num);

	return hr;
}

static HRESULT FiltersInfoDestroy(DS_FILTER_INFO **fi)
{
	HRESULT hr = S_OK;
	for (int i = 0 ; i < (*fi)->i_num ; i++)
	{
		if ((*fi)->ps_filter[i] != NULL)
		{
			free((*fi)->ps_filter[i]);
			(*fi)->ps_filter[i] = NULL;
		}
		if ((*fi)->pws_filter[i] != NULL)
		{
			free((*fi)->pws_filter[i]);
			(*fi)->pws_filter[i] = NULL;
		}
	}

	free((*fi)->ps_filter);
	(*fi)->ps_filter = NULL;
	(*fi)->i_num = 0;

	return hr;
}

static HRESULT AddFileWriter(IGraphBuilder *pGraph, IBaseFilter** ppFileWriteFilter, LPCWSTR ws_output_file)
{
	HRESULT hr = S_OK;
	*ppFileWriteFilter = NULL;
	IFileSinkFilter2 *pSink = NULL;

	hr = CoCreateInstance(CLSID_FileWriter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)ppFileWriteFilter);
	if (!SUCCEEDED(hr)) goto END;
	
	hr = pGraph->AddFilter(*ppFileWriteFilter, NULL);
	if (!SUCCEEDED(hr)) goto END;

	// Set the file name.
	hr = (*ppFileWriteFilter)->QueryInterface(IID_IFileSinkFilter2, (void**)&pSink);
	pSink->SetFileName(ws_output_file, NULL);
	pSink->SetMode(AM_FILE_OVERWRITE);

	END:
	
	return hr;
}

static HRESULT AddMuxFilter(IGraphBuilder *pGraph, MUX_TYPE type, IBaseFilter** ppMux)
{
	HRESULT hr = S_OK;
	*ppMux = NULL;

	if (type == MUX_OGM)
	{
		hr = CoCreateInstance(CLSID_OggMux, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **) ppMux);
	}
	else if (type == MUX_MATROSKA)
	{
		hr = CoCreateInstance(CLSID_MatroskaMuxer, NULL, CLSCTX_INPROC_SERVER, IID_IMkxFilter, (void **) ppMux);
	}

	if (!SUCCEEDED(hr)) goto END;
	
	hr = pGraph->AddFilter(*ppMux, NULL);
	if (!SUCCEEDED(hr)) goto END;

END:

	return hr;
}

static HRESULT FindFilters(IGraphBuilder *pGraph, DS_FILTER_INFO **fi)
{
	HRESULT hr = S_OK;
	IEnumFilters *pEnum = NULL;
	ULONG cFetched = 0;
	IBaseFilter *pFilter;
	int i_num = 0;

	hr = pGraph->EnumFilters(&pEnum);

	while(pEnum->Next(1, &pFilter, &cFetched) == S_OK)
    {
		i_num++;
	}

	pEnum->Reset();

	FilterInfoCreate(fi, i_num);

	i_num = 0;

	while(pEnum->Next(1, &pFilter, &cFetched) == S_OK)
    {
		char szName[MAX_FILTER_NAME];
        FILTER_INFO FilterInfo;
		int cch = 0;

        hr = pFilter->QueryFilterInfo(&FilterInfo);
        
		if (FAILED(hr))
        {
            continue;  // Maybe the next one will work.
        }
        
		(*fi)->pws_filter[i_num] = wcsdup(FilterInfo.achName);

        cch = WideCharToMultiByte(CP_ACP, 0, FilterInfo.achName, MAX_FILTER_NAME, szName, MAX_FILTER_NAME, 0, 0);

        if (cch > 0)
		{
			(*fi)->ps_filter[i_num] = strdup(szName);
            //MessageBox(NULL, szName, TEXT("Filter Name"), MB_OK);
		}

        // The FILTER_INFO structure holds a pointer to the Filter Graph
        // Manager, with a reference count that must be released.
        if (FilterInfo.pGraph != NULL)
        {
            FilterInfo.pGraph->Release();
        }

        pFilter->Release();

		i_num++;
    }

    pEnum->Release();

	return S_OK;
}

static IPin* FindPin_(IBaseFilter* pFilter, PIN_DIRECTION search_pin_dir)
{
	HRESULT hr = S_OK;
	IEnumPins *pEnumPins = NULL;
	IPin *pPin = NULL;
	IPin *pConnectedPin = NULL;
	ULONG Fetched = 0;
	PIN_DIRECTION pin_dir;
	int i = 0;
	int bValidPin = 0;

	pFilter->EnumPins(&pEnumPins);

	do
	{
		pEnumPins->Next(1,&pPin,&Fetched);
		if (Fetched) 
		{	
			hr = pPin->QueryDirection(&pin_dir);

			hr = pPin->ConnectedTo(&pConnectedPin);

			pPin->Release();

			// Check if not already connected
			if (pConnectedPin == NULL)
			{
				if (pin_dir == search_pin_dir)
				{
					// found
					Fetched = 0;
				}
			}
			else
			{
				pConnectedPin->Release();
			}
		}
	}
	while (Fetched);

	return pPin;
}

static IPin* FindInputPin(IBaseFilter* pFilter)
{
	return FindPin_(pFilter, PINDIR_INPUT);
}

static IPin* FindOutputPin(IBaseFilter* pFilter)
{
	return FindPin_(pFilter, PINDIR_OUTPUT);
}

static HRESULT RunGraph(IGraphBuilder *pigb, 
						long l_split_at_milli_sec, 
						char *s_output_file)
{
	HRESULT hr = S_OK;
	LONGLONG duration = 0;
	LONGLONG Current = 0;
	LONGLONG Stop = 0;
	LONGLONG lstart = 0;
	LONGLONG lend = 0;
	GUID timeFormat = TIME_FORMAT_NONE;
	long EvCode = 0;

	IMediaControl *pimc  = NULL;
	IMediaSeeking *pSeek = NULL;
	IMediaEventEx *pimex = NULL;

	// QueryInterface for some basic interfaces
	pigb->QueryInterface(IID_IMediaControl, (void **)&pimc);
	pigb->QueryInterface(IID_IMediaEventEx, (void **)&pimex);
	pigb->QueryInterface(IID_IMediaSeeking, (void**)&pSeek);

	lstart = 0;
	lend = (LONGLONG) (TIMEFORMAT * l_split_at_milli_sec);
	
	hr = pSeek->GetTimeFormat(&timeFormat);
	hr = pSeek->GetDuration(&duration);
	hr = pSeek->GetPositions(&Current,&Stop);
	
	hr = pimc->Pause();

	if ((lend >= Stop) || (Current >= lend))
	{
		// do not split !!!
		hr = pimc->Run();
		do
		{
			hr = pimex->WaitForCompletion(MAX_WAIT_TIME, &EvCode);
		}while (EvCode != EC_COMPLETE);

		hr = pimc->Stop();
	}
	else
	{
		// make 1st part
		hr = pSeek->SetPositions(&lstart, AM_SEEKING_AbsolutePositioning,
			&lend,AM_SEEKING_AbsolutePositioning);
		hr = pimc->Run();

		do
		{
			hr = pimex->WaitForCompletion(MAX_WAIT_TIME, &EvCode);
		}while (EvCode != EC_COMPLETE);
		hr = pimc->Stop();
		
		rename_file(s_output_file, 0);
		
		// make 2nd part
		hr = pSeek->SetPositions(&lend,AM_SEEKING_AbsolutePositioning,
			&Stop,AM_SEEKING_AbsolutePositioning);
		hr = pimc->Run();
		do
		{
			hr = pimex->WaitForCompletion(MAX_WAIT_TIME, &EvCode);
		}while (EvCode != EC_COMPLETE);
		hr = pimc->Stop();
		
		rename_file(s_output_file, 1);
		hr = 1;
	}

	HELPER_RELEASE(pSeek);
	HELPER_RELEASE(pimex);
	HELPER_RELEASE(pimc);

	return hr;
}

static HRESULT ConnectFilter(IGraphBuilder *pGraphBuilder, 
							IBaseFilter *sourceFilter, 
							IBaseFilter *destinationFilter)
{
	HRESULT hr = S_OK;
	IPin *pOutputPin = FindOutputPin(sourceFilter);
	IPin *pInputPin = FindInputPin(destinationFilter);

	hr = pGraphBuilder->Connect(pOutputPin, pInputPin);

	return hr;
}

static HRESULT RemoveAllFiltersExceptInputSources(IGraphBuilder *pifg,
												  char* s_video_file,
												  char* s_audio_file,
												  IBaseFilter **ppVideoSourceFilter,
												  IBaseFilter **ppAudioSourceFilter )
{
	HRESULT hr = S_OK;
	
	DS_FILTER_INFO *fi = NULL;
	FindFilters(pifg, &fi);

	// remove all except video and audio source
	for (int i = 0 ; i < fi->i_num ; i++)
	{
		IBaseFilter* pFilter = NULL;
		hr = pifg->FindFilterByName(fi->pws_filter[i], &pFilter);
		if (!SUCCEEDED(hr)) goto END;

		if (strcmp(fi->ps_filter[i], s_video_file) == 0)
		{
			*ppVideoSourceFilter = pFilter;
		}
		else if (strcmp(fi->ps_filter[i], s_audio_file) == 0)
		{
			*ppAudioSourceFilter = pFilter;
		}
		else
		{
			// Remove it
			hr = pifg->RemoveFilter(pFilter);
		}

		pFilter->Release();
	}

END:

	return hr;
}

genericmm_API int DirectShowCombine(MUX_DATA *pr_muxdata)
{
	HRESULT hr = S_OK;
	IGraphBuilder *pigb  = NULL;
	//IFilterGraph  *pifg  = NULL;
		
	//IPropertyBag *pProp;
	//VARIANT varName;
	
	// Base filters
	IBaseFilter *pVideoSourceFilter = NULL;
	IBaseFilter *pAudioSourceFilter = NULL;
	IBaseFilter *pMuxFilter = NULL;
	IBaseFilter *pFileWriteFilter = NULL;

	DS_FILTER_INFO *fi = NULL;
		
	ULONG Fetched = 0;
	
	
	WCHAR ws_video_file[MAX_PATH];
	WCHAR ws_audio_file[MAX_PATH];
	WCHAR ws_output_file[MAX_PATH];

	char *s_video_file = pr_muxdata->r_mediadata[0].s_filename;
	char *s_audio_file = pr_muxdata->r_mediadata[1].s_filename;
	char *s_output_file = pr_muxdata->r_mediadata[2].s_filename;

	long l_split_at_milli_sec = pr_muxdata->l_split;
	MUX_TYPE r_mux_type = pr_muxdata->r_mux_type;
	
	// TODO
	MultiByteToWideChar( CP_ACP, 0, s_video_file, -1, ws_video_file, MAX_PATH );
	MultiByteToWideChar( CP_ACP, 0, s_audio_file, -1, ws_audio_file, MAX_PATH );
	MultiByteToWideChar( CP_ACP, 0, s_output_file, -1, ws_output_file, MAX_PATH );
	
	//_bstr_t bstr_video(s_video_name);
	//_bstr_t bstr_audio(s_audio_name);
	
	CoInitialize(NULL);

	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pigb);
	if (!SUCCEEDED(hr)) goto END;
	
	//hr = pigb->QueryInterface(IID_IFilterGraph, (void **)&pifg);
	//if (!SUCCEEDED(hr)) goto END;
	
	// Have the graph construct its the appropriate graph automatically
	hr = pigb->RenderFile(ws_video_file, NULL);
	if (!SUCCEEDED(hr)) goto END;

	// Have the graph construct its the appropriate graph automatically
	hr = pigb->RenderFile(ws_audio_file, NULL);
	if (!SUCCEEDED(hr)) goto END;
    
	hr = RemoveAllFiltersExceptInputSources(pigb, 
		s_video_file, 
		s_audio_file, 
		&pVideoSourceFilter,
		&pAudioSourceFilter);

	hr = AddMuxFilter(pigb, r_mux_type, &pMuxFilter);
	if (FAILED(hr)) goto END;

	hr = AddFileWriter(pigb, &pFileWriteFilter, ws_output_file);
	if (FAILED(hr)) goto END;

	// Connect Video Input Source -> Mux
	hr = ConnectFilter(pigb, pVideoSourceFilter, pMuxFilter);

	// Connect Audio Input Source -> Mux
	hr = ConnectFilter(pigb, pAudioSourceFilter, pMuxFilter);

	// Connect Mux -> FileWriter
	hr = ConnectFilter(pigb, pMuxFilter, pFileWriteFilter);
	if (FAILED(hr)) goto END;

	// Do it
	hr = RunGraph(pigb, l_split_at_milli_sec, s_output_file);
	if (FAILED(hr)) goto END;
	
	/*
	
	
	// add language
	varName.vt = VT_BSTR;
	i = 0;
	pOggMultiFilter->EnumPins(&pEnumPins);
	do
	{
		pEnumPins->Next(1,&pStreamNamePin,&Fetched);
		if (Fetched) 
		{	
			hr = pStreamNamePin->QueryDirection(&pin_dir);
			
			if (pin_dir == PINDIR_INPUT)
			{
				pStreamNamePin->QueryInterface(IID_IPropertyBag, (void **)&pProp);
				if (i == 0)
				{
					varName.bstrVal = bstr_video;
				}
				else
				{
					varName.bstrVal = bstr_audio;
				}
				
				hr = pProp->Write(L"LANGUAGE", &varName);
				HELPER_RELEASE(pProp);
				i++;
			}
			HELPER_RELEASE(pStreamNamePin);
		}
	}
	while (Fetched);
	*/
		
END:
	
	HELPER_RELEASE(pMuxFilter);
	HELPER_RELEASE(pVideoSourceFilter);
	HELPER_RELEASE(pAudioSourceFilter);
	HELPER_RELEASE(pFileWriteFilter);
	HELPER_RELEASE(pigb);
	
	CoUninitialize();
	
	return hr;
}

genericmm_API int combine_avi_ogg(char *s_avi_file, 
								  char *s_ogg_file,
								  char *s_out_file,
								  long i_split_at_milli_sec,
								  char *s_video_name,
								  char *s_audio_name)
{
	IGraphBuilder *pigb  = NULL;
	IMediaControl *pimc  = NULL;
	IMediaEventEx *pimex = NULL;
	IVideoWindow  *pivw  = NULL;
	IFilterGraph  *pifg  = NULL;
	IMediaSeeking *pSeek = NULL;
		
	IPropertyBag *pProp;
	VARIANT varName;
		
	IFileSinkFilter2 *pSink= NULL;
	
	IBaseFilter *pFileWriteFilter = NULL;
	IBaseFilter *pAVISplitterFilter = NULL;
	IBaseFilter *pVideoRenderer = NULL;
	IBaseFilter *pOggMultiFilter = NULL;
	IBaseFilter *pVobSub = NULL;
	IBaseFilter *pffdShow = NULL;
	
	IEnumFilters *EnumFilters = NULL;
	
	IEnumPins *pEnumPins = NULL;
	IPin *pPinAVISplitterFilter = NULL;
	IPin *pPinAVISplitterFilter_out = NULL;
	IPin *pPinsOggMultiFilter[10] = {0};
	IPin *pPinVideoRenderer = NULL;
	IPin *pInFileWriteFilter = NULL;
	IPin *pStreamNamePin = NULL;
	
	PIN_DIRECTION pin_dir;
	
	int i_NumberOfFilters = 0;
	int i_NumberOfPins = 0;
	int i = 0;
	int i_input_oggfilter = 0;
	int i_output_oggfilter = 0;
	
	ULONG Fetched = 0;
	HRESULT hr = 0;
	long EvCode = 0;
	
	WCHAR ws_avi_file[MAX_PATH];
	WCHAR ws_ogg_file[MAX_PATH];
	WCHAR ws_out_file[MAX_PATH];
	
	WCHAR ws_divxcodec[] = L"DivX Decoder Filter";
	WCHAR ws_ffdshow[] = L"ffdshow MPEG-4 Video Decoder";
	WCHAR ws_divx3codec[] = L"DivX MPEG-4 DVD Video Decompressor ";
	WCHAR ws_avisplitter[] = L"AVI Splitter";
	WCHAR ws_videorenderer[] = L"Video Renderer";
	WCHAR ws_vobsub[] = L"DirectVobSub (auto-loading version)";
	
	LONGLONG duration = 0;
	LONGLONG Current = 0;
	LONGLONG Stop = 0;
	LONGLONG lstart = 0;
	LONGLONG lend = 0;
	GUID timeFormat = TIME_FORMAT_NONE;
	
	MultiByteToWideChar( CP_ACP, 0, s_avi_file, -1, ws_avi_file, MAX_PATH );
	MultiByteToWideChar( CP_ACP, 0, s_ogg_file, -1, ws_ogg_file, MAX_PATH );
	MultiByteToWideChar( CP_ACP, 0, s_out_file, -1, ws_out_file, MAX_PATH );
	
	HELPER_RELEASE(pSink);	
	HELPER_RELEASE(pFileWriteFilter);
	HELPER_RELEASE(pOggMultiFilter);
	HELPER_RELEASE(pAVISplitterFilter);
	HELPER_RELEASE(pVobSub);
	HELPER_RELEASE(pffdShow);
	HELPER_RELEASE(pVideoRenderer);
	HELPER_RELEASE(pifg);
	HELPER_RELEASE(pSeek);
	HELPER_RELEASE(pivw);
	HELPER_RELEASE(pimex);
	HELPER_RELEASE(pimc);
	HELPER_RELEASE(pigb);
	
	_bstr_t bstr_video(s_video_name);
	_bstr_t bstr_audio(s_audio_name);
	
	CoInitialize(NULL);
	
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pigb);
	
	if (!SUCCEEDED(hr)) goto END;
	
	// QueryInterface for some basic interfaces
	pigb->QueryInterface(IID_IMediaControl, (void **)&pimc);
	pigb->QueryInterface(IID_IMediaEventEx, (void **)&pimex);
	pigb->QueryInterface(IID_IVideoWindow, (void **)&pivw);
	pigb->QueryInterface(IID_IMediaSeeking, (void**)&pSeek);
	
	// Have the graph construct its the appropriate graph automatically
    hr = pigb->RenderFile(ws_avi_file, NULL);
	if (!SUCCEEDED(hr)) goto END;
	
    hr = pigb->QueryInterface(IID_IFilterGraph, (void **)&pifg);
	if (!SUCCEEDED(hr)) goto END;

	// remove videorenderer
	hr = pifg->FindFilterByName(ws_videorenderer, &pVideoRenderer);
	if (!SUCCEEDED(hr)) goto END;
	
	hr = pifg->RemoveFilter(pVideoRenderer);
	if (!SUCCEEDED(hr)) goto END;


	hr = pifg->FindFilterByName(ws_vobsub, &pVobSub);
	if (SUCCEEDED(hr)) 
	{
		(void) pifg->RemoveFilter(pVobSub);
	}
	
	hr = pifg->FindFilterByName(ws_ffdshow, &pffdShow);
	if (SUCCEEDED(hr)) 
	{
		(void) pifg->RemoveFilter(pffdShow);
	}
	
	// find AVI Splitter
	hr = pifg->FindFilterByName(ws_avisplitter, &pAVISplitterFilter);
	if (!SUCCEEDED(hr)) goto END;
	pAVISplitterFilter->EnumPins(&pEnumPins);
	do
	{
		pEnumPins->Next(1,&pPinAVISplitterFilter,&Fetched);
		if (Fetched) 
		{	
			hr = pPinAVISplitterFilter->QueryDirection(&pin_dir);
			
			if (pin_dir == PINDIR_OUTPUT)
			{
				// remember this one !
				hr = pifg->Disconnect(pPinAVISplitterFilter);
				pPinAVISplitterFilter_out = pPinAVISplitterFilter;
				break;
			}
		}
		
		i++;
	}while (Fetched);
	
//	fprintf(fp, "hr = pifg->FindFilterByName(ws_avisplitter, &pAVISplitterFilter);\n");
	
	// add oggmultiplexer
	hr = CoCreateInstance(CLSID_OggMux, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&pOggMultiFilter);
	if (!SUCCEEDED(hr)) goto END;
	
//	fprintf(fp, "hr = CoCreateInstance(CLSID_OggMux, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&pOggMultiFilter);\n");
	
	hr = pifg->AddFilter(pOggMultiFilter, NULL);
	if (!SUCCEEDED(hr)) goto END;
	
//	fprintf(fp, "hr = pifg->AddFilter(pOggMultiFilter, NULL);\n");
	
	// find correct pins
	i = 0;
	pOggMultiFilter->EnumPins(&pEnumPins);
	do
	{
		pEnumPins->Next(1,&pPinsOggMultiFilter[i],&Fetched);
		if (Fetched) 
		{	
			hr = pPinsOggMultiFilter[i]->QueryDirection(&pin_dir);
			
			if (pin_dir == PINDIR_INPUT)
			{
				i_input_oggfilter = i;
			}
			else
			{
				i_output_oggfilter = i;
			}
		}
		
		i++;
		
	}
	while (Fetched);
	
//	fprintf(fp, "pOggMultiFilter->EnumPins(&pEnumPins);\n");
	
	// add filewriter	
	hr = CoCreateInstance(CLSID_FileWriter, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&pFileWriteFilter);
	if (!SUCCEEDED(hr)) goto END;
	
//	fprintf(fp, "hr = CoCreateInstance(CLSID_FileWriter, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&pFileWriteFilter);\n");
	
	hr = pifg->AddFilter(pFileWriteFilter, NULL);
	if (!SUCCEEDED(hr)) goto END;
	
//	fprintf(fp, "hr = pifg->AddFilter(pFileWriteFilter, NULL);\n");
	
	// find pins
	i = 0;
	pFileWriteFilter->EnumPins(&pEnumPins);
	do
	{
		pEnumPins->Next(1,&pInFileWriteFilter,&Fetched);
		if (Fetched) 
		{	
			hr = pInFileWriteFilter->QueryDirection(&pin_dir);
			
			if (pin_dir == PINDIR_OUTPUT)
			{
				// uhuh
				return 0;
			}
		}
		
		i++;
	}
	while (Fetched);
	
//	fprintf(fp, "pFileWriteFilter->EnumPins(&pEnumPins);\n");
	
	// CONNECT :
	// output from AVI splitter: pPinAVISplitterFilter_out
	hr = pifg->ConnectDirect(pPinAVISplitterFilter_out, pPinsOggMultiFilter[i_input_oggfilter], NULL);
	if (!SUCCEEDED(hr)) goto END;
	hr = pifg->ConnectDirect(pPinsOggMultiFilter[i_output_oggfilter], pInFileWriteFilter, NULL);
	if (!SUCCEEDED(hr)) goto END;
	
	// Have the graph construct its the appropriate graph automatically
	hr = pigb->RenderFile(ws_ogg_file, NULL);
	if (!SUCCEEDED(hr)) goto END;
	
//	fprintf(fp, "hr = pigb->RenderFile(ws_ogg_file, NULL);\n");
	
	
	
	// Set the file name.
	pFileWriteFilter->QueryInterface(IID_IFileSinkFilter2, (void**)&pSink);
	pSink->SetFileName(ws_out_file, NULL);
	pSink->SetMode(AM_FILE_OVERWRITE);
	
//	fprintf(fp, "pFileWriteFilter->QueryInterface(IID_IFileSinkFilter2, (void**)&pSink);\n");
	
	
	
	
	// property info...
	/*
	hr = pOggMultiFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pProp);
	
	  CAUUID caGUID;
	  pProp->GetPages(&caGUID);
	  
	HELPER_RELEASE(pProp);*/
	
	//pOggMultiFilter->QueryInterface(IID_IPersistPropertyBag, (void**)&pPersist);
	
	//pOggMultiFilter->QueryInterface(IID_IPersistPropertyBag, (void**)&pPersist);
	
    //pPinsOggMultiFilter[i_input_oggfilter]
	
	
	
	// add language
	varName.vt = VT_BSTR;
	i = 0;
	pOggMultiFilter->EnumPins(&pEnumPins);
	do
	{
		pEnumPins->Next(1,&pStreamNamePin,&Fetched);
		if (Fetched) 
		{	
			hr = pStreamNamePin->QueryDirection(&pin_dir);
			
			if (pin_dir == PINDIR_INPUT)
			{
				pStreamNamePin->QueryInterface(IID_IPropertyBag, (void **)&pProp);
				if (i == 0)
				{
					varName.bstrVal = bstr_video;
				}
				else
				{
					varName.bstrVal = bstr_audio;
				}
				
				hr = pProp->Write(L"LANGUAGE", &varName);
				HELPER_RELEASE(pProp);
				i++;
			}
			HELPER_RELEASE(pStreamNamePin);
		}
	}
	while (Fetched);
	
	/*
	OleCreatePropertyFrame(NULL,
	0,
	0,
	L"Filter",          // Caption for the dialog box
	1,                  // Number of filters
	(IUnknown **)&pOggMultiFilter,  // Pointer to the filter whose property 
	// Pages are being displayed. This can 
	// be an array of pointers if more than
	// one filter's property pages are to 
	// be displayed. Note that only 
	// properties common to all the filters 
	// can be displayed on the same modal 
	// dialog.
	caGUID.cElems,      // Number of property pages
	caGUID.pElems,      // Pointer to property page CLSIDs
	0,
	0,
	NULL);*/
	
	lstart = 0;
	lend = (LONGLONG) (TIMEFORMAT * i_split_at_milli_sec);
	
	hr = pSeek->GetTimeFormat(&timeFormat);
	hr = pSeek->GetDuration(&duration);
	hr = pSeek->GetPositions(&Current,&Stop);
	
	hr = pimc->Pause();

	if ((lend >= Stop) || (Current >= lend))
	{
		// do not split !!!
		hr = pimc->Run();
//		fprintf(fp, "hr = pimc->Run();\n");
//		fclose(fp);
		do
		{
			hr = pimex->WaitForCompletion(MAX_WAIT_TIME, &EvCode);
		}while (EvCode != EC_COMPLETE);
		hr = pimc->Stop();
	}
	else
	{
		// make 1st part
		hr = pSeek->SetPositions(&lstart,AM_SEEKING_AbsolutePositioning,
			&lend,AM_SEEKING_AbsolutePositioning);
		hr = pimc->Run();
		do
		{
			hr = pimex->WaitForCompletion(MAX_WAIT_TIME, &EvCode);
		}while (EvCode != EC_COMPLETE);
		hr = pimc->Stop();
		
		rename_file(s_out_file, 0);
		
		// make 2nd part
		hr = pSeek->SetPositions(&lend,AM_SEEKING_AbsolutePositioning,
			&Stop,AM_SEEKING_AbsolutePositioning);
		hr = pimc->Run();
		do
		{
			hr = pimex->WaitForCompletion(MAX_WAIT_TIME, &EvCode);
		}while (EvCode != EC_COMPLETE);
		hr = pimc->Stop();
		
		rename_file(s_out_file, 1);
		hr = 1;
	}
	
END:
	
	HELPER_RELEASE(pSink);
	HELPER_RELEASE(pFileWriteFilter);
	HELPER_RELEASE(pOggMultiFilter);
	HELPER_RELEASE(pAVISplitterFilter);
	HELPER_RELEASE(pVobSub);
	HELPER_RELEASE(pffdShow);
	HELPER_RELEASE(pVideoRenderer);
	HELPER_RELEASE(pifg);
	HELPER_RELEASE(pSeek);
	HELPER_RELEASE(pivw);
	HELPER_RELEASE(pimex);
	HELPER_RELEASE(pimc);
	HELPER_RELEASE(pigb);
	
	CoUninitialize();
	
	return hr;
}

static void rename_file(char *s_old, int i_part)
{
	int i_len = 0;
	int i = 0;
	char s_tmp_file[256] = {0};
	char s_tmp_1[256] = {0};
	char s_tmp_2[16] = {0};
	
	i_len = (int) strlen(s_old);
	
	for (i = i_len ; i > 0 ; i --)
	{
		if (s_old[i] == '.')
		{
			break;
		}
	}
	
	strncpy(s_tmp_1, s_old, i);
	strcpy(s_tmp_2, &s_old[i]);
	
	// rename file
	//sprintf(s_tmp_file, "%s%d%s", s_tmp_1, i_part, s_tmp_2);
	sprintf(s_tmp_file, "%s - Part %d%s", s_tmp_1, i_part + 1, s_tmp_2);

	(void) rename( s_old, s_tmp_file );
}

genericmm_API void FreeCodecs(int i_num_codecs, char ***as_codecs)
{
	for (int i =  0; i < i_num_codecs ; i++)
	{
		free((*as_codecs)[i]);
		(*as_codecs)[i] = 0;
	}

	free(*as_codecs);
	*as_codecs = 0;
}

genericmm_API int GetCodecs(char ***as_codecs)
{
	COMPVARS			g_Vcompression;
	BITMAPINFOHEADER *bihInput = NULL;
	CCInfo cci;
	ICINFO info;
	int i = 0;
	int nComp = 0;
	char s[64] = {0};
	
	memset(&g_Vcompression, 0, sizeof g_Vcompression);
	g_Vcompression.dwFlags |= ICMF_COMPVARS_VALID;
	g_Vcompression.lQ = 10000;
	g_Vcompression.cbSize = sizeof(COMPVARS);
	
	cci.fccSelect	= NULL;
	cci.pState		= NULL;
	cci.cbState		= 0;
	cci.hic			= NULL;
	cci.piiCurrent	= NULL;
	
	if (g_Vcompression.dwFlags & ICMF_COMPVARS_VALID) 
	{
		cci.fccSelect	= g_Vcompression.fccHandler;
		
		if (g_Vcompression.hic) 
		{
			cci.cbState	= ICGetStateSize(g_Vcompression.hic);
			
			if (cci.cbState>0) 
			{
				cci.pState = new char[cci.cbState];
				
				if (!cci.pState)
					return 0;
				
				ICGetState(g_Vcompression.hic, cci.pState, cci.cbState);
			}
		}
	}
	
	nComp = 0;
	cci.pCompInfo = NULL;
	cci.nComp = 0;
	
	for(i=0; ICInfo(ICTYPE_VIDEO, i, &info); i++) 
	{
		HIC hic;
		
		hic = ICOpen(info.fccType, info.fccHandler, ICMODE_COMPRESS);
		
		if (hic) 
		{
			if (!bihInput || ICERR_OK==ICCompressQuery(hic, bihInput, NULL)) 
			{
				if (cci.nComp+1 > nComp) 
				{
					ICINFO *pNewArray;
					nComp += 16;
					
					pNewArray = new ICINFO[nComp];
					
					if (!pNewArray) 
					{
						delete cci.pState;
						ICClose(hic);
						return 0;
					}
					
					if (cci.nComp)
					{
						memcpy(pNewArray, cci.pCompInfo, cci.nComp*sizeof(ICINFO));
					}
					
					delete cci.pCompInfo;
					cci.pCompInfo = pNewArray;
				}
				
				ICGetInfo(hic, &cci.pCompInfo[cci.nComp], sizeof(ICINFO));
				cci.pCompInfo[cci.nComp].fccHandler = info.fccHandler;
				cci.nComp++;
			}
			ICClose(hic);
		}
	}
	
	//cci.pCV = lpCompVars;
	//cci.bih = bihInput;
	cci.pCV = &g_Vcompression;
	//cci.bih = bihInput;

	*as_codecs = (char**) calloc(cci.nComp, sizeof(char*));
	
	for( i = 0; i < cci.nComp; i++) 
	{
	    WideCharToMultiByte( CP_ACP, 0, cci.pCompInfo[i].szDescription, -1, s, sizeof(s), NULL, NULL );
		(*as_codecs)[i] = _strdup(s);
		//WideCharToMultiByte(CP_ACP, 0, cci.pCompInfo[i].szName, -1, cci.s_name, sizeof cci.s_name, NULL, NULL);
	}
	
	if (cci.hic)
	{
		ICClose(cci.hic);
	}
	
	delete cci.pCompInfo;
	delete cci.pState;
	
	return cci.nComp;
}