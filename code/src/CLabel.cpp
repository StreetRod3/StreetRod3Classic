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


// gui Label class
// Created 10/9/02
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "gui.h"


///////////////////
// Draw the label
void CLabel::Draw(void)
{
	if(!sText[0])
		return;

    Font_SetAlpha(1);
	Font_SetSize(nSize);
    if( iCenter ) {
        Font_Draw( iX - (strlen(sText)*nSize)/2+2, iY-2, CVec(0,0,0), sText);
        Font_Draw( iX - (strlen(sText)*nSize)/2, iY, CVec(1,1,1), sText);
    } else {
        Font_Draw(iX+2, iY-2, CVec(0,0,0), sText);
	    Font_Draw(iX, iY, CVec(1,1,1), sText);
    }
}