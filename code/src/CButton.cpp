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



// gui Button class
// Created 5/6/02
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "gui.h"
#include "Renderer.h"


///////////////////
// Draw the button
void CButton::Draw(void)
{
	State_Enable(GL_TEXTURE_2D);
	State_Disable(GL_LIGHTING);
	State_Enable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(1,1,1,1);

    BlitTexture(tTexture, 0, iPushed ? 32 : 0, iX, 600-iY, 128,32,1,1);

    Font_SetSize(nSize);    
    int fy = 22;
    if( iPushed ) {
        Font_DrawCentered(iX+68,iY-fy-4, CVec(0,0,0), sText);
        Font_DrawCentered(iX+66,iY-fy-2, CVec(1,1,1), sText);
    } else {
        Font_DrawCentered(iX+66,iY-fy-2, CVec(0,0,0), sText);
        Font_DrawCentered(iX+64,iY-fy, CVec(1,1,1), sText);
    }

	iMouseOver = false;
	iPushed = false;
}


///////////////////
// Create event
void CButton::Create(void)
{
	tTexture = Cache_LoadTexture("data/gui/button4.png");
	Tex_Upload(tTexture);

	tTextTexture = Cache_LoadTexture("data/gui/button_text.png");
	Tex_Upload(tTextTexture);

	iWidth = 128;
	iHeight = 32;
    nSize = 14;
    iPushed = false;
}


void	CButton::Destroy(void) { }


///////////////////
// Mouse over event	
int	CButton::MouseOver(mouse_t *tMouse)
{
	iMouseOver = true;
	return -1;
}


///////////////////
// Mouse down event
int	CButton::MouseDown(mouse_t *tMouse)
{
	iMouseOver = true;
	iPushed = true;
    captureMouse();
	return -1;
}


///////////////////
// Mouse up event
int	CButton::MouseUp(mouse_t *tMouse)
{
    releaseMouse();
	return BUT_CLICKED;
}


///////////////////
// Set the button text
void CButton::setText(char *s)
{
    sr_strncpy(sText, s, 63);
}


int		CButton::KeyDown(char c) { return -1; }
int		CButton::KeyUp(char c) { return -1; }

