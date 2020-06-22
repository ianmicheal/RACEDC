//---------------------------------------------------------------------------
//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version. See also the license.txt file for
//	additional informations.
//---------------------------------------------------------------------------

//
// This is the main program entry point
//
/*
######################################################################
#         RACEDC BETA 1 for DreamCast  Beta 1.0                      #
######################################################################
# including :	Neogeo pocket color                                  #
#  Ported by Troy davis GPF  Update Ian micheal                      #
#											                         #
#		                                                             #
#Rather                                                              #
#A                                                                   #
#Cardfighter                                                         #
#@Emulator!                                                          #
#(As In Koyote-Land) for the Sega Dreamcast.			             #
#                                                                    #
######################################################################
#																	 #
#								            (c)2020 from Ian Micheal #
######################################################################
*/

#ifndef __GP32__
#include "stdafx.h"
#endif
  #include <kos.h>
  #include <arch/gdb.h>

#include "gp.h"

#include <SDL.h>
#include <SDL_dreamcast.h>

//extern uint8 romdisk[];
//KOS_INIT_ROMDISK(romdisk);

#include "main.h"


//#include "msounds.h"
#include "memory.h"
//#include "mainemu.h"
#include "tlcs900h.h"
#include "input.h"

#include "graphics.h"
#include "tlcs900h.h"

#include "neopopsound.h"

#ifdef DRZ80
#include "DrZ80_support.h"
#else
#include "z80.h"
#endif

#ifdef TARGET_GP2X
//#define TEST_CFC2_ONLY
#endif

#ifdef TEST_CFC2_ONLY
extern unsigned char _binary_cfc2_ngp_start[];
extern unsigned char _binary_cfc2_ngp_end[];
extern unsigned int _binary_cfc2_ngp_size;
#endif

#ifdef __GP32__

#include "fileio.h"
#include "gfx.h"

#else
#ifdef GIZMONDO
#include "gizmondo.h"
#include <oggvorbis/sndoggvorbis.h>
SDL_Surface* screen = NULL;
SDL_Joystick* joystick = NULL;

#else
SDL_Surface* screen = NULL;						// Main program screen

#endif
#endif

BOOL		m_bIsActive;
EMUINFO		m_emuInfo;
SYSTEMINFO	m_sysInfo[NR_OF_SYSTEMS];

FILE	*errorLog = NULL;
FILE	*outputRam = NULL;

void mainemuinit(char *lpszPathName)
{
	// initialize cpu memory
	mem_init();
#ifndef __GP32__
	graphics_init(NULL);
#else
	graphics_init();
#endif

    // initialize the TLCS-900H cpu
    tlcs_init();

#ifdef DRZ80
    Z80_Reset();
#else
    z80Init();
#endif

    // if neogeo pocket color rom, act if we are a neogeo pocket color
    tlcsMemWriteB(0x6F91,tlcsMemReadB(0x00200023));
    // pretend we're running in English mode
    tlcsMemWriteB(0x00006F87,0x01);
    // kludges & fixes
    switch (tlcsMemReadW(0x00200020))
    {
        case 0x0059:	// Sonic
        case 0x0061:	// Metal SLug 2nd
            *get_address(0x0020001F) = 0xFF;
            break;
    }
    ngpSoundOff();
    //Flavor sound_start();
}

#ifndef __GP32__

void AfxMessageBox(char *a, int b, int c)
{
	dbg_print(a);
}
void AfxMessageBox(char *a, int b)
{
	dbg_print(a);
}
void AfxMessageBox(char *a)
{
	dbg_print(a);
}

#endif

void	SetActive(BOOL bActive)
{
	m_bIsActive = bActive;
}

void	SetEmu(int machine)
{
	m_emuInfo.machine = machine;
	m_emuInfo.drv = &m_sysInfo[machine];
}

bool initRom(char *lpszPathName)
{
	char	*licenseInfo = " BY SNK CORPORATION";
	char	*ggLicenseInfo = "TMR SEGA";
	BOOL	romFound = TRUE;
	int		i, m;

	//dbg_print("in openNgp(%s)\n", lpszPathName);

	// first stop the current emulation
	//dbg_print("openNgp: SetEmu(NONE)\n");
	SetEmu(NONE);
	//dbg_print("openNgp: SetActive(FALSE)\n");
	SetActive(FALSE);

	// check NEOGEO POCKET
	// check license info
	for (i=0;i<19;i++)
	{
		if (mainrom[0x000009 + i] != licenseInfo[i])
			romFound = FALSE;
	}
	if (romFound)
	{
		//dbg_print("openNgp: romFound == TRUE\n");
		i = mainrom[0x000023];
		if (i == 0x10 || i == 0x00)
		{
			// initiazlie emulation
			if (i == 0x10) {
				m = NGPC;
			} else {
				// fix for missing Mono/Color setting in Cool Coom Jam SAMPLE rom
				if (mainrom[0x000020] == 0x34 && mainrom[0x000021] == 0x12)
					m = NGPC;
				else m = NGP;
			}

			//dbg_print("openNgp: SetEmu(%d)\n", m);
			SetEmu(m);

			//dbg_print("openNgp: Calling mainemuinit(%s)\n", lpszPathName);
			mainemuinit(lpszPathName);
			// start running the emulation loop
			//dbg_print("openNgp: SetActive(TRUE)\n");
			SetActive(TRUE);

			// acknowledge opening of the document went fine
			//dbg_print("openNgp: returning success\n");
			return TRUE;
		}

		fprintf(stderr, "Not a valid or unsupported rom file.\n");
		return FALSE;
	}

/*	// check for Wonderswan
	// at the moment only checks of extension of the rom file
	// VERY VERY VERY VERY EVILISHY BAD!!!!
	i = strlen(m_emuInfo.RomFileName) - 1;
	if (((m_emuInfo.RomFileName[i] | 0x20)   == 'c') &&
		((m_emuInfo.RomFileName[i-1] | 0x20) == 's') &&
		((m_emuInfo.RomFileName[i-2] | 0x20) == 'w')) {
		SetEmu( WONDERSWANCOLOR );
		mainemuinit(lpszPathName);
		SetActive(TRUE);
		return TRUE;
	}
	if (((m_emuInfo.RomFileName[i] | 0x20)   == 's') &&
		((m_emuInfo.RomFileName[i-1] | 0x20) == 'w')) {
		SetEmu( WONDERSWAN );
		mainemuinit(lpszPathName);
		SetActive(TRUE);
		return TRUE;
	}*/

	fprintf(stderr, "Not a valid or unsupported rom file. romFound==FALSE\n");
	return FALSE;
}

void initSysInfo()
{
	m_bIsActive = FALSE;

	m_emuInfo.machine = NONE;
	m_emuInfo.drv = &m_sysInfo[m_emuInfo.machine];
	m_emuInfo.romSize = 0;

	strcpy(m_emuInfo.ProgramFolder, "");
	strcpy(m_emuInfo.SavePath, "");
	strcpy(m_emuInfo.DebugPath, "");
	strcpy(m_emuInfo.RomPath, "");
	strcpy(m_emuInfo.RomFileName, "");
	strcpy(m_emuInfo.OpenFileName, "");
	strcpy(m_emuInfo.SaveFileName, "");
	strcpy(m_emuInfo.OpenFileName, "");
	strcpy(m_emuInfo.DebugPath, "");;
	strcpy(m_emuInfo.RomFileName, "");
	strcpy(m_emuInfo.SaveFileName, "");
	strcpy(m_emuInfo.ScreenPath, "");

	m_emuInfo.machine = NONE;

#ifdef NO_SOUND_OUTPUT
	m_emuInfo.sample_rate = 0;
#else
	m_emuInfo.sample_rate = 22050;
#endif
	m_emuInfo.stereo = 1;
//	m_emuInfo.fps = 60;//30;//100;  //Flavor, tweak this!

	m_sysInfo[NGP].hSize = 160;
	m_sysInfo[NGP].vSize = 152;
	m_sysInfo[NGP].Ticks = 6*1024*1024;

	m_sysInfo[NGPC].hSize = 160;
	m_sysInfo[NGPC].vSize = 152;
	m_sysInfo[NGPC].Ticks = 6*1024*1024;
}

int handleInputFile(char *romName)
{
	//printf("romname = %s\n",romName);
#ifndef TEST_CFC2_ONLY
	FILE *romFile;
	//printf("romname2 = %s\n",romName);
	romFile = fopen(romName, "rb");
	//printf("romname3 = %s\n",romName);
	if(!romFile)
	{
#ifdef __GP32__
		error("Couldn't open file");
#else
		printf("Couldn't open %s file\n", romName);
#endif
		return 0;
	}

	initSysInfo();  //initialize it all

#ifdef __GP32__
	m_emuInfo.romSize = fread(mainrom, 1, 16*1024*1024, romFile);
#else
	m_emuInfo.romSize = fread(mainrom, 1, 8*1024*1024, romFile);
#endif
	fclose(romFile);
    strcpy(m_emuInfo.RomFileName, romName);
	printf("filesize = %d\n",m_emuInfo.romSize);
#else
	initSysInfo();  //initialize it all
    m_emuInfo.romSize = _binary_cfc2_ngp_end - _binary_cfc2_ngp_start;//_binary_cfc2_ngp_size;
    memcpy(mainrom, _binary_cfc2_ngp_start, m_emuInfo.romSize);
    strcpy(m_emuInfo.RomFileName, "cfc2.ngp");
    strcpy(romName, "cfc2.ngp");
#endif

	if(!initRom(romName))
	{
#ifdef __GP32__
		error("Couldn't open file");
#else
		printf("initRom couldn't handle %s file\n", romName);
#endif
		return 0;
	}

	setFlashSize(m_emuInfo.romSize);
}

#ifdef __GP32__

extern void gp_startSound();
extern void gp_stopSound();
extern int autosave;

extern "C"
{
	int gp_getButton();
	int gp_startRing(int bit);
	void GpPcmPlay(unsigned short* src, int size, int rep, int bit);
	void GpPcmStop();
}

int mhemain(char* romName)
{
	clearScreen();
	flashStartup();
	//console_write(romName,COLOR_GREEN);
	 bfont_draw_str(vram_s+(10)*640+(10), 640,480, romName); 
	 bfont_draw_str(vram_s+(10)*640+(10), 640,480, "Loading..."); 
//	console_write("Loading...",COLOR_GREEN);

    handleInputFile(romName);

	InitInput();

	clearScreen();

    sound_system_init();

	gp_startSound();

    ngpc_run();

	gp_stopSound();

	if (autosave)
		flashShutdown();

	return 0;
}

#else
extern SDL_Joystick *joystick;

Uint8 *audio_buf;
Uint32 audio_len;
int played = 0;
extern int volume;

void introAudioCallback(void *userdata, Uint8 *stream, int len)
{
    if (played>=audio_len)
        return;
    SDL_MixAudio(stream, &audio_buf[played], played+len>audio_len?audio_len-played:len, volume);
    played+=len;
}

int main(int argc, char **argv)
{
	//gdb_init();
#ifndef _arch_dreamcast
	char romName[150];
#endif

#ifdef _arch_dreamcast
	char* romName;
	pvr_init_defaults();
	setup_util_texture();
	bkg_setup();
	romName=menu_render();
	pvr_shutdown();
//	pvr_mem_free(back_tex);
    bfont_draw_str(vram_s+(10)*640+(10), 640,480, romName); 
    bfont_draw_str(vram_s+(10)*640+(10), 640,480, "Loading..."); 
    	sndoggvorbis_stop();
	sndoggvorbis_shutdown();
	snd_stream_shutdown();
	
	//char romName[150];
	//	strcpy(romName, "/cd/");
	//	strcat(romName, "bustmovp.ngp");
#else
	
#ifdef GIZMONDO
	
		strcpy(romName, "\\SD Card\\GizRACE\\");
		strcat(romName, argv[1]);
#else
#ifndef TEST_CFC2_ONLY
	if(argc == 1)
	{
		dbg_printf("Defaulting to runme.ngp\n");
		strncpy(romName, "runme.ngp", 50);
		//dbg_printf("Defaulting to runme.ws\n");
		//strncpy(romName, "runme.ws", 50);
	}
	else if(argc != 2)
	{
		fprintf(stderr, "argc != 2\n");
		return -1;
	}
	else
		strncpy(romName, argv[1], 50);

	dbg_printf("romName=%s\n", romName);
#endif
#endif
#endif
	// Initialize SDL
#if defined (TARGET_GP2X) || defined (GIZMONDO)
puts("\n\n * SDL-DC : Video drivers test.");
    SDL_DC_ShowAskHz(SDL_FALSE);
    SDL_DC_Default60Hz(SDL_FALSE);
    SDL_DC_VerticalWait(SDL_FALSE);
    //SDL_DC_SetVideoDriver(SDL_DC_DIRECT_VIDEO);
    //SDL_DC_SetVideoDriver(SDL_DC_TEXTURED_VIDEO);
   SDL_DC_SetVideoDriver(SDL_DC_DMA_VIDEO);


    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK) < 0)
#else
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER) < 0)
#endif
	{
		fprintf(stderr, "SDL_Init failed!\n");
		return -1;//Log::Instance()->die(1, SV_FATALERROR, "Couldn't initialize SDL: %s\n", SDL_GetError());
	}

#ifdef TARGET_GP2X
	GP2X_JoystickOpen();
#else
#ifdef GIZMONDO
	joystick = SDL_JoystickOpen(0);
#else
    SDL_WM_SetCaption("RACE!", 0);
#endif
#endif

	// Set up quiting so that it automatically runs on exit.
//	atexit (SDL_Quit);


//#define DO_HWDBLBUF
#ifdef DO_HWDBLBUF
	screen = SDL_SetVideoMode (SIZEX, SIZEY, 16, SDL_SWSURFACE);
#else
	//screen = SDL_SetVideoMode (SIZEX, SIZEY, 8, SDL_SWSURFACE | SDL_HWPALETTE);
	//screen = SDL_SetVideoMode (SIZEX, SIZEY, 16, SDL_SWSURFACE);
#ifdef GIZMONDO
//screen = SDL_SetVideoMode (SIZEX, SIZEY, 16, SDL_FULLSCREEN| SDL_SWSURFACE);
SDL_ShowCursor(0); 
	Uint32 video_flags = SDL_DOUBLEBUF|SDL_HWSURFACE;
	Uint32 video_bits = 0;


	video_flags |= SDL_FULLSCREEN;
	video_bits=16;

//Ian micheal this might help

//screen=SDL_SetVideoMode(TXR_X,TXR_Y,16,SDL_HWSURFACE|SDL_DOUBLEBUF);
	screen = SDL_SetVideoMode(SIZEX, SIZEY, video_bits, video_flags);
	if ( screen == NULL ) {
		fprintf(stderr, "Couldn't set %dx%dx%d video mode: %s\n",
					SIZEX, SIZEY, video_bits, SDL_GetError());
		exit(1);
	}
//	screen = SDL_SetVideoMode (320, 240, 16, SDL_SWSURFACE);
#endif
#endif
	if (screen == NULL)
	{
		fprintf(stderr, "SDL_SetVideoMode failed!\n");
		return -1;
	}
	else
	{
		dbg_print("screen params: bpp=%d\n", screen->format->BitsPerPixel);
	}

	SDL_ShowCursor(0);  //disable the cursor

    SDL_AudioSpec wav,retFmt;

//#ifndef GIZMONDO
    SDL_Surface *splash = IMG_Load("/cd/image/splash.bmp");
    if(splash)
    {
        SDL_BlitSurface(splash, NULL, screen, NULL);
       SDL_Flip(screen);
    }

    SDL_LoadWAV("/cd/image/intro.wav", &wav, &audio_buf, &audio_len);
    wav.callback = introAudioCallback;
    wav.userdata = NULL;
    if ( SDL_OpenAudio(&wav, &retFmt) < 0 ) {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_PauseAudio(0);

	InitInput(NULL);

    while (played<audio_len)
    {
        UpdateInputState();
        if (ngpInputState & 0x70)
            break;
    }
    while (ngpInputState & 0x70)
    {
        UpdateInputState();
    }

    SDL_PauseAudio(0);

    SDL_CloseAudio();
    SDL_FreeSurface(splash);
    SDL_FreeWAV(audio_buf);

//#else
    InitInput(NULL);
//#/endif
	
	printf("openNgp(%s)\n", romName);

	//emulate
	//see ngpc.cpp and ngpcDoc.cpp

    //system_sound_init();
    sound_system_init();


    handleInputFile(romName);

	printf("Running NGPC Emulation\n");

    ngpc_run();

	flashShutdown();

	return 0;
}
#endif

