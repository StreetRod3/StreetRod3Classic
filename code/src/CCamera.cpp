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


// Camera class
// Created 13/5/02
// Jason Boettcher


#include "defs.h"
#include "3D.h"


///////////////////
// Setup the camera
void CCamera::Setup(CVec pos, CVec lookat)
{
	vPos = pos;
	vLookAt = lookat;

	LookAt(vLookAt);
}


///////////////////
// Look at a position
void CCamera::LookAt(CVec lookat)
{
	vLookAt = lookat;

	CVec diff;
	float dist;
	
	// Difference between the two positions
	diff = vPos - vLookAt;

	// Yaw = x / y
	fYaw = (float)-atan2(diff.GetX(),diff.GetY()) * (180/PI)-90;

	// Pitch = dist / z
	dist = (float)sqrt((float)(diff.GetX() * diff.GetX() + diff.GetY() * diff.GetY()));
	fPitch = (float)-atan2(dist,diff.GetZ()) * (180/PI)+90;
}


///////////////////
// Make the camera point in the reverse direction to what it is now
void CCamera::Reverse(void)
{
    LookAt( -vLookAt );
}


///////////////////
// Set the matrix to our camera position/angle
void CCamera::Show(void)
{
	glRotatef(-fRoll,	1,0,0);
	glRotatef(-fPitch,	0,1,0);
	glRotatef(-fYaw,	0,0,1);

	glTranslatef(-vPos.GetX(), -vPos.GetY(), -vPos.GetZ());
}