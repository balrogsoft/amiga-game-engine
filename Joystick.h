/**********************************************
 *											  *
 *				  AGE v 0.4					  *
 *			  Amiga Game Engine			  	  *
 *			 Copyright 2011-2018 			  *
 *		by Pedro Gil Guirado - Balrog Soft	  *
 *			  www.amigaskool.net			  *
 *											  *
 **********************************************/
 

#ifndef _JOYSTICK_H
#define	_JOYSTICK_H

typedef struct {
	BOOL left;
	BOOL right;
	BOOL up;
	BOOL down;
	BOOL button1;
	BOOL button2;
} JoystickState;
    

BOOL 		   joy_init(void);                              
JoystickState* joy_getState(ULONG joyPort);
void		   joy_dealloc(void);

#endif
