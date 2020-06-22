#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <ctype.h>
#include <math.h>

#if !defined (__GP32__) && !defined (GIZMONDO)
#include "GP2X.h"
extern SDL_Surface* screen;
#endif

#ifdef GIZMONDO
#include "memfuncs.h"
#include "gizmondo.h"
extern SDL_Surface* screen;
#endif

#ifdef __GP32__

#define memcpy gp2x_memcpy

extern "C" {
void gp2x_memcpy(void*,void*,unsigned int);
}

#define GENREGSPC_AS_REG  //comment out to do it as a regular mem loc
#define GENREGSSR_AS_REG  //comment out to do it as a regular mem loc

register unsigned char *my_pc asm ("r9");
register unsigned long opcode asm ("r6");
#ifdef GENREGSPC_AS_REG
register unsigned long gen_regsPC asm("r8");
#endif
#ifdef GENREGSSR_AS_REG
register unsigned long gen_regsSR asm("r7");
#endif

#include "gp32.h"

#define BOOL int
#define _MAX_PATH 128
#define _u8 unsigned char
#define Uint8 unsigned char
#define _u16 unsigned short
#define _u32 unsigned long
#define DWORD unsigned long
#define TRUE 1
#define FALSE 0
#define dbg_print

#endif


struct SYSTEMINFO {
	int		hSize;
	int		vSize;
	int		Ticks;
	int		InputKeys[12];
	//MachineSound	sound[4];
	// Dynamic System Info
	BOOL	Back0;
	BOOL	Back1;
	BOOL	Sprites;
};

struct EMUINFO {
	char 	ProgramFolder[_MAX_PATH];	// place holders for filenames
	char 	SavePath[_MAX_PATH];
	char 	DebugPath[_MAX_PATH];
	char 	RomPath[_MAX_PATH];
	char  ScreenPath[_MAX_PATH];

	char  OpenFileName[_MAX_PATH];	// place holders for filenames
	char 	RomFileName[_MAX_PATH];
	char 	SaveFileName[_MAX_PATH];

	int		machine;		// what kind of machine should we emulate
	int		romSize;		// what is the size of the currently loaded file
	int		sample_rate;	// what is the current sample rate
	int		stereo;			// play in stereo?
	//unsigned int		fps;
	int		samples;
	SYSTEMINFO	*drv;
};

#define KEY_UP			0
#define KEY_DOWN		1
#define KEY_LEFT		2
#define KEY_RIGHT		3
#define KEY_START		4
#define KEY_BUTTON_A	5
#define KEY_BUTTON_B	6
#define KEY_SELECT		7
#define KEY_UP_2		8
#define KEY_DOWN_2		9
#define KEY_LEFT_2		10
#define KEY_RIGHT_2		11

#define NONE			0x00
// Possible Neogeo Pocket versions
#define NGP				0x01
#define NGPC			0x02
// Possible Gameboy versions
#define GAMEBOY			0x03
#define	GAMEBOYPOCKET	0x04
#define	SUPERGAMEBOY	0x05
#define	GAMEBOYCOLOR	0x06
// Possible Gamegear versions
#define GAMEGEAR		0x07
// Possible Lynx versions
#define LYNX			0x08
// Possible Wonderswan versions
#define WONDERSWAN		0x09
#define WONDERSWANCOLOR	0x0A
// Possible TurboXpress versions
#define TURBOXPRESS		0x0B
// Possible Adventure Vision versions
#define ADVISION		0x0C
// Possible Supervision versions
#define SUPERVISION		0x0D
// Easter Egg System
#define NES             0x0E
// Game.com
#define GAMECOM			0x0F

#define NR_OF_SYSTEMS	16


extern BOOL		m_bIsActive;
extern EMUINFO		m_emuInfo;
extern SYSTEMINFO	m_sysInfo[NR_OF_SYSTEMS];
extern int romSize;

#ifdef __GP32__
#define HOST_FPS 60  //100 was what it was, originally
#else
#ifdef TARGET_GP2X   //to call these FPS is a bit of a misnomer
#define HOST_FPS 20  //the number of frames we want to draw to the host's screen every second
#else
#define HOST_FPS 60  //100 was what it was, originally
#endif
#endif

#endif
