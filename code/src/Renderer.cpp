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


// Main renderer
// Created 13/5/02
// Jason Boettcher


#include "defs.h"
#include "game.h"


///////////////////
// Initializes the renderer
void InitializeRenderer(void)
{
	GLfloat LightCol[] = {0.3f,0.3f,0.3f, 1.0f};
	GLfloat ambient[] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat specular[] = {0.1f, 0.1f, 0.1f, 1.0f};    
	GLfloat white[] = {1,1,1,1};


	State_Enable(GL_TEXTURE_2D);
	State_Enable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	State_Enable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	State_Enable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.25f);

	glPolygonMode(GL_FRONT,GL_FILL);
    glPolygonMode(GL_BACK,GL_FILL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);

    State_Enable(GL_POINT_SMOOTH);


	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	//glFogfv(GL_FOG_COLOR, FogCol);
	glFogf(GL_FOG_DENSITY, 0.7f);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, 45.0f);
	glFogf(GL_FOG_END, 100.0f);

	// Setup the material & lighting properties
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    //glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);

	//glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 20);
	glLightfv(GL_LIGHT0,GL_AMBIENT,LightCol);
    //glLightfv(GL_LIGHT1,GL_AMBIENT,LightCol);
    //glLightfv(GL_LIGHT0,GL_DIFFUSE,LightCol);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.50f);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.10f);
    
}

GLfloat LightPos[4] = {0, 0, 5,1};


///////////////////
// Setup the frame
void SetupFrame(void)
{
	//glLightfv(GL_LIGHT0,GL_POSITION,LightPos);

	// Setup the perspective matrix	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Setup the viewport (shouldn't need to do this every frame)
	gluPerspective(45.0f,1.333f,1,2048.0f);
	glViewport(0,0,800,600);


	// Setup the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.0f,0.0f,0.0f,1.0f);

	// Swap to the SEA(?) coordinate system
	glTranslatef(0.0,0.0,0.0);
	glRotatef (-90,  1, 0, 0);
	glRotatef (90,  0, 0, 1);
}


///////////////////
// Change to ortho projection
void Render_UseOrthoProj(void)
{
	glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    GLint vPort[4];
    glGetIntegerv(GL_VIEWPORT, vPort);
    glOrtho(vPort[0], vPort[0] + vPort[2], vPort[1], vPort[1] + vPort[3], -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}


///////////////////
// Change back to normal projection
void Render_UseNormalProj(void)
{
	glMatrixMode(GL_PROJECTION);
    glPopMatrix();   
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}


///////////////////
// Begin the 2D stuff
void Render_Begin2D(void)
{
	Render_UseOrthoProj();

	State_Disable(GL_LIGHTING);
	State_Disable(GL_DEPTH_TEST);
}


///////////////////
// End the 2D stuff
void Render_End2D(void)
{
	Render_UseNormalProj();

	State_Enable(GL_LIGHTING);
	State_Enable(GL_DEPTH_TEST);
}


///////////////////
// Set the light position
void Renderer_SetLightPos(CVec pos)
{
	LightPos[0] = pos.GetX();
	LightPos[1] = pos.GetY();
	LightPos[2] = pos.GetZ();
	LightPos[3] = 1;
}


///////////////////
// Draw a texture
void DrawPic(texture_t *psTexture, int x, int y)
{
    if( !psTexture )
        return;

    State_Enable(GL_TEXTURE_2D);
    State_Enable(GL_BLEND);

    Tex_Bind(psTexture);

    glBegin(GL_QUADS);
    glTexCoord2i(0,1);      glVertex2i(x,                   y-psTexture->iHeight);
    glTexCoord2i(1,1);      glVertex2i(x+psTexture->iWidth, y-psTexture->iHeight);
    glTexCoord2i(1,0);      glVertex2i(x+psTexture->iWidth, y);
    glTexCoord2i(0,0);      glVertex2i(x,                   y);
    glEnd();
}


///////////////////
// Draw a texture
void DrawPicEx(texture_t *psTexture, int x, int y, int x2, int y2)
{
    if( !psTexture )
        return;

    State_Enable(GL_TEXTURE_2D);
    State_Enable(GL_BLEND);

    Tex_Bind(psTexture);

    glBegin(GL_QUADS);
    glTexCoord2i(0,1);      glVertex2i(x,  y2);
    glTexCoord2i(1,1);      glVertex2i(x2, y2);
    glTexCoord2i(1,0);      glVertex2i(x2, y);
    glTexCoord2i(0,0);      glVertex2i(x,  y);
    glEnd();
}


///////////////////
// Render a sprite
void RenderBillboardSprite(CVec cPos, texture_t *psTex, CCamera *pcCam, float fSize, float fRoll)
{
	Tex_Bind(psTex);

	// Calculate the rotation
	CVec diff = pcCam->getPos() - cPos;
	VectorNormalize( &diff );

	// Yaw = x / y
	float fYaw = (float)(-atan2(diff.GetX(),diff.GetY()) * (180/PI));

	// Pitch = dist / z
	float dist = (float)sqrt((float)(diff.GetX() * diff.GetX() + diff.GetY() * diff.GetY()));
	float fPitch = (float)(-atan2(dist,diff.GetZ()) * (180/PI)+270);

	glPushMatrix();    
	glTranslatef(cPos.GetX(), cPos.GetY(), cPos.GetZ());

	glRotatef(fYaw,   0, 0, 1);
    glRotatef(fRoll,  0, 1, 0);
	glRotatef(fPitch, 1, 0, 0);

	glBegin(GL_QUADS);
	glTexCoord2i(0,0);		glVertex3f(-fSize, 0, -fSize);
	glTexCoord2i(1,0);		glVertex3f(fSize,  0, -fSize);
	glTexCoord2i(1,1);		glVertex3f(fSize,  0, fSize);
	glTexCoord2i(0,1);		glVertex3f(-fSize, 0, fSize);
	glEnd();

	glPopMatrix();
}