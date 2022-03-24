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


// Widget class
// Created 30/5/02
// Jason Boettcher


#ifndef __CWIDGET_H__
#define __CWIDGET_H__

class CGuiLayout;

class CWidget {
public:
	// Constructor
	CWidget() {
		iID = -1;

		iX = iY = 0;
		iWidth = iHeight = 1;
        pcParent = NULL;
        bFocus = false;
	}


public:
	// Attributes

	int		iX, iY;
	int		iWidth, iHeight;
    bool    bFocus;
    CGuiLayout  *pcParent;


private:
	int		iID;

	CWidget	*cNext;
    CWidget	*cPrev;


public:
	// Methods


	// Widget functions
	void			Setup(int id, int x, int y, int w, int h);
	int				InBox(int x, int y);

	void			setNext(CWidget *w)		{ cNext = w; }
	CWidget			*getNext(void)			{ return cNext; }

    void			setPrev(CWidget *w)		{ cPrev = w; }
	CWidget			*getPrev(void)			{ return cPrev; }

	int				getID(void)				{ return iID; }

    void            setParent(CGuiLayout *gui)  { pcParent = gui; }

    void            move(int dx, int dy);

    void            setFocus(bool focus)    { bFocus = focus; }
    bool            getFocus(void)          { return bFocus; }

    int             getX(void)              { return iX; }
    int             getY(void)              { return iY; }
    void            setPosition(int x, int y)   { iX=x; iY=y; }

    void            captureMouse(void);
    void            releaseMouse(void);


	// Virtual functions
	virtual	void	Create(void) = 0;
	virtual void	Destroy(void) = 0;

	//These events return an event id, otherwise they return -1
	virtual	int		MouseOver(mouse_t *tMouse) = 0;
	virtual	int		MouseUp(mouse_t *tMouse) = 0;
	virtual	int		MouseDown(mouse_t *tMouse) = 0;
	virtual	int		KeyDown(char c) = 0;
	virtual	int		KeyUp(char c) = 0;

	virtual	void	Draw(void) = 0;
};




#endif  //  __CWIDGET_H__