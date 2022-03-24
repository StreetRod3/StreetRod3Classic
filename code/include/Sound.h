/////////////////////////////////////////
//
//             Street Rod 3
//
// Copyright (C) Auxiliary Software 2002-2004
//
//
/////////////////////////////////////////
/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/


// Sound library
// Created 2/6/02
// Jason Boettcher


#ifndef __SOUND_H__
#define __SOUND_H__


// Part sounds
typedef struct {
	HSAMPLE		smpRatchet;
    HSAMPLE     smpRemove;
    HSAMPLE     smpPuton;
    HSAMPLE     smpPaint;

} sfxpart_t;


extern	sfxpart_t	sfxParts;


int		LoadSounds(void);

#endif  //  __SOUND_H__