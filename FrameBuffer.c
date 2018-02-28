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

#include <intuition/screens.h>

#include <libraries/asl.h>

#include <cybergraphx/cybergraphics.h> 


#include <exec/memory.h>

#include <proto/asl.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/cybergraphics.h>
#include <graphics/videocontrol.h>	
#include <graphics/gfxbase.h>	

#include "FrameBuffer.h"


extern struct ExecBase *SysBase;


UWORD __chip EmptyPointer[] = {0, 0, 0, 0};

static struct Rectangle rect;

struct Library *AslBase = NULL;
struct Library *CyberGfxBase = NULL;

   


struct TagItem VCTags[] =
{
	{ VTAG_BORDERBLANK_SET, TRUE },
	{ VC_IntermediateCLUpdate, FALSE },	
	{ TAG_DONE, 0 },
};


FrameBuffer* fb_initExtended(int width, int height, int screenWidth, int screenHeight, int depth, int flags)
{
    FrameBuffer* fb = (FrameBuffer*) malloc(sizeof(FrameBuffer));
 
	struct ScreenModeRequester *screenReq;

	ULONG modeId = INVALID_ID, modeAdd=0;
	ULONG i, cwidth, cheight;

	struct DimensionInfo dimsinfo;
	void *handle;
	
	fb->flags = flags;
	
	if (CyberGfxBase)
		fb->isCGFX = IsCyberModeID(modeId);
		
	fb->scrollX[0] = fb->scrollX[1] = 0;
	fb->scrollY[0] = fb->scrollY[1] = 0;
	
	fb->width  = width;
	fb->height = height;
	fb->screenWidth = screenWidth;
	fb->screenHeight = screenHeight;
	fb->depth  = depth;
	
	fb->isCGFX = FALSE;
	
	if ((GfxBase->ChipRevBits0 & GFXF_AA_LISA)  || 
		(GfxBase->ChipRevBits0 & GFXF_AA_ALICE) || 
		(GfxBase->ChipRevBits0 & GFXF_AA_MLISA))
		fb->isAGA = TRUE;
	else
		fb->isAGA = FALSE;
	
	fb->screen = NULL;
	fb->window = NULL;
	fb->colortable = NULL;
	
	fb->sysVersion = SysBase->LibNode.lib_Version;

	if (fb->flags & FB_SYS36)
		fb->sysVersion = 36;

	if (fb->flags & FB_SYS38)
		fb->sysVersion = 38;

	if (!(fb->flags & FB_SCROLL) || 
		 (fb->flags & DB_SCROLLVPORT))
		fb->isMorphOS = FALSE;
	else
		fb->isMorphOS = FindResident("MorphOS") != NULL;
	
	if (CyberGfxBase = OpenLibrary("cybergraphics.library",0))
		fb->isCGFX = TRUE;

	if (fb->sysVersion > 38)
	{
		modeId = BestModeID(BIDTAG_NominalWidth, screenWidth,
							BIDTAG_NominalHeight, screenHeight,
							BIDTAG_Depth, depth,
							(!CyberGfxBase && (fb->flags & FB_EHBMODE)?BIDTAG_DIPFMustHave:TAG_IGNORE), DIPF_IS_EXTRAHALFBRITE,
							TAG_END);
							

			if (modeId == INVALID_ID || (CyberGfxBase && modeId != INVALID_ID && !IsCyberModeID(modeId)))
			{	
			
				AslBase = OpenLibrary("asl.library",38);
				if (AslBase)
				{
					
					screenReq = (struct ScreenModeRequester*) AllocAslRequest(ASL_ScreenModeRequest, NULL);
				
					if (!AslRequestTags(screenReq, 
									TAG_DONE))
									return NULL;
					modeId = screenReq->sm_DisplayID;
					FreeAslRequest(screenReq);         
					
					CloseLibrary(AslBase);
				}
			}
	}
	else
	{
		if (width == 320)
			modeId = PAL_MONITOR_ID|LORES_KEY;
		else if (width == 640)
			modeId = PAL_MONITOR_ID|HIRES_KEY;
		
		if (!fb->isCGFX && !fb->isAGA && fb->depth == 6)
			modeAdd = EXTRAHALFBRITE_KEY;
	}
	
	handle = FindDisplayInfo(modeId);
	GetDisplayInfoData(handle, (UBYTE *)&dimsinfo,
							   sizeof(struct DimensionInfo), DTAG_DIMS,
								0);
	
	if (fb->sysVersion <= 38 || (fb->flags & DB_SCROLLVPORT))
	{
		fb->oscanHeight = dimsinfo.MaxOScan.MaxY - dimsinfo.MaxOScan.MinY + 1;

		if (fb->oscanHeight < fb->height)
			fb->oscanHeight = fb->height;
	}
	

	cwidth = fb->width;
	
	if (fb->sysVersion <= 38 || (fb->flags & DB_SCROLLVPORT))
		cheight = fb->oscanHeight<<1;
	else
		cheight = fb->height;
		
	if (fb->isMorphOS)
	{
		cwidth = screenWidth;
		cheight = screenHeight;
	}
	
	fb->screenWidth  = dimsinfo.Nominal.MaxX - dimsinfo.Nominal.MinX + 1;
	fb->screenHeight = dimsinfo.Nominal.MaxY - dimsinfo.Nominal.MinY + 1;
	
	fb->screen = OpenScreenTags(NULL,
					   SA_DisplayID, modeId|modeAdd,
					   SA_Left,		 0,
					   SA_Top,		 0,
					   SA_Width,     cwidth,
					   SA_Height,    cheight,
					   SA_Depth,     fb->depth,
					   (fb->sysVersion<=38 ||  (fb->flags & DB_SCROLLVPORT)?SA_Type:TAG_IGNORE), CUSTOMSCREEN,
					   (fb->sysVersion> 38 && !(fb->flags & DB_SCROLLVPORT)?SA_Overscan:TAG_IGNORE), OSCAN_TEXT,
					   (fb->sysVersion> 38 && !(fb->flags & DB_SCROLLVPORT)?SA_Interleaved:TAG_IGNORE), TRUE,
					   (fb->sysVersion> 38 && !(fb->flags & DB_SCROLLVPORT)?SA_MinimizeISG:TAG_IGNORE), TRUE,	
					   (fb->sysVersion> 38 && !(fb->flags & DB_SCROLLVPORT)?SA_VideoControl:TAG_IGNORE), &VCTags,
					   SA_Quiet,	 TRUE,
					   SA_Draggable, FALSE,
					   SA_Exclusive, TRUE,
					   SA_AutoScroll,FALSE,
					   SA_ShowTitle, FALSE,
					   TAG_DONE);

	if (fb->screen) 
	{
	
		fb->window = OpenWindowTags(NULL,
									WA_CustomScreen, fb->screen,
									WA_Title, NULL,
									WA_Flags, WFLG_BORDERLESS | WFLG_REPORTMOUSE | WFLG_ACTIVATE,
									WA_IDCMP, NULL,
									TAG_DONE);

		if (!fb->window)
		{
			CloseScreen(fb->screen);
	
			if (CyberGfxBase)
				CloseLibrary(CyberGfxBase);
			
			free(fb);
			
			return NULL;
		}
		
		if ((fb->flags & FB_NOCURSOR) == FB_NOCURSOR)
			SetPointer(fb->window, EmptyPointer, 1L, 1L, 0L, 0L);
			
		if (fb->sysVersion <= 38 || (fb->flags & DB_SCROLLVPORT))
		{
			fb->rastPort 	= &fb->screen->RastPort;
			fb->frame 		= 0;
			fb->frameOffset = 0;
			
			fb->tmpBM = AllocVec(sizeof(struct BitMap),MEMF_CLEAR);
			InitBitMap(fb->tmpBM,1,fb->width,fb->height<<1);
			fb->tmpBM->Planes[0] = (PLANEPTR) AllocRaster(fb->width,fb->height<<1);
			
			CopyMem(&fb->rastPort,&fb->tmpRP,sizeof(struct RastPort));

			fb->tmpRP.Layer = NULL;
			fb->tmpRP.BitMap = fb->tmpBM;
		}
		else if (fb->sysVersion > 38)
		{
			ULONG rassize = RASSIZE(cwidth,cheight);
			fb->dispPort = CreateMsgPort();
			fb->safePort = CreateMsgPort();

			for (i = 0; i < 2; i++) {
				fb->hardwareScreenBuffer[i] =  AllocScreenBuffer(fb->screen, NULL, (i==0?SB_SCREEN_BITMAP:0));
				fb->hardwareScreenBuffer[i]->sb_DBufInfo->dbi_DispMessage.mn_ReplyPort = fb->dispPort;
				fb->hardwareScreenBuffer[i]->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort = fb->safePort;
				InitRastPort(&fb->rport[i]);
				fb->rport[i].BitMap = fb->hardwareScreenBuffer[i]->sb_BitMap;

				if (fb->tmpbuf[i] = AllocVec(rassize,MEMF_CHIP|MEMF_CLEAR))
				{
					InitTmpRas (&fb->tmpras[i],fb->tmpbuf[i],rassize);
					fb->rport[i].TmpRas = &fb->tmpras[i];
				}

				fb->tempbm[i] = AllocBitMap(cwidth,cheight,1,0,NULL);	

				CopyMem(&fb->rport[i],&fb->temprp[i],sizeof(struct RastPort));

				fb->temprp[i].Layer = NULL;
				fb->temprp[i].BitMap = fb->tempbm[i];
				
				if (fb->isMorphOS)
				{
					fb->bitmapBuffer[i] = AllocBitMap(fb->width,fb->height,fb->depth,BMF_INTERLEAVED|BMF_CLEAR,NULL);
					InitRastPort(&fb->rportbm[i]);
					fb->rportbm[i].BitMap = fb->bitmapBuffer[i];

				}
			}
			fb->safeToWrite = TRUE;
			fb->safeToChange = TRUE;
			fb->frame = 1;
			fb->frameOffset = 0;
			
			fb->tmpBM = fb->tempbm[fb->frame];
			fb->tmpRP = fb->temprp[fb->frame];
			
			if (fb->isMorphOS)
				fb->rastPort = &fb->rportbm[fb->frame];
			else
				fb->rastPort = &fb->rport[fb->frame];
		}

		return fb;
	}
	else 
		return NULL;
	
}

FrameBuffer* fb_init(int width, int height, int depth, int flags)
{
	return fb_initExtended(width, height, width, height, depth, flags);
}

void fb_dealloc(FrameBuffer* fb)
{   
	int i;
	if (fb)
	{
		if (fb->sysVersion <= 38 || (fb->flags & DB_SCROLLVPORT))
		{
			if (fb->tmpBM)
			{
				WaitBlit();
				FreeRaster(fb->tmpBM->Planes[0],fb->width,fb->height<<1);
				FreeVec(fb->tmpBM);
			}
		}
		else if (fb->sysVersion > 38)
		{
			if (!fb->safeToWrite) {
				while(!GetMsg(fb->safePort)) {
					Wait(1l<<(fb->safePort->mp_SigBit));
				}
			}
			DeleteMsgPort(fb->safePort);
		   
			
			if (!fb->safeToChange) {
				while(!GetMsg(fb->dispPort)) {
					Wait(1l<<(fb->dispPort->mp_SigBit));
				}
			}
			DeleteMsgPort(fb->dispPort);

			for (i = 0; i < 2; i++)
			{
				WaitBlit();
				FreeScreenBuffer(fb->screen, fb->hardwareScreenBuffer[i]);
				fb->rport[i].TmpRas = NULL;
				FreeVec(fb->tmpbuf[i]);
				FreeBitMap(fb->tempbm[i]);
				if (fb->isMorphOS)
					FreeBitMap(fb->bitmapBuffer[i]);
			}
		}
		if (fb->window)
			CloseWindow(fb->window);
			
		if (fb->screen)
			CloseScreen(fb->screen);
		
		if (CyberGfxBase)
			CloseLibrary(CyberGfxBase);
			
		free(fb);
	}
}

void fb_flush(FrameBuffer* fb, BOOL sync)
{    
	if (fb->sysVersion <= 38 || (fb->flags & DB_SCROLLVPORT))
	{
		fb->screen->ViewPort.RasInfo->RxOffset = fb->scrollX[fb->frame];
		fb->screen->ViewPort.RasInfo->RyOffset = fb->frame * fb->oscanHeight + fb->scrollY[fb->frame];
	
		fb->frame ^= 1;
		fb->frameOffset = fb->frame*fb->oscanHeight;
	
		ScrollVPort(&fb->screen->ViewPort);
	}
	else if (fb->sysVersion > 38)
	{
		int vpx = fb->screen->ViewPort.RasInfo->RxOffset;
		int vpy = fb->screen->ViewPort.RasInfo->RyOffset;
		
		if (fb->isMorphOS)
		{
			BltBitMapRastPort(fb->bitmapBuffer[fb->frame], fb->scrollX[fb->frame], fb->scrollY[fb->frame], &fb->rport[fb->frame],
							  0, 0, fb->screenWidth, fb->screenHeight, 0x00C0);
		}
			
		if (!fb->safeToChange) {
			while (!GetMsg(fb->dispPort)) {
				Wait(1l<<(fb->dispPort->mp_SigBit));
			}
			fb->safeToChange = TRUE;
		}
		
		if (!fb->isMorphOS &&
			(vpx != fb->scrollX[fb->frame] ||
			 vpy != fb->scrollY[fb->frame]))
		{
			fb->screen->ViewPort.RasInfo->RxOffset = fb->scrollX[fb->frame];
			fb->screen->ViewPort.RasInfo->RyOffset = fb->scrollY[fb->frame];
		
			ScrollVPort(&fb->screen->ViewPort);
		}
		
		ChangeScreenBuffer(fb->screen, fb->hardwareScreenBuffer[fb->frame]);

		if (!fb->safeToWrite) {
			while (!GetMsg(fb->safePort)) {
				Wait(1l<<(fb->safePort->mp_SigBit));
			}
			fb->safeToWrite = TRUE;
		}
		
		fb->frame ^= 1;
		
		fb->safeToWrite = FALSE;
		fb->safeToChange = FALSE;
		
		fb->tmpBM = fb->tempbm[fb->frame];
		fb->tmpRP = fb->temprp[fb->frame];
		
		if (fb->isMorphOS)
			fb->rastPort = &fb->rportbm[fb->frame];
		else
			fb->rastPort = &fb->rport[fb->frame];
	}

	if (sync)
		WaitTOF();
}

void fb_setPalette(FrameBuffer* fb, Bitmap *bm)
{
	LoadRGB4(&fb->screen->ViewPort, bm->colortable, bm->colors);
}

void fb_loadPalette(FrameBuffer* fb, char* file)
{
	BPTR file_ptr;
	
	if (file_ptr = Open(file, MODE_OLDFILE)) 
	{
		Read(file_ptr, &fb->colors, sizeof(UWORD));

		if (fb->colortable)
			free(fb->colortable);
			
		fb->colortable = (USHORT*)malloc(sizeof(UWORD)*fb->colors);
		Read(file_ptr, fb->colortable, fb->colors*sizeof(UWORD));
	
		LoadRGB4(&fb->screen->ViewPort, fb->colortable, fb->colors);
		
		Close(file_ptr);
	}
}
