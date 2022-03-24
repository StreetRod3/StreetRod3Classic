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



// gui Checkbox class
// Created 20/8/03
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "gui.h"
#include "Renderer.h"


///////////////////
// Create the checkbox
void CCheckbox::Create(void)
{
    psTexture = Cache_LoadTexture("data/gui/checkbox.png");
    Tex_Upload(psTexture);
    iWidth = iHeight = 25;
}


///////////////////
// Destroy the checkbox
void CCheckbox::Destroy(void)
{
}


///////////////////
// Mouse over event
int CCheckbox::MouseOver(mouse_t *tMouse)
{
    return -1;
}


///////////////////
// Mouse up event
int	CCheckbox::MouseUp(mouse_t *tMouse)
{
    bDown = !bDown;

    return CHK_CHANGED;
}


///////////////////
// Mouse down event
int CCheckbox::MouseDown(mouse_t *tMouse)
{
    bShowDown = true;

    return -1;
}


///////////////////
// Key down event
int CCheckbox::KeyDown(char c)
{
    return -1;
}


///////////////////
// Key up event
int CCheckbox::KeyUp(char c)
{
    return -1;
}


///////////////////
// Draw the checkbox
void CCheckbox::Draw(void)
{
    State_Enable(GL_TEXTURE_2D);
	State_Disable(GL_LIGHTING);
	State_Enable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(1,1,1,1);

    bool down = bDown || bShowDown;

    BlitTexture(psTexture, 0, down ? 32 : 0, iX, 600-iY, 25,25,1,1);

	bShowDown = false;
}