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



// gui Slider class
// Created 20/8/03
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "gui.h"
#include "Renderer.h"


///////////////////
// Create the slider
void CSlider::Create(void)
{
    psTexture = Cache_LoadTexture("data/gui/slider.png");
    Tex_Upload(psTexture);
}


///////////////////
// Destroy the slider
void CSlider::Destroy(void)
{
}


///////////////////
// Mouse over event
int CSlider::MouseOver(mouse_t *tMouse)
{
    return -1;
}


///////////////////
// Mouse up event
int	CSlider::MouseUp(mouse_t *tMouse)
{
    releaseMouse();
    return -1;
}


///////////////////
// Mouse down event
int CSlider::MouseDown(mouse_t *tMouse)
{
    captureMouse();

    nValue = (float)(tMouse->X - iX) / (float)iWidth * (float)nMax;
    bDown = true;

    return SLD_CHANGED;
}


///////////////////
// Key down event
int CSlider::KeyDown(char c)
{
    return -1;
}


///////////////////
// Key up event
int CSlider::KeyUp(char c)
{
    return -1;
}


///////////////////
// Draw the checkbox
void CSlider::Draw(void)
{
    State_Disable(GL_TEXTURE_2D);
	State_Disable(GL_LIGHTING);
	State_Disable(GL_BLEND);	
	
    int s = 8;
    int h = iY+iHeight/2 - s/2;

    DrawRect(iX,iY-s,iX+iWidth, iY-iHeight+s, CVec(0,0,0), CVec(0,0,0), 1, true, 0);

    // Prevent div-by-zero errors
    if(nMax-nMin == 0)
        return;

    float x = (float)nValue/(float)(nMax-nMin);
    x *= (float)iWidth;
    x += iX;

    State_Enable(GL_TEXTURE_2D);

    glColor4f(1,1,1,1);
    BlitTexture(psTexture, 0,bDown ? 32 : 0, x,600-(iY+s/2),12,25,1,1);

    bDown = false;
}