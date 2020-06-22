#RACE
#DC porting by Troy Davis(GPF)
# Update Ian micheal fixed render tried to speed it up optimizing sound using know how from dreamHAL
# updated gui saving sound  menu music
AR = ar
AR_FLAGS = cru
RANLIB = ranlib

INC=-I. 

OPT= -DDREAMCAST -DGIZMONDO -DDO_PERIODIC_FLASH_SAVES  -O3 -Wall -ffast-math -funroll-loops -fstrict-aliasing -fexpensive-optimizations -falign-functions -fweb -frename-registers -fomit-frame-pointer -finline -finline-functions -fno-builtin -fno-common



OPT2 = -Ofast -fomit-frame-pointer -fno-ident -freorder-blocks -funsigned-char -funsigned-char -funroll-all-loops -frerun-loop-opt  -falign-loops -fstrict-aliasing

OPT3 = -O2 -DGIZMONDO
OPT4 = -DFRAME_POINTER -g3 -O0 -fno-eliminate-unused-debug-types -DGIZMONDO 
#KOS_CFLAGS += $(INC) $(OPT2)
KOS_CFLAGS = $(INC) -ml -m4-single-only -D_arch_dreamcast -D_arch_sub_pristine  \
$(OPT) \
-I$(KOS_BASE)/../../sh-elf/sh-elf/include -I$(KOS_BASE)/../kos-ports/include -I$(KOS_BASE)/include -I$(KOS_BASE)/kernel/arch/dreamcast/include -I$(KOS_BASE)/addons/include-I$(KOS_BASE)/include -I$(KOS_BASE)/addons/include -I$(KOS_BASE)/../kos-ports/include  -I$(KOS_BASE)/../kos-ports/include/zlib-1.2.3 -I$(KOS_BASE)/../kos-ports/include/libpng-1.2.8 -I$(KOS_BASE)/../kos-ports/SDL-1.2.9/include -I$(KOS_BASE)/../kos-ports/SDL_mixer-1.2.6/mikmod -I$(KOS_BASE)/../kos-ports/SDL_mixer-1.2.6 -I$(KOS_BASE)/../kos-ports/SDL_image-1.2.4 -I$(KOS_BASE)/../kos-ports/zlib-1.2.3 -I$(KOS_BASE)/../kos-ports/include/modplug


KOS_CPPFLAGS+= ${KOS_CFLAGS} 




TARGET = DCRACE.elf

.PHONY: all all-before all-after clean clean-custom

all: all-before $(TARGET) all-after

include dreamcast.mak
 

OBJS = 	graphics.o \
	input.o \
	main.o \
	memory.o \
	neopopsound.o \
	ngpBios.o \
	sound.o \
	tlcs900h.o \
	z80.o \
	flash.o \
	menu.o \
	3dutils.o \
	bkg.o \
		texture.o

include $(KOS_BASE)/Makefile.rules

clean:
	-rm -f $(TARGET) $(OBJS)



$(TARGET): $(OBJS) $(PROBJS) 
	$(KOS_CC) $(KOS_CFLAGS) $(KOS_LDFLAGS) -o $(TARGET) $(KOS_START) \
		$(OBJS)  $(PROBJS)   $(OBJEXTRA) -L$(KOS_BASE)/lib -lgcc -lpng -lz -lm -lmodplug -lSDL_mixer_126 -ltremor -lSDL_image_124 -lpng_128 -ljpeg  -lz_123   -lSDL_129  -lstdc++ $(KOS_LIBS)



          
