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


#ifndef __CSCENESCRIPT_H__
#define __CSCENESCRIPT_H__


// Scene object types
enum {
    sco_sprite,
    sco_quad,
    sco_corona
};


// Scene object structure
typedef struct scr_object_s {

    char        szName[128];
    int         nType;
    float       fSize;
    CVec        cDimensions;
    CVec        cPosition;
    CMatrix     cMatrix;
    texture_t   *psTexture;
    float       fColour[4];
    int         nBlend;
    int         nDepthMask;

    struct scr_object_s *psNext;

} scr_object_t;




// Scene script class
class CSceneScript {
private:
    // Attributes

    scr_object_t    *m_psObjectList;

public:
    // Methods

    CSceneScript();

    bool            load(char *szFilename, CModel *pcScene);
    void            shutdown(void);

    void            process(void);
    void            draw(CCamera *pcCam);


private:
    // Private Methods

    void            commonDraw(scr_object_t *psObj);
    void            drawSprite(scr_object_t *psObj, CCamera *pcCam);
    void            drawQuad(scr_object_t *psObj);
    void            drawCorona(scr_object_t *psObj, CCamera *pcCam);

    int             readNum(char *szLine, int nPos, int *nNumber);
    void            translateLine(char *szLine, scr_object_t *psObj);    

    void            addObject(char *szName, scr_object_t *psObj, CModel *pcScene);
    void            defaultObj(scr_object_t *psObj);
};






#endif  //  __CSCENESCRIPT_H__