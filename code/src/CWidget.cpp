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



// Widget class
// Created 5/6/02
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "gui.h"


///////////////////
// Setup the widget
void CWidget::Setup(int id, int x, int y, int w, int h)
{
	iID = id;
	iX = x;
	iY = 600-y;		// Flip the y   TODO: Read screen height (600) from options
	iWidth = w;
	iHeight = h;
    bFocus = false;
}


///////////////////
// Returns true if a point is inside this widget
int CWidget::InBox(int x, int y)
{
	if(x > iX && x < iX+iWidth)
		if(y > iY-iHeight && y < iY)
			return true;

	return false;
}


///////////////////
// Move the widget
void CWidget::move(int dx, int dy)
{
    iX += dx;
    iY += dy;
}


///////////////////
// Capture the mouse
void CWidget::captureMouse(void)
{
    if(pcParent)
        pcParent->captureMouse(this);
}


///////////////////
// Release the mouse
void CWidget::releaseMouse(void)
{
    if(pcParent)
        pcParent->releaseMouse();
}