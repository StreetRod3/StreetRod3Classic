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


// Config file handler
// Created 30/9/01
// By Jason Boettcher


#ifndef __CONFIGHANDLER_H__
#define __CONFIGHANDLER_H__


#include "CVec.h"


#define		MAX_STRING_LENGTH	4096
#define		MAX_MINOR_LENGTH	256


// Internal
int		GetString(char *filename, char *section, char *key, char *string);

// Value reading
int		ReadString(char *filename, char *section, char *key, char *value, char *defaultv);
int		ReadInteger(char *filename, char *section, char *key, int *value, int defaultv);
int		ReadFloat(char *filename, char *section, char *key, float *value, float defaultv);
int		ReadKeyword(char *filename, char *section, char *key, int *value, int defaultv);
//int		ReadVec2d(char *filename, char *section, char *key, CVec2d *value);
int		ReadVec(char *filename, char *section, char *key, CVec *value);
//int		ReadRegion(char *filename, char *section, char *key, CRegion *value);







#endif  //  __CONFIGHANDLER_H__