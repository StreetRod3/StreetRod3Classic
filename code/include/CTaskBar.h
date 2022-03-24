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


// Taskbar class
// Created 30/5/02
// Jason Boettcher


#ifndef __CTASKBAR_H__
#define __CTASKBAR_H__


#define		MAX_TASKITEMS	128


// Taskbar item
typedef struct {
	texture_t	*tTexture;
	char		sId[32];
	float		x;
    bool        bToggled;
	void		(*func) (void);
} task_item_t;


class CTaskBar {
public:
	// Constructor
	CTaskBar() {
		iNumTaskItems = 0;
		iPosx = 2;
		iDir = 0;		
        bEnabled = true;
	}


private:
	// Attributes

	int			iNumTaskItems;
	int			iPosx;
	int			iDir;
	task_item_t tTaskItems[MAX_TASKITEMS];
    bool        bEnabled;


public:
	// Methods
	
	void		Draw(void);
	void		Simulate(void);

	void		setDirection(int _d)		{ iDir = _d; }		
	void		Add(char *sTexture, char *id, void (*func) (void));
	void		Clear(void)					{ iNumTaskItems=0; }
	void		Remove(char *id);

    void        toggleItem(char *id);

	void		setPos(int x)				{ iPosx = x; }
    bool        setEnabled(bool _e)         { bool old = bEnabled; bEnabled = _e; return old; }



};




#endif  //  __CTASKBAR_H__