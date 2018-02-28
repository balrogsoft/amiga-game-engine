/**********************************************
 *											  *
 *				  AGE v 0.4 				  *
 *			  Amiga Game Engine			  	  *
 *			 Copyright 2011-2018 			  *
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
#include "Sprite.h"
                   
#define WIDTH   128
#define HEIGHT  64
#define DEPTH   6

int main(void)
{
	int ret = RETURN_ERROR;  
	
	if (bitmap_init())
	{ 
		Bitmap *bm = bm_create(WIDTH,HEIGHT,DEPTH);
		InputDevice* id = id_init();
		FrameBuffer *fb = fb_init(320, 256, 6);
		if (fb && id)
		{
			GraphicContext *gc = gc_createWithFrameBuffer(fb);
			GraphicContext *bm_gc = gc_createWithBitmap(bm);
			int x = 100, y = 100, fr = 0, key = 0, i,  m = 0;
			int cwidth = fb->width, cheight = fb->height;

			UBYTE *chunky = (UBYTE*)malloc(WIDTH*HEIGHT);

			for (y = 0; y < HEIGHT; y++) 
			{
				for (x=0; x < WIDTH; x++)
				{
					chunky[y*WIDTH+x] = (x-y)&63;
				}
			}
			m=0;
			do
			{ 
				id_getKeys(id);
				
				gc_c2p(bm_gc, chunky, 0, 0, WIDTH, HEIGHT);
				
				gc_c2p(gc, chunky, 112, 130, WIDTH, HEIGHT);
				
				gc_drawBitmap(gc, bm, 10, 10, 0);
				
				gc_setColor(gc, 11);
				
				gc_fillRect(gc, 20+(m%64),20, 10, 10);
				m++;

				gc_setDrawingMode(gc, DMODE_TRANS);
				gc_drawText(gc, "hola", 20, 20);
				fb_flush(fb);
				
			} while (!id->keys[45]); 
			
			free(chunky);
			gc_dealloc(gc);
			fb_dealloc(fb);
			bm_dealloc(bm);
			id_dealloc(id);
		}
		else
			printf("Can't initialize inputdevice.\n");
		ret = RETURN_OK;
	}
	else 
		printf("Can't initialize bitmap.\n");
	bitmap_dealloc();
	
	return ret;
}
