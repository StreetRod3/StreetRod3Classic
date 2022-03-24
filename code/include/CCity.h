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



// City Class
// Created 12/7/02
// Jason Boettcher


#ifndef __CCITY_H__
#define __CCITY_H__


class CCity {
public:
	// Constructor
	CCity() {
		tTileLib = NULL;
		tTiles = NULL;
	}


private:
	// Attributes


	// General
	int			iWidth;
	int			iHeight;


	// Tile Library
	tilelib_t	*tTileLib;

	// Tiles
	tile_t		*tTiles;


public:
	// Methods

	// Loading
	int			Load(char *sFilename);
	int			LoadTileLib(tilelib_t *lib, char *sDir);

	void		Shutdown(void);

	// Drawing
	void		Draw(CCamera *cCamera);
	void		DrawTile(tile_t *tile);


	// Variables
	tile_t		*getTile(int x, int y);

};


#endif  //  __CCITY_H__