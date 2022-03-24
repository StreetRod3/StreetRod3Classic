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



// gui Colour picker class
// Created 3/8/03
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "gui.h"
#include "Renderer.h"


int   g_fColourPickList[] = {
    // White to Black
    255,255,255,    204,204,204,    153,153,153,    102,102,102,    51,51,51,       0,0,0,    
    // Primary Colours
    255,0,0,        0,255,0,        0,0,255,        255,255,0,      0,255,255,      255,0,255,
    // Shades
    16,20,82,       41,40,173,      82,20,16,       107,40,107,     123,60,189,     173,40,41,
    189,60,123,     214,85,214,     255,130,8,      214,150,148,    239,170,239,    107,105,41,
    123,125,123,    148,150,214,    16,85,16,       41,105,107,     57,125,185,     41,170,41,
    57,190,123,     82,215,214,     123,190,57,     148,215,148,    173,235,239,    239,235,173
};




///////////////////
// CColourPicker Constructor
CColourPicker::CColourPicker()
{
    m_nCurrentColour = 0;
}


///////////////////
// Return the colour
CVec CColourPicker::getCurrentColour(void)
{
    return CVec( (float)g_fColourPickList[m_nCurrentColour*3] / 255.0f,
                 (float)g_fColourPickList[m_nCurrentColour*3+1] / 255.0f,
                 (float)g_fColourPickList[m_nCurrentColour*3+2] / 255.0f);
}


///////////////////
// Create the colour picker widget
void CColourPicker::Create(void)
{
    m_nNumColumns = 6;
    m_nNumRows = 6;
}


///////////////////
// Destroy the colour picker widget
void CColourPicker::Destroy(void)
{
}


///////////////////
// Draw
void CColourPicker::Draw(void)
{
    int x,y;    
    float i,j;
    int col = 0;
    int butWidth = iWidth/m_nNumColumns;
    int butHeight = iHeight/m_nNumRows;
    
    State_Disable(GL_TEXTURE_2D);
    State_Disable(GL_BLEND);
    
    for(y=0; y<m_nNumRows; y++) {
        for(x=0; x<m_nNumColumns; x++) {

            CVec colour = CVec( (float)g_fColourPickList[col*3] / 255.0f,
                             (float)g_fColourPickList[col*3+1] / 255.0f,
                             (float)g_fColourPickList[col*3+2] / 255.0f );

            i = iX+x*butWidth;
            j = iY-y*butHeight;

            DrawRect(i,j, i+butWidth,j-butHeight, CVec(0,0,0), colour, 1,true,0);            
            DrawRect(i,j, i+butWidth,j-butHeight, CVec(0,0,0), colour, 1,false,1);

            col++;
            if(col >= sizeof(g_fColourPickList))
                return;
        }
    }

    // Draw the current selection
    col=0;
    for(y=0; y<m_nNumRows; y++) {
        for(x=0; x<m_nNumColumns; x++) {
            i = iX+x*butWidth;
            j = iY-y*butHeight;

            if(col == m_nCurrentColour)
                DrawRect(i,j, i+butWidth,j-butHeight, CVec(1,1,1), CVec(0,0,0), 1,false,1);
            col++;
            if(col >= sizeof(g_fColourPickList))
                return;
        }
    }
}


///////////////////
// MouseOver event
int CColourPicker::MouseOver(mouse_t *tMouse)
{
    return CPK_NONE;
}


///////////////////
// MouseDown event
int CColourPicker::MouseDown(mouse_t *tMouse)
{
    captureMouse();
    int x,y;
    float i,j;
    int col = 0;
    int butWidth = iWidth/m_nNumColumns;
    int butHeight = iHeight/m_nNumRows;   
    
     for(y=0; y<m_nNumRows; y++) {
        for(x=0; x<m_nNumColumns; x++) {
            i = iX+x*butWidth;
            j = iY-y*butHeight;

            if(MouseinRect(i,600-j,i+butWidth,600-(j-butHeight))) {
                m_nCurrentColour = col;
                return CPK_CHANGED;
            }

            col++;
            if(col >= sizeof(g_fColourPickList))
                return CPK_NONE;
        }
    }

    return CPK_NONE;
}


///////////////////
// MouseUp event
int CColourPicker::MouseUp(mouse_t *tMouse)
{
    releaseMouse();

    return CPK_NONE;
}


///////////////////
// KeyDown event
int CColourPicker::KeyDown(char c)
{
    return CPK_NONE;
}


///////////////////
// KeyUp event
int CColourPicker::KeyUp(char c)
{
    return CPK_NONE;
}
