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


// System stuff
// Created 7/5/02
// Jason Boettcher


#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "CVec.h"
#include "glext.h"

// Keyboard structure
typedef struct {
	Uint8	*keys;
	Uint8	KeyUp[SDLK_LAST];
	Uint8	KeyDown[SDLK_LAST];
} keyboard_t;


// Mouse structure
typedef struct {
	int		X,Y;
	CVec	vPos;
	int		Button;

	int		Up;
	int		Down;
} mouse_t;


// The sr3 global structure
typedef struct {
	float	fCurTime;
	float	fDeltaTime;
    
    // Debugging variables
	float	f1, f2;
	int		i1, i2;
	CVec	v1, v2;
    uint    TriCount;
} sr3_t;



// System routines
void	System_Shutdown(void);
int		System_InitializeOpenGL(void);
void	System_InitializeInput(void);
void    System_InitializeSound(void);
void	System_ProcessInput(void);
int     GetFPS(void);

void    ParseArguments(int argc, char *argv[]);

keyboard_t  *System_GetKeyboard(void);
mouse_t     *System_GetMouse(void);
SDL_Event   *System_GetEvent(void);
bool        System_MultiTextureEnabled(void);

void    System_TakeScreenshot(void);


// Logging routines
void	writeLog(char *fmt, ...);
void	closeLog(void);


extern	sr3_t	tMainSR3;


// Multi-texturing
//PFNGLACTIVETEXTUREPROC  glActiveTextureARB
/*extern	void ( APIENTRY * glMultiTexCoord2fARB)( GLenum, GLfloat, GLfloat );
extern	void ( APIENTRY * glActiveTextureARB)( GLenum );
extern	void ( APIENTRY * glClientActiveTextureARB)( GLenum );*/
extern PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
extern PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;


extern  int GL_TEXTURE0, GL_TEXTURE1;




#endif  //  __SYSTEM_H__