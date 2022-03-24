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


// gui scrollbar class
// Created 30/6/02
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "gui.h"
#include "Renderer.h"


///////////////////
// Create the scrollbar
void CScrollbar::Create(void)
{
	iMin=0;
	iMax=0;
	iValue=0;
	iItemsperbox = 1;
	iScrollPos = 0;
	iSliderGrabbed = false;
	iSliderGrabPos = 0;
	iTopButton=false;
	iBotButton=false;
    nButtonsDown = 0;

	tTexture = Cache_LoadTexture("data/gui/scrollbar.png");
	Tex_Upload(tTexture);
}


///////////////////
// Draw the scrollbar
void CScrollbar::Draw(void)
{
	int x=0;
	int length;
    int y = 600-iY;

    // Rectangle bit
    DrawRect(iX+iWidth-19, y, iX+iWidth+1, y-iHeight+1, CVec(0,0,0), CVec(0.2f,0.2f,0.2f), 1, true, 1);


    //
    // Arrows
    //
	State_Enable(GL_TEXTURE_2D);
	State_Disable(GL_BLEND);
	glColor4f(1,1,1,1);

	
    
	
	// Top arrow
    x = 1;
	if(iTopButton)
		x = 22;
    BlitTexture(tTexture, x,1, iX+iWidth-19, iY+1, 20,19, 1,1);
	
	// Bottom arrow
	x = 1;
	if(iBotButton)
		x = 22;
	BlitTexture(tTexture, x,44, iX+iWidth-19, iY+iHeight-21, 20,19, 1,1);


	// Slider
	if(iMax >= iItemsperbox && iMax > 0) {
        length = (int)((float)iItemsperbox/(float)iMax * iHeight-40);
        length = MAX(length, 30);
        
		int pos = iScrollPos;
		if(pos+length > iHeight-40)
			pos=iHeight-40-length;

        BlitTexture(tTexture, 43,1,  iX+iWidth-19, iY+pos+20, 20,10, 1,1);
        BlitTexture(tTexture, 43,15, iX+iWidth-19, iY+pos+30, 20,10, 1,((float)length-19.0f)/10.0f);
        BlitTexture(tTexture, 43,53, iX+iWidth-19, iY+pos+20+length-10, 20,10, 1,1);
	}

    if(!System_GetMouse()->Down) {
	    iTopButton=false;
	    iBotButton=false;
    }
}


///////////////////
// Mouse up
int CScrollbar::MouseUp(mouse_t *tMouse)
{
	iSliderGrabbed = false;
    nButtonsDown = 0;
    iTopButton=false;
	iBotButton=false;
	return SCR_NONE;
}


///////////////////
// Mouse over
int CScrollbar::MouseOver(mouse_t *tMouse)
{
    if(!tMouse->Down) {
		iSliderGrabbed = false;
        nButtonsDown = 0;
        iTopButton=false;
	    iBotButton=false;
    }
	
	return SCR_NONE;
}


///////////////////
// Mouse down
int CScrollbar::MouseDown(mouse_t *tMouse)
{
	int length = (int)((float)iItemsperbox/(float)iMax * iHeight-40);
	length = MAX(length, 30);

    int Y = 600-tMouse->Y;
	
	// Grab the slider
	if(tMouse->Y > iY+20+iScrollPos && tMouse->Y < iY+20+iScrollPos+length) {		
		if(!iSliderGrabbed) {
			iSliderGrabbed = true;
			iSliderGrabPos = tMouse->Y - (iY+20+iScrollPos);
			return SCR_NONE;
		}
	}

	// Move the slider
	if(iSliderGrabbed) {
		float dist = (float)tMouse->Y - (iY+20+iScrollPos + iSliderGrabPos);
		float increment = (float)iHeight/(float)iMax;
		if(increment)
			iValue += (int)(dist / increment);
		UpdatePos();
		return SCR_CHANGE;
	}

    float curTime = tMainSR3.fCurTime;
    bool Down = false;
	for(int i=0; i<3; i++) {
		if(tMouse->Down & SDL_BUTTON(i)) {

			if( nButtonsDown & SDL_BUTTON(i) ) {
				if( fMouseNext[i] < curTime ) {
					Down = true;
					fMouseNext[i] = curTime + 0.05f;
				}
			} else {
				Down = true;
				nButtonsDown |= SDL_BUTTON(i);
				fMouseNext[i] = curTime + 0.25f;
			}
		}
	}

    if(!Down)
        return SCR_NONE;

	// Top arrow
	if(Y > iY+iHeight-19 && Y < iY+iHeight) {
		iTopButton = true;

		// Move up
		iValue--;
		UpdatePos();
		return SCR_CHANGE;
	}

	// Bottom arrow
	if(Y > iY && Y < iY+22) {
		iBotButton = true;

		// Move down
		iValue++;
		UpdatePos();
		return SCR_CHANGE;
	}


	return SCR_NONE;
}


///////////////////
// Update the slider pos
void CScrollbar::UpdatePos(void)
{
	iMax = MAX(iMax,0);
    iMin = MAX(iMin,0);

    if(iMax < iItemsperbox) {
        iValue = 0;
        iScrollPos = 0;
        return;
    }

    int mx = iMax-iItemsperbox+1;

	if(iValue > mx)
		iValue = mx;
	if(iValue < 0)
		iValue = 0;

    // Prevent div by zero errors
    if(mx == 0)
        return;

    int length = (int)((float)iItemsperbox/(float)iMax * iHeight-40);
    length = MAX(length, 30);

	iScrollPos = (int)((float)(iHeight-40-length)/(float)mx * iValue);
}