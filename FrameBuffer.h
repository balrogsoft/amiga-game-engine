/**********************************************
 *											  *
 *				  AGE v 0.4					  *
 *			  Amiga Game Engine			  	  *
 *			 Copyright 2011-2018 			  *
 *		by Pedro Gil Guirado - Balrog Soft	  *
 *			  www.amigaskool.net			  *
 *											  *
 **********************************************/


#ifndef _FRAMEBUFFER_H
#define	_FRAMEBUFFER_H

#include <graphics/rastport.h>
#include <intuition/screens.h>

#include <intuition/intuition.h> 
#include <graphics/gfxbase.h>

#include <exec/execbase.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include "Bitmap.h"

#define DB_SCROLLVPORT  1
#define FB_SCROLL		2
#define FB_NOCURSOR		4
#define FB_EHBMODE		8
#define FB_SYS36		16
#define FB_SYS38		32


typedef struct {
	int width;
    int height;
    int screenWidth;
    int screenHeight;
	int depth;
	int frame;  
	USHORT *colortable;
	USHORT colors;
	int scrollX[2];
	int scrollY[2];
	BOOL isCGFX;
	BOOL isAGA;
	BOOL isMorphOS;
	int frameOffset;
    int oscanHeight;
    int flags;

	/* Internal variables */
	
	/* SysBase >= 38 */
	struct ScreenBuffer *hardwareScreenBuffer[2];
	struct BitMap *tempbm[2];
	struct RastPort rport[ 2 ];
	struct RastPort temprp[2];
	UBYTE *tmpbuf[2];
	struct MsgPort *dispPort;
	struct MsgPort *safePort;
	struct TmpRas tmpras[2];
	BOOL safeToWrite;
	BOOL safeToChange;
	
	/* MorphOS */
	struct BitMap *bitmapBuffer[2];
	struct RastPort rportbm[2];
	
	/* Common */
	LONG sysVersion;
	struct Screen   *screen;
	struct Window	*window;
	struct RastPort *rastPort;
	struct RasInfo  *rastInfo;
	struct BitMap 	*tmpBM;
	struct RastPort tmpRP;
} FrameBuffer;

FrameBuffer* fb_initExtended(int width, int height, int screenWidth, int screenHeight, int depth, int flags);
FrameBuffer* fb_init(int width, int height, int depth, int flags);
void   		 fb_dealloc(FrameBuffer* fb);

void 		 fb_flush(FrameBuffer* fb, BOOL sync);
void 		 fb_setPalette(FrameBuffer* fb, Bitmap *bm);
void		 fb_loadPalette(FrameBuffer* fb, char* file);

#endif