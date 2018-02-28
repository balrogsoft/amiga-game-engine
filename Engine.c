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
#include <math.h>

#include "FrameBuffer.h"
#include "GraphicContext.h"
#include "InputDevice.h"
#include "Bitmap.h"
#include "Sprite.h"
             
#include <libraries/lowlevel.h> 
#include <devices/timer.h>
                          
#include <proto/lowlevel.h> 
#include <exec/memory.h>

#include <proto/exec.h>      
#include <proto/intuition.h>      
#include <proto/timer.h>      

#define WIDTH   320
#define HEIGHT  256

#define DEPTH   6

#define CWIDTH   192
#define CHEIGHT  120

#define MHEIGHT 60
#define MHEIGHT1 59

#define BUFSIZE 23040

#ifndef M_PI
#define M_PI  3.14159265
#endif
#define M_PI2 6.283185

#define TABLE_DEGREES	8100
#define HTABLE_DEGREES	4050
#define HTABLE_DEGREES6 675

#define DEGREE_STEP 900

#define SAMPLES 192
#define SAMPLES3 576

#define FP 14
#define HFP 7
#define FP_UNIT 16384
#define FP_MULT_UNIT 268435456


#include "sinus.h"
#include "cosinus.h"

#include "fp_sinus.h"
#include "fp_cosinus.h"


struct Library *TimerBase;      /* to get at the time comparison functions */

static struct IORequest timereq;


ULONG timer()
{
  static struct timeval tt;
  struct timeval a, b;

  GetSysTime(&a);
  b = a;
  SubTime(&b, &tt);
  tt = a;

  return b.tv_secs*1000 + b.tv_micro/1000;
}


int PRECOMP_DEGREES[SAMPLES];

char map[20][20]=
{
		{4,4,4,4,4,4,4,4,4,2,2,2,2,2,2,2,2,2,2,2},
		{4,0,0,0,0,0,0,0,4,2,0,0,0,0,0,0,0,0,0,2},
		{4,0,4,0,0,0,0,0,4,2,0,0,0,0,0,0,0,0,0,2},
		{4,0,4,0,4,0,4,0,4,2,2,2,0,2,0,4,4,4,0,2},
		{4,0,4,4,4,0,4,0,4,0,0,2,0,2,0,4,0,4,0,2},
		{4,0,4,0,4,0,4,0,4,0,0,1,0,1,0,4,0,4,0,2},
		{2,0,2,0,0,0,0,0,2,0,0,2,0,2,0,4,0,0,0,2},
		{2,0,2,2,2,2,2,2,1,0,0,1,0,1,0,4,0,4,0,2},
		{2,0,0,0,0,0,0,0,0,0,0,2,0,0,0,4,0,4,0,2},
		{2,0,2,2,2,2,2,2,2,0,0,2,0,2,0,4,4,4,0,2},
		{2,0,2,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,2},
		{2,0,0,0,2,2,2,2,2,2,2,2,0,2,2,2,2,2,2,2},
		{1,0,1,2,2,0,0,0,1,0,0,1,0,0,0,0,0,2,2,2},
		{2,0,0,0,0,0,2,0,0,0,0,2,2,2,2,2,0,2,0,2},
		{2,2,2,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,3},
		{5,5,5,0,5,5,5,5,2,0,0,2,3,0,3,0,0,3,0,3},
		{5,0,5,0,0,0,0,5,1,0,0,1,3,0,3,3,3,3,0,3},
		{5,0,5,5,5,5,0,5,2,0,0,2,3,0,0,3,3,0,0,3},
		{5,0,0,0,0,0,0,5,2,0,0,0,0,0,0,0,0,0,0,3},
		{5,5,5,5,5,5,5,5,2,2,2,2,3,3,3,3,3,3,3,3},
};

UBYTE chunky2[BUFSIZE];
UBYTE chunky[BUFSIZE];

UBYTE tex[5][64][64];

			
int player[2]={6<<FP,10<<FP};
int playerDir=0;

int roundAngle[SAMPLES];
int face[SAMPLES];
int fdist[BUFSIZE];
int bit;
int dist;
int maptx;

float playerPosZ=1;
int theta;

int miny=0,maxy=120;

BOOL act_dither = TRUE;
BOOL act_floor = FALSE;
BOOL act_2pixel = FALSE;

BYTE getCollision(int x, int y)
{
	int chk_x, chk_y, i, j;
	for (i = -1; i <= 1; i+=2)
	{
		chk_x = (x+(i<<12))>>FP;
		for (j = -1; j <= 1; j+=2)
		{
			chk_y = (y+(j<<12))>>FP;
			if (map[chk_x][chk_y])
				return 1;
		}
	}
	return 0;
}

void raycasting(void)
{
	int x = player[0];
	int y = player[1];
	int deltaX, deltaY;
	int distX, distY;
	int stepX, stepY;
	int mapX, mapY;
	int mcos, msin;
	int atX = (x>>FP)<<FP;
	int atY = (y>>FP)<<FP;
	int i,n,m;
	int t2 = theta;
	
	int side,d2,d3,d,z,h,start,end;
	int ptr,ptr2,ptr3;
	int tx, ty, tyd, ds, dither, dither2; 
	UBYTE *ctex;
	BYTE inc = 1;
	if (act_2pixel)
		inc = 2;
	for (i=0; i<SAMPLES; i+=inc) {
		t2 = (theta+PRECOMP_DEGREES[i])%TABLE_DEGREES;
		mcos = cosinus[t2];
		msin = sinus[t2];
		
		mapX = atX, mapY = atY;
		
		deltaX=fp_cosinus[t2];
		deltaY=fp_sinus[t2];
		
		if (deltaX>0) {
			stepX = FP_UNIT;
			distX = (((mapX + FP_UNIT - x)>>HFP) * (deltaX>>HFP));
		}
		else {
			stepX = -FP_UNIT;
			distX = (((x - mapX)>>HFP) * ((deltaX*=-1)>>HFP));	
		}
		if (deltaY>0) {
			stepY = FP_UNIT;
			distY = (((mapY + FP_UNIT - y)>>HFP)  * (deltaY>>HFP));
		}
		else {
			stepY = -FP_UNIT;
			distY = (((y - mapY)>>HFP)  * ((deltaY*=-1)>>HFP));
		}
		

		while (TRUE) {
			if (distX < distY) {
				mapX += stepX;
				if (map[mapX>>FP][mapY>>FP]) {
					dist=distX;
					bit=(y+((distX<<HFP)/((deltaY>>HFP)*(stepY>>HFP))<<HFP))%FP_UNIT;
					maptx=map[mapX>>FP][mapY>>FP]-1;
					side=0;
					break;
				}
				distX += deltaX;
			}
			else {
				mapY += stepY;
				if (map[mapX>>FP][mapY>>FP]) {
					dist=distY;
					bit=(x+((distY<<HFP)/((deltaX>>HFP)*(stepX>>HFP))<<HFP))%FP_UNIT;
					maptx=map[mapX>>FP][mapY>>FP]-1;
					side = 1;
					break;
				}
				distY += deltaY;
			}
		}
		
		z = (t2-playerDir);
		if (z < 0)
			z += TABLE_DEGREES;
		z %= TABLE_DEGREES;
		d=(dist>>HFP)*(cosinus[z]>>HFP);
		
		h=(CHEIGHT<<FP)/d;
	
		d2=MHEIGHT-(h>>1);
		d3 = d2+h;		
		if (!act_floor)
		{
			if (d2<miny)
				miny = d2;
				
			if (d3>maxy)
				maxy = d3;
		}
		
		tx = ((bit << 6) >> FP)&63;
		if (side == 0 && stepX > 0)
			tx = 63-tx;
		if (side == 1 && stepY < 0)
			tx = 63-tx;
		if (act_dither)
		{
			ds =(dist>>(FP));
			
			dither  = (7-ds);
			dither2 = (ds-3);
			
			if (dither < 1)
				dither = 1;
			if (dither2 < 1)
				dither2 = 1;
		}	
		h=d3-d2;
		m=d2;
		
		if (d2<0)
			d2=0;
			
		if (d3>CHEIGHT)
			d3=CHEIGHT;
		if (act_floor)
		{
			start=0;
			end = CHEIGHT;
			ptr=i;
			ptr2=i+CWIDTH*(CHEIGHT-1);
		}
		else
		{
			start = d2;
			end = d3;
			ptr=i+d2*CWIDTH;
		}
		
		ctex = tex[maptx][tx];
		ptr3 = 0;
		ty = ((d2-m)<<16)/h;
		tyd = (1<<16)/h;
		for (n = start; n < end; n++)
		{
			int tydi = ty>>10;
			if (!act_floor || (n >= d2 && n <= d3))
			{
				if (act_dither)
				{
					int dtx = (i<<1)+d2+n;
					if ( ds > 2 &&
					   ((ds >= 10) ||
						(dither  >  1 && dtx%dither    == dither -1) ||
						(dither  <= 1 && dtx%(dither2) != dither2-1)))
					{
						chunky[ptr]=ctex[tydi]+32;
						if (act_2pixel)
							chunky[ptr+1]=ctex[tydi]+32;
					}
					else
					{
						chunky[ptr]=ctex[tydi];
						if (act_2pixel)
							chunky[ptr+1]=ctex[tydi];
					}
				}
				else
				{
					chunky[ptr]=ctex[tydi];
					if (act_2pixel)
						chunky[ptr+1]=ctex[tydi];
				}
				ty+=tyd;
			}
			else if (act_floor && n<MHEIGHT)
			{
				int roundx, roundy, xtex, ytex, tx;
				int realDist = fdist[ptr3]/cosinus[z];
			
                int ymap = ((sinus[t2] * realDist) >> FP) + ((player[1]>>7)<<3);
                int xmap = ((cosinus[t2] * realDist) >> FP) + ((player[0]>>7)<<3);
              
				roundx = (xmap >> FP);
                roundy = (ymap >> FP);

                xtex = (((xmap - (roundx << FP))<<10) >> FP)&63;
                ytex = (((ymap - (roundy << FP))<<10) >> FP)&63;
				
				tx=tex[2][xtex][ytex]+32;
				
                chunky[ptr]=tx;
                chunky[ptr2]=tx;
               	if (act_2pixel)
				{
					chunky[ptr+1]=tx;
					chunky[ptr2+1]=tx;
				}
				ptr2-=CWIDTH;
				ptr3+=CWIDTH;
			}
			ptr+=CWIDTH;
		}
	}
}

ULONG mtimer = 0;
int elapsed=0;
WORD fps=0;
WORD fps_val=0;

int main(void)
{
	int ret = RETURN_ERROR;  
	OpenDevice("timer.device", 0, &timereq, 0);
    TimerBase = timereq.io_Device;
	if (bitmap_init())
	{
		InputDevice* id = id_init();
		FrameBuffer *fb = fb_init(320, 256, 6, FB_NOCURSOR|FB_EHBMODE);
		
		UBYTE numstr[16];
		if (fb && id)
		{
			GraphicContext *gc = gc_createWithFrameBuffer(fb);
			int  x = 100, y = 100, i, m = 0;
			
			Bitmap *bm = bm_load("wolftext1.iff");
			GraphicContext *gc_bm = gc_createWithBitmap(bm);
			for (m = 0; m < 5; m++)
			{
				for (y = 0; y < 64; y++)
				{
					for(x = 0; x < 64; x++)
					{
						tex[m][x][y] = gc_getPoint(gc_bm, x+(m<<6), y);
					}
				}
			}
			bm_dealloc(bm);
			gc_dealloc(gc_bm);
	
			for (y = 0; y < CHEIGHT/2; y++) 
			{
				for (x=0; x < CWIDTH; x++)
				{
					int dither = (y/6)+1;
					if (dither < 1)
						dither = 1;
					if (dither > 3)
					{
						chunky2[(MHEIGHT-y)*CWIDTH+x] = 9;
						chunky2[(MHEIGHT+y)*CWIDTH+x] = 9;
					}
					else
					{
						chunky2[(MHEIGHT-y)*CWIDTH+x] = (((x+y)%dither)==dither-1?7:9);
						chunky2[(MHEIGHT+y)*CWIDTH+x] = (((x+y)%dither)==dither-1?7:9);
					}
				}
			}
			
			for (i = 0; i < SAMPLES; i++)
			{
				PRECOMP_DEGREES[i]=((DEGREE_STEP*i)>>7);
			}
			
			fb_setPalette(fb, bm);
			for (y = 0; y < CHEIGHT; y ++)
			{
				if(y!=MHEIGHT)
				{
					int ycast = FP_UNIT*(y-MHEIGHT);
					int distf = (((CHEIGHT<<FP)<<FP) / ycast)<<FP;
					
					theta = playerDir-HTABLE_DEGREES6;
					for (x = 0; x < SAMPLES; x ++)
					{
						int realDist;
						int t2 = (theta+PRECOMP_DEGREES[i])%TABLE_DEGREES;
						
								
						roundAngle[x] = t2%TABLE_DEGREES;
						realDist = (distf*5/10);
						fdist[x+y*SAMPLES] = realDist;

					}
				}
			}
		
			theta=0;

   
			elapsed = timer();
			m=0;
			do
			{ 
				id_getKeys(id);
			
				if (!act_floor)
				{
					if (miny < 0)
						miny = 0;
					if (miny > MHEIGHT1)
						miny = MHEIGHT1;
							
					if (maxy < MHEIGHT) 
						maxy = MHEIGHT;
					if (maxy > CHEIGHT-1)
						maxy = CHEIGHT-1;
						
					if (maxy-miny+1 <= CHEIGHT)
						CopyMemQuick(chunky2+miny*CWIDTH,chunky+miny*CWIDTH,CWIDTH*(maxy-miny+1));

					miny=CHEIGHT-1;
					maxy=0;
				}
				theta = playerDir-HTABLE_DEGREES6;
				if (theta <0)
					theta += TABLE_DEGREES;
				
				theta%=TABLE_DEGREES;
				raycasting();
				fps++;
					
				elapsed = timer();
				
				mtimer+=elapsed;
				if (mtimer>1000)
				{
					fps_val = fps;
					fps = 0;
					mtimer=0;
				}
				
				
				gc_c2p(gc, chunky, 64, 40, CWIDTH, CHEIGHT);
				
				if (id->keys[0x4c] > KEY_UP) 
				{	
					int ox = player[0];
					int oy = player[1];
					int nx = ox+(cosinus[playerDir]*elapsed>>10);
					int ny = oy+(sinus[playerDir]*elapsed>>10);
					
					if (!getCollision(nx,oy))
					{
						player[0] = nx;
					}
					
					if (!getCollision(ox,ny))
					{
						player[1] = ny;
					}
				}
				else if (id->keys[0x4d] > KEY_UP)
				{
					int ox = player[0];
					int oy = player[1];
					int nx = ox-(cosinus[playerDir]*elapsed>>10);
					int ny = oy-(sinus[playerDir]*elapsed>>10);
					
					if (!getCollision(nx,oy))
					{
						player[0] = nx;
					}
					
					if (!getCollision(ox,ny))
					{
						player[1] = ny;
					}
				}
				
				if (id->keys[0x4f] > KEY_UP) {
					playerDir-=elapsed;
					if (playerDir<0)
						playerDir += TABLE_DEGREES;
					
				}
				else if (id->keys[0x4e] > KEY_UP) {
					playerDir+=elapsed;
					playerDir%=TABLE_DEGREES;
				}
				
				if (id->keys[0x50] == KEY_DOWN)
				{
					act_floor ^= TRUE;
					miny=0;
					maxy=119;
				}
				if (id->keys[0x51] == KEY_DOWN)
				{
					act_dither ^= TRUE;
				}
				if (id->keys[0x52] == KEY_DOWN)
				{
					act_2pixel ^= TRUE;
				}
				
				sprintf(numstr, "%d fps", fps_val);
				gc_setColor(gc,11);
				gc_drawText(gc, numstr, 4, 10);
				
				fb_flush(fb,TRUE);
			} while (!id->keys[0x45]);

			gc_dealloc(gc);
			fb_dealloc(fb);
			id_dealloc(id);
		}
		else
			printf("Can't initialize inputdevice.\n");
		ret = RETURN_OK;
	}
	else 
		printf("Can't initialize bitmap.\n");
	bitmap_dealloc();
	
	CloseDevice(&timereq);
	return ret;
}
