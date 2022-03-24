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

// Menu system
// Created 4/3/03
// Jason Boettcher


#ifndef __MENU_H__
#define __MENU_H__


// Menu modes
enum {
    MNU_NEWGAME,
    MNU_LOADGAME,
    MNU_QUIT
};



// Functions
int     Menu_Initialize(bool bShowLogo);
void    Menu_Logo(void);
int     Menu_Main(void);
void    Menu_Options(void);
void    Menu_ControlOptions(void);
bool    Menu_LoadGame(void);
void    Menu_EnterName(void);



#endif  //  __MENU_H__