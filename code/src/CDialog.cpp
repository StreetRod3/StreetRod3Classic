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


// gui Dialog class
// Created 5/6/02
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "gui.h"


///////////////////
// Draw the dialog box
void CDialog::Draw(void)
{
	int titlesizes[] = {100,110,100,160,145,256,110,110};

	State_Disable(GL_LIGHTING);
	State_Enable(GL_BLEND);
	State_Disable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	
	// Black box
	State_Disable(GL_TEXTURE_2D);
	glColor4f(0,0,0,0.75f);
	glBegin(GL_QUADS);
		glVertex2i(iX, iY);
		glVertex2i(iX+iWidth, iY);
		glVertex2i(iX+iWidth, iY-iHeight);
		glVertex2i(iX, iY-iHeight);
	glEnd();

	
	// 'Rails'
	State_Enable(GL_TEXTURE_2D);
	glColor4f(1,1,1,1);
	Tex_Bind( tHRail );
	DrawHoriRail( iX, iY );
	DrawHoriRail( iX, iY-iHeight+9 );
	Tex_Bind( tVRail );
	DrawVertRail( iX, iY );
	DrawVertRail( iX+iWidth-9, iY );


	// The 4 tyres
	Tex_Bind( tDlgTexture );
	DrawBigTyre( iX-10, iY+10 );
	DrawBigTyre( iX + iWidth-18, iY+10 );
	DrawBigTyre( iX-10, iY - iHeight+18 );
	DrawBigTyre( iX + iWidth-18, iY - iHeight+18 );


	// Title
	if(iTitle >= 0) {
		assert( iTitle < 8 );

		Tex_Bind( tTitleTexture );
		float y = 2 + (30*iTitle);
		y /= 256.0f;
        int c = iX + iWidth/2;
		int x = c - titlesizes[iTitle]/2;
		int x2 = c + titlesizes[iTitle]/2;
		float w = (float)titlesizes[iTitle]/256.0f;

		glBegin(GL_QUADS);
			glTexCoord2f(0, y);			glVertex2i(x, iY-15);
			glTexCoord2f(w, y);			glVertex2i(x2, iY-15);
			glTexCoord2f(w, y+0.1f);	glVertex2i(x2, iY-40);
			glTexCoord2f(0, y+0.1f);	glVertex2i(x, iY-40);
		glEnd();
	}
}


///////////////////
// Draw a big tyre
void CDialog::DrawBigTyre(int x, int y)
{
	glBegin(GL_QUADS);
		glTexCoord2f( 0, 0 );			glVertex2i( x, y);
		glTexCoord2f( 0.22f, 0 );		glVertex2i( x+27, y);
		glTexCoord2f( 0.22f, 0.22f );	glVertex2i( x+27, y-27);
		glTexCoord2f( 0, 0.22f );		glVertex2i( x, y-27);
	glEnd();
}


///////////////////
// Draw a horizontal rail
void CDialog::DrawHoriRail(int x, int y)
{
    float u = (float)iWidth/32;
	glBegin(GL_QUADS);
		glTexCoord2f( 0, 0 );		glVertex2i( x, y );
		glTexCoord2f( u, 0 );		glVertex2i( x+iWidth, y );
		glTexCoord2f( u, 0.3f );	glVertex2i( x+iWidth, y-9 );
		glTexCoord2f( 0, 0.3f );	glVertex2i( x, y-9 );
	glEnd();
}


///////////////////
// Draw a vertical rail
void CDialog::DrawVertRail(int x, int y)
{
	float v = (float)iHeight/32;
	glBegin(GL_QUADS);
		glTexCoord2f( 0, 0 );		glVertex2i( x, y );
		glTexCoord2f( 0.3f, 0 );	glVertex2i( x+9, y );
		glTexCoord2f( 0.3f, v );	glVertex2i( x+9, y-iHeight );
		glTexCoord2f( 0, v );		glVertex2i( x, y-iHeight );
	glEnd();
}


///////////////////
// Create the dialog box
void CDialog::Create(void)
{
	// Load the textures
	tDlgTexture = Cache_LoadTexture( "data/gui/dialog.png" );
	Tex_Upload(tDlgTexture);

	tHRail = Cache_LoadTexture( "data/gui/hrail.png" );
	Tex_Upload(tHRail);
	tVRail = Cache_LoadTexture( "data/gui/vrail.png" );
	Tex_Upload(tVRail);

	tTitleTexture = Cache_LoadTexture( "data/gui/titles.png");
	Tex_Upload(tTitleTexture);


	// Change the position & size depending on size
	switch(iSize) {
		// TODO: Other sizes
		case dlgs_small:
			break;
		case dlgs_medium:
			iWidth = 500;
			iHeight = 150;
			break;

		case dlgs_large:
			iWidth = 500;
			iHeight = 330;
			break;
	}

	iX = 400 - iWidth/2;
	iY = 300 + iHeight/2;


	// Check if it's a title we have an image for
	iTitle = -1;
	if(stricmp(sTitle, "cars") == 0)
		iTitle = dlgt_cars;
	else if(stricmp(sTitle, "parts") == 0)
		iTitle = dlgt_parts;
	else if(stricmp(sTitle, "paint") == 0)
		iTitle = dlgt_paint;
	else if(stricmp(sTitle, "car info") == 0)
		iTitle = dlgt_carinfo;
    else if(stricmp(sTitle, "system") == 0)
        iTitle = dlgt_system;
    else if(stricmp(sTitle, "part change") == 0)
        iTitle = dlgt_partchange;
    else if(stricmp(sTitle, "Decal") == 0)
        iTitle = dlgt_decal;
    else if(stricmp(sTitle, "Radio") == 0)
        iTitle = dlgt_radio;

    bGrabbed = false;
}


///////////////////
// Mouse down event
int CDialog::MouseDown(mouse_t *tMouse)
{
    // Drag the dialog
    if( bGrabbed ) {
        pcParent->moveWidgets(tMouse->X-GrabX, GrabY-tMouse->Y);
        GrabX = tMouse->X;
        GrabY = tMouse->Y;
    } else {
        bGrabbed = true;
        GrabX = tMouse->X;
        GrabY = tMouse->Y;
    }

    captureMouse();

    return -1;
}


///////////////////
// Mouse up event
int CDialog::MouseUp(mouse_t *tMouse)
{
    bGrabbed = false;
    releaseMouse();

    return -1;
}


///////////////////
// Mouse over event
int	CDialog::MouseOver(mouse_t *tMouse)
{
    if( !tMouse->Down )
        bGrabbed = false;
      
    return -1;
}



void	CDialog::Destroy(void) { }
int		CDialog::KeyDown(char c) { return -1; }
int		CDialog::KeyUp(char c) { return -1; }

