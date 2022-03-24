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


// Texture manager
// Created 12/5/02
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "System.h"


texdirlist_t    sTextureDirList[MAX_TEXDIRLIST];
int     nCurrentTexUnit = 0;
int     nCurrentTexID[2];


///////////////////
// Initialize the texture manager
void Tex_Initialize(void)
{
    //memset( sTextureDirList, 0, sizeof(sTextureDirList) );
    for(int i=0; i<MAX_TEXDIRLIST; i++) {
        sTextureDirList[i].bUsed = false;
        sTextureDirList[i].szDir[0] = '\0';
    }

    // Add base texture dir
    Tex_AddDir("data/textures");

    nCurrentTexUnit = 0;
    nCurrentTexID[0] = nCurrentTexID[1] = -666;
}


///////////////////
// Load a texture
texture_t *Tex_Load(char *sFilename)
{
	texture_t *tex;

	// Allocate it
	tex = new texture_t;
	if(tex == NULL)
		return NULL;

	// Load the surface
	tex->bmpImage = IMG_Load(sFilename);
	if(tex->bmpImage == NULL) {
		//17/06/2005
		writeLog("Texture Load: file \"%s\" not found\n",sFilename);
		//
		delete tex;
		return NULL;
	}

	// Check for 8bit textures (bad)
	/*if(tex->bmpImage->format->BitsPerPixel == 8) {
		writeLog("%s is an 8 bit image, not loading\n",sFilename);
		SDL_FreeSurface(tex->bmpImage);
		delete tex;
		return NULL;
	}*/

	// Generate the id for the texture
	glGenTextures(1, &tex->iID);

	tex->iUploaded = false;

    tex->iBytesPerPixel = tex->bmpImage->format->BytesPerPixel;
	tex->iWidth = tex->bmpImage->w;
	tex->iHeight = tex->bmpImage->h;
	tex->iMipmaps = true;
	tex->bMipFiltering = true;
    tex->nWrap_S = GL_REPEAT;
    tex->nWrap_T = GL_REPEAT;

    // Set the appropriate format

    // 8bpp
    if( tex->iBytesPerPixel == 1 ) {
        tex->iFormat = GL_ALPHA;
        tex->iBytesPerPixel = GL_ALPHA;
    } else
    // 24bpp
    if( tex->iBytesPerPixel == 3 ) {
        tex->iFormat = GL_RGB;
        if( tex->bmpImage->format->Rshift == 16 )
            tex->iFormat = GL_BGR_EXT;
       
    } else
    // 32bpp
    if( tex->iBytesPerPixel == 4 ) {
        tex->iFormat = GL_RGBA;
        if( tex->bmpImage->format->Rshift == 16 )
            tex->iFormat = GL_BGRA_EXT;
    }
   	
	return tex;
}


///////////////////
// Create a texture
texture_t *Tex_Create(int nWidth, int nHeight)
{
    texture_t *tex;

	// Allocate it
	tex = new texture_t;
	if(tex == NULL)
		return NULL;
    
	tex->bmpImage = SDL_CreateRGBSurface(SDL_SWSURFACE, nWidth, nHeight, 24, 
								0,0,0,0);

    // Generate the id for the texture
	glGenTextures(1, &tex->iID);

	tex->iUploaded = false;

	tex->iBytesPerPixel = tex->bmpImage->format->BytesPerPixel;
	tex->iWidth = tex->bmpImage->w;
	tex->iHeight = tex->bmpImage->h;
	tex->iMipmaps = false;
	tex->bMipFiltering = true;
    tex->nWrap_S = GL_REPEAT;
    tex->nWrap_T = GL_REPEAT;

	// Setup the opengl parameters
	tex->iFormat = GL_RGB;    
    
    return tex;
}


///////////////////
// Free a texture
void Tex_Free(texture_t *tex)
{
	if(tex) {

		// Free the surface
		if(tex->bmpImage)
			SDL_FreeSurface(tex->bmpImage);
		tex->bmpImage = NULL;
		
		// Delete the opengl texture
		glDeleteTextures(1,&tex->iID);

		// Free the texture
		delete tex;
		tex = NULL;
	}
}


///////////////////
// Upload a texture
void Tex_Upload(texture_t *tex, int force)
{
	if(!tex)
		return;

	if(tex->iUploaded && !force)
		return;

	// Bind the id
	glBindTexture( GL_TEXTURE_2D, tex->iID );
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    
	// Generate mipmaps
	if(tex->iMipmaps) {
		// Build mipmaps from the texture, and upload them all
		gluBuild2DMipmaps( GL_TEXTURE_2D, 
						   tex->iBytesPerPixel,
						   tex->iWidth,
						   tex->iHeight,
						   tex->iFormat,
						   GL_UNSIGNED_BYTE,
						   tex->bmpImage->pixels);
	} else {

		// No mipmaps
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 tex->iBytesPerPixel,
					 tex->iWidth,
					 tex->iHeight,
					 0,
					 tex->iFormat,
					 GL_UNSIGNED_BYTE,
					 tex->bmpImage->pixels);
	}


	if(tex->bMipFiltering) {
		// Bi-Linear mipmapping Filtering
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	} else {
		// Bi-Linear filtering
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}

    // Modulate the texture w/ colour
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // Set the wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex->nWrap_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex->nWrap_T);


	tex->iUploaded = true;
}


///////////////////
// Bind the texture
void Tex_Bind(texture_t *tex)
{
	if( !tex )
		return;

	if( tex->iID == nCurrentTexID[nCurrentTexUnit] )
		return;

	nCurrentTexID[nCurrentTexUnit] = tex->iID;
	glBindTexture( GL_TEXTURE_2D, tex->iID );
}


///////////////////
// Add a dir to the texture dir list
void Tex_AddDir(char *szDir)
{
    // Find an unused spot
    for( int i=0; i<MAX_TEXDIRLIST; i++ ) {
        // Check for matching dirs
        if(sTextureDirList[i].bUsed) {

            // Check if we have got one already entered
            if(stricmp(sTextureDirList[i].szDir,szDir) == 0)
                return;
        }
        else {
            // Add it
            strcpy(sTextureDirList[i].szDir, szDir);
            sTextureDirList[i].bUsed = true;
            return;
        }
    }

    writeLog("Warning: Could not add %s to texture dir list\n",szDir);
}


///////////////////
// Remove a dir from the texture dir list
void Tex_RemoveDir(char *szDir)
{
    // Find it
    for( int i=MAX_TEXDIRLIST-1; i>=0; i-- ) {
        if( sTextureDirList[i].bUsed ) {

            if( stricmp(sTextureDirList[i].szDir, szDir) == 0 ) {
                sTextureDirList[i].bUsed = false;
                return;
            }
        }
    }

    writeLog("Warning: Could not remove %s from texture dir list\n",szDir);
}


///////////////////
// Copy the texture onto the dir and return it
char *Tex_GetDir(char *buf, char *texture, int i)
{
    // Safety
    assert( i >= 0 && i < MAX_TEXDIRLIST );

    buf[0] = '\0';

    if( !sTextureDirList[i].bUsed )
        return buf;

    sprintf(buf, "%s/%s", sTextureDirList[i].szDir, texture);

    return buf;
}


///////////////////
// Returns the used state of a texture dir
bool Tex_DirOK(int i)
{
    // Safety
    assert( i >= 0 && i < MAX_TEXDIRLIST );

    return sTextureDirList[i].bUsed;
}





/*
=======================

    Multitexturing

=======================
*/


///////////////////
// Enable multitexturing
void Tex_EnableMultitexturing(bool bEnable)
{
    if( !glActiveTextureARB || !System_MultiTextureEnabled() )
        return;

    if( bEnable ) {
        glActiveTextureARB( GL_TEXTURE1 );
        glEnable( GL_TEXTURE_2D );     
    } else {
        glActiveTextureARB( GL_TEXTURE1 );
        glDisable( GL_TEXTURE_2D );
    }

    glActiveTextureARB( GL_TEXTURE0 );
}


///////////////////
// Select the texture unit to use
void Tex_SelectTexture(int nTex)
{
    if( !glActiveTextureARB || !System_MultiTextureEnabled() )
        return;

    int unit;
    if( nTex == GL_TEXTURE0 )
        unit = 0;
    else
        unit = 1;

    // Only change the texture unit if it isn't already set to this
    if( nCurrentTexUnit == unit )
        return;

    nCurrentTexUnit = unit;

    glActiveTextureARB( nTex );
}