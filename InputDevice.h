/**********************************************
 *											  *
 *				  AGE v 0.4					  *
 *			  Amiga Game Engine			  	  *
 *			 Copyright 2011-2018 			  *
 *		by Pedro Gil Guirado - Balrog Soft	  *
 *			  www.amigaskool.net			  *
 *											  *
 **********************************************/
 

#ifndef _INPUTDEVICE_H
#define	_INPUTDEVICE_H

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <dos/dos.h>
#include <devices/keyboard.h>

#define KEY_DOWN 2
#define KEY_PRESSED 3
#define KEY_UP 1

typedef struct {
	UBYTE keys[256];
	
	/* Internal variables */
	struct IOStdReq *KeyIO;
	struct MsgPort  *KeyMP;
	UBYTE    *keyMatrix;
} InputDevice;
    

InputDevice*   id_init(void);  
void		   id_getKeys(InputDevice* id);             
void		   id_dealloc(InputDevice *id);

#endif
