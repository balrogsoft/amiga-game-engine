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

#include <libraries/ptreplay.h>

#include <proto/exec.h>
#include <proto/ptreplay.h>

#include "Music.h"

struct Library* PTReplayBase = NULL;
struct Module *PTMod = NULL;


BOOL music_init(void) 
{
	if (PTReplayBase = OpenLibrary("ptreplay.library", 6))
		return TRUE;
	
	printf("ptreplay.library not found.\n");
	
	return FALSE;
}


void music_dealloc(void)
{
	if (PTReplayBase && PTMod)
	{
		PTStop(PTMod);
		PTUnloadModule(PTMod);
	}
	
	if (PTReplayBase)
		CloseLibrary(PTReplayBase);
}


BOOL mod_load(char* file)
{	
	if (PTReplayBase && PTMod) 
	{
		PTStop(PTMod);
		PTUnloadModule(PTMod);
	}
	if (PTReplayBase && (PTMod = (struct Module*)PTLoadModule(file)))
		return TRUE;
	
	return FALSE;
}

void mod_play(void)
{
	if (PTReplayBase && PTMod)
		PTPlay(PTMod);
}

void mod_stop(void)
{
	if (PTReplayBase && PTMod)
		PTStop(PTMod);
}


void mod_pause(void)
{
	if (PTReplayBase && PTMod)
		PTPause(PTMod);
}

void mod_resume(void)
{
	if (PTReplayBase && PTMod)
		PTResume(PTMod);
}
