/*
*   This file is part of NSMB Editor DS
*
*   NSMB Editor DS is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   NSMB Editor DS is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with NSMB Editor DS.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "spritedataeditor.h"
#include "text.h"
#include <nds.h>
#include <cstdlib>
#include <cstdio>
#include "oamUtil.h"

namespace spritedataeditor
{
    touchPosition touch;
    u32 keysNowPressed, keysNowHeld;
    bool lastTouchPress = false;
    int selection = -1;
    u8* ptr;
    u8 origval;
	bool SprDtaLoaded=false;
    string msg1, msg2;

}

using namespace spritedataeditor;
#define xstart 7
#define ystart 11
#define LIST 11
void readSpriteData(int neededspr){
	if (!SprDtaLoaded){
		int spriten;
		int t=0;
		int ch=0;
		int s1=0;
		int s2=0;
		int s3=0;
		bool gsn=false;
		spriten=0;
		FILE* d=fopen("sprdata.txt","rb");
		while(t!=20){
			if (t==0){

				gsn=false;

				while (!gsn){
					ch=fgetc(d);
					if (ch=='s') gsn=true; //continue until we find the character "s"
				}
				if (gsn){
					s1=fgetc(d);
					s1-=48;
					s2=fgetc(d);
					if (s2!='<'){
						s3=fgetc(d);
						s2-=48;
					}
				}
				if (s2=='<'){
					spriten=s1;
				}
				else if (s3=='<'){
					spriten=(s1*10)+s2;
				}
				else if (fgetc(d)!='<'){
					iprintf("There is an error with the spritedata file!\nPlease tell somebody on the JUL forums with a copy of the spritedata file");//No end of sprite
					while(1);
				}
				else spriten=(s1*100)+(s2*10)+(s3-48);//Keep in mind s3 is still in an ASCII form.
				iprintf("%d \n",spriten);
				if (spriten==neededspr){
					t=20;
					iprintf("Stopping Read!\n");
				}
			}
		}
	}
}
void renderSpriteData()
{
    textClearOpaque();
    renderText(0, 0, 32, 1, msg1);
    renderText(0, 1, 32, 0, msg2);
    for(int i = 0; i < 6; i++)
    {
        renderChar(i*3+xstart, ystart-1, 0, (char)30);
        renderChar(i*3+xstart+1, ystart-1, 0, (char)30);
        renderHexChar(i*3+xstart, ystart, selection==i*2?1:0, ptr[i] >> 4);
        renderHexChar(i*3+xstart+1, ystart, selection==i*2+1?1:0, ptr[i] & 0xF);
        renderChar(i*3+xstart, ystart+1, 0, (char)31);
        renderChar(i*3+xstart+1, ystart+1, 0, (char)31);
    }
    renderText(3, 22, 5, 0, "OK");
}

u8 getNibble(int i)
{
    u8 n = ptr[i/2];
    if(i % 2 == 0) return n >> 4;
    else return n & 0xF;
}

void setNibble(int i, u8 nval)
{
    nval &= 0xF;
    
    u8 val = ptr[i/2];
    if(i % 2 == 0)
    {
        val &= 0xF;
        val |= nval << 4;
    }
    else
    {
        val &= 0xF0;
        val |= nval;
    }
    ptr[i/2] = val;
}

void editSpriteData(u8* sptr, string sa, string sb, int spritenum)
{
    ptr = sptr;
    msg1 = sa;
    msg2 = sb;
    
    textScroll(0);
    bool selecting = true;
    iprintf("YAY YAY\n");
    selection = -1;
    readSpriteData(spritenum);
    while(selecting)
    {        
        renderSpriteData();
        scanKeys();
        touchRead(&touch);
        keysNowPressed = keysDown();
        keysNowHeld = keysHeld();

        if(keysNowPressed & KEY_TOUCH)
        {
            int y = touch.py / 8;
            if(y > 21) selecting = false;
            selection = -1;
            if(y > ystart-2 && y < ystart+2)
            {
                int x = touch.px / 8;
                x -= xstart;
                int bx = x / 3;
                if(x % 3 != 2)
                    selection = bx * 2 + x % 3;
            }
            if(selection != -1)
                origval = getNibble(selection);
        }
        else if(keysNowHeld & KEY_TOUCH && selection != -1)
        {
            int y = ystart - touch.py / 8;
            setNibble(selection, origval+y);
        }
        

        lastTouchPress = (keysNowHeld & KEY_TOUCH) != 0;
        oamFrame();
        swiWaitForVBlank();
    }
    textClearTransparent();
}
