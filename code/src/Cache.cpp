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


// Cache
// Created 12/5/02
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "System.h"

cache_t		*tCache = NULL;


///////////////////
// Initialize the cache
int Cache_Initialize(void)
{
	// Allocate memory for the cache
	tCache = new cache_t[MAX_CACHE];
	if(tCache == NULL) {
		return false;
	}


	// Set each item to unused
	for(int i=0;i<MAX_CACHE;i++)
		tCache[i].iUsed = false;


	return true;
}


///////////////////
// Shutdown the cache
void Cache_Shutdown(void)
{
	// Go through each item freeing it
	cache_t *cache = tCache;
	for(int i=0;i<MAX_CACHE;i++,cache++) {
		if(cache->iUsed) {

			switch(cache->iType) {
				
				// Texture
				case CCH_TEXTURE:
					if(cache->tTexture) {
						if(cache->tTexture->bmpImage)
							SDL_FreeSurface(cache->tTexture->bmpImage);

						// TODO: Delete the texture from opengl

						delete cache->tTexture;
						cache->tTexture = NULL;
					}
					break;

				
				// Model
				case CCH_MODEL:
					if(cache->cModel) {
						cache->cModel->shutdown();
						
						delete cache->cModel;
						cache->cModel = NULL;
					}
					break;

				// Sample
				case CCH_SAMPLE:
					BASS_SampleFree(cache->smpSample);
					break;
			}
		}
	}
}


///////////////////
// This finds an unused cache slot
cache_t *Cache_FindUnused(void)
{
	cache_t *cch = tCache;

	for(int i=0;i<MAX_CACHE;i++,cch++) {
		if(cch->iUsed)
			continue;

		return cch;
	}

	// No free spaces
	return NULL;
}


///////////////////
// Find a matching file in the cache
cache_t *Cache_FindMatch(char *sFilename)
{
	cache_t *cch = tCache;

	for(int i=0;i<MAX_CACHE;i++,cch++) {
		if(!cch->iUsed)
			continue;

		if(strcmp(sFilename,cch->sFilename) == 0)
			return cch;
	}

	// No match
	return NULL;
}



///////////////////
// Load a texture
texture_t *Cache_LoadTexture(char *sFilename)
{
	cache_t *cache = Cache_FindMatch(sFilename);

	// No match
	if(cache == NULL) {
		cache = Cache_FindUnused();

		// No unused
		if(cache == NULL)
			return NULL;

	} else
		return cache->tTexture;
	

	// Load the texture from the texture manager
	cache->tTexture = Tex_Load(sFilename);
	if(cache->tTexture == NULL)
		return NULL;

	// Setup the texture
	cache->iUsed = true;
	cache->iType = CCH_TEXTURE;
	strcpy(cache->sFilename,sFilename);
	
	return cache->tTexture;
}


///////////////////
// Load a model
CModel *Cache_LoadModel(char *sFilename)
{
	cache_t *cache = Cache_FindMatch(sFilename);

	// No match
	if(cache == NULL) {
		cache = Cache_FindUnused();

		// No unused
		if(cache == NULL)
			return NULL;

	} else
		return cache->cModel;

	
	// Load the model
	cache->cModel = new CModel;
	if(cache->cModel == NULL)
		return NULL;

	if(!cache->cModel->load(sFilename)) {
		delete cache->cModel;
		return NULL;
	}

	cache->iType = CCH_MODEL;
	cache->iUsed = true;
	strcpy(cache->sFilename,sFilename);

	return cache->cModel;
}


///////////////////
// Load a sample
HSAMPLE Cache_LoadSample(char *sFilename, int max)
{
	cache_t *cache = Cache_FindMatch(sFilename);

	// No match
	if(cache == NULL) {
		cache = Cache_FindUnused();

		// No unused
		if(cache == NULL)
			return NULL;

	} else
		return cache->smpSample;

	
	// Load the sample
	cache->smpSample = BASS_SampleLoad(false,sFilename,0,0,max,0);
	if(cache->smpSample < 0)
	{
		// 17/06/05
		writeLog("File \"%s\" could not be loaded as a sample",sFilename);
		//
		return -1;
	}

	cache->iType = CCH_SAMPLE;
	cache->iUsed = true;
	strcpy(cache->sFilename,sFilename);

	return cache->smpSample;
}