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


// Racing header file
// Created 22/8/02
// Jason Boettcher


#ifndef __RACING_H__
#define __RACING_H__


#include "CLightManager.h"
#include "CQuadTree.h"


// Racing modes
enum {
    race_red,
    race_racing,
    race_finished
};

#define     SPEED_SCALE     1.6f


// Race structure
typedef struct {

    CModel          *pcTrackModel;
    CCamera         cCamera;
    CQuadTree       cQuadTree;
    CCar            *pcRaceCar;
    CCar            *pcOppCar;
    CSky            cSky;
	float			fFinishLine;		// TODO: Properly

    CSceneScript    cSceneScript;

    CLightManager   cLightMan;

} race_t;



// Main routines
int		Race_Start(char *track);
int     Race_Load(char *szTrack);
void	Race_ProcessDriving(void);
void    Race_ProcessAI(carsim_t *psCar, carsim_t *psOpp);
void	Race_CheckCollisions(CCar *car);

void    Race_DrawHUD(CCar *pcCar);
void    Race_DrawRearView(CCar *pcCar);
void    Race_DrawFinished(void);

void    Race_InitMenu(void);
void    Race_ProcessMenu(void);



#endif  //  __RACING_H__