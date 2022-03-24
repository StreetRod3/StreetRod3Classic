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


// keyword manager
// Created 23/5/02
// Jason Boettcher


#ifndef __KEYWORDMAN_H__
#define __KEYWORDMAN_H__


// Keyword structure
typedef struct {
	char	sName[64];
	int		iValue;
} keyword_t;


// Routines
void	Key_Initialize(void);
int		Key_FindValue(char *sName);





#endif  //  __KEYWORDMAN_H__