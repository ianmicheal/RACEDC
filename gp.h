#ifndef __GP_H
#define __GP_H
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
#include <kos.h>
#include <math.h>
#include <png/png.h>
#include <zlib/zlib.h>
#include "StdAfx.h"

#include "memory.h"
#include "flash.h"
/* Floating-point Sin/Cos; 256 angles, -1.0 to 1.0 */
#define msin(angle) fsin((angle) * 2 * M_PI / 256)
#define mcos(angle) fcos((angle) * 2 * M_PI / 256)

/* bkg.c */

void bkg_render();
void bkg_setup();

/* texture.c */
/* textures */ 
//extern pvr_ptr_t back_tex; 
 extern pvr_ptr_t		util_texture;
extern pvr_poly_hdr_t		util_txr_hdr;
void setup_util_texture();

/* 3dutils.c */
void rotate(int zang, int xang, int yang, float *x, float *y, float *z);
void draw_poly_mouse(int ptrx, int ptry, float alpha);
void draw_poly_char(float x1, float y1, float z1, float a, float r, float g, float b, int c);
void draw_poly_strf(float x1, float y1, float z1, float a, float r, float g, float b, char *fmt, ...);
void draw_poly_box(float x1, float y1, float x2, float y2, float z,
		float a1, float r1, float g1, float b1,
		float a2, float r2, float g2, float b2);

/* songmenu.c */
char* menu_render();

#endif	/* __GP_H */
