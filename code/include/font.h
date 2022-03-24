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


// Font stuff
// Created 29/5/02
// Jason Boettcher


#ifndef __FONT_H__
#define __FONT_H__



// Routines
int		Font_Load(char *name);
void	Font_Draw(int x, int y, CVec col, char *fmt, ...);
void	Font_DrawCentered(int x, int y, CVec col, char *fmt, ...);
void    Font_DrawShadow(int x, int y, CVec col, CVec shad, char *fmt, ...);
void	Font_SetSize(int s);
void    Font_SetAlpha(float a);




#endif  //  __FONT_H__