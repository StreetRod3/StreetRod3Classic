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



// gui Checkbox class
// Created 20/8/03
// Jason Boettcher


#ifndef __CCHECKBOX_H__
#define __CCHECKBOX_H__


// Event msg's for this control
enum {
	CHK_NONE=-1,
	CHK_CHANGED=0
};


// Checkbox control class
class CCheckbox: public CWidget {
public:
	// Constructors
	CCheckbox() {		
		psTexture = NULL;
		bDown = false;
        bShowDown = false;
	}

	CCheckbox(bool down) {
		CCheckbox();
		bDown = down;
	}



private:
	// Attributes

	bool        bDown;
    bool        bShowDown;
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

    bool    getCheck(void)      { return bDown; }
    void    setCheck(bool c)    { bDown = c; }

};



#endif  //  __CBUTTON_H__