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



// Graphical User Interface class
// Created 12/5/02
// Jason Boettcher


#ifndef __CGUI_H__
#define __CGUI_H__


class CGUI {
public:
	// Constructor
	CGUI() {


	}


private:
	// Attributes




	// Graphics
	texture_t		*texMouse;



public:
	// Methods

	int			Load(void);
	void		Shutdown(void);


	void		Process(void);
	void		Draw(void);

    void        DrawMouse(void);


    


};





#endif  //  __CGUI_H__