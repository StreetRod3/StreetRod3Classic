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


#ifndef __CLIGHTMANAGER_H__
#define __CLIGHTMANAGER_H__


/*
Notes:

  The light manager holds all the lights in the scene. Because the 3d hardware has a limit of lights,
  we pick the n closest lights and only use them.

*/



// Used light structure
typedef struct {

    int     nUsed;
    float   fDistance;
    light_t *psLight;

} usedlight_t;



class CLightManager {
private:
    // Attributes

    // Scene lights
    int         m_nLights;
    light_t     *m_psLights;

    // Used lights
    int         m_nMaxLights;
    usedlight_t *m_psUsedLights;


public:
    // Methods

    CLightManager();

    void        Setup(int nMaxLights, int nNumLights);
    light_t     *getLights(void);

    void        calculateLights(CVec carPos);
    void        useLights(void);

};


#endif  //  __CLIGHTMANAGER_H__