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


// Taskbar class
// Created 30/5/02
// Jason Boettcher


#include "defs.h"
#include "game.h"
#include "System.h"


///////////////////
// Draw the taskbar
void CTaskBar::Draw(void)
{
	if(!iNumTaskItems)
		return;

	float w=64;
	float h=64;
	float ch = h-50;

	mouse_t *Mouse = System_GetMouse();
	int inv_h = 600-Mouse->Y;

	// Setup the material
	State_Enable(GL_TEXTURE_2D);
	State_Enable(GL_BLEND);
	State_Disable(GL_CULL_FACE);
	State_Disable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	for(int i=0;i<iNumTaskItems;i++) {
		Tex_Bind(tTaskItems[i].tTexture);

		glColor4f(1,1,1,0.5f);
        int highlight = tTaskItems[i].bToggled;

		// Check if the mouse is over it
        if(Mouse->X > tTaskItems[i].x && Mouse->X < tTaskItems[i].x+w)
            if(inv_h > 16+ch && inv_h < 16+h)
                highlight = true;

        if(!bEnabled) {
            glColor4f(0.5f,0.5f,0.5f,0.5f);
            highlight = false;
        }

        if(highlight)
            glColor4f(1,1,1,1);

		glBegin(GL_QUADS);
		glTexCoord2i(0,0);	glVertex2f(tTaskItems[i].x,16+h);
		glTexCoord2i(0,1);	glVertex2f(tTaskItems[i].x,16);
		glTexCoord2i(1,1);	glVertex2f(tTaskItems[i].x+w,16);
		glTexCoord2i(1,0);	glVertex2f(tTaskItems[i].x+w,16+h);
		glEnd();
	}


	/*State_Disable(GL_TEXTURE_2D);

	glLineWidth(2);
	glColor4f(1,1,1,1);
	glBegin(GL_LINE_STRIP);
	glVertex2i(300,128);	glVertex2i(300,32);
	glVertex2i(600,32);		glVertex2i(600,128);glVertex2i(300,128);
	glEnd();

	glColor4f(0,0,0,0.5f);
	glBegin(GL_QUADS);
	glVertex2i(300,128);	glVertex2i(300,32);
	glVertex2i(600,32);		glVertex2i(600,128);
	glEnd();*/
}


///////////////////
// Add an item to the task bar
void CTaskBar::Add(char *sTexture, char *id, void (*func) (void))
{
	task_item_t *t = &tTaskItems[iNumTaskItems++];

	// Load the texture from the cache
	t->tTexture = Cache_LoadTexture(sTexture);
	if(t->tTexture == NULL)
		return;

	if(!t->tTexture->iUploaded)
		Tex_Upload(t->tTexture);

	if(iDir == 0)
		t->x = (float)( (iNumTaskItems-1)*64+ iPosx );
	else
		t->x = (float)( iPosx - (iNumTaskItems-1)*64 );

	t->func = func;
    t->bToggled = false;
	strcpy(t->sId,id);
}


///////////////////
// Simulate the task bar
void CTaskBar::Simulate(void)
{
	float w=64;
	float h=64;
	float ch = h-50;
	mouse_t *Mouse = System_GetMouse();
	int inv_h = 600-Mouse->Y;

	if(!iNumTaskItems)
		return;

	// Go through the items
	for(int i=0;i<iNumTaskItems;i++) {
		
		// Check if the mouse clicked it
		if(Mouse->X > tTaskItems[i].x && Mouse->X < tTaskItems[i].x+w)
			if(inv_h > 16+ch && inv_h < 16+h)
				if(Mouse->Up && !tTaskItems[i].bToggled && bEnabled) {
					if(tTaskItems[i].func)
						tTaskItems[i].func();

					// If the func called clear, just leave it at that
					// ugly method
					if(iNumTaskItems==0)
						return;

					break;
				}
	}
}


///////////////////
// Toggle an item
void CTaskBar::toggleItem(char *id)
{
    if(!iNumTaskItems)
		return;

    // Go through the items
	for(int i=0;i<iNumTaskItems;i++) {
		if(stricmp(tTaskItems[i].sId,id) == 0) {

            // Toggle this item
            tTaskItems[i].bToggled = true;
        } else
            tTaskItems[i].bToggled = false;
    }

}


///////////////////
// Remove a taskbar item based on id
void CTaskBar::Remove(char *id)
{
	int a,n;
	if(!iNumTaskItems)
		return;

	// Go through the items
	for(int i=0;i<iNumTaskItems;i++) {
		if(stricmp(tTaskItems[i].sId,id) == 0) {

			// Remove this button
			for(n=0,a=0;n<iNumTaskItems;n++) {
				if(n!=i)
					tTaskItems[a++] = tTaskItems[n];
			}
			iNumTaskItems--;

			break;
		}
	}
}