/////////////////////////////////////////
//
//             Street Rod 3
//
//      Copyright Auxiliary Software 2000
//
//
/////////////////////////////////////////


// City Class
// Created 12/7/02
// Jason Boettcher


#include "defs.h"
#include "game.h"
#include "City.h"


///////////////////
// Load the city
int CCity::Load(char *sFilename)
{
	// QUICK HACK
	iWidth = iHeight = 10;

	tTileLib = new tilelib_t[3];
	if(tTileLib == NULL) {
		return false;
	}

	// Load a grass tile
	if(!LoadTileLib(&tTileLib[0], "grass"))
		return false;
	if(!LoadTileLib(&tTileLib[1], "stroad"))
		return false;
	if(!LoadTileLib(&tTileLib[2], "ramp"))
		return false;


	tTiles = new tile_t[iWidth * iHeight];
	if(tTiles == NULL) {
		return false;
	}


	// Assign a 10x10 block of tiles to grass
	tile_t *t = tTiles;
	for(int y=0;y<10;y++) {
		for(int x=0;x<10;x++) {
			t->iUsed = true;
			t->tLib = tTileLib;
			t->iXpos = x;
			t->iYpos = y;
			t++;
		}
	}

	tTiles[1*iHeight].tLib = &tTileLib[1];
	tTiles[1*iHeight + 1].tLib = &tTileLib[2];



	return true;
}


///////////////////
// Load a tile library item
int CCity::LoadTileLib(tilelib_t *lib, char *sDir)
{
	char buf[128];

	// Load the model
	sprintf(buf,"data/tiles/%s/tile",sDir);

	lib->cModel = Cache_LoadModel(buf);
	if(lib->cModel == NULL) {
		return false;
	}


	// Load the info
	sprintf(buf,"data/tiles/%s/info.sr3",sDir);
	ReadInteger(buf,"General","Width",&lib->iWidth,1);
	ReadInteger(buf,"General","Height",&lib->iHeight,1);

	return true;
}


///////////////////
// Draw the city
void CCity::Draw(CCamera *cCamera)
{
	// Just draw all the tiles
	tile_t *t = tTiles;
	int x,y;

	for(y=0;y<iHeight;y++)
		for(x=0;x<iWidth;x++)
			DrawTile(t++);
	
}


///////////////////
// Draw a tile
void CCity::DrawTile(tile_t *tile)
{
	if(!tile->iUsed)
		return;

	int TileSize = 48;
	int ht = TileSize / 2;

	if(tile->tLib) {

		glPushMatrix();
		glTranslated(tile->iXpos*TileSize + ht, tile->iYpos*TileSize + ht, 0);

		tile->tLib->cModel->Render();

		glPopMatrix();
	}
}


///////////////////
// Get a tile based on x,y coords
tile_t *CCity::getTile(int x, int y)
{
	// Do a safety check first
	if(x<0 || y<0)
		return NULL;
	if(x>=iWidth || y>=iHeight)
		return NULL;

	return &tTiles[y * iHeight + x];
}


///////////////////
// Shutdown the city
void CCity::Shutdown(void)
{
	// Free the tile lib
	if(tTileLib) {
		delete[] tTileLib;
		tTileLib = NULL;
	}

	// Free the tiles
	if(tTiles) {
		delete tTiles;
		tTiles = NULL;
	}
}