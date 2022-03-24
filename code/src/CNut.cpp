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


// Nut class
// Created 30/5/02
// Jason Boettcher


#include "defs.h"
#include "game.h"


///////////////////
// Setup the nut
void CNut::Setup(CVec pos, CMatrix mat, CMatrix pmat, CVec partpos)
{
	cMatrix = mat;

	vPos = pmat.MultiplyVector(pos) + partpos;
	cMatrix = cMatrix * pmat;
    
    cNut = Cache_LoadModel("data/misc/nut");
}


///////////////////
// Draw the nut
void CNut::Draw(void)
{
	float m[16];

	glPushMatrix();

	cMatrix.CreateGLMatrix(vPos,m);
	glMultMatrixf(m);

	if(iHighlight)
		DrawHighlight();
	else {
		if(iIn && cNut)
			cNut->render();
		else
			DrawHighlight();
	}    

	glPopMatrix();
}


///////////////////
// Draw the highlighted nut
void CNut::DrawHighlight(void)
{
	// Manually set the material properties
	State_Disable(GL_TEXTURE_2D);
	State_Disable(GL_BLEND);
	glBlendFunc(GL_SRC_COLOR,GL_ONE_MINUS_SRC_COLOR);
    State_Enable(GL_CULL_FACE);
    
    State_Disable(GL_LIGHTING);
	
	if(!iHighlight)
		glColor4f(1,1,1,0.3f);
	else {
		if(iIn)
			glColor4f(0,0,1,0.75f);
		else
			glColor4f(0,0,1,0.5f);
	}

    if(cNut)
	    cNut->renderRaw();

	iHighlight = false;

    State_Enable(GL_LIGHTING);
}


///////////////////
// Returns true if a line has intersected with this nut
int CNut::LineIntersect(CVec p1, CVec dir)
{
    if(!cNut)
        return false;

	float m[16];
	float p[3],d[3];
	CollisionModel3D *cd = cNut->getCDModel();

	// Set the matrix for the part
	cMatrix.CreateCDMatrix(vPos,m);
	cd->setTransform(m);

	for(int i=0;i<3;i++) {
		p[i] = p1.GetIndex(i);
		d[i] = dir.GetIndex(i);
	}

	return cd->rayCollision(p,d);
}