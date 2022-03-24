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


// Sidebar class
// Created 25/6/02
// Jason Boettcher


#include "defs.h"
#include "System.h"
#include "Game.h"


///////////////////
// Process the sidebar
void CSidebar::Process(void)
{
	mouse_t *Mouse = System_GetMouse();
	int w = 42;
	int h = 38;
	int gap = 2;
	int y = iY;
	int invy = 600-Mouse->Y;

	if(!(Mouse->Up & SDL_BUTTON(1)))
		return;

    if( !bEnabled )
        return;

	for(int n=0;n<iNumItems;n++) {

		if(Mouse->X > iX && Mouse->X < iX + w)
			if(invy > y && invy < y+h)
				if(tItems[n].iEnabled)
					if(tItems[n].Click)
						tItems[n].Click();

		y -= gap+h;
	}
}


///////////////////
// Draw the sidebar
void CSidebar::Draw(void)
{
	mouse_t *Mouse = System_GetMouse();
	int w = 42;
	int h = 38;
	int gap = 2;
	int y = iY;
	float alpha = 0.75f;
	int invy = 600-Mouse->Y;

	float fx = 1.0f / 128.0f;
	float fy = 1.0f / 256.0f;
	float fw = 42.0f / 128.0f;
	float fh = 40.0f / 256.0f;

	State_Enable(GL_TEXTURE_2D);
	State_Disable(GL_CULL_FACE);
	State_Enable(GL_BLEND);
    State_Disable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	

	Tex_Bind(tTexture);

	for(int n=0;n<iNumItems;n++) {
		alpha = 0.5f;

		fy = (float)(tItems[n].iImageNum * 43) / 256.0f;
        if( tItems[n].iImageNum >= 6 ) {
            fy = (float)((tItems[n].iImageNum-6) * 43) / 256.0f;
            fx = 56.0f / 128.0f;
        }

		if(Mouse->X > iX && Mouse->X < iX + w)
			if(invy > y && invy < y+h)
				if(tItems[n].iEnabled)
					alpha = 1;

        if( !bEnabled || !tItems[n].iEnabled )
            glColor4f(0.5f,0.5f,0.5f,0.5f);
        else
			glColor4f(1,1,1,alpha);

		glBegin(GL_QUADS);
		glTexCoord2f(fx, fy+fh);		glVertex2i(iX,y);
		glTexCoord2f(fx, fy);			glVertex2i(iX,y+h);
		glTexCoord2f(fx+fw, fy);		glVertex2i(iX+w,y+h);
		glTexCoord2f(fx+fw, fy+fh);		glVertex2i(iX+w,y);
		glEnd();

		y -= gap+h;
	}
}


///////////////////
// Set all the item's enabled field
void CSidebar::enableAll(bool _e)
{
    for(int n=0;n<iNumItems;n++)
        tItems[n].iEnabled = _e;
}