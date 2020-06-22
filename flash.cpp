//---------------------------------------------------------------------------
//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version. See also the license.txt file for
//	additional informations.
//---------------------------------------------------------------------------

//
// Flash chip emulation by Flavor
//   with ideas from Koyote (who originally got ideas from Flavor :)
// for emulation of NGPC carts
//
// NeoPop save-game support from NeoPop
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

#include "StdAfx.h"

#include "memory.h"
#include "flash.h"
#include <string.h>

#include <unistd.h>


/* Manuf ID's
Supported
0x98		Toshiba
0xEC		Samsung
0xB0		Sharp

Other
0x89		Intel
0x01		AMD
0xBF		SST
*/
unsigned char manufID = 0x98;   //we're always Toshiba!
unsigned char deviceID = 0x2F;
unsigned char cartSize = 32;
unsigned long bootBlockStartAddr = 0x1F0000;
unsigned char bootBlockStartNum = 31;

//with selector, I get
//writeSaveGameFile: Couldn't open Battery//mnt/sd/Games/race/ChryMast.ngf file

#define SAVEGAME_DIR "/vmu/a1/"

unsigned char currentWriteCycle = 1;  //can be 1 through 6
unsigned char currentCommand = NO_COMMAND;

#define FLASH_VALID_ID  0x0053

typedef struct NGFheaderStruct
{
	unsigned short version;		//always 0x53?
	unsigned short numBlocks;	//how many blocks are in the file
	unsigned long fileLen;		//length of the file

} ;

typedef struct blockStruct
{
	unsigned long NGPCaddr;  //where this block starts (in NGPC memory map)
	unsigned long len;  // length of following data
} ;

#define MAX_BLOCKS 34 //a 16m chip has 34 blocks
unsigned char blocksDirty[2][MAX_BLOCKS];  //max of 2 chips
char ngfFilename[200] = {0};


void setupFlashParams()
{
    switch(cartSize)
    {
        default:
        case 32:
	        deviceID = 0x2F;  //the upper chip will always be 16bit
            bootBlockStartAddr = 0x1F0000;
            bootBlockStartNum = 31;
            break;
        case 16:
			deviceID = 0x2F;
            bootBlockStartAddr = 0x1F0000;
            bootBlockStartNum = 31;
            break;
        case 8:
	        deviceID = 0x2C;
            bootBlockStartAddr = 0xF0000;
            bootBlockStartNum = 15;
            break;
        case 4:
	        deviceID = 0xAB;
            bootBlockStartAddr = 0x70000;
            bootBlockStartNum = 7;
            break;
        case 0:
	        manufID = 0x00;
	        deviceID = 0x00;
            bootBlockStartAddr = 0x00000;
            bootBlockStartNum = 0;
            break;
    }
}

unsigned char blockNumFromAddr(unsigned long addr)
{
    addr &= 0x1FFFFF;

    if(addr >= bootBlockStartAddr)
    {
        unsigned long bootAddr = addr-bootBlockStartAddr;
        //boot block is 32k, 8k, 8k, 16k (0x8000,0x2000,0x2000,0x4000)
        if(bootAddr < 0x8000)
            return (bootBlockStartAddr / 0x10000);
        else if(bootAddr < 0xA000)
            return (bootBlockStartAddr / 0x10000) + 1;
        else if(bootAddr < 0xC000)
            return (bootBlockStartAddr / 0x10000) + 2;
        else if(bootAddr < 0x10000)
            return (bootBlockStartAddr / 0x10000) + 3;
    }

    return addr / 0x10000;
}

unsigned long blockNumToAddr(unsigned char chip, unsigned char blockNum)
{
    unsigned long addr;

    if(blockNum >= bootBlockStartNum)
    {
        addr = bootBlockStartNum * 0x10000;

        unsigned char bootBlock = blockNum - bootBlockStartNum;
        if(bootBlock>=1)
            addr+= 0x8000;
        if(bootBlock>=2)
            addr+= 0x2000;
        if(bootBlock>=3)
            addr+= 0x2000;
    }
    else
        addr = blockNum * 0x10000;

    if(chip)
        addr+=0x200000;

	return addr;
}

unsigned long blockSize(unsigned char blockNum)
{
    if(blockNum >= bootBlockStartNum)
    {
        unsigned char bootBlock = blockNum - bootBlockStartNum;
        if(bootBlock==3)
            return 0x4000;
        if(bootBlock==2)
            return 0x2000;
        if(bootBlock==1)
            return 0x2000;
        if(bootBlock==0)
            return 0x8000;
    }


    return 0x10000;
}

void setupNGFfilename()
{
	int dotSpot = -1, pos = 0;
	int slashSpot = -1;

    if(strlen(ngfFilename) != 0)
        return;  //already set up


    strcpy(ngfFilename, SAVEGAME_DIR);

    pos = strlen(m_emuInfo.RomFileName);

    while(pos>=0)
    {
#ifdef __GP32__
        if(m_emuInfo.RomFileName[pos] == '\\')
#else
        if(m_emuInfo.RomFileName[pos] == '/')
#endif
        {
            slashSpot = pos;
            break;
        }

        pos--;
    }

	strcat(ngfFilename, &m_emuInfo.RomFileName[slashSpot+1]);
	fprintf(stdout, "setupNGFfilename: using %s for save-game info\n", ngfFilename);


	for(pos=0;pos<strlen(ngfFilename) && dotSpot == -1; pos++)
	{
		if(ngfFilename[pos] == '.')
			dotSpot = pos;
	}
	if(dotSpot == -1)
	{
		fprintf(stderr, "setupNGFfilename: Couldn't find the . in %s file\n", ngfFilename);
		return;
	}

	strcpy(&ngfFilename[dotSpot+1], "ngf");
}

//write all the dirty blocks out to a file
void writeSaveGameFile()
{
	//find the dirty blocks and write them to the .NGF file
	int totalBlocks = bootBlockStartNum+4;
	int i;
	FILE *ngfFile;

	int bytes;
	NGFheaderStruct NGFheader;
	blockStruct block;

    setupNGFfilename();


	ngfFile = fopen(ngfFilename, "wb");
	if(!ngfFile)
	{
		fprintf(stderr, "writeSaveGameFile: Couldn't open %s file\n", ngfFilename);
		return;
	}


	NGFheader.version = 0x53;
	NGFheader.numBlocks = 0;
	NGFheader.fileLen = sizeof(NGFheaderStruct);
	//add them all up, first
	for(i=0;i<totalBlocks;i++)
	{
		if(blocksDirty[0][i])
		{
			NGFheader.numBlocks++;
			NGFheader.fileLen += blockSize(i);
		}
	}

	if(cartSize == 32)  //do the second chip, also
	{
		for(i=0;i<totalBlocks;i++)
		{
			if(blocksDirty[1][i])
			{
				NGFheader.numBlocks++;
				NGFheader.fileLen += blockSize(i);
			}
		}
	}

	NGFheader.fileLen += NGFheader.numBlocks * sizeof(blockStruct);

	bytes = fwrite(&NGFheader, 1, sizeof(NGFheaderStruct), ngfFile);
	if(bytes != sizeof(NGFheaderStruct))
	{
		fprintf(stderr, "writeSaveGameFile: wrote %d bytes, but exptected %d bytes\n", bytes, sizeof(NGFheaderStruct));
		fclose(ngfFile);
		return;
	}


	for(i=0;i<totalBlocks;i++)
	{
		if(blocksDirty[0][i])
		{

			block.NGPCaddr = blockNumToAddr(0, i)+0x200000;
			block.len = blockSize(i);


			bytes = fwrite(&block, 1, sizeof(blockStruct), ngfFile);
			if(bytes != sizeof(blockStruct))
			{
				fprintf(stderr, "writeSaveGameFile: wrote %d bytes, but exptected %d bytes\n", bytes, sizeof(blockStruct));
				fclose(ngfFile);
				return;
			}

			bytes = fwrite(&mainrom[blockNumToAddr(0, i)], 1, blockSize(i), ngfFile);
			if(bytes != blockSize(i))
			{
				fprintf(stderr, "writeSaveGameFile: wrote %d bytes, but exptected %d bytes\n", bytes, blockSize(i));
				fclose(ngfFile);
				return;
			}
		}
	}

	if(cartSize == 32)  //do the second chip, also
	{
		for(i=0;i<totalBlocks;i++)
		{
			if(blocksDirty[1][i])
			{
				block.NGPCaddr = blockNumToAddr(1, i)+0x800000;
				block.len = blockSize(i);


				bytes = fwrite(&block, 1, sizeof(blockStruct), ngfFile);
				if(bytes != sizeof(blockStruct))
				{
					fprintf(stderr, "writeSaveGameFile: wrote %d bytes, but exptected %d bytes\n", bytes, sizeof(blockStruct));
					fclose(ngfFile);
					return;
				}

				bytes = fwrite(&mainrom[blockNumToAddr(1, i)], 1, blockSize(i), ngfFile);
				if(bytes != blockSize(i))
				{
					fprintf(stderr, "writeSaveGameFile: wrote %d bytes, but exptected %d bytes\n", bytes, blockSize(i));
					fclose(ngfFile);
					return;
				}
			}
		}
	}

	fclose(ngfFile);
#ifndef __GP32__
#ifdef TARGET_GP2X
	sync();
	//system("sync");
#endif
#endif
}

//read the save-game file and overlay it onto mainrom
void loadSaveGameFile()
{
	//find the NGF file and read it in
	FILE *ngfFile;
	int bytes, i;
	unsigned char *blocks;
	void *blockMem;
	NGFheaderStruct NGFheader;
	blockStruct *blockHeader;

    setupNGFfilename();

	ngfFile = fopen(ngfFilename, "rb");
	if(!ngfFile)
	{
		fprintf(stderr, "loadSaveGameFile: Couldn't open %s file\n", ngfFilename);
		return;
	}

	bytes = fread(&NGFheader, 1, sizeof(NGFheaderStruct), ngfFile);

	if(bytes != sizeof(NGFheaderStruct))
	{
		fprintf(stderr, "loadSaveGameFile: Bad NGF file %s\n", ngfFilename);
		fclose(ngfFile);
		return;
	}


	/*
	unsigned short version;		//always 0x53?
	unsigned short numBlocks;	//how many blocks are in the file
	unsigned long fileLen;		//length of the file
	*/

	if(NGFheader.version != 0x53)
	{
		fprintf(stderr, "loadSaveGameFile: Bad NGF file version %s 0x%X\n", ngfFilename, NGFheader.version);
		fclose(ngfFile);
		return;
	}

    blockMem = malloc(NGFheader.fileLen - sizeof(NGFheaderStruct));
    //error handling?
    if(!blockMem)
    {
		fprintf(stderr, "loadSaveGameFile: can't malloc %d bytes\n", (NGFheader.fileLen - sizeof(NGFheaderStruct)));
        return;
    }


	blocks = (unsigned char *)blockMem;

	bytes = fread(blocks, 1, NGFheader.fileLen - sizeof(NGFheaderStruct), ngfFile);
	fclose(ngfFile);

	if(bytes != (NGFheader.fileLen - sizeof(NGFheaderStruct)))
	{
		fprintf(stderr, "loadSaveGameFile: read %d bytes, but exptected %d bytes\n", bytes, (NGFheader.fileLen - sizeof(NGFheaderStruct)));
		free(blockMem);
		return;
	}


	//loop through the blocks and insert them into mainrom
	for(i=0; i < NGFheader.numBlocks; i++)
	{
	    blockHeader = (blockStruct*) blocks;
		blocks += sizeof(blockStruct);

		if(blockHeader->NGPCaddr >= 0x800000)
		{
			blockHeader->NGPCaddr -= 0x600000;
            blocksDirty[1][blockNumFromAddr(blockHeader->NGPCaddr-0x200000)] = 1;
		}
		else if(blockHeader->NGPCaddr >= 0x200000)
		{
			blockHeader->NGPCaddr -= 0x200000;
            blocksDirty[0][blockNumFromAddr(blockHeader->NGPCaddr)] = 1;
		}

		memcpy(&mainrom[blockHeader->NGPCaddr], blocks, blockHeader->len);

		blocks += blockHeader->len;
	}

	free(blockMem);
}

void flashWriteByte(unsigned long addr, unsigned char data)
{
	//set a dirty flag for the block that we are writing to
	if(addr < 0x200000)
		blocksDirty[0][blockNumFromAddr(addr)] = 1;
	else if(addr < 0x400000)
		blocksDirty[1][blockNumFromAddr(addr)] = 1;
	else
		return;  //panic

	mainrom[addr] = data;
}

unsigned char flashReadInfo(unsigned long addr)
{
    currentWriteCycle = 1;
    currentCommand = COMMAND_INFO_READ;

    switch(addr&0x03)
    {
        case 0:
            return manufID;
        case 1:
            return deviceID;
        case 2:
            return 0;  //block not protected
        case 3:  //thanks Koyote
		default:
            return 0x80;
    }
}

void flashChipWrite(unsigned long addr, unsigned char data)
{
    if(addr >= 0x800000 && cartSize != 32)
        return;

    switch(currentWriteCycle)
    {
        case 1:
            if((addr & 0xFFFF) == 0x5555 && data == 0xAA)
                currentWriteCycle++;
            else if(data == 0xF0)
			{
                currentWriteCycle=1;//this is a reset command
				writeSaveGameFile();
			}
            else
                currentWriteCycle=1;

            currentCommand = NO_COMMAND;
            break;
        case 2:
            if((addr & 0xFFFF) == 0x2AAA && data == 0x55)
                currentWriteCycle++;
            else
                currentWriteCycle=1;

            currentCommand = NO_COMMAND;
            break;
        case 3:
            if((addr & 0xFFFF) == 0x5555 && data == 0x80)
                currentWriteCycle++;//continue on
            else if((addr & 0xFFFF) == 0x5555 && data == 0xF0)
			{
                currentWriteCycle=1;
				writeSaveGameFile();
			}
            else if((addr & 0xFFFF) == 0x5555 && data == 0x90)
            {
                currentWriteCycle++;
                currentCommand = COMMAND_INFO_READ;
                //now, the next time we read from flash, we should return a ID value
                //  or a block protect value
                break;
            }
            else if((addr & 0xFFFF) == 0x5555 && data == 0xA0)
            {
                currentWriteCycle++;
                currentCommand = COMMAND_BYTE_PROGRAM;
                break;
            }
            else
                currentWriteCycle=1;

            currentCommand = NO_COMMAND;
            break;

        case 4:
            if(currentCommand == COMMAND_BYTE_PROGRAM)//time to write to flash memory
            {
				if(addr >= 0x200000 && addr < 0x400000)
					addr -= 0x200000;
				else if(addr >= 0x800000 && addr < 0xA00000)
					addr -= 0x600000;

				//should be changed to just write to mainrom
				flashWriteByte(addr, data);

                currentWriteCycle=1;
            }
            else if((addr & 0xFFFF) == 0x5555 && data == 0xAA)
                currentWriteCycle++;
            else
                currentWriteCycle=1;

            currentCommand = NO_COMMAND;
            break;
        case 5:
            if((addr & 0xFFFF) == 0x2AAA && data == 0x55)
                currentWriteCycle++;
            else
                currentWriteCycle=1;

            currentCommand = NO_COMMAND;
            break;
        case 6:
            if((addr & 0xFFFF) == 0x5555 && data == 0x10)//chip erase
            {
                currentWriteCycle=1;
                currentCommand = COMMAND_CHIP_ERASE;

                //erase the entire chip
                //memset it to all 0xFF
                //I think we won't implement this

                break;
            }
            if(data == 0x30 || data == 0x50)//block erase
            {
                unsigned char chip=0;
                currentWriteCycle=1;
                currentCommand = COMMAND_BLOCK_ERASE;

                //erase the entire block that contains addr
                //memset it to all 0xFF

                if(addr >= 0x800000)
                    chip = 1;

                vectFlashErase(chip, blockNumFromAddr(addr));
                break;
            }
            else
                currentWriteCycle=1;

            currentCommand = NO_COMMAND;
            break;


        default:
            currentWriteCycle = 1;
            currentCommand = NO_COMMAND;
            break;
    }
}

//this should be called when a ROM is unloaded
void flashShutdown()
{
	writeSaveGameFile();
}

//this should be called when a ROM is loaded
void flashStartup()
{
	memset(blocksDirty[0], 0, MAX_BLOCKS*sizeof(blocksDirty[0][0]));
	memset(blocksDirty[1], 0, MAX_BLOCKS*sizeof(blocksDirty[0][0]));

	loadSaveGameFile();
}

void vectFlashWrite(unsigned char chip, unsigned int to, unsigned char *fromAddr, unsigned int numBytes)
{
    if(chip)
        to+=0x200000;

    //memcpy(dest,fromAddr,numBytes);
	while(numBytes--)
	{
		flashWriteByte(to, *fromAddr);
		fromAddr++;
		to++;
	}
}

void vectFlashErase(unsigned char chip, unsigned char blockNum)
{
    //this needs to be modified to take into account boot block areas (less than 64k)
    unsigned long blockAddr = blockNumToAddr(chip, blockNum);
	unsigned long numBytes = blockSize(blockNum);

    //memset block to 0xFF
    //memset(&mainrom[blockAddr], 0xFF, numBytes);
	while(numBytes--)
	{
		flashWriteByte(blockAddr, 0xFF);
		blockAddr++;
	}
}

void setFlashSize(unsigned long romSize)
{
    //add individual hacks here.
    if(strncmp((const char *)&mainrom[0x24], "DELTA WARP ", 11)==0)//delta warp
    {
        //1 8mbit chip
        cartSize = 4;
    }
    else if(romSize > 0x200000)
    {
        //2 16mbit chips
        cartSize = 32;
    }
    else if(romSize > 0x100000)
    {
        //1 16mbit chip
        cartSize = 16;
    }
    else if(romSize > 0x080000)
    {
        //1 8mbit chip
        cartSize = 8;
    }
    else if(romSize > 0x040000)
    {
        //1 4mbit chip
        cartSize = 4;
    }
    else if(romSize == 0)  //no cart, just emu BIOS
    {
        cartSize = 0;
    }
    else
    {
        //we don't know.  It's probablly a homebrew or something cut down
        // so just pretend we're a Bung! cart
        //2 16mbit chips
        cartSize = 32;
    }

    setupFlashParams();


	flashStartup();
}

