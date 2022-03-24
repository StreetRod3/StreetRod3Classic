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


#include "defs.h"
#include "game.h"


sfxpart_t	sfxParts;


///////////////////
// Load the sounds
int LoadSounds(void)
{
	sfxParts.smpRatchet = Cache_LoadSample("data/sounds/rachet.wav",4);
    sfxParts.smpPaint = Cache_LoadSample("data/sounds/paint.wav",1);
    sfxParts.smpRemove = Cache_LoadSample("data/sounds/remove.wav",1);
    sfxParts.smpPuton = Cache_LoadSample("data/sounds/puton.wav",1);


	return true;
}