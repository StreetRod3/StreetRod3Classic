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


// Entity header file
// Created 18/3/03
// Jason Boettcher


#ifndef __ENTITY_H__
#define __ENTITY_H__


#define     MAX_ENTITIES    1024


// Entity types
enum {
    ent_spark
};


// Entity structure
typedef struct {
    bool    bUsed;

    int     nType;
    int     nType2;
    CVec    cPos;
    CVec    cOldPos;
    CVec    cVel;
    float   fLife;
    float   fAge;
} entity_t;


// Functions
bool    InitializeEntities(void);
void    ClearEntities(void);
void    ShutdownEntities(void);

entity_t    *SpawnEntity(int nType, int nType2, CVec cPos, CVec cVel, float fAge);
void        SimulateEntities(float dt);
void        DrawEntities(void);


#endif  //  __ENTITY_H__