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

// Entity source file
// Created 18/3/03
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "Entity.h"


entity_t    *psEntities = NULL;
texture_t   *psSpark = NULL;


///////////////////
// Initialize the entities
bool InitializeEntities(void)
{
    psEntities = new entity_t[MAX_ENTITIES];
    if( !psEntities )
        return false;

    // Clear the entities
    ClearEntities();

    // Load some textures
    psSpark = Cache_LoadTexture("data/textures/spark.png");
    Tex_Upload(psSpark);

    return true;
}


///////////////////
// Clear the entities
void ClearEntities(void)
{
    if( !psEntities )
        return;

    entity_t *ent = psEntities;
    for( int i=0; i<MAX_ENTITIES; i++, ent++)
        ent->bUsed = false;
}


///////////////////
// Shutdown the entities
void ShutdownEntities(void)
{
    if( psEntities ) {
        delete[] psEntities;
        psEntities = NULL;
    }
}


///////////////////
// Spawn a new entity
entity_t *SpawnEntity(int nType, int nType2, CVec cPos, CVec cVel, float fAge)
{
    if( !psEntities )
        return NULL;

    // Find a free entity
    entity_t *ent = psEntities;
    for(int i=0; i<MAX_ENTITIES; i++, ent++) {
        if( !ent->bUsed )
            break;
    }

    // All used?
    if( i == MAX_ENTITIES-1 )
        return NULL;

    // Fill in the entity
    ent->bUsed = true;
    ent->cPos = cPos;
    ent->cOldPos = cPos;
    ent->cVel = cVel;
    ent->fAge = fAge;
    ent->fLife = 0;
    ent->nType = nType;
    ent->nType2 = nType2;    

    return ent;
}


///////////////////
// Simulate the entities
void SimulateEntities(float dt)
{
    entity_t *ent = psEntities;

    for(int i=0; i<MAX_ENTITIES; i++, ent++) {
        if( !ent->bUsed )
            continue;

        ent->fLife += dt;
        ent->cOldPos = ent->cPos;

        switch( ent->nType ) {

            // Spark
            case ent_spark:
                ent->cVel += CVec(0,0,-50)*dt;
                ent->cPos += ent->cVel*dt;
                if( ent->fLife > ent->fAge )
                    ent->bUsed = false;
                break;
        }
    }
}


///////////////////
// Draw the entities
void DrawEntities(void)
{
    entity_t *ent = psEntities;

    State_Enable(GL_TEXTURE_2D);
    State_Disable(GL_LIGHTING);
    State_Disable(GL_BLEND);
    State_Disable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1);
    glColor4f(1,1,1,1);

    CVec dir;

    for(int i=0; i<MAX_ENTITIES; i++, ent++) {
        if( !ent->bUsed )
            continue;

        switch( ent->nType ) {

            // Spark
            case ent_spark:
                dir = ent->cPos - ent->cOldPos;
                ent->cOldPos -= dir*2;
                Tex_Bind(psSpark);
                glBegin(GL_LINES);
                    glTexCoord2f(0,0.8);  glVertex3f(ent->cOldPos.GetX(), ent->cOldPos.GetY(),ent->cOldPos.GetZ());
                    glTexCoord2f(0,0);  glVertex3f(ent->cPos.GetX(), ent->cPos.GetY(),ent->cPos.GetZ());
                glEnd();
                break;
        }
    }

    State_Enable(GL_CULL_FACE);

}