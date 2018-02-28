/**********************************************
 *											  *
 *				  AGE v 0.4					  *
 *			  Amiga Game Engine			  	  *
 *			 Copyright 2011-2018 			  *
 *		by Pedro Gil Guirado - Balrog Soft	  *
 *			  www.amigaskool.net			  *
 *											  *
 **********************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <graphics/rastport.h>
#include <cybergraphx/cybergraphics.h> 


#include <proto/graphics.h>
#include <proto/cybergraphics.h>


#include "GraphicContext.h"

#include "c2p/c2p.h"

GraphicContext* gc_createWithFrameBuffer(FrameBuffer *frameBuffer)
{
	GraphicContext* gc = malloc(sizeof(GraphicContext));
	gc->fb = frameBuffer;
	return gc;
}

GraphicContext* gc_createWithBitmap(Bitmap *bm)
{
	GraphicContext* gc = malloc(sizeof(GraphicContext));
	gc->bm = bm;
	gc->fb = malloc(sizeof(FrameBuffer));
	
	gc->fb->width = bm->width;
	gc->fb->height = bm->height;
	gc->fb->depth = bm->depth;
	
	gc->fb->frameOffset = 0;
	
	gc->fb->rastPort = gc->bm->rastPort;
	
	return gc;
}

void gc_dealloc(GraphicContext* gc)
{
	if (gc)
		free(gc);
}

void gc_c2p(GraphicContext* gc, UBYTE* chunky,int x, int y, int width, int height)
{
	int i;
	if (gc->fb->isCGFX)
	{
		WritePixelArray8(gc->fb->rastPort,
						 x,			y+gc->fb->frameOffset,
						 x+width-1, y+height-1+gc->fb->frameOffset,
						 chunky,&gc->fb->tmpRP);
	}
	else
		c2p1x1_6_c5_bm(width, height,
					   x, y+gc->fb->frameOffset,
					   chunky, gc->fb->rastPort->BitMap);		
}

void gc_drawBitmap(GraphicContext* gc, Bitmap *bm, int x, int y, int align)
{
	if (!(bm->bitmap && bm->rastPort))
		return;
		
	if (bm->mask)
		BltMaskBitMapRastPort(bm->bitmap, 0, 0, gc->fb->rastPort,
							  x, y+gc->fb->frameOffset, bm->width, bm->height, ABC|ABNC|ANBC, bm->mask);
	else
		BltBitMapRastPort(bm->bitmap, 0, 0, gc->fb->rastPort,
						  x, y+gc->fb->frameOffset, bm->width, bm->height, 0x00C0);
}

void gc_drawSprite(GraphicContext* gc, Sprite* spr, int x, int y) 
{
	if (!spr->bm)
		return;
		
	y += gc->fb->scrollY[gc->fb->frame];
	
	if (spr->bm->mask)
		BltMaskBitMapRastPort(spr->bm->bitmap, 0, spr->frameOffset, gc->fb->rastPort,
							  x, y+gc->fb->frameOffset, spr->width, spr->height, ABC|ABNC|ANBC, spr->bm->mask);
	else
		BltBitMapRastPort(spr->bm->bitmap, 0, spr->frameOffset, gc->fb->rastPort,
						  x, y+gc->fb->frameOffset, spr->width, spr->height, 0x00C0);
}

void gc_predrawSprite(GraphicContext* gc, Sprite* spr, int x, int y) 
{	
	if (!spr->rest_bm)
		return;
	if (gc->fb->sysVersion <= 38 || (gc->fb->flags & DB_SCROLLVPORT))
	{
		BltBitMapRastPort(&gc->fb->screen->BitMap, x+gc->fb->scrollX[gc->fb->frame], y+gc->fb->scrollY[gc->fb->frame]+gc->fb->frameOffset, spr->rest_bm->rastPort, 
						  0, gc->fb->frame*spr->height, spr->width, spr->height, 0x00C0);
	}
	else if (!gc->fb->isMorphOS && gc->fb->sysVersion > 38)
	{
		BltBitMapRastPort(gc->fb->hardwareScreenBuffer[gc->fb->frame]->sb_BitMap, x+gc->fb->scrollX[gc->fb->frame], y+gc->fb->scrollY[gc->fb->frame]+gc->fb->frameOffset, spr->rest_bm->rastPort, 
						  0, gc->fb->frame*spr->height, spr->width, spr->height, 0x00C0);				  
	}
	else if (gc->fb->isMorphOS)
	{
		BltBitMapRastPort(gc->fb->bitmapBuffer[gc->fb->frame], x+gc->fb->scrollX[gc->fb->frame], y+gc->fb->scrollY[gc->fb->frame]+gc->fb->frameOffset, spr->rest_bm->rastPort, 
						  0, gc->fb->frame*spr->height, spr->width, spr->height, 0x00C0);
	}
	spr->px[gc->fb->frame] = x + gc->fb->scrollX[gc->fb->frame];
	spr->py[gc->fb->frame] = y + gc->fb->scrollY[gc->fb->frame];
}

void gc_restoreSpriteBack(GraphicContext* gc, Sprite* spr)
{
	if (!spr->rest_bm)
		return;
		
	if (spr->restore[gc->fb->frame] == FALSE)
		spr->restore[gc->fb->frame] = TRUE;
	else 
	{
		BltBitMapRastPort(spr->rest_bm->bitmap, 0, (gc->fb->frame*spr->height), gc->fb->rastPort, 
					      spr->px[gc->fb->frame], spr->py[gc->fb->frame]+gc->fb->frameOffset, spr->width, spr->height, 0x00C0);
	}		
}

int gc_getPoint(GraphicContext* gc, int x, int y) 
{
	return (int)ReadPixel(gc->fb->rastPort, x, y);
}

void gc_drawPoint(GraphicContext* gc, int x, int y)
{
	WritePixel(gc->fb->rastPort, x, y);
}

void gc_drawLine(GraphicContext* gc, int x1, int y1, int x2, int y2)
{
	Move(gc->fb->rastPort, x1, y1+gc->fb->frameOffset);
	Draw(gc->fb->rastPort, x2, y2+gc->fb->frameOffset);
}

void gc_fillRect(GraphicContext* gc, int x, int y, int width, int height)
{
	RectFill(gc->fb->rastPort, x, y+gc->fb->frameOffset, x+width, y+gc->fb->frameOffset+height);
}

void gc_setColor(GraphicContext* gc, char pen)
{
	SetAPen(gc->fb->rastPort, pen);
}

void gc_setBackColor(GraphicContext* gc, char pen)
{
	SetBPen(gc->fb->rastPort, pen);
}

void gc_setOutlineColor(GraphicContext* gc, char pen)
{
	SetOutlinePen(gc->fb->rastPort, pen);
}

void gc_setDrawingMode(GraphicContext* gc, char mode)
{
	SetDrMd(gc->fb->rastPort, mode);	
}

void gc_drawText(GraphicContext* gc, char* str, int x, int y)
{
	Move(gc->fb->rastPort, x, y+gc->fb->frameOffset);
	Text(gc->fb->rastPort, str, strlen(str));
}

int gc_getTextWidth(GraphicContext* gc, char *str)
{
	return TextLength(gc->fb->rastPort, str, strlen(str));
}