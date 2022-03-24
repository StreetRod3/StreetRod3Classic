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


// Texture manager
// Created 12/5/02
// Jason Boettcher


#ifndef __TEXTUREMANAGER_H__
#define __TEXTUREMANAGER_H__


#define     MAX_TEXDIRLIST      8

// Texture dir list
typedef struct {

    bool    bUsed;
    char    szDir[256];

} texdirlist_t;


// Routines
void        Tex_Initialize(void);
texture_t	*Tex_Load(char *sFilename);
texture_t   *Tex_Create(int nWidth, int nHeight);
void		Tex_Free(texture_t *tex);
void		Tex_Upload(texture_t *tex, int force = false);
void		Tex_Bind(texture_t *tex);

void        Tex_AddDir(char *szDir);
void        Tex_RemoveDir(char *szDir);
char        *Tex_GetDir(char *buf, char *texture, int i);
bool        Tex_DirOK(int i);

void        Tex_EnableMultitexturing(bool bEnable);
void        Tex_SelectTexture(int nTex);


#endif  //  __TEXTUREMANAGER_H__