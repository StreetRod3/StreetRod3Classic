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


// Drawing functions
// Created 17/6/02
// Jason Boettcher


#include "defs.h"
#include "3D.h"


///////////////////
// Draw a rectangle
void DrawRect(int x, int y, int x2, int y2, CVec outcol, CVec fillcol, float alpha, int filled, int width)
{
	State_Disable(GL_TEXTURE_2D);
    State_Disable(GL_BLEND);

	if(filled) {
		// Filled rectangle
		if(alpha < 1) {
			State_Enable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		glColor4f(fillcol.GetX(),fillcol.GetY(),fillcol.GetZ(),alpha);
		glBegin(GL_QUADS);
        glVertex2i(x,y);
	    glVertex2i(x,y2);
	    glVertex2i(x2,y2);
	    glVertex2i(x2,y);
        glEnd();
	}
	else {
		// Outline rectangle		
        // Filled rectangle
		if(alpha < 1) {
			State_Enable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		glColor4f(outcol.GetX(),outcol.GetY(),outcol.GetZ(),alpha);
		glLineWidth(width);
		glBegin(GL_LINE_STRIP);
	    glVertex2i(x,y);
	    glVertex2i(x,y2);
	    glVertex2i(x2,y2);
	    glVertex2i(x2,y);	
		glVertex2i(x,y);
        glEnd();
    }
}


///////////////////
// Do a 'blit' from a texture onto the screen
void BlitTexture(texture_t *psTex, int sx, int sy, int dx, int dy, int w, int h, float mw, float mh)
{
    /*
       Notes:
       - Coordinates start from top left (as opposed to bottom left in OpenGL)
       - Mw & Mh are size multipliers for the screen
       - Assumes the correct states have been set
    */

    assert(psTex);
    int scrW = 600;

    // Calculate the texture coords
    float u1 = (float)sx / (float)psTex->iWidth;
    float v1 = (float)sy / (float)psTex->iHeight;
    float u2 = u1 + (float)w / (float)psTex->iWidth;
    float v2 = v1 + (float)h / (float)psTex->iHeight;

    w *= mw;
    h *= mh;

    Tex_Bind(psTex);
    
    glBegin(GL_QUADS);
        glTexCoord2f(u1, v1);   glVertex2i(dx,scrW-dy);
        glTexCoord2f(u1, v2);   glVertex2i(dx,scrW-(dy+h));
        glTexCoord2f(u2, v2);   glVertex2i(dx+w,scrW-(dy+h));
        glTexCoord2f(u2, v1);   glVertex2i(dx+w,scrW-dy);
    glEnd();
}


///////////////////
// Creates a buffer with the same details as the screen
SDL_Surface *CreateSurface(int bpp, int width, int height)
{
	Uint32 amask,rmask,gmask,bmask;

	int a = 1;
	if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
	    int s = a ? 0 : 8;
	    amask = 0x000000ff >> s;
	    rmask = 0x0000ff00 >> s;
	    gmask = 0x00ff0000 >> s;
	    bmask = 0xff000000 >> s;
	} else {
	    amask = a ? 0xff000000 : 0;
	    rmask = 0x00ff0000;
	    gmask = 0x0000ff00;
	    bmask = 0x000000ff;
	}


	return SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, bpp, 
								rmask, gmask, bmask, amask);
}

