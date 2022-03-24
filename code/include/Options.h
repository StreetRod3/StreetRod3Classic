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

// Options header file
// Created 23/03/03
// Jason Boettcher


#ifndef __OPTIONS_H__
#define __OPTIONS_H__


// Controls
enum {
    INP_ACCELERATE = 0,
    INP_BRAKE,
    INP_STEERLEFT,
    INP_STEERRIGHT,
    INP_UPGEAR,
    INP_DOWNGEAR,
    INP_CHANGECAMERA
};


// Options structure
typedef struct {

    // Video
    int     nFullscreen;
    int     nColourDepth;
    int     nZDepth;
	int		nMultiTextureEnabled;

    // Sound
    int     nSoundOn;
    int     nSoundVolume;
    int     nMusicVolume;

    // Mouse
    float   fSensitivity;
    float   fRotateSensitivity;
    int     nInverseX;
    int     nInverseY;

    // Controls
    char    szControls[32][7];

    // Misc
    int     nShowFPS;
    int     nDeveloper;
    int     nQuadTree;

} option_t;


extern  option_t    *psOptions;


bool    LoadOptions(void);
void    ShutdownOptions(void);
char    *getBool(char *str, int value);



#endif  //  __OPTIONS_H__