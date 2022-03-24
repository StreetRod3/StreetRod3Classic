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


#include "defs.h"


///////////////////
// Initialize the event queue
int CEventQueue::Initialize(void)
{
	tEvents = new event_t[MAX_EVENTS];
	if(tEvents == NULL)
		return false;

	for(int i=0;i<MAX_EVENTS;i++)
		tEvents[i].iUsed = false;
	iNumQueued = 0;

	return true;
}


///////////////////
// Add a new event to the list
event_t *CEventQueue::addNew(void)
{
	if(iNumQueued+1 >= MAX_EVENTS-1)
		return NULL;

	event_t *ev = &tEvents[iNumQueued++];
	ev->iUsed = true;

	return ev;
}


///////////////////
// Execute the first event on the list
void CEventQueue::Execute(void)
{
	if(iNumQueued == 0)
		return;

	if(tEvents->Func)
		tEvents->Func(tEvents);


	// If this event closed itself, shift the queue down one
	if(!tEvents->iUsed) {
		iNumQueued--;
		for(int i=0;i<iNumQueued;i++)
			tEvents[i] = tEvents[i+1];
	}
}

