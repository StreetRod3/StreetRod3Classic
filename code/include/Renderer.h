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


// Main renderer
// Created 13/5/02
// Jason Boettcher


#ifndef __RENDERER_H__
#define __RENDERER_H__



// Routines
void	InitializeRenderer(void);
void	SetupFrame(void);


void	Render_UseOrthoProj(void);
void	Render_UseNormalProj(void);

void	Render_Begin2D(void);
void	Render_End2D(void);

void	Renderer_SetLightPos(CVec pos);

void    DrawPic(texture_t *psTexture, int x, int y);
void    DrawPicEx(texture_t *psTexture, int x, int y, int x2, int y2);
void    BlitTexture(texture_t *psTex, int sx, int sy, int dx, int dy, int w, int h, float mw, float mh);

void    RenderBillboardSprite(CVec cPos, texture_t *psTex, CCamera *pcCam, float fSize, float fRoll);


#endif  //  __RENDERER_H__