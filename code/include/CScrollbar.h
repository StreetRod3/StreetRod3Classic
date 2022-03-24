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



// gui scrollbar class
// Created 30/6/02
// Jason Boettcher


#ifndef __CSCROLLBAR_H__
#define __CSCROLLBAR_H__


// Scrollbar events
enum {
	SCR_NONE=-1,
	SCR_CHANGE=0
};


class CScrollbar : public CWidget {
public:
	// Constructor
	CScrollbar() {
	}


private:
	// Attributes
	int		iMin;
	int		iMax;
	int		iValue;

	int		iScrollPos;
	int		iItemsperbox;
	int		iSliderGrabbed;
	int		iSliderGrabPos;

	int		iTopButton;
	int		iBotButton;
    int		nButtonsDown;
	float	fMouseNext[3];


	texture_t *tTexture;


public:
	// Methods

	void	Create(void);
	void	Destroy(void) { }

	//These events return an event id, otherwise they return -1
	int		MouseOver(mouse_t *tMouse);
	int		MouseUp(mouse_t *tMouse);
	int		MouseDown(mouse_t *tMouse);
	int		KeyDown(char c)					{ return SCR_NONE; }
	int		KeyUp(char c)					{ return SCR_NONE; }

	void	Draw(void);

	void	UpdatePos(void);


	void	setMin(int _min)				{ iMin = _min; }
	void	setMax(int _max)				{ iMax = _max; }
	void	setValue(int _value)			{ iValue = _value; UpdatePos(); }

	void	setItemsperbox(int _i)			{ iItemsperbox = _i; }

	int		getValue(void)					{ return iValue; }
	int		getMax(void)					{ return iMax; }

	//int		SendMessage(int iMsg, DWORD Param1, DWORD Param2)	{ return 0; }



};


#endif  //  __CSCROLLBAR_H__