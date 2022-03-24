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


// Cache
// Created 7/5/02
// Jason Boettcher


#ifndef __CACHE_H__
#define __CACHE_H__


#define		MAX_CACHE		2048


// Cache types
#define		CCH_TEXTURE		0
#define		CCH_SAMPLE		1
#define		CCH_MODEL		2


// A cache item
typedef struct {

	int			iUsed;
	int			iType;	
	char		sFilename[128];

	texture_t	*tTexture;
	CModel		*cModel;
	HSAMPLE		smpSample;

} cache_t;


// Cache routines
int			Cache_Initialize(void);
void		Cache_Shutdown(void);
cache_t		*Cache_FindUnused(void);
cache_t		*Cache_FindMatch(char *sFilename);

texture_t	*Cache_LoadTexture(char *sFilename);
CModel		*Cache_LoadModel(char *sFilename);
HSAMPLE		Cache_LoadSample(char *sFilename, int max);






#endif  //  __CACHE_H__