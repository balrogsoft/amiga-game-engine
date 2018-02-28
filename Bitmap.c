/**********************************************
 *											  *
 *				  AGE v 0.4					  *
 *			  Amiga Game Engine			  	  *
 *			 Copyright 2011-2018 			  *
 *		by Pedro Gil Guirado - Balrog Soft	  *
 *			  www.amigaskool.net			  *
 *											  *
 **********************************************/
 
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <exec/memory.h>

#include <libraries/iffparse.h>
#include <proto/iffparse.h>

#include <proto/exec.h>
#include <proto/dos.h>

#include "Bitmap.h"

struct Library* IFFParseBase = NULL;

BOOL bitmap_init(void) 
{
	if (IFFParseBase = OpenLibrary("iffparse.library", 23))
		return TRUE;
	
	printf("IFFparse.library not found.\n");	
	return FALSE;
}

void bitmap_dealloc(void) 
{
	if (IFFParseBase)
		CloseLibrary(IFFParseBase);
}


void bm_decodeILBM(struct BitMap* bm, BMHD* bmhdr, BYTE* body)
{
	BYTE *source = body;
	int sindex = 0, dindex;
	WORD x, y, d, height = bmhdr->Height, depth = bmhdr->Depth, 
		 bytesperrow = bm->BytesPerRow, compression = bmhdr->Compression;

	for (y = 0; y < height; y ++)
	{
		for (d = 0; d < depth; d++)
		{
			dindex = 0;
			if (compression == 0)
			{
				for (x = 0; x < bytesperrow; x++) 
				{
					bm->Planes[d][bytesperrow*y+x]=source[sindex];
					sindex++;
				}
			}
			
			else if (compression == 1)
			{
				x = 0;
				
				while (x < bytesperrow)
				{
					BYTE c = source[sindex];
					sindex++;
					if (c >= 0)
					{
						x+=c+1;
						for (;c >= 0; c--){
							bm->Planes[d][bytesperrow*y+dindex]=source[sindex];
						
							dindex++;
							sindex++;
						}
					}
					else
					{
						BYTE data=source[sindex];
						sindex++;
						x-=c-1;
						for(;c<=0; c++){
							bm->Planes[d][bytesperrow*y+dindex]=data;
							dindex++;;
						}
					}
				};
			}
		}
	}
} 




Bitmap* bm_load(char* name)
{
	BPTR file_ptr;
	BMHD bmhdr;
	Bitmap* bm;
	struct IFFHandle *handle;

	if (!IFFParseBase)
		return NULL;
	
	bm = (Bitmap*) malloc(sizeof(Bitmap));
    
	handle = AllocIFF();
	InitIFFasDOS(handle);
	
	if (file_ptr = Open(name, MODE_OLDFILE)) 
	{
		int ret, i, rlen; 
		handle->iff_Stream=file_ptr;
		OpenIFF(handle, IFFF_READ);
		
		ret=ParseIFF(handle, IFFPARSE_RAWSTEP);
			
		while (ret != IFFERR_EOF)
		{
			struct ContextNode* node = CurrentChunk(handle);
			if (!ret&&node&&node->cn_Type==ID_ILBM) {
			
				if (node->cn_ID == ID_BMHD)
				{
					bm->bitmap = AllocVec(sizeof(struct BitMap),MEMF_CLEAR);
					
					rlen = ReadChunkBytes(handle,&bmhdr,node->cn_Size);
					if(rlen <0) {
						printf("iffparse error: %i\n",rlen);
						break;
					}
					
					InitBitMap(bm->bitmap, bmhdr.Depth, bmhdr.Width, bmhdr.Height);
					
					for (i = 0; i < bmhdr.Depth; i++) 
						bm->bitmap->Planes[i] = (PLANEPTR) AllocRaster(bmhdr.Width, bmhdr.Height);
					
				}
				else if (node->cn_ID == ID_CMAP)
				{
				    UBYTE *colortable = (UBYTE*) malloc(node->cn_Size);
					bm->colortable = (UWORD*) malloc(node->cn_Size);
					rlen = ReadChunkBytes(handle, colortable, node->cn_Size);
					if(rlen <0) {
						printf("iffparse error: %i\n",rlen);
						break;
					}
					for (i = 0; i < (1 << bmhdr.Depth); i++)
					{
						bm->colortable[i]  = (colortable[i*3+0]<<4)&0xf00;
						bm->colortable[i] |=  colortable[i*3+1]	   &0x0f0; 
						bm->colortable[i] |= (colortable[i*3+2]>>4)&0x00f;
					}
					bm->colors = 1 << bmhdr.Depth;
					free(colortable);
				}
				else if (node->cn_ID == ID_BODY)
				{
					BYTE* body = (BYTE*)malloc(node->cn_Size);
					rlen = ReadChunkBytes(handle, body, node->cn_Size);
					if(rlen <0) {
						printf("iffparse error: %i\n",rlen);
						break;
					}
					bm_decodeILBM(bm->bitmap, &bmhdr, body);
				}
				
			}
			ret=ParseIFF(handle, IFFPARSE_RAWSTEP);
		};
		
		FreeIFF(handle);
		Close(file_ptr);
				
		bm->rastPort = (struct RastPort*) malloc(sizeof(struct RastPort));
		InitRastPort(bm->rastPort);
		bm->rastPort->BitMap = bm->bitmap;
	  	
	  	bm->width  	  = bmhdr.Width;
		bm->height 	  = bmhdr.Height;
		bm->depth  	  = bmhdr.Depth;
		bm->mask   	  = NULL;	
		bm->rasterOwn = TRUE;
		
		return bm;
	}

	return NULL;
}




void bm_save(Bitmap* bm)
{
	/*IFFL_SaveBitMap("out.iff", bm->bitmap, bm->colortable, 1);*/
}

Bitmap* bm_create(int w, int h, int d)
{       
	int i;
	Bitmap* bm = (Bitmap*) malloc(sizeof(Bitmap));

	bm->width  = w;
	bm->height = h;
	bm->depth  = d;

	bm->mask = NULL;
	bm->colortable = (UWORD*) malloc(sizeof(UWORD)*(2<<d));
	bm->rastPort = NULL;
	
	bm->rasterOwn = TRUE;
	bm->bitmap = AllocVec(sizeof(struct BitMap),MEMF_CLEAR);
					
	InitBitMap(bm->bitmap, d, w, h);
	
	for (i = 0; i < d; i++) 
		bm->bitmap->Planes[i] = (PLANEPTR) AllocRaster(w, h);
	
	if (bm->bitmap) {
		bm->rastPort = (struct RastPort*) malloc(sizeof(struct RastPort));
		InitRastPort(bm->rastPort);
		bm->rastPort->BitMap = bm->bitmap;
		SetRast(bm->rastPort, 0);

		return bm;
	}
	else
		return NULL;
}

Bitmap* bm_createDisp(int w, int h, int d)
{
	int i;
	Bitmap* bm = AllocVec(sizeof(Bitmap),MEMF_CLEAR);

	bm->width  = w;
	bm->height = h;
	bm->depth  = d;

	bm->mask = NULL;
	bm->colortable = NULL;
	bm->rastPort = NULL;
	bm->rasterOwn = TRUE;
	
	bm->bitmap = AllocVec(sizeof(struct BitMap),MEMF_CHIP|MEMF_CLEAR);
	
	for (i = 0; i < d; i++) 
		bm->bitmap->Planes[i] = (PLANEPTR) AllocRaster(w, h);				

	if (bm->bitmap)
		return bm;

	return NULL;
}


void bm_dealloc(Bitmap* bm)
{
	if (bm)
	{
		WaitBlit();
		
		if (bm->bitmap) {
			if (bm->rasterOwn)
			{
				int d;
				for (d = 0; d < bm->depth; d++)
					FreeRaster(bm->bitmap->Planes[d], bm->width, bm->height);
				
				FreeVec(bm->bitmap);
			}
			else
				FreeBitMap(bm->bitmap);
		}
		
		if (bm->mask)
			FreeMem(bm->mask, bm->width*bm->height);
		
		if (bm->colortable)
			free(bm->colortable);
			
		if (bm->rastPort)
			free(bm->rastPort);
		
		free(bm);
	}
}

void bm_createMask(Bitmap* bm, int color) 
{
	int x, y, pen, bit = 0, byte, bit_pos;
	
	if (!bm->mask)
		bm->mask = (unsigned char*)AllocMem(bm->width*bm->height, MEMF_CLEAR|MEMF_CHIP);
	
	for (y = 0; y < bm->height; y++) 
	{
		for (x = 0; x < bm->width; x++)
		{
			pen = ReadPixel(bm->rastPort, x, y);
			if (pen != color)
			{
				byte = bit / 8;
				bit_pos = bit & 7;
				
				bm->mask[byte] ^= 1<<(7-bit_pos);
			}
					
			bit++;
		}
		bit += 15-((bit-1)&15);
	}
}

void bm_savePalette(Bitmap* bm, char* file)
{
	BPTR file_ptr;
	UWORD *palette_buffer = (UWORD*) malloc(sizeof(UWORD)*(bm->colors+1));
    palette_buffer[0] = bm->colors;
	memcpy(&palette_buffer[1], bm->colortable, sizeof(UWORD)*bm->colors);
		
	if (file_ptr = Open(file, MODE_NEWFILE)) 
	{
		Write(file_ptr, palette_buffer, (bm->colors+1)*sizeof(UWORD));	
		Close(file_ptr);
	}
	
	free(palette_buffer);	
	
	free(bm);
}



