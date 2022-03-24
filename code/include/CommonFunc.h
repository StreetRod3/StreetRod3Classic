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


// Common functions
// Created 23/5/02
// Jason Boettcher


#ifndef __COMMONFUNCS_H__
#define __COMMONFUNCS_H__



// Routines
void	TrimSpaces(char *str);
Uint32	GetPixel(SDL_Surface *bmpSrc, int x, int y);
void	GetColour4(Uint32 pixel, SDL_Surface *img, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a);
void    sr_strncpy(char *dest, char *src, int count);
void    ConvertNewlines(char *str);
char    *newString(char *szString);
char    *addString(char *dst, char *src);
void    writePascalString(char *szString, FILE *fp);
char    *readPascalString(char *szString, FILE *fp);
char    *wrapText(char *szString, int nWidth, int nFontSize);


#endif  //  __COMMONFUNCS_H__