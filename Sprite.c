/**********************************************
 *											  *
 *				  RGE v 0.3					  *
 *			  Retro Game Engine			  	  *
 *			 Copyright 2011-2014 			  *
 *		by Pedro Gil Guirado - Balrog Soft	  *
 *			  www.amigaskool.net			  *
 *											  *
 **********************************************/
 

#include <stdio.h>
#include <stdlib.h>

#include <proto/exec.h>

#include "Sprite.h"

#include "Bitmap.h"

Sprite* sp_load(char* file) 
{
	Sprite* spr = (Sprite*)malloc(sizeof(Sprite));
	
	spr->bm = bm_load(file);
	spr->rest_bm = NULL;
	
	bm_createMask(spr->bm, 0);
	
	spr->px[0] = spr->px[1] = 0;
	spr->py[0] = spr->py[1] = 0;
	
	spr->restore[0] = spr->restore[1] = FALSE;

	spr->width 	= spr->bm->width;
	spr->height = spr->bm->height;
	spr->depth  = spr->bm->depth;
	spr->sheight = spr->bm->height;
	spr->frameOffset = 0;
	spr->frame = 0;
	
	spr->rest_bm = bm_create(spr->width, spr->height<<1, spr->depth);

	return spr;
}


Sprite* sp_loadSheet(char* file, int spriteHeight) 
{
	Sprite* spr = (Sprite*)malloc(sizeof(Sprite));
	
	spr->bm = bm_load(file);
	spr->rest_bm = NULL;
	
	bm_createMask(spr->bm, 0);
	
	spr->px[0] = spr->px[1] = 0;
	spr->py[0] = spr->py[1] = 0;
	
	spr->restore[0] = spr->restore[1] = FALSE;

	spr->width 	= spr->bm->width;
	spr->height = spriteHeight;
	spr->depth  = spr->bm->depth;
	spr->sheight = spr->bm->height;
	spr->frameOffset = 0;
	spr->frame = 0;

	spr->rest_bm = bm_create(spr->width, spr->height<<1, spr->depth);

	return spr;
}

void sp_setFrame(Sprite* spr, int frame)
{
	if (frame * spr->height + spr->height > spr->sheight)
		return;
		
	spr->frame = frame;
	spr->frameOffset = frame * spr->height;
}

void sp_dealloc(Sprite* spr) 
{
	if (spr)
	{
		if (spr->bm)
			bm_dealloc(spr->bm);
			
		if (spr->rest_bm)
			bm_dealloc(spr->rest_bm);
			
		free(spr);
	}
}