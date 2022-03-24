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



// Font wrapper
// Created 29/5/02
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "font.h"
#include "text.h"

text_t text;
float   FontAlpha = 1;

///////////////////
// Load the font
int Font_Load(char *name)
{
	//platformGL::font.Load(name);
	//platformGL::font.SetManualProjection(true);
    text.Load(name);

	return true;
}


///////////////////
// Set the font size
void Font_SetSize(int s)
{
	//platformGL::font.SetSize(s);
    text.Size(s);
}


///////////////////
// Set the font alpha
void Font_SetAlpha(float a)
{
    FontAlpha = a;
}



///////////////////
// Draw a piece of text
void Font_Draw(int x, int y, CVec col, char *fmt, ...)
{
	char buf[1024];

	State_Enable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    text.Color4f(col.GetX(), col.GetY(), col.GetZ(), FontAlpha);

	va_list arg;	
	va_start(arg, fmt);
	vsprintf(buf, fmt, arg);
	va_end(arg);

	text.Draw(x, y, buf);
}


///////////////////
// Draw a piece of text shadowed
void Font_DrawShadow(int x, int y, CVec col, CVec shad, char *fmt, ...)
{
	char buf[1024];

	State_Enable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);    

	va_list arg;	
	va_start(arg, fmt);
	vsprintf(buf, fmt, arg);
	va_end(arg);

    text.Color4f(shad.GetX(), shad.GetY(), shad.GetZ(), FontAlpha);
	text.Draw(x+2, y-2, buf);
    text.Color4f(col.GetX(), col.GetY(), col.GetZ(), FontAlpha);
	text.Draw(x, y, buf);
}


///////////////////
// Draw a piece of text centered
void Font_DrawCentered(int x, int y, CVec col, char *fmt, ...)
{
    char buf[1024];

    va_list arg;	
	va_start(arg, fmt);
	vsprintf(buf, fmt, arg);
	va_end(arg);

    int w = strlen(buf) * text.GetSize();

    Font_Draw( x - w/2, y, col, buf );
}