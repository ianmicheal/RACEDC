#ifndef _GFX_H_
#define _GFX_H_

#include "gp32.h"

#define COLOR_WHITE			0xffff
#define COLOR_BLACK			0x0000
#define COLOR_RED			0xf800
#define COLOR_GREEN			0x07e0
#define COLOR_BLUE			0x003e
#define COLOR_MAGENTA		0xf83e
#define COLOR_CYAN			0x07fe
#define COLOR_YELLOW		0xffc0
#define COLOR_ORANGE		0xfc00
#define COLOR_VIOLET		COLOR_MAGENTA

#define GPC_PAL_RED_MASK	0xf800
#define GPC_PAL_GREEN_MASK	0x07c0
#define GPC_PAL_BLUE_MASK	0x003e
#define GPC_PALCHAN_RED_BIT		0x4
#define GPC_PALCHAN_GREEN_BIT	0x2
#define GPC_PALCHAN_BLUE_BIT	0x1

#ifdef __cplusplus
extern "C" {
#endif
void initGfx();
void clearScreen();
void flip(u32);
void console_write(char *,u16);
void console_write_no_lineup(char *,u16);
u16* getCurrentBuffer();
void error(char *s);
void drawTextAt(char*,int,int,u16);

#ifdef __cplusplus
}
#endif

#endif
