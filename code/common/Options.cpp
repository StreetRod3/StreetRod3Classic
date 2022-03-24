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


// Options
// Created 23/03/03
// Jason Boettcher


#include "defs.h"
#include "System.h"
#include "Options.h"


option_t    *psOptions = NULL;


///////////////////
// Load the options
bool LoadOptions(void)
{
    // Allocate the options structure
    psOptions = new option_t;
    if( !psOptions ) {
        // TODO: System Error
        return false;
    }


    // Load the options
    // Video
    ReadKeyword( "config.cfg", "Video", "Fullscreen", &psOptions->nFullscreen, true );
    ReadInteger( "config.cfg", "Video", "ColourDepth", &psOptions->nColourDepth, 16 );
    ReadInteger( "config.cfg", "Video", "ZDepth", &psOptions->nZDepth, 16 );
	ReadKeyword( "config.cfg", "Video", "MultiTextureEnabled", &psOptions->nMultiTextureEnabled, true );

    // Mouse
    ReadFloat( "config.cfg", "Mouse", "Sensitivity", &psOptions->fSensitivity, 1.0f );
    ReadFloat( "config.cfg", "Mouse", "RotateSensitivity", &psOptions->fRotateSensitivity, 1.0f );
    ReadKeyword( "config.cfg", "Mouse", "InverseX", &psOptions->nInverseX, false );
    ReadKeyword( "config.cfg", "Mouse", "InverseY", &psOptions->nInverseY, false );

    // Sound
    ReadKeyword( "config.cfg", "Sound", "Sound", &psOptions->nSoundOn, true );
    ReadInteger( "config.cfg", "Sound", "Volume", &psOptions->nSoundVolume, 100 );
    ReadInteger( "config.cfg", "Sound", "MusicVolume", &psOptions->nMusicVolume, 100 );

    // Controls
    ReadString( "config.cfg", "Controls", "Accelerate",     psOptions->szControls[INP_ACCELERATE],  "up" );
    ReadString( "config.cfg", "Controls", "Brake",          psOptions->szControls[INP_BRAKE],       "down" );
    ReadString( "config.cfg", "Controls", "SteerLeft",      psOptions->szControls[INP_STEERLEFT],   "left" );
    ReadString( "config.cfg", "Controls", "SteerRight",     psOptions->szControls[INP_STEERRIGHT],  "right" );
    ReadString( "config.cfg", "Controls", "UpGear",         psOptions->szControls[INP_UPGEAR],      "a" );
    ReadString( "config.cfg", "Controls", "DownGear",       psOptions->szControls[INP_DOWNGEAR],    "z" );
    ReadString( "config.cfg", "Controls", "ChangeCamera",   psOptions->szControls[INP_CHANGECAMERA],"c" );

    // Misc
    ReadKeyword( "config.cfg", "Misc", "ShowFPS", &psOptions->nShowFPS, false );
    ReadKeyword( "config.cfg", "Misc", "Developer", &psOptions->nDeveloper, false );
    ReadKeyword( "config.cfg", "Misc", "QuadTree", &psOptions->nQuadTree, true );

    // Controls

    return true;
}


///////////////////
// Shutdown the options
void ShutdownOptions(void)
{
    char    str[16];

    if( !psOptions )
        return;

    // Save the options
    FILE *opt = fopen( "config.cfg", "wt" );
    if( !opt ) {
        writeLog("Warning: Could not write to config.cfg");
        return;
    }

    fprintf( opt, "# Street Rod 3 Options\n" );
    fprintf( opt, "# Note: This is an automatically generated file\n" );
    
    // Video
    fprintf( opt, "\n[Video]\n" );
    fprintf( opt, "Fullscreen = %s\n", getBool(str, psOptions->nFullscreen) );
    fprintf( opt, "ColourDepth = %d\n", psOptions->nColourDepth );
    fprintf( opt, "ZDepth = %d\n", psOptions->nZDepth );
	fprintf( opt, "MultiTextureEnabled = %s\n", getBool(str, psOptions->nMultiTextureEnabled) );

    // Mouse
    fprintf( opt, "\n[Mouse]\n" );
    fprintf( opt, "Sensitivity = %.2f\n", psOptions->fSensitivity );
    fprintf( opt, "RotateSensitivity = %.2f\n", psOptions->fRotateSensitivity );
    fprintf( opt, "InverseX = %s\n", getBool(str, psOptions->nInverseX) );
    fprintf( opt, "InverseY = %s\n", getBool(str, psOptions->nInverseY) );

    // Sound
    fprintf( opt, "\n[Sound]\n" );
    fprintf( opt, "Sound = %s\n", getBool(str, psOptions->nSoundOn) );
    fprintf( opt, "Volume = %d\n", psOptions->nSoundVolume );
    fprintf( opt, "MusicVolume = %d\n", psOptions->nMusicVolume );

    // Controls
    fprintf( opt, "\n[Controls]\n" );
    fprintf( opt, "Accelerate = %s\n",  psOptions->szControls[INP_ACCELERATE] );
    fprintf( opt, "Brake = %s\n",       psOptions->szControls[INP_BRAKE] );
    fprintf( opt, "SteerLeft = %s\n",   psOptions->szControls[INP_STEERLEFT] );
    fprintf( opt, "SteerRight = %s\n",  psOptions->szControls[INP_STEERRIGHT] );
    fprintf( opt, "UpGear = %s\n",      psOptions->szControls[INP_UPGEAR] );
    fprintf( opt, "DownGear = %s\n",    psOptions->szControls[INP_DOWNGEAR] );
    fprintf( opt, "ChangeCamera = %s\n",psOptions->szControls[INP_CHANGECAMERA] );

    // Misc
    fprintf( opt, "\n[Misc]\n" );
    fprintf( opt, "ShowFPS = %s\n", getBool(str, psOptions->nShowFPS) );
    fprintf( opt, "Developer = %s\n", getBool(str, psOptions->nDeveloper) );
    fprintf( opt, "QuadTree = %s\n", getBool(str, psOptions->nQuadTree) );

    fclose( opt );
}


///////////////////
// Get a string from a boolean value
char *getBool(char *str, int value)
{
    if( value )
        strcpy( str, "true" );
    else
        strcpy( str, "false" );
    return str;
}