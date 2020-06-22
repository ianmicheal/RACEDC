//---------------------------------------------------------------------------
//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version. See also the license.txt file for
//	additional informations.
//---------------------------------------------------------------------------

// sound.cpp: implementation of the sound class.
//
//////////////////////////////////////////////////////////////////////

//Flavor - Convert from DirectSound to SDL
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
#include "main.h"
#include "sound.h"
#include "memory.h"

#ifdef DRZ80
#include "DrZ80_support.h"
#endif
#ifndef __GP32__
#include "z80.h"
#include <math.h>
#endif

#define Machine (&m_emuInfo)


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int sndCycles = 0;

void soundStep(int cycles)
{
	sndCycles+= cycles;
}

/////////////////////////////////////////////////////////////////////////////////
///
/// Neogeo Pocket Sound system
///
/////////////////////////////////////////////////////////////////////////////////

unsigned int	ngpRunning;

void ngpSoundStart()
{
	ngpRunning = 1;	// ?
#ifdef DRZ80
    Z80_Reset();
#else
	z80Init();
	z80SetRunning(1);
#endif
}

/// Execute all gained cycles (divided by 2)
void ngpSoundExecute()
{
#ifdef DRZ80
    int toRun = sndCycles/2;
    if(ngpRunning)
    {
        Z80_Execute(toRun);
    }
    //timer_add_cycles(toRun);
    sndCycles -= toRun;
#else
	int		elapsed;
	while(sndCycles > 0)
	{
		elapsed = z80Step();
		sndCycles-= (2*elapsed);
		//timer_add_cycles(elapsed);
	}
#endif
}

/// Switch sound system off
void ngpSoundOff() {
	ngpRunning = 0;
#ifdef DRZ80

#else
	z80SetRunning(0);
#endif
}

// Generate interrupt to ngp sound system
void ngpSoundInterrupt() {
	if (ngpRunning)
	{
#ifdef DRZ80
        Z80_Cause_Interrupt(0x100);//Z80_IRQ_INT???
#else
		z80Interrupt(0);
#endif
	}
}

