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



// Lighting manager class for driving
// Created 4/03/03
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "CLightManager.h"


///////////////////
// Light manager constructor
CLightManager::CLightManager()
{
    m_nLights = 0;
    m_nMaxLights = 0;
    m_psLights = NULL;
    m_psUsedLights = NULL;
}


///////////////////
// Setup the light manager
void CLightManager::Setup(int nMaxLights, int nNumLights)
{
    m_nMaxLights = nMaxLights;
    m_nLights = nNumLights;

    // Scene lights list
    m_psLights = new light_t[m_nLights];
    if( !m_psLights )
        m_nLights = 0;

    // Used lights list
    m_psUsedLights = new usedlight_t[nMaxLights];
    if( !m_psUsedLights ) {
        if(m_psLights)
            delete [] m_psLights;
        m_nLights = 0;
        m_nMaxLights = 0;
    }
       
}


///////////////////
// Return a pointer to the light array
light_t *CLightManager::getLights(void)
{
    return m_psLights;
}


///////////////////
// Calculate the lights that are closest to the car
void CLightManager::calculateLights(CVec carPos)
{
    int     n, i;
    light_t *l = m_psLights;
    usedlight_t *ul;

    // Set the used lights to false
    for(n=0; n<m_nMaxLights; n++)
        m_psUsedLights[n].nUsed = false;


    // Go through all the lights in the scene
    for(n=0; n<m_nLights; n++, l++) {

        float fDistance = GetDistance(l->Position, carPos);
        
        // Go through all the used lights replacing any lights that are further in distance
        ul = m_psUsedLights;
        bool place = false;
        for(i=0; i<m_nMaxLights; i++, ul++) {

            if( !ul->nUsed ) {
                ul->nUsed = true;
                ul->fDistance = fDistance;
                ul->psLight = l;
                place = true;
            }
        }

        // If we didn't place a light, go through and find the light with the furthest distance that is
        // greater then this light and replace it
        if( !place ) {
            ul = m_psUsedLights;
            float topdist = -999999;
            int id = -1;

            ul = m_psUsedLights;        
            for(i=0; i<m_nMaxLights; i++, ul++) {

                if( ul->fDistance > topdist ) {
                    topdist = ul->fDistance;
                    id = i;
                }
            }

            if( topdist > fDistance && id >= 0 ) {
                m_psUsedLights[id].fDistance = fDistance;
                m_psUsedLights[id].psLight = l;
                m_psUsedLights[id].nUsed = true;
            }
        }
    }
}


///////////////////
// Add the lights into OpenGL
void CLightManager::useLights(void)
{
    // Assumes that GL_LIGHTn has already been enabled
    GLfloat amb[4] = {0,0,0,1};    

    int     n;
    usedlight_t *ul = m_psUsedLights;

    for(n=0; n<m_nMaxLights; n++, ul++) {

        float pos[4] = { ul->psLight->Position.GetX(),
                         ul->psLight->Position.GetY(),
                         ul->psLight->Position.GetZ(),
                         1 };

        float col[4] = { ul->psLight->Colour.R, 
                         ul->psLight->Colour.G,
                         ul->psLight->Colour.B,
                         1 };

        glLightfv(GL_LIGHT1+n, GL_POSITION, pos);
        glLightfv(GL_LIGHT1+n, GL_DIFFUSE, col);
        glLightfv(GL_LIGHT1+n, GL_AMBIENT, amb);
    }
}