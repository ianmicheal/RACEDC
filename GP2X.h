//---------------------------------------------------------------------------
//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version. See also the license.txt file for
//	additional informations.
//---------------------------------------------------------------------------


#ifndef __GP2X_H_
#define __GP2X_H_

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_image.h>

#ifdef TARGET_GP2X
#define GP2X_BUTTON_UP              (0)
#define GP2X_BUTTON_DOWN            (4)
#define GP2X_BUTTON_LEFT            (2)
#define GP2X_BUTTON_RIGHT           (6)
#define GP2X_BUTTON_UPLEFT          (1)
#define GP2X_BUTTON_UPRIGHT         (7)
#define GP2X_BUTTON_DOWNLEFT        (3)
#define GP2X_BUTTON_DOWNRIGHT       (5)
#define GP2X_BUTTON_CLICK           (18)
#define GP2X_BUTTON_A               (12)
#define GP2X_BUTTON_B               (13)
#define GP2X_BUTTON_X               (15)
#define GP2X_BUTTON_Y               (14)
#define GP2X_BUTTON_L               (10)
#define GP2X_BUTTON_R               (11)
#define GP2X_BUTTON_START           (8)
#define GP2X_BUTTON_SELECT          (9)
#define GP2X_BUTTON_VOLUP           (16)
#define GP2X_BUTTON_VOLDOWN         (17)

extern SDL_Joystick *joystick;

#else
//#   define GP2X_BUTTON_START           (SDLK_RETURN)
//#   define GP2X_BUTTON_SELECT          (SDLK_RSHIFT)
#   define GP2X_BUTTON_UP              (SDLK_UP)
#   define GP2X_BUTTON_DOWN            (SDLK_DOWN)
#   define GP2X_BUTTON_LEFT            (SDLK_LEFT)
#   define GP2X_BUTTON_RIGHT           (SDLK_RIGHT)
#   define GP2X_BUTTON_UPLEFT          (SDLK_KP7)
#   define GP2X_BUTTON_UPRIGHT         (SDLK_KP9)
#   define GP2X_BUTTON_DOWNLEFT        (SDLK_KP1)
#   define GP2X_BUTTON_DOWNRIGHT       (SDLK_KP3)
//#   define GP2X_BUTTON_A               (SDLK_j)
//#   define GP2X_BUTTON_B               (SDLK_k)
//#   define GP2X_BUTTON_X               (SDLK_m)
//#   define GP2X_BUTTON_Y               (SDLK_i)
//#   define GP2X_BUTTON_L               (SDLK_q)
//#   define GP2X_BUTTON_R               (SDLK_w)
//#   define GP2X_BUTTON_VOLUP           (SDLK_KP8)
//#   define GP2X_BUTTON_VOLDOWN         (SDLK_KP2)
#define GP2X_BUTTON_CLICK SDLK_RETURN
#define GP2X_BUTTON_START SDLK_ESCAPE
#define GP2X_BUTTON_SELECT SDLK_SPACE
#define GP2X_BUTTON_A SDLK_a
#define GP2X_BUTTON_B SDLK_b
#define GP2X_BUTTON_X SDLK_x
#define GP2X_BUTTON_Y SDLK_y
#define GP2X_BUTTON_L SDLK_l
#define GP2X_BUTTON_R SDLK_r
#define GP2X_BUTTON_VOLUP SDLK_PLUS
#define GP2X_BUTTON_VOLDOWN SDLK_MINUS

#endif


SDLKey GP2X_to_SDLK(int gp2xButton);
Uint8 *GP2X_GetKeyStateArray(int *numkeys);
void GP2X_SetKeyStates();
void GP2X_MouseOn();
void GP2X_MouseOff();
void GP2X_JoystickOpen();
unsigned char GP2X_GetMouseState(int *mx, int *my);
SDL_Rect drawNumber(unsigned int number, unsigned int x, unsigned int y);

#endif
