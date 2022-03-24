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


// Opponent class
// Created 16/7/03
// Jason Boettcher


#include "defs.h"
#include "game.h"


///////////////////
// COpponent constructor
COpponent::COpponent()
{
    m_szName[0] = '\0';
    m_pcCar = NULL;
}


///////////////////
// Load the opponent
bool COpponent::load(char *szName, char *szCarSetup)
{
    sr_strncpy(m_szName, szName, 63);

    // Load the car
    m_pcCar = new CCar;
    if( !m_pcCar )
        return false;

    if( !m_pcCar->LoadSetup(szCarSetup) ) {
        delete m_pcCar;
        return false;
    }

    m_pcCar->calculateRest(CVec(0,0,0));

    return true;
}


///////////////////
// Rotate the car
void COpponent::rotate(CMatrix m)
{
    m_pcCar->applyMatrix(m);
}


///////////////////
// Zero the rotation of the car
void COpponent::zeroRotation(void)
{
    m_pcCar->getMatrix()->LoadIdentity();
}


///////////////////
// Draw the opponent
void COpponent::draw(CVec cPosition)
{
    m_pcCar->setPosition(cPosition);
    m_pcCar->Draw();
}


///////////////////
// Create a description for the diner
char *COpponent::getDescription(char *buf)
{
    sprintf(buf, "%s  %s", m_szName, m_pcCar->getName());
    return buf;
}


///////////////////
// Check for intersection between the car and a ray
bool COpponent::intersect(CVec pos, CVec end)
{
    end -= pos;

    VectorNormalize(&end);
    
    if( m_pcCar->LineIntersect(pos, end, false) )
        return true;   
   
    return false;
}