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


// gui Colour slider class
// Created 25/6/02
// Jason Boettcher


#ifndef __CCOLOURSLIDER_H__
#define __CCOLOURSLIDER_H__


// Event msg's for this control
enum {
	CSL_NONE=-1,
	CSL_CHANGED=0
};



// Colour slider control class
class CColourSlider: public CWidget {
public:
	// Constructors
	CColourSlider() {
		vColour = CVec(1,1,1);
		fColSliderPos = 0.5f;
		fBrightSliderPos = 0.99f;
		tTexture = NULL;
	}



private:
	// Attributes

	CVec		vColour;
	
	float		fColSliderPos;
	float		fBrightSliderPos;

	texture_t	*tTexture;


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


    void    calculateColour(void);

	CVec	getColour(void)			{ return vColour; }

};



#endif  //  __CCOLOURSLIDER_H__