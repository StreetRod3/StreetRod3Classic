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



// gui Button class
// Created 30/5/02
// Jason Boettcher


#ifndef __CBUTTON_H__
#define __CBUTTON_H__


// Event msg's for this control
enum {
	BUT_NONE=-1,
	BUT_CLICKED=0
};

// Button text id's
enum {
	btn_forgit,
	btn_yeah,
	btn_sellit,
	btn_seeit,
	btn_swapit,
	btn_done,
	btn_parts,
	btn_notha,
    btn_quit
};



// Button control class
class CButton: public CWidget {
public:
	// Constructors
	CButton() {
		iPushed = false;
		iMouseOver = false;
		sText[0] = 0;
		tTexture = NULL;
		tTextTexture = NULL;
		iTextID = -1;
        nSize = 14;
	}

	CButton(char *text, int id) {
		CButton();
		strcpy(sText,text);	
		iTextID = id;
        nSize = 14;
	}



private:
	// Attributes

	char		sText[64];
    int         nSize;
	int			iTextID;
	int			iPushed;
	int			iMouseOver;
	texture_t	*tTexture;
	texture_t	*tTextTexture;


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

    void    setText(char *s);
    void    setSize(int s)      { nSize = s; }

};



#endif  //  __CBUTTON_H__