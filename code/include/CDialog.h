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


// gui Dialog class
// Created 30/5/02
// Jason Boettcher


#ifndef __CDIALOG_H__
#define __CDIALOG_H__


// Event msg's for this control
enum {
	DLG_NONE=-1,
};


// Dialog sizes
enum {
	dlgs_small,
	dlgs_medium,
	dlgs_large
};


// Dialog titles
enum {
	dlgt_cars,
	dlgt_parts,
	dlgt_paint,
	dlgt_carinfo,
    dlgt_system,
    dlgt_partchange,
    dlgt_decal,
    dlgt_radio
};



// Dialog control class
class CDialog: public CWidget {
public:
	// Constructors
	CDialog() {
		sTitle[0] = 0;
		iSize = dlgs_large;
		iTitle = -1;
        bGrabbed = false;
	}

	CDialog(char *title, int Size) {
		CDialog();
		strcpy(sTitle,title);
		iSize = Size;
        bGrabbed = false;
	}



private:
	// Attributes

	char	sTitle[32];
	int		iSize;
	int		iTitle;

	texture_t *tDlgTexture;
	texture_t *tHRail, *tVRail;
	texture_t *tTitleTexture;

    bool    bGrabbed;
    int     GrabX, GrabY;


public:
	// Methods

	void	Create(void);
	void	Destroy(void);

	//These events return an event id, otherwise they return -1
	int		MouseOver(mouse_t *tMouse);
	int		MouseUp(mouse_t *tMouse);
	int		MouseDown(mouse_t *tMouse);
	int		KeyDown(char c);
	int		KeyUp(char c);

	void	Draw(void);

	void	DrawBigTyre(int x, int y);
	void	DrawHoriRail(int x, int y);
	void	DrawVertRail(int x, int y);

};



#endif  //  __CDIALOG_H__