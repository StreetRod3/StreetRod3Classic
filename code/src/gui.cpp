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



// Main GUI
// Created 22/8/03
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "System.h"
#include "Renderer.h"


texture_t   *psMouseTex = NULL;


///////////////////
// Initialize the gui
bool gui_Initialize(void)
{
    psMouseTex = Cache_LoadTexture("data/gui/mouse2.png");
   
    if(!psMouseTex)
	{
		writeLog("Unable to initialize Mouse");
        return false;
	}

	Tex_Upload(psMouseTex);
    return true;
}


///////////////////
// Draw the mouse
void gui_DrawMouse(void)
{
    mouse_t *psMouse = System_GetMouse();

    State_Disable(GL_TEXTURE_GEN_S);
    State_Disable(GL_TEXTURE_GEN_T);
    State_Enable(GL_TEXTURE_2D);
    State_Disable(GL_LIGHTING);
    State_Enable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1,1,1,1);

    DrawPic(psMouseTex, psMouse->X, 600-psMouse->Y);
}