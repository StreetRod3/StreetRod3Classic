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


// Sky class
// Created 8/10/01
// Jason Boettcher


#include "defs.h"
#include "3d.h"
#include "Renderer.h"
#include "CSky.h"


///////////////////
// Initialize the sky
bool CSky::Initialize(void)
{
	// Allocate the stars
	m_psStars = new star_t[MAX_STARS];
	if(m_psStars == NULL)
		return false;

	// Set the star details
    float radius = 90;
    CVec f;
    for(int i=0; i<MAX_STARS; i++) {
        GetAngles(-fabs(GetRandomNum()*45), 0, fabs(GetRandomNum()*360), &f, NULL, NULL);
        m_psStars[i].pos = f*radius;        
        m_psStars[i].size = fabs(GetRandomNum());
        m_psStars[i].flicker = fabs(GetRandomNum());
        
        // Calculate the rotation
	    CVec p = m_psStars[i].pos;
	    VectorNormalize( &p );

        m_psStars[i].yaw = (float)(-atan2(p.GetX(),p.GetY()) * (180/PI));	
	    
        float dist = (float)sqrt((float)(p.GetX() * p.GetX() + p.GetY() * p.GetY()));
	    m_psStars[i].pitch = (float)(-atan2(dist,p.GetZ()) * (180/PI)+270);
    }


	// Load the moon texture
	m_psMoonTexture = Cache_LoadTexture("data/textures/sky/moonlg.png");	
	if(m_psMoonTexture == NULL)
		return false;
	Tex_Upload(m_psMoonTexture);

	// Load the star texture
	m_psStarTexture = Cache_LoadTexture("data/textures/sky/star.png");
	if(m_psStarTexture == NULL)
		return false;
	Tex_Upload(m_psStarTexture);

	return true;
}


///////////////////
// Simulate the sky
void CSky::Simulate(float dt)
{
}


///////////////////
// Render the sky
void CSky::Render(CCamera *psCamera)
{
    int     i;
    float   fSize;    

	State_Enable(GL_TEXTURE_2D);
	State_Disable(GL_FOG);
	State_Disable(GL_LIGHTING);
    State_Enable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_COLOR);
    glDepthMask(0);

	// Make the sky be around the camera
	glPushMatrix();
    if(psCamera) {
        CVec p = psCamera->getPos();
	    glTranslatef(p.GetX(), p.GetY(), p.GetZ());
    }


	// Draw the stars
    Tex_Bind(m_psStarTexture);    

    star_t *s = m_psStars;
    for(i=0; i<MAX_STARS; i++, s++) {
        glPushMatrix();
        glTranslatef(s->pos.GetX(), s->pos.GetY(), s->pos.GetZ());
        glRotatef(s->yaw-180, 0,0,1);
        glRotatef(s->pitch, 0,0,1);

        float f = 1-(fabs(GetRandomNum()*0.6f) * s->flicker);
        glColor4f(f,f,f,1.0f);

        fSize = s->size;

        glBegin(GL_QUADS);
	    glTexCoord2i(0,0);		glVertex3f(-fSize, 0, -fSize);
	    glTexCoord2i(1,0);		glVertex3f(fSize,  0, -fSize);
	    glTexCoord2i(1,1);		glVertex3f(fSize,  0, fSize);
	    glTexCoord2i(0,1);		glVertex3f(-fSize, 0, fSize);
	    glEnd();

        glPopMatrix();
    }


    // Draw the moon    
    glColor4f(1,1,1,1);
    Tex_Bind(m_psMoonTexture);
    glPushMatrix();
    fSize = 5.5f;
    glTranslatef(-60,100,40);
    glRotatef(10,0,0,1);
    glBegin(GL_QUADS);
	    glTexCoord2i(0,0);		glVertex3f(-fSize, 0, -fSize);
	    glTexCoord2i(1,0);		glVertex3f(fSize,  0, -fSize);
	    glTexCoord2i(1,1);		glVertex3f(fSize,  0, fSize);
	    glTexCoord2i(0,1);		glVertex3f(-fSize, 0, fSize);
	glEnd();
    glPopMatrix();

	
    glDepthMask(1);
	glPopMatrix();
}


///////////////////
// Shutdown the sky
void CSky::Shutdown(void)
{
    if(m_psStars) {
        delete[] m_psStars;
        m_psStars = NULL;
    }

}