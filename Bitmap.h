/**********************************************
 *											  *
 *				  AGE v 0.4					  *
 *			  Amiga Game Engine			  	  *
 *			 Copyright 2011-2018 			  *
 *		by Pedro Gil Guirado - Balrog Soft	  *
 *			  www.amigaskool.net			  *
 *											  *
 **********************************************/
 

#ifndef _BITMAP_H
#define	_BITMAP_H

#define ID_ILBM	MAKE_ID('I','L','B','M')
#define ID_BMHD	MAKE_ID('B','M','H','D')
#define ID_CMAP	MAKE_ID('C','M','A','P')
#define ID_BODY	MAKE_ID('B','O','D','Y')

#include <proto/graphics.h>
#include <graphics/rastport.h>                            

typedef struct {
	WORD Width, Height;
	LONG Reserved1;
	UBYTE Depth,Reserved2,Compression,Reserved3;
	WORD Transparency;
	UBYTE pixelratiox, pixelratioy;
	WORD ScreenWidth,ScreenHeight;
}BMHD;

typedef struct {
	unsigned char *mask;
	int width;
	int height;
	int depth;
	int colors;
	BOOL rasterOwn;
	USHORT *colortable;
	struct BitMap *bitmap;
	struct RastPort *rastPort;
} Bitmap;

BOOL	bitmap_init(void);
void	bitmap_dealloc(void);

void	bm_decodeILBM(struct BitMap* bm, BMHD* bmhdr, BYTE* body);
Bitmap* bm_load(char* name);
void	bm_save(Bitmap* bm);
Bitmap* bm_create(int w, int h, int d);
Bitmap* bm_createDisp(int w, int h, int d);
void 	bm_createMask(Bitmap* bm, int color);
void	bm_savePalette(Bitmap* bm, char* file);
void 	bm_dealloc(Bitmap* bm);         

#endif
