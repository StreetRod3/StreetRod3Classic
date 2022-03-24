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



// gui Textbox class
// Created 24/8/03
// Jason Boettcher


#ifndef __CTEXTBOX_H__
#define __CTEXTBOX_H__


// Event msg's for this control
enum {
	TXT_NONE=-1,
	TXT_CHANGED=0,
    TXT_ENTER
};


// Textbox control class
class CTextbox: public CWidget {
public:
	// Constructors
	CTextbox() {
        sText[0] = '\0';
        nLength = 0;
        bHolding = false;
        cfgCol = CVec(0,0,0);
        cbgCol = CVec(0.7f,0.7f,0.7f);
	}


private:
	// Attributes

	char		sText[64];
    int         nLength;
    bool        bHolding;
	float	    fTimePushed;
	char	    iLastchar;
    CVec        cfgCol;
    CVec        cbgCol;


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

    char    *getText(void)      { return sText; }
    void    setText(char *text);

    void    setFGCol(CVec fg)   { cfgCol = fg; }
    void    setBGCol(CVec bg)   { cbgCol = bg; }

};



#endif  //  __CBUTTON_H__