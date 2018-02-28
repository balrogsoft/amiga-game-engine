/**********************************************
 *											  *
 *				  RGE v 0.2					  *
 *			  Retro Game Engine			  	  *
 *				Copyright 2011 				  *
 *		by Pedro Gil Guirado - Balrog Soft	  *
 *			  www.amigaskool.net			  *
 *											  *
 **********************************************/
 

#ifndef _C2P_H
#define	_C2P_H
/*
void __asm c2p1x1_6_c5_bm(register __d0 ULONG chunky_x,
						  register __d1 ULONG chunky_y,
						  register __d2 ULONG offset_x,
						  register __d3 ULONG offset_y,
						  register __a0 UBYTE *chunky_buffer,
						  register __a1 struct BitMap *bitmap);
*/
/*extern void asm c2p1x1_6_c5_bm(__reg("d0") ULONG chunkyx,__reg("d1") ULONG chunkyy,__reg("d2") ULONG offsetx,__reg("d3") ULONG offsety,__reg("a0") UBYTE *chunky_buffer,__reg("a1") struct BitMap *bitmap);
*/
#ifdef __VBCC__ 
#define REG(x,y) __reg(#x) y
extern void c2p1x1_6_c5_bm(REG(d0,ULONG),REG(d1,ULONG),REG(d2, ULONG),REG(d3, ULONG),REG(a0, *UBYTE),REG(a1, *BitMap));
#else
void __asm c2p1x1_6_c5_bm(register __d0 ULONG chunky_x,
						  register __d1 ULONG chunky_y,
						  register __d2 ULONG offset_x,
						  register __d3 ULONG offset_y,
						  register __a0 UBYTE *chunky_buffer,
						  register __a1 struct BitMap *bitmap);
#endif

#endif
