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



// GUI Layout class
// Created 30/5/02
// Jason Boettcher


#ifndef __CGUILAYOUT_H__
#define __CGUILAYOUT_H__


// layout event structure
typedef struct {
	int		iControlID;
	int		iEventMsg;

	CWidget	*cWidget;
} gui_event_t;


class CGuiLayout {
public:
	// Constructor
	CGuiLayout() {
		cWidgets = NULL;
        pcCapturedWidget = NULL;
        pcFocus = NULL;
	}

	// Destructor
	~CGuiLayout() {
		Shutdown();
	}


private:
	// Attributes

	CWidget		*cWidgets;
	gui_event_t	*tEvent;
    CWidget     *pcCapturedWidget;
    CWidget     *pcFocus;


public:
	// Methods

	void		Initialize(void);

	void		Add(CWidget *widget, int id, int x, int y, int w, int h);
	CWidget		*getWidget(int id);

	gui_event_t	*Process(void);
	void		Draw(void);

    void        moveWidgets(int dx, int dy);

	void		Shutdown(void);

    void        captureMouse(CWidget *psWidget);
    void        releaseMouse(void);

    int         processWidget(CWidget *w);

};




#endif  //  __CGUILAYOUT_H__