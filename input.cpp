// input.cpp: implementation of the input class.
//
//////////////////////////////////////////////////////////////////////

//Flavor - Convert from DirectInput to SDL/GP2X
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
#include "input.h"
#include "memory.h"

#ifdef GIZMONDO
#include "gizmondo.h"
#include <kos.h>
void *subelf; 
int length;	
#endif 

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// address where the state of the input device(s) is stored
//unsigned char	*InputByte = get_address(0x00006F82);
unsigned char	ngpInputState = 0;
unsigned char	*InputByte = &ngpInputState;

#ifdef __GP32__
#define DIK_UP BUTTON_UP
#define DIK_DOWN BUTTON_DOWN
#define DIK_LEFT BUTTON_LEFT
#define DIK_RIGHT BUTTON_RIGHT
#define DIK_SPACE BUTTON_A
#define DIK_N BUTTON_B
#define DIK_S BUTTON_SELECT
#define DIK_O BUTTON_START
#else
#define DIK_UP SDLK_UP
#define DIK_DOWN SDLK_DOWN
#define DIK_LEFT SDLK_LEFT
#define DIK_RIGHT SDLK_RIGHT
#define DIK_SPACE SDLK_a
#define DIK_N SDLK_b
#define DIK_S SDLK_ESCAPE
#define DIK_O SDLK_SPACE
#endif

#define DOWN(x)	keystates[x]
#ifdef __GP32__
u16 keystates = 0;
#else
Uint8 *keystates = NULL;
#endif

#ifdef __GP32__
void InitInput()
#else
BOOL InitInput(HWND hwnd)
#endif
{
#if !defined (__GP32__) && !defined (GIZMONDO)
    keystates = GP2X_GetKeyStateArray(NULL);
#endif

#ifdef GIZMONDO
    keystates = SDL_GetKeyState(NULL);
#endif

	// setup standard values for input
	// NGP/NGPC:
	m_sysInfo[NGP].InputKeys[KEY_UP]			= DIK_UP;
	m_sysInfo[NGP].InputKeys[KEY_DOWN]		= DIK_DOWN;
	m_sysInfo[NGP].InputKeys[KEY_LEFT]		= DIK_LEFT;
	m_sysInfo[NGP].InputKeys[KEY_RIGHT]		= DIK_RIGHT;
	m_sysInfo[NGP].InputKeys[KEY_BUTTON_A]	= DIK_SPACE;
	m_sysInfo[NGP].InputKeys[KEY_BUTTON_B]	= DIK_N;
	m_sysInfo[NGP].InputKeys[KEY_SELECT]		= DIK_O;	// Option button
	m_sysInfo[NGPC].InputKeys[KEY_UP]			= DIK_UP;
	m_sysInfo[NGPC].InputKeys[KEY_DOWN]		= DIK_DOWN;
	m_sysInfo[NGPC].InputKeys[KEY_LEFT]		= DIK_LEFT;
	m_sysInfo[NGPC].InputKeys[KEY_RIGHT]		= DIK_RIGHT;
	m_sysInfo[NGPC].InputKeys[KEY_BUTTON_A]	= DIK_SPACE;
	m_sysInfo[NGPC].InputKeys[KEY_BUTTON_B]	= DIK_N;
	m_sysInfo[NGPC].InputKeys[KEY_SELECT]		= DIK_O;	// Option button

#ifndef __GP32__
    return TRUE;
#endif

}

#ifdef __GP32__
extern "C"
{
	int gp_getButton();
	void clearScreen();
}
int zoom=0,zoomy=16;
#endif

void UpdateInputState()
{
#ifdef __GP32__
	int key = gp_getButton();

	if ((key & BUTTON_R) && (key & BUTTON_L))
    {
        m_bIsActive = FALSE;//Flavor exit emulation
        return;
    }

	if (key & BUTTON_R) {
		zoom ^= 1;
		while ((key=gp_getButton())&BUTTON_R);
		if (!zoom)
			clearScreen();
	}

	if (key & BUTTON_L) {
		if (key & BUTTON_DOWN) {
			if (zoomy<32)
				zoomy+=1;
			return;
		}
		if (key & BUTTON_UP) {
			if (zoomy>0)
				zoomy-=1;
			return;
		}
	}
	if(key & BUTTON_DOWN)
	    *InputByte = 0x02;
	else if(key & BUTTON_UP)
	    *InputByte = 0x01;
	else
	    *InputByte = 0;

	if(key & BUTTON_RIGHT)
	    *InputByte |= 0x08;
	else if(key & BUTTON_LEFT)
	    *InputByte |= 0x04;

    if (key & BUTTON_A)
        *InputByte|= 0x10;
    if (key & BUTTON_B)
        *InputByte|= 0x20;
    if (key & BUTTON_SELECT)
        *InputByte|= 0x40;
#else
    SDL_Event event;

#ifdef TARGET_GP2X
    while(SDL_PollEvent(&event))
    {
    }

    if (SDL_JoystickGetButton(joystick, GP2X_BUTTON_R) && SDL_JoystickGetButton(joystick, GP2X_BUTTON_L))
    {
        m_bIsActive = FALSE;//Flavor exit emulation
        return;
    }


	if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_DOWN))
	{
	    *InputByte = 0x02;
	}
	else if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_UP))
	{
	    *InputByte = 0x01;
	}
	else if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_RIGHT))
	{
	    *InputByte = 0x08;
	}
	else if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_LEFT))
	{
	    *InputByte = 0x04;
	}
	else if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_UPRIGHT))
	{
	    *InputByte = 0x09;
	}
	else if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_UPLEFT))
	{
	    *InputByte = 0x05;
	}
	else if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_DOWNRIGHT))
	{
	    *InputByte = 0x0A;
    }
	else if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_DOWNLEFT))
	{
	    *InputByte = 0x06;
	}
	else
        *InputByte = 0;

    if (SDL_JoystickGetButton(joystick, GP2X_BUTTON_A))
        *InputByte|= 0x10;
    if (SDL_JoystickGetButton(joystick, GP2X_BUTTON_B))
        *InputByte|= 0x20;
    if (SDL_JoystickGetButton(joystick, GP2X_BUTTON_SELECT))
        *InputByte|= 0x40;

    if (SDL_JoystickGetButton(joystick, GP2X_BUTTON_VOLUP))
        increaseVolume();
    else if (SDL_JoystickGetButton(joystick, GP2X_BUTTON_VOLDOWN))
        decreaseVolume();
#else
#ifdef GIZMONDO
/*
	while(SDL_PollEvent(&event))
	{
	}

	if (DOWN(SDLK_F1))
        m_bIsActive = FALSE;//Flavor exit emulation

	int hat= (SDL_JoystickGetHat(joystick,0));
	if (hat & SDL_HAT_LEFT)
	{
		*InputByte = 0x04;
	}
	else if (hat & SDL_HAT_RIGHT)
	{
		*InputByte = 0x08;
	}
	else if (hat & SDL_HAT_UP)
	{
		*InputByte = 0x01;
	}
	else if (hat & SDL_HAT_DOWN)
	{
		*InputByte = 0x02;
	}
	else
	{
		 *InputByte = 0;
	}

    if (SDL_JoystickGetButton(joystick, GIZ_PLAY))
        *InputByte|= 0x10;
    if (SDL_JoystickGetButton(joystick, GIZ_FORWARDS))
        *InputByte|= 0x20;
    if (SDL_JoystickGetButton(joystick, GIZ_STOP))
        *InputByte|= 0x40;
*/	 *InputByte = 0;
			cont_cond_t cond;
			uint8	c;
			c = maple_first_controller();
	if (c) 
	{
		if (cont_get_cond(c, &cond) < 0)
		{
			printf("Error reading controller\n");
		}
			if(!(cond.buttons & CONT_A))
        {
			*InputByte|= 0x10;
        }

  		if(!(cond.buttons & CONT_Y))
        {
              ngpSoundOff(); 
              flashShutdown();           
length = fs_load("/cd/menu.bin", &subelf); 
arch_exec(subelf, length);


}


	if (cond.ltrig > 0) {
       decreaseVolume(); 
       flashShutdown();
                   }
       // increaseVolume();
      // SetActive(FALSE);
      
      if (cond.rtrig > 0) {
                     increaseVolume();
                     }
         if(!(cond.buttons & CONT_A))
        {
		 increaseVolume();
        }   
        if(!(cond.buttons & CONT_B))
        {
			*InputByte|= 0x20;
        }             
  		if (!(cond.buttons & CONT_START)) 
		{
    		*InputByte|= 0x40;
    	}
		if (!(cond.buttons &CONT_DPAD_UP)) 
		{
			*InputByte |= 0x01;
		}
		if (!(cond.buttons &CONT_DPAD_DOWN))
		{
			*InputByte |= 0x02;
		}
		if (!(cond.buttons &CONT_DPAD_LEFT)) 
		{
			*InputByte |= 0x04;
		}			   	
		if (!(cond.buttons &CONT_DPAD_RIGHT)) 
		{
			*InputByte |= 0x08;
		}

	}

#else
    SYSTEMINFO *si;
    GP2X_SetKeyStates(); //make sure they're updated

    while(SDL_PollEvent(&event))
    {
    }

    if (DOWN(SDLK_r) && DOWN(SDLK_l))
    void bkg_setup();
void bkg_render();
       SetActive(FALSE);
       // m_bIsActive = FALSE;//Flavor exit emulation


    si = &m_sysInfo[NGP];
    *InputByte = 0;
    if (DOWN(si->InputKeys[KEY_BUTTON_A]))
        *InputByte|= 0x10;
    if (DOWN(si->InputKeys[KEY_BUTTON_B]))
        *InputByte|= 0x20;
    if (DOWN(si->InputKeys[KEY_SELECT]))
        *InputByte|= 0x40;
    if (DOWN(si->InputKeys[KEY_UP]))
        *InputByte|= 0x01;
    if (DOWN(si->InputKeys[KEY_DOWN]))
        *InputByte|= 0x02;
    if (DOWN(si->InputKeys[KEY_LEFT]))
        *InputByte|= 0x04;
    if (DOWN(si->InputKeys[KEY_RIGHT]))
        *InputByte|= 0x08;

    if (DOWN(SDLK_KP_PLUS))
        increaseVolume();
    else if (DOWN(SDLK_KP_MINUS))
        decreaseVolume();
#endif
#endif
#endif
}

void FreeInput()
{

}
