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


// Event queue
// Created 29/5/02
// Jason Boettcher


#ifndef __CEVENTQUEUE_H__
#define __CEVENTQUEUE_H__


#define		MAX_EVENTS		256


// An event item
typedef struct event_s {
	int		iID;
	int		iUsed;

	// User-defined data
	void	*uData;
	int		iData;
	char	sData[64];

	void	(*Func) (struct event_s *e);
} event_t;


// Event queue class
class CEventQueue {
public:
	// Constructor
	CEventQueue() {
		iNumQueued = 0;
		tEvents = NULL;
	}

	// Destructor
	~CEventQueue() {
		if(tEvents) {
			delete[] tEvents;
			tEvents = NULL;
		}
	}



private:
	// Attributes

	int		iNumQueued;
	event_t	*tEvents;


public:
	// Methods

	int		Initialize(void);

	event_t	*addNew(void);
	void	Execute(void);

	int		isEmpty(void)		{ return iNumQueued > 0 ? false : true; }
};





#endif  //  __CEVENTQUEUE_H__