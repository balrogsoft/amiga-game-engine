/**********************************************
 *											  *
 *				  AGE v 0.4					  *
 *			  Amiga Game Engine			  	  *
 *			 Copyright 2011-2018 			  *
 *		by Pedro Gil Guirado - Balrog Soft	  *
 *			  www.amigaskool.net			  *
 *											  *
 **********************************************/


#ifndef _GRAPHICS_H
#define	_GRAPHICS_H

#include <proto/dos.h>

#include "FrameBuffer.h"
#include "Bitmap.h"
#include "Sprite.h"

#define DMODE_NORMAL	JAM2
#define DMODE_TRANS		JAM1
#define DMODE_INVERT	INVERSVID
#define DMODE_NEG		COMPLEMENT

typedef struct {
	FrameBuffer* fb;
	Bitmap* bm;
} GraphicContext;

GraphicContext* gc_createWithFrameBuffer(FrameBuffer* fb);
GraphicContext* gc_createWithBitmap(Bitmap* bm);
void gc_dealloc(GraphicContext* gc);

void gc_c2p(GraphicContext* gc, UBYTE* chunky,int x, int y, int width, int height);
void gc_drawBitmap(GraphicContext* gc, Bitmap* bm, int x, int y, int align);
void gc_drawSprite(GraphicContext* gc, Sprite* spr, int x, int y);        
void gc_predrawSprite(GraphicContext* gc, Sprite* spr, int x, int y);
void gc_restoreSpriteBack(GraphicContext* gc, Sprite* spr);
int  gc_getPoint(GraphicContext* gc, int x, int y);
void gc_drawPoint(GraphicContext* gc, int x, int y);
void gc_drawLine(GraphicContext* gc, int x1, int y1, int x2, int y2);
void gc_fillRect(GraphicContext* gc, int x, int y, int width, int height);
void gc_setColor(GraphicContext* gc, char pen);
void gc_setBackColor(GraphicContext* gc, char pen);
void gc_setOutlineColor(GraphicContext* gc, char pen);
void gc_setDrawingMode(GraphicContext* gc, char mode);
void gc_drawText(GraphicContext* gc, char *str, int x, int y);
int  gc_getTextWidth(GraphicContext* gc, char *str);

#endif
