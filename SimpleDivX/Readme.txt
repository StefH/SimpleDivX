/*--SimpleDivx--------------------------------------------------------------------------------------*/
Info:
This is a graphical user-interface for converting a dvd to avi/mkv/ogm with 
xvid/divx/h264 video codec and mp3/ac3 as audio codec.

/*--------------------------------------------------------------------------------------------------*/

+Executables:
virtualdubmod	        (VirtualDubMod_1.5.10.3_b2550)
mencoder and mplayer    (MPlayer-generic-r37853+g6d7f566)

+Internal Dll's
genericmm.dll	(0.0.0.40)
vobinfo.dll     (0.0.0.41)


+External Dll's
General
mfc71.dll       (7.10.3077.0)
msvcr71.dll     (7.10.3052.4)
msvcp71.dll     (7.10.3077.0)

Needed for Subtitles
vobsub.dll      (2.0.23.0)

Needed for VirtualDubMod
corona.dll	(?)

Needed for mencoder
intl.dll        (0.12.0.0)
iconv.dll       (1. 9.0.0)
pthreadGC2.dll  (?)


/*--------------------------------------------------------------------------------------------------*/

Version History:

1.5.0 (15-05-2016)
NEW*
	--Software--
	- Latest version from mencoder / mplayer is supported (http://oss.netfarm.it/mplayer/)
    - Latest VirtualDubMod_1.5.10.3_b2550
	
1.40.25 (07-07-2007)
NEW*
	--Software--
	- Latest version from mencoder from http://esby.free.fr/CelticDruid [ 25/10/2006 ]
	
	--Codec--
	- Added multiprocessor support encoding for H264, xvid and divx. (threads=auto and threads=2)
	
	
	--Video--
	- Added 10 and 15 fps selection.
	
	--System--
	- Batch size increased till 200.
	- Language translation for new items added. (Italian)
	
	
1.40.24 (21-10-2006)
NEW*
	--Video--
	- Added 'all' option to title dropdown box to allow processing from all titles if more than 1 are found.
	
FIX*    
    - Codec selection box was not updated when using batchmode -> fixed.


1.40 Preview 23 (14-04-2006)
NEW*
	--Software--
	- Registry key is deleted when program is un-installed.
	- Added latest virtualdubmod (1.5.10.2_b2542)
	- Added debug option. (add -d or -debug to the startup settings from SimpleDivX.)
	
	--Audio--
	- Better AC3 detection for VOB files.
	
	--Video--
	- Added title support for IFO files.
	
	--System--
	- Language translation for new items added. (German, Czech, Polish, English,Italian and Dutch.)
	- More items are translated now.
	
	--Codec--
	- 2-pass logfile is now updated when project name is changed.
	- Maximum bitrate is limited to 16000kbps.
	
	--Guide--
	- Screenshots updated. 
	- Russian Guide added.
	  
FIX*    
        - "Print debug settings" is fixed.
        - AC3 conversion was not correct when using xvid or h264 --> fixed.
        - IVTC was automatically chosen if fps was 29.9 --> fixed.
        - Saved encoding batch list was corrupt.
        - Switching "Check Audio" to enabled/disabled did not update languages id list.
        - Movie length not ok --> fixed.
        
TODO*
        
        - Investigate the 'harddup' issue with NTSC movies ?
        - More support for .ts files.
        - More NTSC resolutions.
        - Check out 2-pass problem h264.

1.40 Preview 19 (09-02-2006)
NEW*
	--Software--
	- New virtualdubmod (1.5.10.2)
	
	- Audio -
	- Possible to convert pcm and mp2 to ac3 [2 channel].
	
	--Video--
	- Added source input option to video-page.
	- Basic support for .ts files.
	
	--System--
	- Added Matroska + OGM container formats.
	  
FIX*    
    - NTSC vob files are correctly detected as NTSC instead of unknown.
    - FMP4 issue fixed.
    - Add mouse-over helptext to de-interlace field on video-page.
    - AC3 conversion was not correct when using xvid or h264 --> fixed.
        
TODO*
        
        - Investigate the 'harddup' issue with NTSC movies ?
        - More support for .ts files.
        

1.40 Preview 14 (20-01-2006)
NEW*
	--Software--
	- Added missing dll's which were needed by mencoder.
	  
FIX*    
        - Re-enabled frame-rate selection.
        
TODO*
        - Add mouse-over helptext to De-interlace field on video-page.
        - Investigate the 'harddup' issue.
        - Investigate FMP4 issue.


1.40 Preview 13 (18-01-2006)
NEW*
	--Software--
	- Latest version from INNO setup used (5.1.6.)
	- Latest version from mencoder from http://www.aziendeassociate.it/cd.asp?dir=/mplayer [ 01/01/2006 ]
	- Added msvcp71.dll to setup package.
	
	--System--
	- Automatically select the best mencoder suitable for the CPU for maximum speed.
	  (Pentium 3, Pentium 4, AMD XP and AMD 64)
	  
FIX*    
        - Disk and Video priority where not valid after first install simpledivx -> fixed.
        - Re-enabled frame-rate selection.
        
TODO*
        - Add mouse-over helptext to De-interlace field on video-page.
        - Investigate the 'harddup' issue.
        	

1.40 Preview 12 (07-11-2005)
NEW*
	--Software--
	- Latest version from mencoder + mplayer from http://oss.netfarm.it which supports h264 codec.
	- Added msvcp71.dll to setup package.

        --GUI--

	--Language--
	- Italian language file updated.
		
	--Compression--
	- Added H264 codec.
	
	--Audio--
	
	--System--
		
	
FIX*    
        - Advanced option "dark masking" was not saved in registry -> fixed.
        - Possible to select AC3 audio when audio creation was disabeld -> fixed.

	
TODO*
	- Update the guide.
	- Only possible to select Video_TS or 1 vob-file to encode.
	- Pass the min/max quantizer to mencoder
	
BUG*
        - Resample audio to 44100hz does not work correct
        - Add end-credits detection does not work anymore.


1.40 Preview 11 (26-06-2005)
NEW*
	--Software--
	- New encoder for mpeg to avi is used : mencoder and mplayer.

        --GUI--
        

	--Language--
	- Italian language file updated.
		
	--Compression--
	
	
	--Audio--
	
	
	--System--
	
		
	
FIX*    
        - When accessing tab 7 , crash. This is fixed.
        - msvcp71.dll was missing -> added to setup package.

	
TODO*
	- Update the guide.
	- Only possible to select Video_TS or 1 vob-file to encode.
	- Pass the min/max quantizer to mencoder
	
BUG*
        - Resample audio to 44100hz does not work correct
        - Add end-credits detection does not work anymore.


1.40 Preview 10 (16-05-2005)
NEW*
	--Software--
	- New encoder for mpeg to avi is used : mencoder and mplayer.
	  These files are not included in setup package, please download them from :
	  http://www.mplayerhq.hu/ Make sure to download the pre6 release.

        --GUI--
        - The last used input directory is remembered in Folder-Browse-Dialog.

	--Language--
	- Norwegian and Danish added.
		
	--Compression--
	- NTSC audio/video sync problems are fixed now.
	- Added more advanced options for mpeg4 and xvid.
	- Possible to split on fade-out.
	
	--Audio--
	- Now its possible to convert LPCM and MP2 (mpeg) audio !
	
	--System--
	- Re-enabled start at LBA
	- Get default-values is working again.
		
	
FIX*    
        - Detection from AC3 is improved.
        - Detection from audio in VOB files is fixed.
        - Splitting a movie from > 1GB is fixed.
        - Min/Max quantizer error (2..16) fixed.
        - When a IFO was selected from dvd with lcpm audio -> maybe crash and wrong audio info.
        - Problem fixed when AC3 could not be selected even when the source was AC3.
        - Wrong time displayed in NFO file when VOB was selected.
        - Batch-list is saved correctly.
        - Settings are saved correctly when 'shutdown when ready' is checked.
        - Vobsub subtitle extraction fixed.
        - When accessing tab 7 , crash. This is fixed.
        - Small fixes + layout changes.
	
TODO*
	- Update the guide.
	- Only possible to select Video_TS or 1 vob-file to encode.
	- Pass the min/max quantizer to mencoder
	
BUG*
        - Resample audio to 44100hz does not work correct
        - Add end-credits detection does not work anymore.

	

1.39 (08-01-2005)
NEW*
	--Software--
	- Latest stable build from Lame is included in the package. (3.96.1)

	--Language--
	- Norwegian added.
		
	--Compression--
	- Added support for DivX 5.2.1 PRO
		
	--System--
		
	
FIX*    
	- 
	
	 
	
BUG*
	- Add end-credits detection when using xvid.
	- Vobsub subtitle extraction doesn't work on windows 9x ?
	- When no audio detected, virtualdub can give error when muxing.
	- NTSC audio/video sync problems.
	
	
TODO*	
	- Update screenshots for the guide.
	- Update all language files with the tags : SPLIT_FILE, SPLIT_PARTS and SPLIT_MB.
	- Add support for LPCM audio.


1.38 (25-09-2004)
NEW*
	--Software--
	- All dll's and exe's are no longer compressed with upx, to avoid problems with WindowsXP SP2.
	- Mpeg2avi 2.1-04b is used.

	--Language--
	- Language files are updated : italian and czech.
	- Language files are added : simplified chinese, romanian.
		
	--Compression--
	- Added all the IDCT options again.
	- Better and faster splitting when using virtualdub.
	
	--System--
		
	
FIX*    
	- Split settings (MB and parts) is fixed when using batch-mode.
	- Vobsub splitting is fixed.
	 
	
BUG*
	- Add end-credits detection when using xvid.
	- Vobsub subtitle extraction doesn't work on windows 9x ?
	- When no audio detected, virtualdub can give error when muxing.
	
	
TODO*	
	- Update screenshots for the guide.
	- Update all language files with the tags : SPLIT_FILE, SPLIT_PARTS and SPLIT_MB.
	- Add support for LPCM audio.


1.37 (13-08-2004)
NEW*
	--Software--
	- New mpeg2avi included. (2.1-04)
	- New vobinfo.dll (0.0.0.33)
	- Used ogg dll's from the megamix2 package. (http://rarewares.soniccompression.com/ogg.html)

	
	--Compression--
	- Support for DivX 5.2
	
	--System--
	- Added support for splitting the output file in multiple parts.
	
	
FIX*    
	- Preview creates .ac3 temp files.
	- Small fix when selecting 2 or 6 channel audio by AC3.
	- Settings were not correctly saved on the "Project" page.
	- Fix for divx incorrect bitrate.
	- mpeg2avi.exe is updated -> 2-pass mode for divx 5.1/5.2 is supported.
	- Selected IFO was not remembered correctly.
	- Video and disk-priority is fixed.
	- Better Audio stream detection for vob files.
	 
	
BUG*
	- Vobsub subtitle extraction doesn't work on windows 9x ?
	- When no audio detected, virtualdub can give error when muxing.
	- Vobsub splitting only works when splitting in 2 parts.
	- Split settings (MB and parts) do not work when using batch-mode.
	
TODO*	
	- Update screenshots for the guide.
	- Update all language files with the tags : SPLIT_FILE, SPLIT_PARTS and SPLIT_MB.
	

1.36 (07-06-2004)
NEW*
	--Software--
	- New BeSweet included. (1.5 beta 28)
	- New inno-setup (4.2.2) used for installer
	
	--Layout--
	- New Screenshots for the Guide.
	
	
FIX*    
	- When ogg audio was choosen, the OGM container was selected as default.
	  And if the user choose MKV this was not correctly remembered.
	- The maximum of IFO files which can be present in same directory is increased from 32 to 64.
	- The maximum of VOB files which can be present in same directory is decreased from 99 to 64.
	- XviD doesn't support de-interlacing -> removed from advanced settings window.
	- Advanced settings from XviD not written in .nfo file.

BUG*
	- Vobsub subtitle extraction doesn't work on windows 9x ?
	- 2-pass DivX 5.1 is not working. (m2a has some troubles starting the 2nd pass)



1.35 (20-05-2004)
NEW*
	--Software--
	- New XviD rc4 supported
	- New inno-setup (4.2.2) used for installer
	
	--System--
	- Updated the language files (italian, polish, portuguese, croatian, bulgarian, 
	  french, swedish and russian)
	- Updated the Guides (italian and czech)
	
	--Layout--
	- Added a manifest file which makes SimpleDivX look more XP-like when using XP. (Thanks to Fron)
	
	--Audio--
	- Max dB changed to 20.


FIX*    
	- Small fix when selecting ifo-files.

BUG*
	- Vobsub subtitle extraction doesn't work on windows 9x ?
	- 2-pass DivX 5.1 is not working. (m2a has some troubles starting the 2nd pass)
	

1.34 (05-04-2004)
NEW*
	--Software--
	- New BeSweetv1.5b26 + new dll's
		
	--System--
	- TABS can be translated
	- Updated the languages. (Czech)
		

FIX*    
	- Selectbox for "extract subtitles" now correctly remembered.
	- When no delay was detected on video/audio and ac3 was choosen as audio stream -> crash

BUG*
	- Vobsub subtitle extraction doesn't work on windows 9x ?
	- 2-pass DivX 5.1 is not working. (m2a has some troubles starting the 2nd pass)


1.33 (0x-03-2004)
NEW*
	--Software--
		
	--System--
	- Updated the Guide.
		
	--Compression--
	

FIX*    
	- When vts_01_0.ifo was choosen, only first vob file was processed.

BUG*
	- Vobsub subtitle extraction doesn't work on windows 9x ?
	- 2-pass DivX 5.1 is not working. (m2a has some troubles starting the 2nd pass)


1.32 (06-03-2004)
NEW*
	--Software--
	- New VirtualDubMod (1.5.10.1)
	
	--System--
	- Better OGM and Matroska support.
	
	--Compression--
	- Removed 3ivx support.
	
	

FIX*    
	- Xvid 1.0 rc 2 is supported only now.
	- SimpleDivX.dat file is much smaller now.

BUG*
	- Vobsub subtitle extraction doesn't work on windows 9x ?
	- 2-pass DivX 5.1 is not working. (m2a has some troubles starting the 2nd pass)


1.30 (26-11-2003)
NEW*
	--Software--
	- New VirtualDub (1.5.9)
	- New BeSweet (1.5beta22)
	
	--Layout--
	- Guide updated.
	- Languages updated.
	
	--Compression--
	- Added a "Custom Codec" option to the codec list.


FIX*    
	- Fixed some small bugs with subtitles.


BUG*
	- Vobsub subtitle extraction doesn't work on windows 9x ?
	- 2-pass DivX 5.1 is not working. (m2a has some troubles starting the 2nd pass)

	

1.29 (23-09-2003)
NEW*
	--Software--
	- New BeSweet.
	
	--Layout--
	- Languages updated : English, Italian, French, German
	- Guides updated : Brazilian Portuguese, Czech & Italian.
    	
	--Audio--
	   	
	--System--
	- Added subtitle extraction using VobSub.
	    	
	--Video--
	
	--Compression--
	- DivX 5.1 single pass supported.
		
		
FIX*    
	- When a frame-range from 0 to xxx was choosen not needed to use besweet when audio is ac3.
	- Filenames/directories with a space no longer fail with vobsub spliting.

	
BUG*
	- Vobsub subtitle extraction doesn't work on windows 9x ?
	- 2-pass DivX 5.1 is not working. (m2a has some troubles starting the 2nd pass)
	
    	
TODO*
	--Layout--
	- English guide must be updated.
		
	--System--
	- Remove batch-entries when ready.
	- make it possible to split to more than 2 cd's.	
			
	--Audio--
	- Add support for VBR ogg.
	- Added support for ABR/VBR mp3.
	- Add support for Mpeg multi-channel. (Is this possible ?)
	- Add support for DTS audio. (Is this possible ?)
	
	--Video--
	- Add Psychovisual Enhancement (DIVX 5 pro)
	- More ntsc resolutions.
	
	--Compression--
	- Add end-credits recompression for xvid.
		
	--Dll--
	- Release genericmm.dll & vobinfo.dll to the public.
		

/*--------------------------------------------------------------------------------------------------*/



1.23 (22-06-2003)
NEW*
	--Software--
	- Virtualdub 1.5.4 included.
	
	--Layout--
	- Czech.lang has been updated.
	- Added 'reset to default settings' button.
	- Added Bulgarian language.
    	
	--Audio--
	- Audio does now match the video when a frame-range is choosen.
	   	
	--System--
	- I removed the AC3 filters and OGG Direct Show Filters from the package.
	    	
	--Video--
	- Added more advanced XviD options
	
	--Compression--
		
		
FIX*    
	- SimpleDivX crashes when using end-credits-recompression with divx 5.05.
	- When only 1 codec is installed, adv. settings dialog not correct.
	- small EndCredits problems fixed.
	- nfo file does now display correct Audio information.
	- Bitrate calculation was about 3% wrong. -> fixed
	- AC3 audio did not match video when frame-range was choosen. -> fixed
	#- Accessing audio-tab crashes when Audio-Language was longer than 16 characters. -> fixed.
				
BUG*
	-
	
    	
TODO*
	--Layout--
		
	--System--
	- Remove batch-entries when ready.
	- add subtitle extracting using vobsub ?
	- make it possible to split to more than 2 cd's.	
			
	--Audio--
	- Add support for VBR ogg.
	- Added support for ABR/VBR mp3.
	- Add support for Mpeg multi-channel.
	- Add support for DTS audio.
	
	--Video--
	- Add Psychovisual Enhancement (DIVX 5 pro)
	- More ntsc resolutions.
	
	--Compression--
	- Add end-credits recompression for xvid.
		
	--Dll--
	- Release genericmm.dll & vobinfo.dll to the public.
		

/*--------------------------------------------------------------------------------------------------*/

Report tips/bugs to CyberDemonII@hotmail.com

Copyright (c)  CyberDemonII
http://members.lycos.nl/simpledivx