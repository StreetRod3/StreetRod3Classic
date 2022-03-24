/////////////////////////////////////////
//
//             Street Rod 3
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Font wrapper
// Created 29/5/02
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "font.h"


///////////////////
// Load the font
int Font_Load(char *name)
{
	//platformGL::font.Load(name);
	//platformGL::font.SetManualProjection(true);

	return true;
}


///////////////////
// Set the font size
void Font_SetSize(int s)
{
	platformGL::font.SetSize(s);
}



///////////////////
// Draw a piece of text
void Font_Draw(int x, int y, CVec col, char *fmt, ...)
{
	char buf[256];

	State_Enable(GL_BLEND);
	//glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_COLOR);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	platformGL::font.Color3f(col.GetX(),col.GetY(),col.GetZ());

	va_list arg;	
	va_start(arg, fmt);
	vsprintf(buf, fmt, arg);
	va_end(arg);

	platformGL::font.Draw(x,y,buf);
}

//}		// End of glFont stuff


