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


// Sidebar class
// Created 25/6/02
// Jason Boettcher


#ifndef __CSIDEBAR_H__
#define __CSIDEBAR_H__


// Sidebar item
typedef struct {
	int		iImageNum;
	int		iEnabled;
	void	(*Click)	(void);
} sidebitem_t;


class CSidebar {
public:
	// Constructor
	CSidebar() {
		tTexture = NULL;
		iX = iY = 0;
		iNumItems = 0;
        bEnabled = true;
	}


private:
	// Attributes

	texture_t	*tTexture;
	int			iX,iY;

	int			iNumItems;
	sidebitem_t	tItems[16];
    bool        bEnabled;


public:
	// Methods		

	void		addItem(int imagenum, void (*click) (void)) {
					tItems[iNumItems].iImageNum = imagenum;
					tItems[iNumItems].iEnabled = true;
					tItems[iNumItems++].Click = click;
				}

	void		Clear(void)						{ iNumItems = 0; }
	void		Setup(int x, int y, char *tex) {
					Clear(); iX=x; iY=y;
					tTexture = Cache_LoadTexture(tex);
					if(tTexture)
						tTexture->bMipFiltering = false;
					Tex_Upload(tTexture);
				}

	void		setItemEnabled(int e, int i)		{ tItems[i].iEnabled = e; }
    bool        setEnabled(bool _e)                 { bool old = bEnabled; bEnabled = _e; return old; }
    void        enableAll(bool _e);
	void		Process(void);
	void		Draw(void);

};



#endif  //  __CSIDEBAR_H__
