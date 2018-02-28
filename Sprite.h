/**********************************************
 *											  *
 *				  AGE v 0.4					  *
 *			  Amiga Game Engine			  	  *
 *			 Copyright 2011-2018 			  *
 *		by Pedro Gil Guirado - Balrog Soft	  *
 *			  www.amigaskool.net			  *
 *											  *
 **********************************************/
 

#ifndef _SPRITE_H
#define	_SPRITE_H

#include <proto/graphics.h>
#include <graphics/sprite.h>

#include "Bitmap.h"

typedef struct {
	Bitmap* bm;
	Bitmap* rest_bm;
		
	int px[2];
	int py[2];
	
	BOOL restore[2];
	
	int width;
	int height;
	int depth;
	int sheight;
	int frameOffset;
	int frame;
	
	int x;
	int y;
} Sprite;

Sprite* sp_load(char* file);  
Sprite* sp_loadSheet(char* file, int spriteHeight);
void sp_setFrame(Sprite* spr, int frame);
void sp_dealloc(Sprite* spr);

#endif
