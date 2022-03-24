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


// Scene script class
// Created 17/7/03
// Jason Boettcher


#include "defs.h"
#include "game.h"


///////////////////
// CSceneScript Constructor
CSceneScript::CSceneScript()
{
    m_psObjectList = NULL;
}


///////////////////
// Shutdown the scenescript
void CSceneScript::shutdown(void)
{
    scr_object_t *o = m_psObjectList;
    scr_object_t *n = NULL;

    for(; o; o=n) {
        n=o->psNext;

        delete o;
    }

    m_psObjectList = NULL;
}


///////////////////
// Draw the scene script
void CSceneScript::draw(CCamera *pcCam)
{
    scr_object_t *o = m_psObjectList;
    CVec    f,r;
    plane_t frustum[4];
    float   farDistance = 1000;

    // Create a camera frustum
    GetAngles( pcCam->getPitch(), 0, pcCam->getYaw(), &f, &r, NULL);
 
    // Near
    frustum[0].vNormal = f;
    frustum[0].fDistance = DotProduct(-frustum[0].vNormal, pcCam->getPos());
    // Far
    frustum[1].vNormal = -f;
    frustum[1].fDistance = DotProduct(-frustum[1].vNormal, pcCam->getPos()+f*farDistance);
    // Left
    frustum[2].vNormal = f+r;
    frustum[2].fDistance = DotProduct(-frustum[2].vNormal, pcCam->getPos());
    // Right
    frustum[3].vNormal = f-r;
    frustum[3].fDistance = DotProduct(-frustum[3].vNormal, pcCam->getPos());

    for(; o; o=o->psNext) {

        // Can we see this object?
        if(ProjectPoint(&frustum[0],o->cPosition) < -EPSILON)
            continue;
        if(ProjectPoint(&frustum[1],o->cPosition) < -EPSILON)
            continue;
        if(ProjectPoint(&frustum[2],o->cPosition) < -EPSILON ||
           ProjectPoint(&frustum[3],o->cPosition) < -EPSILON)
            continue;
        // TODO: Show/hide objects


        switch(o->nType) {

            // Sprite
            case sco_sprite:
                drawSprite(o, pcCam);
                break;

            // Quad
            case sco_quad:
                drawQuad(o);
                break;

            // Corona
            case sco_corona:
                drawCorona(o, pcCam);
                break;


        }
    }
}


///////////////////
// Common drawing
void CSceneScript::commonDraw(scr_object_t *psObj)
{
    // Colour
    glColor4fv(psObj->fColour);

    // Texture
    if(psObj->psTexture)
        State_Enable(GL_TEXTURE_2D);
    else
        State_Disable(GL_TEXTURE_2D);
    Tex_Bind(psObj->psTexture);

    State_Disable(GL_LIGHTING);    

    // Blending
    if( psObj->nBlend == BLEND_NONE )
        State_Disable(GL_BLEND);
    else
        State_Enable(GL_BLEND);    

    switch(psObj->nBlend) {
        case BLEND_ONEONE:
            glBlendFunc(GL_ONE,GL_ONE);
            break;
        case BLEND_ONEZERO:
            glBlendFunc(GL_ONE,GL_ZERO);
            break;
        case BLEND_ALPHA:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case BLEND_COLOUR:
            glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_COLOR);
            break;
        case BLEND_ALPHAONE:
            glBlendFunc(GL_SRC_ALPHA,GL_ONE);
            break;
    }

    // Depth mask
    if( psObj->nDepthMask )
        glDepthMask(1);
    else
        glDepthMask(0);
}


///////////////////
// Draw a sprite
void CSceneScript::drawSprite(scr_object_t *psObj, CCamera *pcCam)
{
    commonDraw(psObj);
    
    RenderBillboardSprite( psObj->cPosition, psObj->psTexture, pcCam, psObj->fSize, 0 );

    // Clean up behind ourselves
    if( !psObj->nDepthMask )
        glDepthMask(1);
}


///////////////////
// Draw a quad
void CSceneScript::drawQuad(scr_object_t *psObj)
{
    float m[16];
    commonDraw(psObj);

    glPushMatrix();
    psObj->cMatrix.CreateGLMatrix(psObj->cPosition, m);
    glMultMatrixf(m);

    glBegin(GL_QUADS);
    glTexCoord2i(0,0);      glVertex2f(-psObj->cDimensions.GetX(), -psObj->cDimensions.GetY());
    glTexCoord2i(1,0);      glVertex2f(psObj->cDimensions.GetX(), -psObj->cDimensions.GetY());
    glTexCoord2i(1,1);      glVertex2f(psObj->cDimensions.GetX(), psObj->cDimensions.GetY());
    glTexCoord2i(0,1);      glVertex2f(-psObj->cDimensions.GetX(), psObj->cDimensions.GetY());
    glEnd();

    glPopMatrix();

    // Clean up behind ourselves
    if( !psObj->nDepthMask )
        glDepthMask(1);
}


///////////////////
// Draw a corona
void CSceneScript::drawCorona(scr_object_t *psObj, CCamera *pcCam)
{    
    commonDraw(psObj);

    float fLength = GetDistance(pcCam->getPos(), psObj->cPosition);
    
    float roll = fLength / 5;
    float size = psObj->fSize * (fLength/15);
    // Cap the size
    size = MIN(size,psObj->fSize*15);
    
    RenderBillboardSprite( psObj->cPosition, psObj->psTexture, pcCam, size, roll );

    // Clean up behind ourselves
    if( !psObj->nDepthMask )
        glDepthMask(1);
}


///////////////////
// Load the scene script
bool CSceneScript::load(char *szFilename, CModel *pcScene)
{
    FILE            *fp = NULL;
    char            szLine[1024];
    scr_object_t    sCurObject;
    bool            bInObject = false;
    int             nLength;
    int             nStart, nEnd;
    bool            bIsArray = false;

    fp = fopen(szFilename, "rt");
    if( !fp )
        return false;


    // Parse the script
    while( fgets(szLine, 1023, fp) ) {
        TrimSpaces(szLine);
        nLength = strlen(szLine);
        
        // Check for blank lines
        if( nLength == 0 )
            continue;

        // Remove the newline at the end of the line
        if( szLine[nLength-1] == '\n' ) {
            szLine[nLength-1] = '\0';
            nLength--;
            if( nLength == 0 )
                continue;
        }

        // Check for a commented line
        if( szLine[0] == '#' )
            continue;

        // If we aren't currently in an object, check for an object name        
        if( !bInObject && szLine[0] == '$' && szLine[nLength-1] == '{' ) {

            // Setup the default values for the object
            defaultObj(&sCurObject);

            sr_strncpy(sCurObject.szName, szLine, nLength-1);
            sCurObject.szName[nLength-1] = '\0';
            TrimSpaces(sCurObject.szName);
            bInObject = true;
            
            // Check for square brackets
            char *d = strrchr(szLine, '[');
            if(d) {
                // Object array
                sr_strncpy(sCurObject.szName, szLine, 127);
                sCurObject.szName[d-szLine] = '\0';
                TrimSpaces(sCurObject.szName);

                bIsArray = true;
                int pos = readNum(szLine, d-szLine+1, &nStart);
                readNum(szLine, pos+1, &nEnd);

                // Array check
                if( nStart > nEnd ) {
                    printf("Script Error: Object Array is not valid\n");
                    bIsArray = false;
                    bInObject = false;
                }

            } else {
                // Normal single object
                bIsArray = false;
            }
            continue;
        }


        // End of object?
        if( bInObject && szLine[0] == '}' ) {
            // Add the object(s) to the list
            bInObject = false;

            if( bIsArray ) {
                char buf[256];
                for(int i=nStart; i<=nEnd; i++ ) {
                    sprintf(buf, "%s%d",sCurObject.szName,i);

                    addObject(buf, &sCurObject, pcScene);
                }
            } else {
                addObject(sCurObject.szName, &sCurObject, pcScene);
            }
            continue;
        }

        // Read the object information
        translateLine(szLine, &sCurObject);
    }

    fclose(fp);



    return true;
}


///////////////////
// Read a token and convert it to a number
//
// Returns the current position in the line after reading
int CSceneScript::readNum(char *szLine, int nPos, int *nNumber)
{
    char szToken[512];
    int  i = 0;
    unsigned int n = nPos;

    // Read characters until we reach a delimeter
    for(n=nPos; n<strlen(szLine); n++ ) {
        if( szLine[n] == '-' || szLine[n] == '[' || szLine[n] == ']' || szLine[n] == '\n' )
            break;

        szToken[i++] = szLine[n];
    }
    szToken[i] = '\0';
    TrimSpaces(szToken);

    *nNumber = atoi(szToken);

    return n;
}


///////////////////
// Add an object to the list
void CSceneScript::addObject(char *szName, scr_object_t *psObj, CModel *pcScene)
{
    scr_object_t *psNewObj = new scr_object_t;
    if( !psNewObj )
        return;

    // Fill in the details
    *psNewObj = *psObj;
    sr_strncpy(psNewObj->szName, szName, 127);
    gobject_t *g = pcScene->findGameObject(szName);

    if( g ) {
        psNewObj->cPosition = g->vPosition;
        psNewObj->cMatrix = g->cMatrix;
    } else {
        printf("Script Warning: Could not find object '%s' in scene\n",szName);

        // Don't add the object
        delete psNewObj;
        return;
    }

    // Link in the object
    psNewObj->psNext = m_psObjectList;
    m_psObjectList = psNewObj;
}


///////////////////
// Translate a line
void CSceneScript::translateLine(char *szLine, scr_object_t *psObj)
{
    char *tok = strtok(szLine, " ");
    if(!tok)
        return;

    // Type
    if( stricmp(tok, "type") == 0 ) {
        char *t = strtok(NULL," ");
        if(t)
            psObj->nType = Key_FindValue(t);
    } else
    // Texture
    if( stricmp(tok, "texture") == 0 ) {
        char *t = strtok(NULL," ");
        if(t) {
            psObj->psTexture = Cache_LoadTexture(t);
            Tex_Upload(psObj->psTexture);
        }
    } else
    // Alpha
    if( stricmp(tok, "alpha") == 0 ) {
        char *t = strtok(NULL," ");
        if(t)
            psObj->fColour[3] = (float)atof(t);
    } else
    // Blend
    if( stricmp(tok, "blend") == 0 ) {
        char *t = strtok(NULL," ");
        if(t)
            psObj->nBlend  = Key_FindValue(t);
    } else
    // Colour
    if( stricmp(tok, "colour") == 0 ) {
        tok = strtok(NULL,",");
		psObj->fColour[0] = (float)atof(tok);
		tok = strtok(NULL,",");
		psObj->fColour[1] = (float)atof(tok);
		tok = strtok(NULL,",");
		psObj->fColour[2] = (float)atof(tok);
    } else
    // Size
    if( stricmp(tok, "size") == 0 ) {
        char *t = strtok(NULL," ");
        if(t)
            psObj->fSize = (float)atof(t);
    } else
    // Depth mask
    if( stricmp(tok, "depthmask") == 0 ) {
        char *t = strtok(NULL," ");
        if(t)
            psObj->nDepthMask = Key_FindValue(t);
    } else
    // Dimension
    if( stricmp(tok, "dimension") == 0 ) {
        tok = strtok(NULL,",");
		psObj->cDimensions.SetX( (float)atof(tok) );
		tok = strtok(NULL,",");
		psObj->cDimensions.SetY( (float)atof(tok) );		
		psObj->cDimensions.SetZ(0);
    } else

        printf("Script Warning: Unknown token '%s'\n",tok);
}


///////////////////
// Setup a default object
void CSceneScript::defaultObj(scr_object_t *psObj)
{
    psObj->cMatrix.LoadIdentity();
    psObj->cPosition = CVec(0,0,0);
    psObj->cDimensions = CVec(0,0,0);
    psObj->fColour[0] = psObj->fColour[1] = psObj->fColour[2] = 0.0f;
    psObj->fColour[3] = 1.0f;
    psObj->nBlend = BLEND_NONE;
    psObj->nType = sco_sprite;
    psObj->psTexture = NULL;
    psObj->szName[0] = '\0';
    psObj->fSize = 1;
    psObj->nDepthMask = true;
}