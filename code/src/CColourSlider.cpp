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



// gui Colour slider class
// Created 25/6/02
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "gui.h"


///////////////////
// Create event
void CColourSlider::Create(void)
{
	tTexture = Cache_LoadTexture("data/gui/spectrum.png");
	Tex_Upload(tTexture);

	
	vColour = CVec(0, 0.232605f, 0.201172f);
    calculateColour();
}


///////////////////
// Draw the colour slider
void CColourSlider::Draw(void)
{
	State_Enable(GL_TEXTURE_2D);
	State_Disable(GL_BLEND);

	Tex_Bind(tTexture);

	glColor4f(1,1,1,1);

	// Draw the spectrum
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);	glVertex2i(iX,iY);
	glTexCoord2f(0,1);	glVertex2i(iX,iY-iHeight);
	glTexCoord2f(1,1);	glVertex2i(iX+iWidth,iY-iHeight);
	glTexCoord2f(1,0);	glVertex2i(iX+iWidth,iY);
	glEnd();


	// Draw the sliders
	float x = iX + (iWidth * fColSliderPos);

	// Colour
	glLineWidth(2);
	State_Disable(GL_TEXTURE_2D);
	State_Enable(GL_BLEND);
	glColor4f(0,0,0,0.4f);
	glBegin(GL_QUADS);
	glVertex2i((int)x-4,iY);
	glVertex2i((int)x-4,iY-iHeight/2);
	glVertex2i((int)x+4,iY-iHeight/2);
	glVertex2i((int)x+4,iY);
	glEnd();
	
	glColor4f(0,0,0,0.9f);
	glBegin(GL_LINES);
	glVertex2i((int)x,iY);
	glVertex2i((int)x,iY-iHeight/2);
	glEnd();

	// Brightness
	x = iX + (iWidth * fBrightSliderPos);
	glColor4f(0,0,0,0.4f);
	glBegin(GL_QUADS);
	glVertex2i((int)x-4,iY-iHeight/2);
	glVertex2i((int)x-4,iY-iHeight);
	glVertex2i((int)x+4,iY-iHeight);
	glVertex2i((int)x+4,iY-iHeight/2);
	glEnd();
	
	glColor4f(0,0,0,0.9f);
	glBegin(GL_LINES);
	glVertex2i((int)x,iY-iHeight/2);
	glVertex2i((int)x,iY-iHeight);
	glEnd();


}


void	CColourSlider::Destroy(void) { }


///////////////////
// Mouse over event	
int	CColourSlider::MouseOver(mouse_t *tMouse)
{
	return -1;
}


///////////////////
// Mouse down event
int	CColourSlider::MouseDown(mouse_t *tMouse)
{
    captureMouse();

	// Colour
	if((600-tMouse->Y) > iY-(iHeight/2)) {
		fColSliderPos = (float)(tMouse->X-iX) / (float)iWidth;
		// Clamp it
		fColSliderPos = MIN(fColSliderPos,1.0f);
		fColSliderPos = MAX(fColSliderPos,0.0f);
	}

	// Intensity
	if((600-tMouse->Y) < iY-(iHeight/2)) {
		fBrightSliderPos = (float)(tMouse->X-iX) / (float)iWidth;
		// Clamp it
		fBrightSliderPos = MIN(fBrightSliderPos,1.0f);
		fBrightSliderPos = MAX(fBrightSliderPos,0.0f);
	}
	
	if(!tTexture)
		return CSL_CHANGED;

	calculateColour();


	return CSL_CHANGED;
}


///////////////////
// Calculate the colour
void CColourSlider::calculateColour(void)
{
    // Find the colour from the 2d surface in the texture
	Uint32 pixel = GetPixel(tTexture->bmpImage,(int)(fColSliderPos * (float)tTexture->bmpImage->w),5);

	// Find the intensity from the 2d surface
	Uint32 brightness = GetPixel(tTexture->bmpImage,(int)(fBrightSliderPos * (float)tTexture->bmpImage->w),45);

	Uint8 r,g,b,a;
	GetColour4(pixel,tTexture->bmpImage,&r,&g,&b,&a);

	vColour = CVec((float)r / 256.0f, (float)g / 256.0f, (float)b / 256.0f);

	GetColour4(brightness,tTexture->bmpImage,&r,&g,&b,&a);

	// Scale the colour
	float i = (float)r / 256.0f;
	vColour = vColour * CVec(i,i,i);
}


///////////////////
// Mouse up event
int	CColourSlider::MouseUp(mouse_t *tMouse)
{
    releaseMouse();
	return CSL_CHANGED;
}


int		CColourSlider::KeyDown(char c) { return -1; }
int		CColourSlider::KeyUp(char c) { return -1; }

