//---------------------------------------------------------------------------
//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version. See also the license.txt file for
//	additional informations.
//---------------------------------------------------------------------------

// graphics.cpp: implementation of the graphics class.
//
//////////////////////////////////////////////////////////////////////

//Flavor - Convert from DirectDraw to SDL

//Flavor #define WIN32_LEAN_AND_MEAN
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
#ifdef __GP32__

#include "main.h"
#include "graphics.h"
#include "memory.h"
#include "gp32.h"
#include "gfx.h"
extern int getTickCount();
extern int zoom,zoomy;
extern int turbo;

#else

#include "stdafx.h"
#include "main.h"
#include "graphics.h"
#include "memory.h"

#ifdef GIZMONDO
#include "gizmondo.h"
#else
#include "GP2X.h"
#endif
#endif

#define INITGUID


//SDL_Color SDLpalette[256];


//
// 16 bit graphics buffers
// At the moment there's no system which uses more than 16 bit
// colors.
// A grpaphics buffer holds number referencing to the color from
// the "total palette" for that system.
//
int    totalpalette[32*32*32];

#ifdef __GP32__

#define OFFSETX 80
#define OFFSETY 44
unsigned short drawBuffer[NGPC_SIZEX*NGPC_SIZEY];

#else

unsigned short *drawBuffer;

#endif

int    displayDirty;

// NGP specific
// precalculated pattern structures
unsigned char mypatterns[256*4] =
    {
        0,0,0,0, 0,0,0,1, 0,0,0,2, 0,0,0,3, 0,0,1,0, 0,0,1,1, 0,0,1,2, 0,0,1,3,
        0,0,2,0, 0,0,2,1, 0,0,2,2, 0,0,2,3, 0,0,3,0, 0,0,3,1, 0,0,3,2, 0,0,3,3,
        0,1,0,0, 0,1,0,1, 0,1,0,2, 0,1,0,3, 0,1,1,0, 0,1,1,1, 0,1,1,2, 0,1,1,3,
        0,1,2,0, 0,1,2,1, 0,1,2,2, 0,1,2,3, 0,1,3,0, 0,1,3,1, 0,1,3,2, 0,1,3,3,
        0,2,0,0, 0,2,0,1, 0,2,0,2, 0,2,0,3, 0,2,1,0, 0,2,1,1, 0,2,1,2, 0,2,1,3,
        0,2,2,0, 0,2,2,1, 0,2,2,2, 0,2,2,3, 0,2,3,0, 0,2,3,1, 0,2,3,2, 0,2,3,3,
        0,3,0,0, 0,3,0,1, 0,3,0,2, 0,3,0,3, 0,3,1,0, 0,3,1,1, 0,3,1,2, 0,3,1,3,
        0,3,2,0, 0,3,2,1, 0,3,2,2, 0,3,2,3, 0,3,3,0, 0,3,3,1, 0,3,3,2, 0,3,3,3,
        1,0,0,0, 1,0,0,1, 1,0,0,2, 1,0,0,3, 1,0,1,0, 1,0,1,1, 1,0,1,2, 1,0,1,3,
        1,0,2,0, 1,0,2,1, 1,0,2,2, 1,0,2,3, 1,0,3,0, 1,0,3,1, 1,0,3,2, 1,0,3,3,
        1,1,0,0, 1,1,0,1, 1,1,0,2, 1,1,0,3, 1,1,1,0, 1,1,1,1, 1,1,1,2, 1,1,1,3,
        1,1,2,0, 1,1,2,1, 1,1,2,2, 1,1,2,3, 1,1,3,0, 1,1,3,1, 1,1,3,2, 1,1,3,3,
        1,2,0,0, 1,2,0,1, 1,2,0,2, 1,2,0,3, 1,2,1,0, 1,2,1,1, 1,2,1,2, 1,2,1,3,
        1,2,2,0, 1,2,2,1, 1,2,2,2, 1,2,2,3, 1,2,3,0, 1,2,3,1, 1,2,3,2, 1,2,3,3,
        1,3,0,0, 1,3,0,1, 1,3,0,2, 1,3,0,3, 1,3,1,0, 1,3,1,1, 1,3,1,2, 1,3,1,3,
        1,3,2,0, 1,3,2,1, 1,3,2,2, 1,3,2,3, 1,3,3,0, 1,3,3,1, 1,3,3,2, 1,3,3,3,
        2,0,0,0, 2,0,0,1, 2,0,0,2, 2,0,0,3, 2,0,1,0, 2,0,1,1, 2,0,1,2, 2,0,1,3,
        2,0,2,0, 2,0,2,1, 2,0,2,2, 2,0,2,3, 2,0,3,0, 2,0,3,1, 2,0,3,2, 2,0,3,3,
        2,1,0,0, 2,1,0,1, 2,1,0,2, 2,1,0,3, 2,1,1,0, 2,1,1,1, 2,1,1,2, 2,1,1,3,
        2,1,2,0, 2,1,2,1, 2,1,2,2, 2,1,2,3, 2,1,3,0, 2,1,3,1, 2,1,3,2, 2,1,3,3,
        2,2,0,0, 2,2,0,1, 2,2,0,2, 2,2,0,3, 2,2,1,0, 2,2,1,1, 2,2,1,2, 2,2,1,3,
        2,2,2,0, 2,2,2,1, 2,2,2,2, 2,2,2,3, 2,2,3,0, 2,2,3,1, 2,2,3,2, 2,2,3,3,
        2,3,0,0, 2,3,0,1, 2,3,0,2, 2,3,0,3, 2,3,1,0, 2,3,1,1, 2,3,1,2, 2,3,1,3,
        2,3,2,0, 2,3,2,1, 2,3,2,2, 2,3,2,3, 2,3,3,0, 2,3,3,1, 2,3,3,2, 2,3,3,3,
        3,0,0,0, 3,0,0,1, 3,0,0,2, 3,0,0,3, 3,0,1,0, 3,0,1,1, 3,0,1,2, 3,0,1,3,
        3,0,2,0, 3,0,2,1, 3,0,2,2, 3,0,2,3, 3,0,3,0, 3,0,3,1, 3,0,3,2, 3,0,3,3,
        3,1,0,0, 3,1,0,1, 3,1,0,2, 3,1,0,3, 3,1,1,0, 3,1,1,1, 3,1,1,2, 3,1,1,3,
        3,1,2,0, 3,1,2,1, 3,1,2,2, 3,1,2,3, 3,1,3,0, 3,1,3,1, 3,1,3,2, 3,1,3,3,
        3,2,0,0, 3,2,0,1, 3,2,0,2, 3,2,0,3, 3,2,1,0, 3,2,1,1, 3,2,1,2, 3,2,1,3,
        3,2,2,0, 3,2,2,1, 3,2,2,2, 3,2,2,3, 3,2,3,0, 3,2,3,1, 3,2,3,2, 3,2,3,3,
        3,3,0,0, 3,3,0,1, 3,3,0,2, 3,3,0,3, 3,3,1,0, 3,3,1,1, 3,3,1,2, 3,3,1,3,
        3,3,2,0, 3,3,2,1, 3,3,2,2, 3,3,2,3, 3,3,3,0, 3,3,3,1, 3,3,3,2, 3,3,3,3,
    };

// standard VRAM table adresses
unsigned char *sprite_table   = get_address(0x00008800);
unsigned char *pattern_table   = get_address(0x0000A000);
unsigned short*patterns = (unsigned short*)pattern_table;
unsigned short *tile_table_front  = (unsigned short *)get_address(0x00009000);
unsigned short *tile_table_back  = (unsigned short *)get_address(0x00009800);
unsigned short *palette_table   = (unsigned short *)get_address(0x00008200);
unsigned char *bw_palette_table  = get_address(0x00008100);
unsigned char *sprite_palette_numbers = get_address(0x00008C00);
// VDP registers
//
// wher is the vdp rendering now on the lcd display?
//unsigned char *scanlineX  = get_address(0x00008008);
unsigned char *scanlineY  = get_address(0x00008009);
// frame 0/1 priority registers
unsigned char *frame0Pri  = get_address(0x00008000);
unsigned char *frame1Pri  = get_address(0x00008030);
// windowing registers
unsigned char *wndTopLeftX = get_address(0x00008002);
unsigned char *wndTopLeftY = get_address(0x00008003);
unsigned char *wndSizeX  = get_address(0x00008004);
unsigned char *wndSizeY  = get_address(0x00008005);
// scrolling registers
unsigned char *scrollSpriteX = get_address(0x00008020);
unsigned char *scrollSpriteY = get_address(0x00008021);
unsigned char *scrollFrontX = get_address(0x00008032);
unsigned char *scrollFrontY = get_address(0x00008033);
unsigned char *scrollBackX = get_address(0x00008034);
unsigned char *scrollBackY = get_address(0x00008035);
// background color selection register and table
unsigned char *bgSelect  = get_address(0x00008118);
unsigned short *bgTable  = (unsigned short *)get_address(0x000083E0);
unsigned char *oowSelect  = get_address(0x00008012);
unsigned short *oowTable  = (unsigned short *)get_address(0x000083F0);
// machine constants
unsigned char *color_switch = get_address(0x00006F91);


// Defines
#define ZeroStruct(x) ZeroMemory(&x, sizeof(x)); x.dwSize = sizeof(x);
#define SafeRelease(x) if(x) { x->Release(); x = NULL; }


// target window
/*
unsigned short p2[16] = {
                            0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
                            0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000
                        };
*/

#define BLIT_X_OFFSET 8
#define BLIT_Y_OFFSET 8
#define BLIT_OFFSET (BLIT_X_OFFSET + (BLIT_Y_OFFSET*SIZEX))
#define BLIT_WIDTH (160)
#define BLIT_HEIGHT (152)
#define SCREEN_X_OFFSET ((screen->w - BLIT_WIDTH)/2)
#define SCREEN_Y_OFFSET ((screen->h - BLIT_HEIGHT)/2)
#define SCREEN_OFFET (SCREEN_X_OFFSET + (SCREEN_Y_OFFSET*screen->w))


#ifdef DO_FPS_DISPLAY  //in graphics.h
static unsigned int frameCount = 0;
#ifdef __GP32__
static char fps[32];
#endif
#endif

//Flavor - For speed testing, you can turn off screen updates
//#define NO_SCREEN_OUTPUT  //seems to give about 4-6FPS on GP2X

// flip buffers and indicate that one of the buffers is ready
// for blitting to the screen.
void graphicsBlitEnd()
{
    displayDirty=1;
}



// blit the display buffer, if necessary, and put it to the screen
inline void graphics_paint()
{
#ifndef NO_SCREEN_OUTPUT
    //if (displayDirty)
    //{
        //dbg_print("in graphics_paint displayDirty=%d\n", displayDirty);

//        unsigned short *p;
//        int    i,j;


        // write to the surface
/*        SDL_LockSurface(screen);

        int  *sp = (int *)screen->pixels + SCREEN_OFFET, *sp2;
        short *sp16 = (short *)screen->pixels + SCREEN_OFFET, *sp162;
        byte *sp8 = (byte *)screen->pixels + SCREEN_OFFET, *sp82;
        p = drawBuffer + BLIT_OFFSET;
        switch (screen->format->BitsPerPixel)
        {
            case 8:
            // handle the window function
            sp82 = sp8;
            for (i=0; i<SIZEY;i++)
            {
                sp8 = sp82;
                for (j=0; j<SIZEX; j++)
                {
                    if(*p > 255)
                        dbg_print("graphics_paint: *p > 255");
                    *sp8 = *p;
                    sp8++;
                    p++;
                }
                sp82 += (screen->w);
            }
            break;
            case 16:
            // handle the window function
            sp162 = sp16;
            for (i=0; i<BLIT_HEIGHT;i++)
            {
                //sp16 = sp162;
                memcpy(sp162, p, BLIT_WIDTH*2);
                p+=BLIT_WIDTH;
//                for (j=0; j<BLIT_WIDTH; j++)
//                {
//                    *sp16 = *p;//totalpalette[*p];
//                    sp16++;
//                    p++;
 //               }
                if(SIZEX > BLIT_WIDTH)
                    p+=(SIZEX-BLIT_WIDTH);
                sp162 += (screen->w);
            }
            break;
            default:
            fprintf(stderr, "graphics_paint: unsupported %d bpp\n", screen->format->BitsPerPixel);
            break;
            case 32:
             // handle the window function
             sp2 = sp;
             for (i=0; i<SIZEY;i++)
             {
              sp = sp2;
              for (j=0; j<SIZEX; j++) {
               *sp = totalpalette[*p];
               sp++; p++;
              }
              sp2+= (screen->w);
             }
             break;
        }
        SDL_UnlockSurface(screen);*/

        //displayDirty = 0;
#ifndef __GP32__
        SDL_UpdateRect(screen, SCREEN_X_OFFSET, SCREEN_Y_OFFSET, BLIT_WIDTH, BLIT_HEIGHT);
#endif
        SDL_Flip(screen);
    }
#endif

#ifdef DO_FPS_DISPLAY
    static unsigned int startTime = 0;
    unsigned int currTime;

#ifndef __GP32__
    if(startTime == 0)
        startTime = SDL_GetTicks();

    currTime = SDL_GetTicks();

    if((currTime - startTime) >= 1000)
    {
        SDL_Rect numRect = drawNumber(frameCount, 10, 10);
        SDL_UpdateRect(screen, numRect.x, numRect.y, numRect.w, numRect.h);
         SDL_Flip(screen);
        startTime = currTime;
        frameCount = 0;
    }
#else
    if(startTime == 0)
        startTime = getTickCount();

    currTime = getTickCount();

    if((currTime - startTime) >= 1000)
    {
        sprintf(fps,"%d",frameCount);
        startTime = currTime;
        frameCount = 0;
    }
    drawTextAt(fps,OFFSETX,OFFSETY,COLOR_WHITE);
#endif
#endif


#ifdef DO_FPS_DISPLAY
void incFrameCount()
{
    frameCount++;
}
#endif


//////////////////////////////////////////////////////////////////////////////
//
// Palette Initialization
//
//////////////////////////////////////////////////////////////////////////////

void (*palette_init)(DWORD dwRBitMask, DWORD dwGBitMask, DWORD dwBBitMask);

void palette_init32(DWORD dwRBitMask, DWORD dwGBitMask, DWORD dwBBitMask)
{
/*    dbg_print("in palette_init32(0x%X, 0x%X, 0x%X)\n", dwRBitMask, dwGBitMask, dwBBitMask);

    char RShiftCount = 0, GShiftCount = 0, BShiftCount = 0;
    char RBitCount = 0, GBitCount = 0, BBitCount = 0;
    int r,g,b;
    DWORD i;

    i = dwRBitMask;
    while (!(i&1))
    {
        i = i >> 1;
        RShiftCount++;
    }
    while (i&1)
    {
        i = i >> 1;
        RBitCount++;
    }
    i = dwGBitMask;
    while (!(i&1))
    {
        i = i >> 1;
        GShiftCount++;
    }
    while (i&1)
    {
        i = i >> 1;
        GBitCount++;
    }
    i = dwBBitMask;
    while (!(i&1))
    {
        i = i >> 1;
        BShiftCount++;
    }
    while (i&1)
    {
        i = i >> 1;
        BBitCount++;
    }
    switch(m_emuInfo.machine)
    {
        case NGP:
        case NGPC:
        case GAMEGEAR:
        case LYNX:
        case WONDERSWAN:
        case WONDERSWANCOLOR:
        case ADVISION:
        for (b=0; b<16; b++)
            for (g=0; g<16; g++)
                for (r=0; r<16; r++)
                    totalpalette[b*256+g*16+r] =
                        (((b<<(BBitCount-4))+(b>>(4-(BBitCount-4))))<<BShiftCount) +
                        (((g<<(GBitCount-4))+(g>>(4-(GBitCount-4))))<<GShiftCount) +
                        (((r<<(RBitCount-4))+(r>>(4-(RBitCount-4))))<<RShiftCount);
        break;
        case GAMEBOY:
        case GAMEBOYPOCKET:
        case GAMEBOYCOLOR:
        case SUPERGAMEBOY:
        case SUPERVISION:
        case NES:
        for (b=0; b<32; b++)
            for (g=0; g<32; g++)
                for (r=0; r<32; r++)
                    totalpalette[b*32*32+g*32+r] =
                        (((b<<(BBitCount-5))+(b>>(5-(BBitCount-5))))<<BShiftCount) +
                        (((g<<(GBitCount-5))+(g>>(5-(GBitCount-5))))<<GShiftCount) +
                        (((r<<(RBitCount-5))+(r>>(5-(RBitCount-5))))<<RShiftCount);
        break;
    }*/
}

void palette_init16(DWORD dwRBitMask, DWORD dwGBitMask, DWORD dwBBitMask)
{
    //dbg_print("in palette_init16(0x%X, 0x%X, 0x%X)\n", dwRBitMask, dwGBitMask, dwBBitMask);

    char RShiftCount = 0, GShiftCount = 0, BShiftCount = 0;
    char RBitCount = 0, GBitCount = 0, BBitCount = 0;
    int  r,g,b;
    DWORD i;

    i = dwRBitMask;
    while (!(i&1))
    {
        i = i >> 1;
        RShiftCount++;
    }
    while (i&1)
    {
        i = i >> 1;
        RBitCount++;
    }
    i = dwGBitMask;
    while (!(i&1))
    {
        i = i >> 1;
        GShiftCount++;
    }
    while (i&1)
    {
        i = i >> 1;
        GBitCount++;
    }
    i = dwBBitMask;
    while (!(i&1))
    {
        i = i >> 1;
        BShiftCount++;
    }
    while (i&1)
    {
        i = i >> 1;
        BBitCount++;
    }
    switch(m_emuInfo.machine)
    {
        case NGP:
        case NGPC:
        case GAMEGEAR:
        case LYNX:
        case WONDERSWAN:
        case WONDERSWANCOLOR:
        case ADVISION:
        for (b=0; b<16; b++)
            for (g=0; g<16; g++)
                for (r=0; r<16; r++)
                    totalpalette[b*256+g*16+r] =
                        (((b<<(BBitCount-4))+(b>>(4-(BBitCount-4))))<<BShiftCount) +
                        (((g<<(GBitCount-4))+(g>>(4-(GBitCount-4))))<<GShiftCount) +
                        (((r<<(RBitCount-4))+(r>>(4-(RBitCount-4))))<<RShiftCount);
        break;
        case GAMEBOY:
        case GAMEBOYPOCKET:
        case GAMEBOYCOLOR:
        case SUPERGAMEBOY:
        case SUPERVISION:
        for (b=0; b<32; b++)
            for (g=0; g<32; g++)
                for (r=0; r<32; r++)
                    totalpalette[b*32*32+g*32+r] =
                        (((b<<(BBitCount-5))+(b>>(5-(BBitCount-5))))<<BShiftCount) +
                        (((g<<(GBitCount-5))+(g>>(5-(GBitCount-5))))<<GShiftCount) +
                        (((r<<(RBitCount-5))+(r>>(5-(RBitCount-5))))<<RShiftCount);
        break;
    }
}

void palette_init8(DWORD dwRBitMask, DWORD dwGBitMask, DWORD dwBBitMask)
{
/*
    dbg_print("in palette_init8(0x%X, 0x%X, 0x%X)\n", dwRBitMask, dwGBitMask, dwBBitMask);
    SDL_Color SDLcolors[256];
    unsigned char r, g, b;

    //Generic 332 RGB palette
    for (b=0; b<0x3; b++)
        for (g=0; g<0x7; g++)
            for (r=0; r<0x7; r++)
            {
                SDLcolors[b+(g<<2)+(r<<5)].r = r<<5;
                SDLcolors[b+(g<<2)+(r<<5)].g = g<<5;
                SDLcolors[b+(g<<2)+(r<<5)].b = b<<6;
            }

    if(!SDL_SetColors(screen, SDLcolors, 0, 256))
        dbg_print("palette_init8: SDL_SetPalette failed\n");
*/
}


void pngpalette_init(void)
{
/*    int r,g,b;

    switch(m_emuInfo.machine)
    {
        case NGP:
        case NGPC:
        case GAMEGEAR:
        case LYNX:
        case WONDERSWAN:
        case WONDERSWANCOLOR:
        case ADVISION:
        for (b=0; b<16; b++)
            for (g=0; g<16; g++)
                for (r=0; r<16; r++)
                {
                    totalpalette32[b*256+g*16+r] =
                        (((b<<(8-4))+(b>>(4-(8-4))))<<0) +
                        (((g<<(8-4))+(g>>(4-(8-4))))<<8) +
                        (((r<<(8-4))+(r>>(4-(8-4))))<<16);
                }
        break;
        case GAMEBOY:
        case GAMEBOYPOCKET:
        case GAMEBOYCOLOR:
        case SUPERGAMEBOY:
        case SUPERVISION:
        case NES:
        for (b=0; b<32; b++)
            for (g=0; g<32; g++)
                for (r=0; r<32; r++)
                {
                    totalpalette32[b*32*32+g*32+r] =
                        (((b<<(8-5))+(b>>(5-(8-5))))<<0) +
                        (((g<<(8-5))+(g>>(5-(8-5))))<<8) +
                        (((r<<(8-5))+(r>>(5-(8-5))))<<16);
                }
        break;
    }*/
}

//////////////////////////////////////////////////////////////////////////////
//
// Neogeo Pocket & Neogeo Pocket color rendering
//
//////////////////////////////////////////////////////////////////////////////

//static const bwTable[8] = { 0x0000, 0x0333, 0x0555, 0x0777, 0x0999, 0x0BBB, 0x0DDD, 0x0FFF };
static const unsigned short bwTable[8] =
    {
        0x0FFF, 0x0DDD, 0x0BBB, 0x0999, 0x0777, 0x0555, 0x0333, 0x0000
    };

#ifndef __GP32__

// used for rendering the sprites

typedef struct
{
    unsigned short offset;    // offset in graphics buffer to blit, Flavor hopes 16bits is good enough
    unsigned short pattern;   // pattern code including palette number
    unsigned short *tilept;   // pointer into the tile description
    unsigned short *palette;   // palette used to render this sprite
}
SPRITE;

typedef struct
{
    unsigned short *gbp;    // (0,x) base for drawing
    unsigned char count[152];
    SPRITE   sprite[152][64];
}
SPRITEDEFS;

//unsigned int spritesDirty = true;

SPRITEDEFS spriteDefs[3];    // 4 priority levels

// definitions of types and variables that are used internally during
// rendering of a screen

typedef struct
{
    unsigned short *gbp;     // pointer into graphics buffer
    unsigned char oldScrollX;    // keep an eye on the old and previous values
    unsigned char *newScrollX;   // of the scroll values
    unsigned char oldScrollY;
    unsigned char *newScrollY;
    unsigned short *tileBase;    // start address of the tile table this structure represents
    short   tile[21];    // the tile code
    unsigned short *palettes[21];   // palettes associated with tiles
    unsigned short *tilept[21];   // tile lines to render
    unsigned short *palette;    // palette for the tiles this VSYNC
}
TILECACHE;

unsigned short palettes[16*4+16*4+16*4]; // placeholder for the converted palette


TILECACHE  tCBack;      // tile pointer cache for the back buffer
TILECACHE  tCFront;     // tile pointer cache for the front buffer

//int    BGCol;      // placeholder for the background color this VSYNC
//int    OOWCol;      // placeholder for the outside window color this VSYNC
//unsigned char SprPriLo, SprPriHi, SprPri = 0;


inline void set_paletteCol(unsigned short *palette_table,
                    unsigned short *sprite,
                    unsigned short *front,
                    unsigned short *back)
{
    int i;
    // initialize palette table
    //
    // initialize back plane palette table

    //these should actually be setting the SDL palette

    for(i=0;i<16*4;i++)
    {
        //sprite[i] = *palette_table & 0x0FFF;
        sprite[i] = NGPC_TO_SDL16(palette_table[i]);
        //SDLpalette[i].r=(palette_table[i]&0xF)<<4;
        //SDLpalette[i].g=(palette_table[i]&0xF0);
        //SDLpalette[i].b=((palette_table[i]>>4)&0xF0);
    }

    // initialize front plane palette table
    for(i=0;i<16*4;i++)
    {
        //front[i] = *palette_table & 0x0FFF;
        front[i] = NGPC_TO_SDL16(palette_table[i+16*4]);
        //SDLpalette[i].r=(palette_table[i+16*4]&0xF)<<4;
        //SDLpalette[i].g=(palette_table[i+16*4]&0xF0);
        //SDLpalette[i].b=((palette_table[i+16*4]>>4)&0xF0);
    }

    // initialize sprite palette table (?)
    for(i=0;i<16*4;i++)
    {
        //back[i] = *palette_table & 0x0FFF;
        back[i] = NGPC_TO_SDL16(palette_table[i+16*4*2]);
        //SDLpalette[i].r=(palette_table[i+16*4*2]&0xF)<<4;
        //SDLpalette[i].g=(palette_table[i+16*4*2]&0xF0);
        //SDLpalette[i].b=((palette_table[i+16*4*2]>>4)&0xF0);
    }

    //if(!SDL_SetColors(screen, SDLpalette, 16*4*2, 16*4))
    //    dbg_print("set_paletteCol: SDL_SetPalette failed\n");

    //Flavor
    //I could take all the colors from sprite, front, back , BGCol, OOWCol, and set the SDL palette
    //or, just take palette_table and set it
}

inline void set_paletteBW(unsigned short *palette_table,
                   unsigned short *sprite,
                   unsigned short *front,
                   unsigned short *back)
{
    int i;
    unsigned char *pt = ((unsigned char *)palette_table)-0x0100; // get b/w color table
    // initialize palette table
    //
    for(i=0;i<4;i++)
    {
        // initialize sprite palette table
        sprite[i] = NGPC_TO_SDL16(bwTable[pt[i] & 0x07]);
        sprite[4+i] = NGPC_TO_SDL16(bwTable[pt[4+i] & 0x07]);
        // initialize front plane palette table
        front[i] = NGPC_TO_SDL16(bwTable[pt[8+i] & 0x07]);
        front[4+i] = NGPC_TO_SDL16(bwTable[pt[8+4+i] & 0x07]);
        // initialize back plane palette table
        back[i] = NGPC_TO_SDL16(bwTable[pt[16+i] & 0x07]);
        back[4+i] = NGPC_TO_SDL16(bwTable[pt[16+4+i] & 0x07]);
    }
}

//I think there's something wrong with this on the GP2X, because CFC2's intro screen is all red
inline void lineClear(TILECACHE *tC, unsigned short col)
{
    for(int i=0; i<21*8; i++)
    {
        tC->gbp[i] = col;
    }
}

inline void clipLeftRight(SPRITEDEFS *sprDef, unsigned short OOWCol)
{
    int  i,j;

    j = *wndTopLeftX + 8;
    if (j > (NGPC_SIZEX+8))
        j = NGPC_SIZEX+8;
    for(i=8; i<j; i++)
        sprDef->gbp[i] = OOWCol;
    j = j + *wndSizeX;
    if (j > (NGPC_SIZEX+8))
        j = NGPC_SIZEX+8;
    for(i=j; i < (NGPC_SIZEX+8); i++)
        sprDef->gbp[i] = OOWCol;
}

inline void lineFront(TILECACHE *tC)
{
    int    i;
    unsigned char a,b;
    unsigned char *p2;
    unsigned short *gb;

    //for 8bit SDL, this would set gb to the index of the proper color
    //then, we'd set gb to p2[n]+(i*sizeof(palette))

    gb = tC->gbp;
    for (i=0;i<21;i++)
    {
        a = *(((unsigned char *)tC->tilept[i])+1);
        b = *((unsigned char *)tC->tilept[i]);
        if (tC->tile[i]&0x8000)
        {
            p2 = &mypatterns[b*4];
            if (p2[3])
                gb[0] = tC->palettes[i][p2[3]];
            if (p2[2])
                gb[1] = tC->palettes[i][p2[2]];
            if (p2[1])
                gb[2] = tC->palettes[i][p2[1]];
            if (p2[0])
                gb[3] = tC->palettes[i][p2[0]];
            p2 = &mypatterns[a*4];
            if (p2[3])
                gb[4] = tC->palettes[i][p2[3]];
            if (p2[2])
                gb[5] = tC->palettes[i][p2[2]];
            if (p2[1])
                gb[6] = tC->palettes[i][p2[1]];
            if (p2[0])
                gb[7] = tC->palettes[i][p2[0]];
        }
        else
        {
            p2 = &mypatterns[a*4];
            if (p2[0])
                gb[0] = tC->palettes[i][p2[0]];
            if (p2[1])
                gb[1] = tC->palettes[i][p2[1]];
            if (p2[2])
                gb[2] = tC->palettes[i][p2[2]];
            if (p2[3])
                gb[3] = tC->palettes[i][p2[3]];
            p2 = &mypatterns[b*4];
            if (p2[0])
                gb[4] = tC->palettes[i][p2[0]];
            if (p2[1])
                gb[5] = tC->palettes[i][p2[1]];
            if (p2[2])
                gb[6] = tC->palettes[i][p2[2]];
            if (p2[3])
                gb[7] = tC->palettes[i][p2[3]];
        }
        if (tC->tile[i]&0x4000)
            tC->tilept[i]-= 1;
        else
            tC->tilept[i]+= 1;
        gb+= 8;
    }
}

inline void lineSprite(SPRITEDEFS *sprDefs)
{
    SPRITE   *spr;
    unsigned short *gb;
    unsigned char a,b;
    unsigned char *p2;

    //for 8bit SDL, this would set gb to the index of the proper color
    //then, we'd set gb to p2[n]

    for (int i=sprDefs->count[*scanlineY];i>0;i--)
    {
        spr = &sprDefs->sprite[*scanlineY][i-1];
        gb = &sprDefs->gbp[spr->offset];
        a = *(((unsigned char *)spr->tilept)+1);
        b = *((unsigned char *)spr->tilept);
        if (spr->pattern&0x8000)
        {
            p2 = &mypatterns[b*4];
            if (p2[3])
                gb[0] = spr->palette[p2[3]];
            if (p2[2])
                gb[1] = spr->palette[p2[2]];
            if (p2[1])
                gb[2] = spr->palette[p2[1]];
            if (p2[0])
                gb[3] = spr->palette[p2[0]];
            p2 = &mypatterns[a*4];
            if (p2[3])
                gb[4] = spr->palette[p2[3]];
            if (p2[2])
                gb[5] = spr->palette[p2[2]];
            if (p2[1])
                gb[6] = spr->palette[p2[1]];
            if (p2[0])
                gb[7] = spr->palette[p2[0]];
        }
        else
        {
            p2 = &mypatterns[a*4];
            if (p2[0])
                gb[0] = spr->palette[p2[0]];
            if (p2[1])
                gb[1] = spr->palette[p2[1]];
            if (p2[2])
                gb[2] = spr->palette[p2[2]];
            if (p2[3])
                gb[3] = spr->palette[p2[3]];
            p2 = &mypatterns[b*4];
            if (p2[0])
                gb[4] = spr->palette[p2[0]];
            if (p2[1])
                gb[5] = spr->palette[p2[1]];
            if (p2[2])
                gb[6] = spr->palette[p2[2]];
            if (p2[3])
                gb[7] = spr->palette[p2[3]];
        }
    }
}




#ifndef NO_FASTMEM

#ifdef DREAMCAST
#include<kos.h>
#endif

#define LIMIT 40
//#define TOO_SHORT_PATCH
//#define USE_TWIDLE_PATCH

#if defined(TOO_SHORT_PATCH) && !defined(USE_TWIDLE_PATCH)
#undef TOO_SHORT_PATCH
#endif

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<SDL.h>

#if defined(__cplusplus) && !defined(USE_CPLUS)
extern "C" {
void* fast_memcpy(void *OUT, const void *IN, size_t N);
void* fast_memset(const void *DST, int C, size_t LENGTH);
}
#endif

void* fast_memcpy(void *OUT, const void *IN, size_t N)
{
	register unsigned char *s=(unsigned char *)IN;
	register unsigned char *d=(unsigned char *)OUT;
#ifdef USE_TWIDLE_PATCH
	if (N<LIMIT)
#else
	if ((N<LIMIT)||((((unsigned)s)&3) != (((unsigned)d)&3)))
#endif
	{
#ifdef TOO_SHORT_PATCH
		if ((N>8)&&( (((unsigned)s)&3) == (((unsigned)d)&3)))
		{
			while (((unsigned)d)&3)
			{
				*d++=*s++;
				N--;
			}
			register int n=N>>2;
			N-=n<<2;
			while (n--)
			{
				*((unsigned *)d)=*((unsigned *)s);
				d+=4;
				s+=4;
			}
		}
#endif
	}
	else
	{
#ifdef USE_TWIDLE_PATCH
		if ((((unsigned)s)&3) != (((unsigned)d)&3))
		{
			while ((((unsigned)d)&3)&&(N))
			{
				*d++=*s++;
				N--;
			}
			while ((((unsigned)d)&31)&&(N))
			{
				*((unsigned *)d)=(s[3]<<24)|(s[2]<<16)|(s[1]<<8)|s[0];
				d+=4;
				s+=4;
				N-=4;
			}
			register int n=N>>5;
			N -= n<<5;
#ifdef DREAMCAST
			{
			unsigned *dsq=(unsigned *)(0xe0000000 | (((unsigned long)d) & 0x03ffffe0));
			QACR0 = ((((unsigned int)d)>>26)<<2)&0x1c;
			QACR1 = ((((unsigned int)d)>>26)<<2)&0x1c;
#else
			unsigned *dsq=d;
#endif
			while (n--)
			{
#ifdef DREAMCAST
				asm("pref @%0" : : "r" (s + 32));
#endif
				dsq[0]=(s[3]<<24)|(s[2]<<16)|(s[1]<<8)|s[0]; s+=4;
				dsq[1]=(s[3]<<24)|(s[2]<<16)|(s[1]<<8)|s[0]; s+=4;
				dsq[2]=(s[3]<<24)|(s[2]<<16)|(s[1]<<8)|s[0]; s+=4;
				dsq[3]=(s[3]<<24)|(s[2]<<16)|(s[1]<<8)|s[0]; s+=4;
				dsq[4]=(s[3]<<24)|(s[2]<<16)|(s[1]<<8)|s[0]; s+=4;
				dsq[5]=(s[3]<<24)|(s[2]<<16)|(s[1]<<8)|s[0]; s+=4;
				dsq[6]=(s[3]<<24)|(s[2]<<16)|(s[1]<<8)|s[0]; s+=4;
				dsq[7]=(s[3]<<24)|(s[2]<<16)|(s[1]<<8)|s[0]; s+=4;
#ifdef DREAMCAST
				asm("ocbwb @%0" : : "r" (d));
				asm("pref @%0" : : "r" (dsq));
				asm("ocbi @%0" : : "r" (d));
#endif
				d+=32;
				dsq+=8;
			}
#ifdef DREAMCAST
			dsq=(unsigned *)0xe0000000;
			dsq[0] = dsq[8] = 0;
#endif
			}
			
			n=N>>2;
			N -= n<<2;
			while (n--)
			{
				*((unsigned *)d)=(s[3]<<24)|(s[2]<<16)|(s[1]<<8)|s[0];
				d+=4;
				s+=4;
			}
		}
		else
#endif
		{
			while ((((unsigned)d)&3)&&(N))
			{
				*d++=*s++;
				N--;
			}
			while ((((unsigned)d)&31)&&(N))
			{
				*((unsigned *)d)=*((unsigned *)s);
				d+=4;
				s+=4;
				N-=4;
			}
			register int n=N>>5;
			N -= n<<5;
			{
#ifdef DREAMCAST
			unsigned *dsq=(unsigned *)(0xe0000000 | (((unsigned long)d) & 0x03ffffe0));
			QACR0 = ((((unsigned int)d)>>26)<<2)&0x1c;
			QACR1 = ((((unsigned int)d)>>26)<<2)&0x1c;
#else
			unsigned *dsq=(unsigned *)d;
#endif
			while (n--)
			{
#ifdef DREAMCAST
				asm("pref @%0" : : "r" (s + 32));
#endif
				dsq[0]=*((unsigned *)s); s+=4;
				dsq[1]=*((unsigned *)s); s+=4;
				dsq[2]=*((unsigned *)s); s+=4;
				dsq[3]=*((unsigned *)s); s+=4;
				dsq[4]=*((unsigned *)s); s+=4;
				dsq[5]=*((unsigned *)s); s+=4;
				dsq[6]=*((unsigned *)s); s+=4;
				dsq[7]=*((unsigned *)s); s+=4;
#ifdef DREAMCAST
				asm("ocbwb @%0" : : "r" (d));
				asm("pref @%0" : : "r" (dsq));
				asm("ocbi @%0" : : "r" (d));
#endif
				dsq+=8;
				d+=32;
			}
#ifdef DREAMCAST
			dsq=(unsigned *)0xe0000000;
			dsq[0] = dsq[8] = 0;
#endif
			}
			n=N>>2;
			N -= n<<2;
			while (n--)
			{
				*((unsigned *)d)=*((unsigned *)s);
				d+=4;
				s+=4;
			}
		}
	}
	while (N--)
		*d++=*s++;
	return OUT;
}

void* fast_memset(const void *DST, int C, size_t LENGTH)
{
	register unsigned char *d=(unsigned char *)DST;
	register unsigned dat = C & 0xff;
	dat = (dat << 24) | (dat << 16) | (dat << 8) | dat;

	if (LENGTH<LIMIT)
	{
#ifdef TOO_SHORT_PATCH
		if (LENGTH>=8)
		{
			while (((unsigned)d)&3)
			{
				*d++=dat;
				LENGTH--;
			}
			register int n=LENGTH>>2;
			LENGTH-=n<<2;
			while (n--)
			{
				*((unsigned *)d)=dat;
				d+=4;
			}
		}
#endif
		while(LENGTH--)
			*d++=dat;
		return (void *)DST;
	}
	while (((unsigned)d)&3)
	{
		*d++=dat;
		LENGTH--;
	}
	while (((unsigned)d)&31)
	{
		*((unsigned *)d)=dat;
		d+=4;
		LENGTH-=4;
	}
	register int n=LENGTH>>5;
	LENGTH -= n<<5;
	if (n)
	{
#ifdef DREAMCAST
		unsigned *dsq=(unsigned *)(0xe0000000 | (((unsigned long)d) & 0x03ffffe0));
		QACR0 = ((((unsigned int)d)>>26)<<2)&0x1c;
		QACR1 = ((((unsigned int)d)>>26)<<2)&0x1c;
		dsq[0]=dsq[1]=dsq[2]=dsq[3]=dsq[4]=dsq[5]=dsq[6]=dsq[7]=dsq[8]=dsq[9]=dsq[10]=dsq[11]=dsq[12]=dsq[13]=dsq[14]=dsq[15]=dat;
#else
		unsigned *dsq=(unsigned *)d;
#endif
		while (n--)
		{
#ifndef DREAMCAST
			dsq[0]=dat;
			dsq[1]=dat;
			dsq[2]=dat;
			dsq[3]=dat;
			dsq[4]=dat;
			dsq[5]=dat;
			dsq[6]=dat;
			dsq[7]=dat;
#else
			asm("ocbwb @%0" : : "r" (d));
			asm("pref @%0" : : "r" (dsq));
			asm("ocbi @%0" : : "r" (d));
#endif
			dsq+=8;
			d+=32;
		}
#ifdef DREAMCAST
		dsq=(unsigned *)0xe0000000;
		dsq[0] = dsq[8] = 0;
#endif
	}
	n=LENGTH>>2;
	LENGTH -= n<<2;
	while (n--)
	{
		*((unsigned *)d)=dat;
		d+=4;
	}
	while (LENGTH--)
		*d++=dat;
	return (void *)DST;
}

#endif

inline void spriteSortAll(unsigned int bw)
{
    unsigned int spriteCode;
    unsigned short *pt;
    unsigned char x, y, prevx=0, prevy=0;
    unsigned int i, j, k, scanline;
    SPRITE *spr;

    SPRITEDEFS *SprPri40 = &spriteDefs[0];
    SPRITEDEFS *SprPri80 = &spriteDefs[1];
    SPRITEDEFS *SprPriC0 = &spriteDefs[2];

    // initialize the number of sprites in each structure
    fast_memset(SprPri40->count, 0, 152);
    fast_memset(SprPri80->count, 0, 152);
    fast_memset(SprPriC0->count, 0, 152);
    for (i=0;i<64;i++)
    {
        spriteCode = *((unsigned short *)(sprite_table+4*i));

        if ((spriteCode<=0xff) || ((spriteCode & 0x1800)==0))
            continue;

        if (spriteCode & 0x0400)
            prevx+= *(sprite_table+4*i+2);
        else
            prevx = *(sprite_table+4*i+2) + 8;
        x = prevx + *scrollSpriteX;

        if (spriteCode & 0x0200)
            prevy+= *(sprite_table+4*i+3);
        else
            prevy = *(sprite_table+4*i+3) + 8;
        y = prevy + *scrollSpriteY;

        if (x>167 || y>151)
            continue;

        for(k=0; k<8; k++)
        {
            scanline = y+k-8;
            if(scanline < 0 || scanline >= 152)
                continue;

//            if ((x<168) && (spriteCode>0xff) && (spriteCode & 0x1800))
//            {
                switch (spriteCode & 0x1800)
                {
                    // case order reversed because priority 3 (and 2) sprites occur most of the time
                    case 0x1800:
                    spr = &SprPriC0->sprite[scanline][SprPriC0->count[scanline]++];
                    break;
                    case 0x1000:
                    spr = &SprPri80->sprite[scanline][SprPri80->count[scanline]++];
                    break;
                    case 0x0800:
                    spr = &SprPri40->sprite[scanline][SprPri40->count[scanline]++];
                    break;
                }
                j = scanline+8 - y;
                spr->pattern = spriteCode;
                spr->offset = x;
                spr->tilept = (unsigned short *)(pattern_table + 16*(spriteCode & 0x01ff)
                                        + ((spriteCode&0x4000) ? (7-j)*2 : j*2));
                spr->palette = ((bw) ? &palettes[(spriteCode>>11)&0x04]
                                : &palettes[(*(sprite_palette_numbers+i)&0x0F)*4]);
//            }
        }
    }
}


// initialize drawing/blitting of a screen
void graphicsBlitInit()
{
    // buffers 0 and 1
    // definitions for the back frame
    tCBack.gbp   = &drawBuffer[8*SIZEX + (8-((*scrollBackX)&7))];
    tCBack.newScrollX = scrollBackX;
    tCBack.newScrollY = scrollBackY;
    tCBack.tileBase  = tile_table_back;
    tCBack.palette  = &palettes[16*4+16*4];
    // definitions for the front frame
    tCFront.gbp   = &drawBuffer[8*SIZEX + (8-((*scrollFrontX)&7))];
    tCFront.newScrollX = scrollFrontX;
    tCFront.newScrollY = scrollFrontY;
    tCFront.tileBase = tile_table_front;
    tCFront.palette  = &palettes[16*4];
    // definitions for sprite priorities
    //SprPriLo = *frame1Pri>>6;
    //SprPriHi = *frame0Pri>>6; // ?
    spriteDefs[0].gbp = &drawBuffer[8*SIZEX];
    spriteDefs[1].gbp = &drawBuffer[8*SIZEX];
    spriteDefs[2].gbp = &drawBuffer[8*SIZEX];
    //spriteDefs[3].gbp = &drawBuffer[8*SIZEX];
    // force recalculations for first line
    tCBack.oldScrollX = *tCBack.newScrollX;
    tCBack.oldScrollY = *tCBack.newScrollY+1;   // force calculation of structure data
    tCFront.oldScrollX = *tCFront.newScrollX;
    tCFront.oldScrollY = *tCFront.newScrollY+1;  // force calculation of structure data
    // start drawing at line 0
    // *scanlineY = 0;
}

inline void RenderTileCache(TILECACHE *tC, unsigned int bw)
{
    int    i;
    unsigned char line;
    unsigned short *temp;

    if ((tC->oldScrollX != *tC->newScrollX) ||
            (tC->oldScrollY != *tC->newScrollY) ||
            (((*tC->newScrollY + *scanlineY)&7) == 0))
    {
        // update the structure to reflect the changed scroll registers
        // first update pointer into render buffer
        tC->gbp = tC->gbp + (tC->oldScrollX&7) - (*tC->newScrollX&7);

        tC->oldScrollX = *tC->newScrollX;
        tC->oldScrollY = *tC->newScrollY;

        temp = tC->tileBase + (((tC->oldScrollY + *scanlineY)&0xf8)<<2);
        for (i=0;i<21;i++)
        {
            tC->tile[i] = *(temp + (((tC->oldScrollX>>3) + i)&31));
            line = (*tC->newScrollY + *scanlineY)&7;
            tC->palettes[i] = ((bw) ? tC->palette + ((tC->tile[i] & 0x2000) ? 4 : 0)
                                       : tC->palette + ((tC->tile[i]>>7) & 0x3C));
            tC->tilept[i] = (unsigned short *)(pattern_table + ((tC->tile[i] & 0x01ff)<<4));
            tC->tilept[i]+= ((tC->tile[i]&0x4000) ? 7-line : line);
        }
    }
}

void graphicsBlitLine(unsigned char render)
{
    //dbg_print("in graphicsBlitLine *scanlineY = %d\n", *scanlineY);


    if (*scanlineY < 152)
    {
        //if(*scanlineY == 0)  //Flavor moved set_palette off of every line.  Helps speed, hurts hi-color apps
        //Flavor now only changes palette when dirty

        //this is currently broken for BW games
        //if(*scanlineY == 0)


        // set the palettes, background color, and outside window color
        //Flavor moving to scanline 0 set_palette(palette_table,&palettes[0],&palettes[16*4],&palettes[16*4+16*4]);
        // sort sprites by priority
        if(render)
        {
            unsigned int bw = (m_emuInfo.machine == NGP);
            unsigned short OOWCol = NGPC_TO_SDL16(oowTable[*oowSelect & 0x07]);

            SDL_LockSurface(screen);
            if(*scanlineY == 0)
            {
                if(bw)
                    set_paletteBW(palette_table,&palettes[0],&palettes[16*4],&palettes[16*4+16*4]);
                else
                    set_paletteCol(palette_table,&palettes[0],&palettes[16*4],&palettes[16*4+16*4]);

                /*if(spritesDirty)
                {
                    spriteSortAll(bw);
                    spritesDirty = false;
                }*/
                spriteSortAll(bw);
            }

            //spriteSort(bw);  //this needs to be re-done faster.  We shouldn't need to sort them every scanline

            // change the tile caches if needed
            RenderTileCache(&tCBack, bw);
            RenderTileCache(&tCFront, bw);

            // blit the planes, take priority registers into account

            if(*bgSelect & 0x80) /*== 0x80)*/
                lineClear(&tCBack, NGPC_TO_SDL16(bgTable[*bgSelect & 0x07]));
            else if(bw)
                lineClear(&tCBack, NGPC_TO_SDL16(bwTable[0]));  //in 8-bit mode, this would be the index of BGCol in the SDL palette
            else
                lineClear(&tCBack, 0);  //in 8-bit mode, this would be the index of BGCol in the SDL palette


            lineSprite(&spriteDefs[0]);

            if (*frame1Pri & 0x80)
            {
                lineFront(&tCFront);
                lineSprite(&spriteDefs[1]);
                lineFront(&tCBack);
            }
            else
            {
                lineFront(&tCBack);
                lineSprite(&spriteDefs[1]);
                lineFront(&tCFront);
            }

            lineSprite(&spriteDefs[2]);


            // clip left and right sides of screen if necessary
            clipLeftRight(&spriteDefs[2], OOWCol);

            if (*wndTopLeftY > *scanlineY || *scanlineY >= (*wndTopLeftY + *wndSizeY))
            {
                //tCBack.gbp  -= SIZEX;  //Flavor, I don't get why these were here
                lineClear(&tCBack, OOWCol);  //in 8-bit mode, this would be the index of OOWCol in the SDL palette
                //tCBack.gbp  += SIZEX;  //Flavor, I don't get why these were here
            }

            SDL_UnlockSurface(screen);
        }

        // increase scanline count
        tCFront.gbp+= SIZEX;
        tCBack.gbp+= SIZEX;
        spriteDefs[0].gbp+= SIZEX;
        spriteDefs[1].gbp+= SIZEX;
        spriteDefs[2].gbp+= SIZEX;
        //spriteDefs[3].gbp+= SIZEX;

        if (*scanlineY == 151)
        {
            // start VBlank period
            tlcsMemWriteB(0x00008010,tlcsMemReadB(0x00008010) | 0x40);
            if(render)
                graphics_paint();//displayDirty = 1;
        }

        *scanlineY+= 1;
    }
    else if (*scanlineY == 198)
    {
        // stop VBlank period
        tlcsMemWriteB(0x00008010,tlcsMemReadB(0x00008010) & ~0x40);
        graphicsBlitInit();

#ifdef DO_FPS_DISPLAY
        incFrameCount();
#endif

        *scanlineY = 0;
    }
    else
        *scanlineY+= 1;

}

#endif // __GP32__

//
// THOR'S GRAPHIC CORE
//

typedef struct
{
	unsigned char flip;
	unsigned char x;
	unsigned char pal;
} MYSPRITE;

typedef struct
{
	unsigned short tile;
	unsigned char id;
} MYSPRITEREF;

typedef struct
{
	unsigned char count[152];
	MYSPRITEREF refs[152][64];
} MYSPRITEDEF;

MYSPRITEDEF mySprPri40,mySprPri80,mySprPriC0;
MYSPRITE mySprites[64];
unsigned short myPalettes[192];

void sortSprites(unsigned int bw)
{
    unsigned int spriteCode;
    unsigned char x, y, prevx=0, prevy=0;
    unsigned int i, j;
    unsigned short tile;
    MYSPRITEREF *spr;

    // initialize the number of sprites in each structure
    fast_memset(mySprPri40.count, 0, 152);
    fast_memset(mySprPri80.count, 0, 152);
    fast_memset(mySprPriC0.count, 0, 152);

    for (i=0;i<64;i++)
    {
        spriteCode = *((unsigned short *)(sprite_table+4*i));

        prevx = (spriteCode & 0x0400 ? prevx : 0) + *(sprite_table+4*i+2);
        x = prevx + *scrollSpriteX;

        prevy = (spriteCode & 0x0200 ? prevy : 0) + *(sprite_table+4*i+3);
        y = prevy + *scrollSpriteY;

        if ((x>167 && x<249) || (y>151 && y<249) || (spriteCode<=0xff) || ((spriteCode & 0x1800)==0))
            continue;

		mySprites[i].x = x;
		mySprites[i].pal = bw ? ((spriteCode>>11)&0x04) : ((sprite_palette_numbers[i]&0xf)<<2);
		mySprites[i].flip = spriteCode>>8;
		tile = (spriteCode & 0x01ff)<<3;

        for (j = 0; j < 8; ++j,++y)
        {
        	if (y>151)
        		continue;
            switch (spriteCode & 0x1800)
            {
                case 0x1800:
                spr = &mySprPriC0.refs[y][mySprPriC0.count[y]++];
                break;
                case 0x1000:
                spr = &mySprPri80.refs[y][mySprPri80.count[y]++];
                break;
                case 0x0800:
                spr = &mySprPri40.refs[y][mySprPri40.count[y]++];
                break;
                default: continue;
            }
            spr->id = i;
            spr->tile = tile + (spriteCode&0x4000 ? 7-j : j);
        }
    }
}

void drawSprites(unsigned short* draw,
				 MYSPRITEREF *sprites,int count,
				 int x0,int x1)
{
	unsigned short*pal;
	unsigned short pattern,pix;
	MYSPRITE *spr;
	int i,j,k,cx,cnt;

	for (i=count-1;i>=0;--i)
	{
		pattern = patterns[sprites[i].tile];
		if (pattern==0)
			continue;
		spr = &mySprites[sprites[i].id];
		pal = &myPalettes[spr->pal];

		if (spr->x>248)
		{
			cx = 0;
			cnt = 8-(256-spr->x);
		}
		else
		{
			cx = spr->x;
			cnt = 8;
		}

		if (cx+cnt<=x0 || cx>=x1)
			continue;

		if (cx<x0)
		{
			j = x0-cx;
			cx += j;
			cnt -= j;
		}

		if (cx+cnt>=x1)
			cnt = x1-cx;
		pattern>>=((8-cnt)<<1);

		if (spr->flip&0x80)
			k = 1;
		else
		{
			cx += cnt-1;
			k = -1;
		}
		while (pattern)
		{
			pix = pattern & 0x3;
			if (pix)
				draw[cx] = pal[pix];
			cx+=k;
			pattern>>=2;
		}
	}
}

void drawScrollPlane(unsigned short* draw,
					 unsigned short* tile_table,int scrpal,
					 unsigned char dx,unsigned char dy,
					 int x0,int x1,int bw)
{
	unsigned short*tiles;
	unsigned short*pal;
	unsigned short pattern;
	int i,j,k,count,pix,idy,tile;

	dx+=x0;
	tiles = tile_table+((dy>>3)<<5)+(dx>>3);

	count = 8-(dx&0x7);
	dx &= 0xf8;
	dy &= 0x7;
	idy = 7-dy;

	i = x0;
	if ((i+count)>=x1)
		count = x1-i;

	while (i<x1)
	{
		tile = *(tiles++);
		pattern = patterns[(((tile&0x1ff))<<3) + (tile&0x4000 ? idy:dy)];
		if (pattern)
		{
			pal = &myPalettes[scrpal + (bw ? (tile&0x2000 ? 4 : 0) : ((tile>>7)&0x3c))];
			pattern>>=((8-count)<<1);
			if (tile&0x8000)
			{
				j = i;
				i += count;
				k = 1;
			}
			else
			{
				i += count;
				j = i-1;
				k = -1;
			}
			while (pattern)
			{
				pix = pattern & 0x3;
				if (pix)
					draw[j] = pal[pix];
				j+=k;
				pattern>>=2;
			}
		}
		else
			i+=count;
		dx+=8;
		if (dx==0)
			tiles-=32;
		count =  (i+8<x1)? 8 : x1-i;
	}
}

void myGraphicsBlitLine(unsigned char render)
{
	int i,j,k,x0,x1;
    if (*scanlineY < 152)
    {
        if(render)
        {
#ifdef __GP32__
			unsigned short* draw = &drawBuffer[*scanlineY*NGPC_SIZEX];
#else
			unsigned short* draw = &drawBuffer[(8+*scanlineY)*SIZEX+8];
#endif
			unsigned short bgcol;
            unsigned int bw = (m_emuInfo.machine == NGP);
            unsigned short OOWCol = NGPC_TO_SDL16(oowTable[*oowSelect & 0x07]);
            unsigned short* pal;
            unsigned short* mempal;

#ifndef __GP32__
            SDL_LockSurface(screen);
#endif

			if (*scanlineY==0)
				sortSprites(bw);
			if (*scanlineY<*wndTopLeftY || *scanlineY>*wndTopLeftY+*wndSizeY || *wndSizeX==0 || *wndSizeY==0)
			{
				for (i=0;i<NGPC_SIZEX;i++)
					draw[i] = OOWCol;
			}
#ifdef __GP32__
			else if (zoom==0 ||( *scanlineY>=zoomy && *scanlineY<zoomy+120))
#else
			else
#endif
			{
#ifdef __GP32__
				if ((turbo==0) || (((*scanlineY)&7)==0))
				{
#endif
		            if (bw)
    		        {
        		        for(i=0;i<4;i++)
            		    {
	                	    myPalettes[i]     = NGPC_TO_SDL16(bwTable[bw_palette_table[i]    & 0x07]);
		                    myPalettes[4+i]   = NGPC_TO_SDL16(bwTable[bw_palette_table[4+i]  & 0x07]);
    		                myPalettes[64+i]  = NGPC_TO_SDL16(bwTable[bw_palette_table[8+i]  & 0x07]);
        		            myPalettes[68+i]  = NGPC_TO_SDL16(bwTable[bw_palette_table[12+i] & 0x07]);
            		        myPalettes[128+i] = NGPC_TO_SDL16(bwTable[bw_palette_table[16+i] & 0x07]);
                		    myPalettes[132+i] = NGPC_TO_SDL16(bwTable[bw_palette_table[20+i] & 0x07]);
		                }
    		        }
        		    else
					{
						pal = myPalettes;
						mempal = palette_table;
						for (i=0;i<192;i++)
							*(pal++) = NGPC_TO_SDL16(*(mempal++));
        		    }
#ifdef __GP32__
				}
#endif

	            if(*bgSelect & 0x80)
    	            bgcol = NGPC_TO_SDL16(bgTable[*bgSelect & 0x07]);
        	    else if(bw)
	                bgcol = NGPC_TO_SDL16(bwTable[0]);
    	        else
        	        bgcol = NGPC_TO_SDL16(bgTable[0]);//maybe 0xFFF?

				x0 = *wndTopLeftX;
				x1 = x0+*wndSizeX;
				if (x1>NGPC_SIZEX)
					x1 = NGPC_SIZEX;

				for (i=x0;i<x1;i++)
					draw[i] = bgcol;

				if (mySprPri40.count[*scanlineY])
					drawSprites(draw,mySprPri40.refs[*scanlineY],mySprPri40.count[*scanlineY],x0,x1);

	            if (*frame1Pri & 0x80)
	            {
        			drawScrollPlane(draw,tile_table_front,64,*scrollFrontX,*scrollFrontY+*scanlineY,x0,x1,bw);
	            	if (mySprPri80.count[*scanlineY])
						drawSprites(draw,mySprPri80.refs[*scanlineY],mySprPri80.count[*scanlineY],x0,x1);
		        	drawScrollPlane(draw,tile_table_back,128,*scrollBackX,*scrollBackY+*scanlineY,x0,x1,bw);
	            }
	            else
	            {
		        	drawScrollPlane(draw,tile_table_back,128,*scrollBackX,*scrollBackY+*scanlineY,x0,x1,bw);
					if (mySprPri80.count[*scanlineY])
						drawSprites(draw,mySprPri80.refs[*scanlineY],mySprPri80.count[*scanlineY],x0,x1);
	    	    	drawScrollPlane(draw,tile_table_front,64,*scrollFrontX,*scrollFrontY+*scanlineY,x0,x1,bw);
	            }

				if (mySprPriC0.count[*scanlineY])
					drawSprites(draw,mySprPriC0.refs[*scanlineY],mySprPriC0.count[*scanlineY],x0,x1);

				for (i=0;i<x0;i++)
					draw[i] = OOWCol;
				for (i=x1;i<NGPC_SIZEX;i++)
					draw[i] = OOWCol;
	        }
#ifndef __GP32__
            SDL_UnlockSurface(screen);
#endif
        }
        if (*scanlineY == 151)
        {
            // start VBlank period
            tlcsMemWriteB(0x00008010,tlcsMemReadB(0x00008010) | 0x40);
#ifndef __GP32__
            if (render)
                graphics_paint();
#endif
        }
        *scanlineY+= 1;
    }
    else if (*scanlineY == 198)
    {
        // stop VBlank period
        tlcsMemWriteB(0x00008010,tlcsMemReadB(0x00008010) & ~0x40);

#ifdef __GP32__
	    if(render)
	    {
	    	unsigned short *buf = getCurrentBuffer();
	    	int dx;
	    	unsigned short pix;
			if (!zoom)
			{
	    		for (i=0,k=0;i<NGPC_SIZEY;i++)
	    		{
	    			dx = OFFSETX*SIZEY+(SIZEY-OFFSETY-i);
		    		for (j=0;j<NGPC_SIZEX;j++,dx+=SIZEY)
		    			buf[dx] = drawBuffer[k++];
	    		}
			}
			else
			{
		    	for (i=0,k=zoomy*NGPC_SIZEX;i<NGPC_SIZEY-32;i++)
		    	{
		    		dx = SIZEY-i*2-2;
			    	for (j=0;j<NGPC_SIZEX;j++)
			    	{
		    			pix = drawBuffer[k++];
		    			buf[dx] = buf[dx+1] = pix;
			    		dx+=SIZEY;
			    		buf[dx] = buf[dx+1] = pix;
			    		dx+=SIZEY;
			    	}
		    	}
	    	}
            graphics_paint();
			flip(0);
		}
#endif

#ifdef DO_FPS_DISPLAY
        incFrameCount();
#endif

        *scanlineY = 0;
    }
    else
        *scanlineY+= 1;
}


//////////////////////////////////////////////////////////////////////////////
//
// General Routines
//
//////////////////////////////////////////////////////////////////////////////

const char ErrDirectDrawCreate[] = "DirectDrawCreate Failed";
const char ErrQueryInterface[]  = "QueryInterface Failed";
const char ErrSetCooperativeLevel[] = "SetCooperativeLevel Failed";
const char ErrCreateSurface[]  = "CreateSurface Failed";

#define DDOK(x) if (hRet != DD_OK) { LastErr = x; return FALSE; }

#ifdef __GP32__
BOOL graphics_init()
#else
BOOL graphics_init(HWND phWnd)
#endif
{
    //put SDL setup stuff here
    //Flavor

#ifndef __GP32__
    dbg_print("in graphics_init\n");


    switch (screen->format->BitsPerPixel)
    {
        case 8:
        palette_init = palette_init8;
        break;
        case 16:
        palette_init = palette_init16;
        break;
        case 32:
        palette_init = palette_init32;
        break;
    }

    //palettes = palette_table;
    drawBuffer = (unsigned short*) screen->pixels + SCREEN_OFFET - BLIT_OFFSET;

    palette_init(screen->format->Rmask,
                 screen->format->Gmask, screen->format->Bmask);

    pngpalette_init();
#else
    palette_init = palette_init16;
    palette_init(0xf800,0x07c0,0x003e);
#endif

    switch(m_emuInfo.machine)
    {
        case NGP:
            bgTable  = (unsigned short *)bwTable;
            oowTable = (unsigned short *)bwTable;
            //set_palette = set_paletteBW;
#ifndef __GP32__
            graphicsBlitInit();
#endif
            *scanlineY = 0;
            break;
        case NGPC:
            //set_palette = set_paletteCol;
#ifndef __GP32__
            graphicsBlitInit();
#endif
            *scanlineY = 0;
            break;
    }
//#if !defined (__GP32__) && !defined (GIZMONDO)
    SDL_Surface *skin = IMG_Load("/cd/image/skin.bmp");
    if(skin)
    {
        SDL_BlitSurface(skin, NULL, screen, NULL);
        SDL_Flip(screen);
    }
//#endif
    return TRUE;
}

void graphics_cleanup()
{}

