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


// gui Slider class
// Created 20/8/03
// Jason Boettcher


#ifndef __CSLIDER_H__
#define __CSLIDER_H__


// Event msg's for this control
enum {
	SLD_NONE=-1,
	SLD_CHANGED=0
};


// Slider control class
class CSlider: public CWidget {
public:
	// Constructors
	CSlider() {		
		psTexture = NULL;
        nMin = 0;
        nMax = 100;
        nValue = 0;
        bDown = false;
	}

	CSlider(int max, int value) {
		CCheckbox();
		nMax = max;
        nValue = value;
        nMin = 0;
	}



private:
	// Attributes

    int         nMin;
	int         nMax;
    int         nValue;
    bool        bDown;
	texture_t	*psTexture;


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

    int     getValue(void)      { return nValue; }

};



#endif  //  __CBUTTON_H__