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


#include <exec/interrupts.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include <proto/dos.h>

#include "InputDevice.h"


#define MATRIX_SIZE 16L


InputDevice* id_init(void) 
{	
	
	InputDevice* id = (InputDevice*) malloc(sizeof(InputDevice));
	id->KeyMP=CreateMsgPort();
	if (id->KeyMP)
	{
		if (id->KeyIO=(struct IOStdReq *)CreateExtIO(id->KeyMP,sizeof(struct IOStdReq)))
		{
			if (!OpenDevice("keyboard.device",0,(struct IORequest *)id->KeyIO,0))
			{
				if (id->keyMatrix=AllocMem(MATRIX_SIZE,MEMF_PUBLIC|MEMF_CLEAR))
				{
					int i;
					for (i = 0; i < 256; i++)
						id->keys[i] = 0;
				
					return id;
				}
				CloseDevice((struct IORequest *)id->KeyIO);
			}
			DeleteExtIO((struct IORequest *)id->KeyIO);
		}
		DeletePort(id->KeyMP);
	}
	
	return NULL;
}

void id_getKeys(InputDevice* id)
{
	SHORT  bitcount;
	SHORT  bytecount;
	SHORT   mask;
	USHORT twobyte;

	id->KeyIO->io_Command=KBD_READMATRIX;
	id->KeyIO->io_Data=(APTR)id->keyMatrix;
	id->KeyIO->io_Length = MATRIX_SIZE;
	DoIO((struct IORequest *)id->KeyIO);

	for (bitcount=0;bitcount<16;bitcount++)
    {
		mask=1 << bitcount;
		for (bytecount=0;bytecount<16;bytecount+=2)
        {
			UBYTE key = (bytecount<<3)+bitcount;
			twobyte=id->keyMatrix[bytecount] | (id->keyMatrix[bytecount+1] << 8);
			if (twobyte & mask) {
				if (id->keys[key] == 0)
					id->keys[key] = 2;
				else if (id->keys[key] == 2)
					id->keys[key] = 3;
			}
			else
				if (id->keys[key] == 1)
					id->keys[key] = 0;
				else if (id->keys[key] > 0)
					id->keys[key] = 1;
				

        }
    }
}

void id_dealloc(InputDevice *id)
{
	if (id)
	{
    	FreeMem(id->keyMatrix,MATRIX_SIZE);
    	
		CloseDevice((struct IORequest *)id->KeyIO);
    	DeleteExtIO((struct IORequest *)id->KeyIO);
    	
    	DeleteMsgPort(id->KeyMP);
		free(id);
	}
}