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


// Keyword Manager
// Created 23/5/02
// Jason Boettcher


#include "defs.h"
#include "System.h"
#include "3d.h"
#include "game.h"


// Keywords
keyword_t tKeywords[] = {

	// Standard
	{"true",			true},
	{"false",			false},

	// Material keywords
	{"b_none",			BLEND_NONE},
	{"b_oneone",		BLEND_ONEONE},
	{"b_onezero",		BLEND_ONEZERO},
	{"b_alpha",			BLEND_ALPHA},
	{"b_colour",		BLEND_COLOUR},
    {"b_alphaone",      BLEND_ALPHAONE},

	{"tc_none",			TC_NONE},
	{"tc_sphere",		TC_SPHERE},
	{"tc_eye_linear",	TC_EYE_LINEAR},

	// Part types
	{"PRT_BLOCK",		PRT_BLOCK},
	{"PRT_INTAKEMAN",	PRT_INTAKEMAN},
	{"PRT_CARBY",		PRT_CARBY},
	{"PRT_AIRFILTER",	PRT_AIRFILTER},
	{"PRT_ALTER",		PRT_ALTER},
	{"PRT_DIST",		PRT_DIST},
	{"PRT_STARTER",		PRT_STARTER},
	{"PRT_FAN",			PRT_FAN},

	{"PRT_TIRE",		PRT_TIRE},
	{"PRT_DIFF",		PRT_DIFF},
	{"PRT_TRANS",		PRT_TRANS},
	{"PRT_LMUFFLER",	PRT_LMUFFLER},
	{"PRT_RMUFFLER",	PRT_RMUFFLER},
    {"PRT_SHOCK",	    PRT_SHOCK},

    {"small",           SZ_SMALL},
    {"medium",          SZ_MEDIUM},
    {"large",           SZ_LARGE},

    {"MAN_ANY",         MAN_ANY},
    {"MAN_GM",          MAN_GM},
    {"MAN_FORD",        MAN_FORD},
    {"MAN_CHRYSLER",    MAN_CHRYSLER},

    {"Sprite",          sco_sprite},
    {"Quad",            sco_quad},
    {"Corona",          sco_corona},

    {"NormalHood",      HOOD_NORMAL},
    {"PerformanceHood", HOOD_PERFORMANCE},

};



int		iNumKeywords = 0;

///////////////////
// Initialize the keywords
void Key_Initialize(void)
{
	iNumKeywords = sizeof(tKeywords) / sizeof(keyword_t);
}


///////////////////
// Find a keyword
int Key_FindValue(char *sName)
{
	for(int i=0;i<iNumKeywords;i++) {
		if(stricmp(tKeywords[i].sName,sName) == 0)
			return tKeywords[i].iValue;
	}

	// Nothing
	return -666;
}