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

#include <libraries/lowlevel.h>

#include <proto/exec.h>
#include <proto/lowlevel.h>

#include "Joystick.h"

struct Library* LowLevelBase = NULL;

JoystickState joyRet;

BOOL joy_init(void) 
{
	joyRet.up 	   = FALSE;
	joyRet.down    = FALSE;
	joyRet.left    = FALSE;
	joyRet.right   = FALSE;
	joyRet.button1 = FALSE;
	joyRet.button2 = FALSE;	
	
	if (LowLevelBase = OpenLibrary("lowlevel.library",1))
		return TRUE;
		
	printf("LowLevel.library not found.\n");	
	return FALSE;
}

JoystickState* joy_getState(ULONG joyPort)
{
	if (LowLevelBase) 
	{
		ULONG joyState = ReadJoyPort(joyPort);
		if ((joyState & JP_TYPE_GAMECTLR) || (joyState & JP_TYPE_JOYSTK))
		{
			joyRet.up      = (joyState & JPF_JOY_UP);
			joyRet.down    = (joyState & JPF_JOY_DOWN);
			joyRet.left    = (joyState & JPF_JOY_LEFT);
			joyRet.right   = (joyState & JPF_JOY_RIGHT);
			joyRet.button1 = (joyState & JPF_BUTTON_RED);
			joyRet.button2 = (joyState & JPF_BUTTON_BLUE);
			return &joyRet;
		}
		else
			return NULL;
	}
	else
		return NULL;
	
}             

void joy_dealloc(void) 
{
	if (LowLevelBase)
		CloseLibrary(LowLevelBase);
}