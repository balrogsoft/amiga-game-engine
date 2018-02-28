/**********************************************
 *											  *
 *				  AGE v 0.4					  *
 *			  Amiga Game Engine			  	  *
 *			 Copyright 2011-2018 			  *
 *		by Pedro Gil Guirado - Balrog Soft	  *
 *			  www.amigaskool.net			  *
 *											  *
 **********************************************/


#ifndef _MUSIC_H
#define	_MUSIC_H

BOOL music_init(void); 
void music_dealloc(void);

BOOL mod_load(char* file);
void mod_play(void);
void mod_stop(void);
void mod_pause(void);
void mod_resume(void);

#endif
