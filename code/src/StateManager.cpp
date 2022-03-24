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


// State manager
// Created 12/5/02
// Jason Boettcher


#include "defs.h"
#include "3D.h"


///////////////////
// Enable a state
void State_Enable(int state)
{
	if(!glIsEnabled(state))
		glEnable(state);

	/*state_t *st = tStates;
	int found = false;

	// Go through the states
	for(int i=0;i<iNumStates;i++,st++) {
		if(st->iState == state) {
			
			if(!st->iValue) {
				glEnable(state);
				st->iValue = true;
				found = true;
			}

			break;
		}
	}

	// If we didn't find the state, enable it anyway
	if(!found)
		glEnable(state);*/
}


///////////////////
// Disable a state
void State_Disable(int state)
{
	if(glIsEnabled(state))
		glDisable(state);

	/*state_t *st = tStates;
	int found = false;

	// Go through the states
	for(int i=0;i<iNumStates;i++,st++) {
		if(st->iState == state) {
			
			if(st->iValue) {
				glDisable(state);
				st->iValue = false;
				found = true;
			}

			break;
		}
	}

	// If we didn't find the state, disable it anyway
	if(!found)
		glDisable(state);*/
}

