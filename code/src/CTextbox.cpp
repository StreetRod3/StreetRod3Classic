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



// gui Textbox class
// Created 24/8/03
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "gui.h"
#include "Renderer.h"


///////////////////
// Create the textbox
void CTextbox::Create(void)
{
    sText[0] = '\0';
    nLength = 0;
    bHolding = false;
    iLastchar = -1;
}


///////////////////
// Destroy the checkbox
void CTextbox::Destroy(void)
{
}


///////////////////
// Mouse over event
int CTextbox::MouseOver(mouse_t *tMouse)
{
    return TXT_NONE;
}


///////////////////
// Mouse up event
int	CTextbox::MouseUp(mouse_t *tMouse)
{
    return TXT_NONE;
}


///////////////////
// Mouse down event
int CTextbox::MouseDown(mouse_t *tMouse)
{
    return TXT_NONE;
}


///////////////////
// Key down event
int CTextbox::KeyDown(char c)
{
    keyboard_t *kb = System_GetKeyboard();

	if(bHolding) {
		if(iLastchar != c)
			bHolding = false;
		else {
			if(tMainSR3.fCurTime - fTimePushed < 0.20f) {
				return TXT_NONE;
			}
		}
	}

    fTimePushed = tMainSR3.fCurTime;
	if(!bHolding)
		bHolding = true;

	iLastchar = c;


	// Backspace
	if(c == '\b') {
        if(nLength > 0)
            sText[--nLength] = '\0';

		return TXT_CHANGED;        
	}

	// Enter
	if(c == '\r') {
		return TXT_ENTER;
	}

	// Add character
    if(nLength < 64 && c != 0) {
        sText[nLength++] = c;
        sText[nLength] = '\0';
    }
	
	return TXT_CHANGED;
}


///////////////////
// Key up event
int CTextbox::KeyUp(char c)
{
    bHolding = false;

    return TXT_NONE;
}


///////////////////
// Draw the checkbox
void CTextbox::Draw(void)
{
    State_Enable(GL_TEXTURE_2D);	
	State_Disable(GL_BLEND);

    Font_SetSize(16);
    Font_SetAlpha(1);
    Font_DrawShadow(iX,iY-16,cfgCol, cbgCol, sText);

    if(bFocus) {
        int c = nLength*16;
        DrawRect(iX+c,iY-2, iX+c+1,iY-18, CVec(0,0,1), CVec(0,0,0),1,false,1);
    }

    
    //DrawRect(iX,iY, iX+iWidth,iY-iHeight, CVec(0,0,0),CVec(0,0,0),1,false,1);
}


///////////////////
// Set the text
void CTextbox::setText(char *text)
{
    sr_strncpy(sText, text, 63);
    nLength = strlen(sText);
}