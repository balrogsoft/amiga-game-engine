/**********************************************
 *											  *
 *				  AGE v 0.4  				  *
 *			  Amiga Game Engine			  	  *
 *			 Copyright 2011-2018 		 	  *
 *		by Pedro Gil Guirado - Balrog Soft	  *
 *			  www.amigaskool.net			  *
 *											  *
 **********************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "FrameBuffer.h"
#include "GraphicContext.h"
#include "InputDevice.h"
#include "Bitmap.h"
#include "Music.h"
#include "Sprite.h"

                   
#define WIDTH   320
#define HEIGHT  256*2+64
#define DEPTH   5
#define TILE	16
#define MAX_MAP (57*20)

int main(int argc, char *argv[])
{
	int ret = RETURN_ERROR;  
	Bitmap* gamebm[10];
	int tile[2];
	BYTE music = music_init();
	if (bitmap_init())
	{ 
		FrameBuffer* fb = fb_initExtended(WIDTH, HEIGHT, 320, 256, DEPTH, FB_SCROLL|FB_NOCURSOR);
		if (fb)
		{
			InputDevice* id = id_init();
			ULONG signals = 0;
			GraphicContext* gc = gc_createWithFrameBuffer(fb);
			char* map = (char*)malloc(1200);
			int x = 100, y = 100, fr = 0, key = 0, i, j, 
				sprx = 40, spry = 60;
			Sprite *spr, *spr2;
			char* file = "Gfx/Back_ ";
			BPTR file_ptr = 0;

			for (i = 0; i < 10; i++) { 
				file[9] = 48+i;
				gamebm[i] = bm_load(file);
			}
			
			if (file_ptr = Open("Gfx/DefaultMap", MODE_OLDFILE)) 
			{
				Read(file_ptr, map, 1200);
				
				Close(file_ptr);
			}
				
			fb_setPalette(fb, gamebm[0]);
			
			if (music)
			{
				mod_load("mod.stardust");
				mod_play();
			}
			
			fb->frameOffset = 0;
			
			i = 0;
			
			for (j = 0; j < 2; j++)
			{
				i = 0;
				for (y = HEIGHT-16; y >= 256; y -= TILE) 
				{
					for (x = 0; x < WIDTH-1; x+= TILE) 
					{
						gc_drawBitmap(gc, gamebm[map[i]], x, y, 0);
						i++;
					}
				}
				fb_flush(fb, FALSE);
			}
			tile[0] = tile[1] = 17*20;
			 
			fb->frameOffset = fr;
			
			fb->scrollY[0] = fb->scrollY[1] = 256+48;
			
			spr = sp_loadSheet("Gfx/PlayerShip", 23);
			
			spr2 = sp_loadSheet("Gfx/PlayerShip", 23);
			
			do
			{  
				int off;
			
				id_getKeys(id);
 
				sp_setFrame(spr, 0);
					
				if (id->keys[0x4c]) 
					spry --;
				else if (id->keys[0x4d])
					spry ++;
				
				if (id->keys[0x4f]) {
					sp_setFrame(spr, 2);
					sprx --;
				}
				else if (id->keys[0x4e]) {
					sp_setFrame(spr, 1);
					sprx ++;
				}
				
				if (sprx < 0)
					sprx = 0;
				else if (sprx > 320-27)
					sprx = 320-27;
					
				if (spry < 0)
					spry = 0;
				else if (spry > fb->screenHeight-23)
					spry = fb->screenHeight-23;
				
				//ScreenPosition(fb->screen,SPOS_ABSOLUTE,fb->scrollX[fb->frame],0,0,0);
					
				//MoveScreen(fb->screen,-1,0);
				//fb->scrollX[fb->frame]++;
				fb->scrollY[fb->frame]--;
			
				off = (2*(fb->scrollY[fb->frame]))%20;

				fr = fb->frameOffset;
				
				fb->frameOffset = 0;
				
				if (fb->scrollY[fb->frame]+256+32 < HEIGHT - 16) {
					gc_drawBitmap(gc, gamebm[map[20+(tile[fb->frame]+off)%MAX_MAP]], off*gamebm[0]->width, 256+32+(fb->scrollY[fb->frame]>>4)*16, 0);
					gc_drawBitmap(gc, gamebm[map[20+(tile[fb->frame]+1+off)%MAX_MAP]], (1+off)*gamebm[0]->width, 256+32+(fb->scrollY[fb->frame]>>4)*16, 0);
					if (fb->sysVersion <=38 || (fb->flags & DB_SCROLLVPORT))
					{
						gc_drawBitmap(gc, gamebm[map[20+(tile[fb->frame]+off)%MAX_MAP]], off*gamebm[0]->width, 256+32+(fb->scrollY[fb->frame]>>4)*16+HEIGHT, 0);
						gc_drawBitmap(gc, gamebm[map[20+(tile[fb->frame]+1+off)%MAX_MAP]], (1+off)*gamebm[0]->width, 256+32+(fb->scrollY[fb->frame]>>4)*16+HEIGHT, 0);
					}
				}
				
				if (fb->scrollY[fb->frame] >= 16) {
					gc_drawBitmap(gc, gamebm[map[20+(tile[fb->frame]+off)%MAX_MAP]], (off%20)*gamebm[0]->width, -16+(fb->scrollY[fb->frame]>>4)*16, 0);
					gc_drawBitmap(gc, gamebm[map[20+(tile[fb->frame]+1+off)%MAX_MAP]], ((1+off)%20)*gamebm[0]->width, -16+ (fb->scrollY[fb->frame]>>4)*16, 0);
					if (fb->sysVersion <=38 || (fb->flags & DB_SCROLLVPORT))
					{
						gc_drawBitmap(gc, gamebm[map[20+(tile[fb->frame]+off)%MAX_MAP]], (off%20)*gamebm[0]->width, -16+(fb->scrollY[fb->frame]>>4)*16+HEIGHT, 0);
						gc_drawBitmap(gc, gamebm[map[20+(tile[fb->frame]+1+off)%MAX_MAP]], ((1+off)%20)*gamebm[0]->width, -16+(fb->scrollY[fb->frame]>>4)*16+HEIGHT, 0);
					}
				}
				fb->frameOffset = fr;
				
				gc_restoreSpriteBack(gc, spr);
				gc_restoreSpriteBack(gc, spr2);
			
				if ((fb->scrollY[fb->frame]&15) == 0) {
					tile[fb->frame] += 20;
				}

				if (fb->scrollY[fb->frame] <= 0)
				{
					fb->scrollY[fb->frame] = 256+48;
					spr->py[fb->frame] += 256+48;
				}
				
				gc_predrawSprite(gc, spr2, 100, 100);
				gc_predrawSprite(gc, spr, sprx, spry);
				
				gc_drawSprite(gc, spr2, 100, 100);	
				gc_drawSprite(gc, spr, sprx, spry);

				fb_flush(fb,TRUE);
				
			} while (!id->keys[0x45]); 
			
			sp_dealloc(spr);
			
			sp_dealloc(spr2);
			
			if (music)
				mod_stop();
			
			free(map);
			
			for (i = 0; i < 10; i++) 
				bm_dealloc(gamebm[i]);
				
			gc_dealloc(gc);
			id_dealloc(id);
			fb_dealloc(fb);
		}
		else
			printf("Can't initialize framebuffer.\n");

		ret = RETURN_OK;
	}
	else
		printf("Can't initialize bitmap.\n");
	bitmap_dealloc();
	music_dealloc();
	
	return ret;
}
