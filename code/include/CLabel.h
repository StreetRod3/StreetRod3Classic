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



// gui Label class
// Created 10/9/02
// Jason Boettcher


#ifndef __CLABEL_H__
#define __CLABEL_H__


class CLabel: public CWidget {
public:
	// Constructor
	CLabel() {
		sText[0] = 0;
        iCenter = false;
        nSize = 12;
	}
	CLabel(char *text) {
		CLabel();
        nSize = 12;
        iCenter = false;
		if(strlen(text) > 126)
			return;
		strncpy(sText, text, 127 );
	}


private:
	// Attributes
	char		sText[128];
    int         nSize;
    int         iCenter;

public:
	// Methods

	void	Create(void)					{ };
	void	Destroy(void)					{ };

	//These events return an event id, otherwise they return -1
	int		MouseOver(mouse_t *tMouse)		{ return -1; }
	int		MouseUp(mouse_t *tMouse)		{ return -1; }
	int		MouseDown(mouse_t *tMouse)		{ return -1; }
	int		KeyDown(char c)					{ return -1; }
	int		KeyUp(char c)					{ return -1; }

	void	Draw(void);

    void    setCenter(int c)                { iCenter = c; }
    void    setSize(int s)                  { nSize = s; }
};



#endif  //  __CLABEL_H__