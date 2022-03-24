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


// Material class
// Created 8/5/02
// Jason Boettcher


#ifndef __CMATERIAL_H__
#define __CMATERIAL_H__


#define		MAX_PASSES		8


// Pass structure
typedef struct {
	int			iHasTexture;
	texture_t	*tTexture;
	int			iBlending;
	int			iTcGen;
	float		fAlpha;
	colour_t	tColour;
	int			iBackCull;
	int			iLightmap;
    int         iUseColour;
    int         iLighting;
} pass_t;


// Blending types
enum {
	BLEND_NONE=0,
	BLEND_ONEONE,
	BLEND_ONEZERO,
	BLEND_ALPHA,
	BLEND_COLOUR,
    BLEND_ALPHAONE
};

// Texture generation
enum {
	TC_NONE=0,
	TC_SPHERE,
	TC_EYE_LINEAR
};





class CMaterial {
public:
	// Constructor
	CMaterial() {
        nID = 0;
		iNumPasses = 0;
		tLightmap = NULL;
        bLoaded = false;

		iContainsBlending = false;

	}


private:
	// Attributes
    int         nID;
	char		sName[64];
    bool        bLoaded;


	// Passes
	int			iNumPasses;
	pass_t		tPasses[MAX_PASSES];

	texture_t	*tLightmap;

	int			iContainsBlending;


	
public:
	// Methods

	void		Load(int id, FILE *fp);
	void		TranslateFunction(char *sLine, int p);
	
	void		setupPass(int p, int nLighting);


	// Variables
	char		*getName(void)				{ return sName; }

	int			getNumPasses(void)			{ return iNumPasses; }
	pass_t		*getPass(int p)				{ return &tPasses[p]; }
	
	int			getContainsBlending(void)	{ return iContainsBlending; }
	int			hasLightmap(int pass)		{ return tPasses[pass].iLightmap; }
	void		setLightmap(texture_t *tex)	{ tLightmap = tex; }

    int         getID(void)                 { return nID; }
    bool        isLoaded(void)              { return bLoaded; }
	


	







};




#endif  //  __CMATERIAL_H__