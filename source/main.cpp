#include <nds.h>
#include <fat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "rom.h"
#include "tilesets.h"
#include "level.h"
#include "levelrendering.h"
#include "ui.h"
#include "control.h"
#include "oamUtil.h"

#define mapxy(x, y) map[x+y*32]



int main(void)  
{
	consoleDemoInit(); 
	iprintf("Helloooooooo World!\n");
	
	BG_PALETTE[0] = 0xEE0E;
	
	lcdMainOnBottom();
	
//	REG_POWERCNT = POWER_ALL_2D;
	REG_DISPCNT = MODE_5_2D | DISPLAY_BG_EXT_PALETTE | DISPLAY_SCREEN_BASE(1);

	vramSetBankC(VRAM_C_MAIN_BG_0x06000000);
	vramSetBankA(VRAM_A_MAIN_SPRITE_0x06400000);

	if (fatInitDefault())
	{
		iprintf("Loading Rom!\n");
		loadROM();
		
    	iprintf("Loading tilesets!\n");
	}
	uiShow();
	loadOAM();
	loadEditor(0);

	bgInit(2, BgType_ExRotation, BgSize_ER_512x512, 0x10, 0);
	bgShow(2);
	bgSetPriority(2, 1);
	REG_BG2CNT |= BG_WRAP_ON;
	bgInit(3, BgType_ExRotation, BgSize_ER_512x512, 0x14, 0);
	bgShow(3);
	bgSetPriority(3, 0);
	REG_BG3CNT |= BG_WRAP_ON;
	bgUpdate();
	iprintf("Done!\n");
	
	touchPosition touch;

	int lastx = 0;
	int lasty = 0;

	while(1)
	{
		checkControls();
		swiWaitForVBlank();
		oamUpdate(&oamMain);
	}
}
