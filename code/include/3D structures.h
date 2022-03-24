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


// 3D Structures
// Created 7/5/01
// By Jason Boettcher


#ifndef __3DSTRUCTURES__
#define __3DSTRUCTURES__



// Texture Coordinate structure
typedef struct {
	float		U,V;
} texcoord_t;



// Colour structure
typedef struct {
	float		R,G,B;
} colour_t;

// Vector structure
typedef struct {
	float       X,Y,Z;
} vector_t;



// Plane structure
typedef struct {
	CVec		vNormal;
	float		fDistance;
} plane_t;


// Mapping channel
typedef struct {
    uint        nTexCoord[3];
} mapchan_t;


// Texture structure
typedef struct {
	SDL_Surface	*bmpImage;
	int			iUploaded;
	GLuint		iID;

	int			iFormat;
    int         nWrap_S, nWrap_T;
	int			iBytesPerPixel;
	int			iWidth, iHeight;
	int			iMipmaps;
	bool		bMipFiltering;
} texture_t;



// Polygon flags
#define         PLY_HIDDEN      0x0001


// Polygon structure
typedef struct {
	
    ushort      nID;
    uint        nFlags;
    uint        nTextureID;
    ushort      nLightMapID;

	uint		nVertex[3];
    uint        nVertexNormal[3];
    uchar       nNumChannels;
    mapchan_t   *psChannels;
    struct mesh_s   *psMesh;

	plane_t		sPlane;

    // Decals
    texcoord_t  sDecalCoords[3];    

} polygon_t;


#endif  //  __3DSTRUCTURES__