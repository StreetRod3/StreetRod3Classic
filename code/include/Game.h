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


// Main game header file
// Created 13/5/02
// Jason Boettcher


#ifndef __GAME_H__
#define __GAME_H__


#define VERSION     0.42


// Command structure
typedef struct {
	float	Steering;		// -1 = full left; 1 = full right
	int		Accel;
	int		Brake;
	int		Handbrake;
} car_command_t;


#include "3D.h"

// Car simulation
#include "CarSimulation.h"


#include "CNut.h"
#include "CPart.h"
#include "CCar.h"
#include "PartPool.h"
#include "CSceneScript.h"
#include "CLocation.h"
#include "Garage.h"
#include "CSky.h"
#include "COpponent.h"
#include "Diner.h"
#include "CGUI.h"
#include "Renderer.h"
#include "CTaskBar.h"
#include "CPlayer.h"
#include "CSidebar.h"
#include "Options.h"

#include "Sound.h"
#include "CRadio.h"


// Locations
#define		LOC_EXIT		0
#define		LOC_GARAGE		1
#define		LOC_DINER   	2
#define		LOC_GASSTATION	3
#define		LOC_CITYDRIVING	4
#define		LOC_RACE		5


#define	SIM_GRAVITY	        9.8f
#define LIMIT(a,b,c)        (((a)>(b)) ? (a) : (((b)>(c)) ? (c) : (b)))



//
// Race types
//
enum {
    rc_drag,
    rc_road
};

// Bet types
enum {
    rcb_kicks,
    rcb_pink,
    rcb_money
};

// Race details structure
typedef struct {
    int         nType;
    int         nBetType;
    int         nBetMoney;

    char        szTrackDir[128];
    COpponent   *pcOpponent;

} race_details_t;



// Routines
void	Game_Start(void);
void	Game_SetLocation(int l);
void    Game_NewGame(void);
void    Game_LoadGame(int nSlot);
void    Game_QuittoMenu(void);

void    Game_InitBook(void);
void    Game_DrawBook(void);


// Globals
extern		CPlayer		*cMainGuy;
extern      CRadio      *pcRadio;
extern      race_details_t  sRaceInfo;

#endif  //  __GAME_H__