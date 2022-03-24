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


// Input class
// Created 10/12/01
// By Jason Boettcher


#ifndef __CINPUT_H__
#define __CINPUT_H__


// Input variable types
#define		INP_KEYBOARD		0
#define		INP_MOUSE			1
#define		INP_JOYSTICK1		2
#define		INP_JOYSTICK2		3


// Joystick data
#define		JOY_UP				0
#define		JOY_DOWN			1
#define		JOY_LEFT			2
#define		JOY_RIGHT			3
#define		JOY_BUTTON			4


class CInput {
public:
	// Constructor
	CInput() {
		Type = INP_KEYBOARD;
		Data = 0;
		Extra = 0;
		Down = false;

	}


private:
	// Attributes

	int		Type;
	int		Data;
	int		Extra;
	int		Down;


public:
	// Methods

	int		Load(char *name, char *section);
	int		Setup(char *text);
	int		Wait(char *strText);

	int		isUp(void);
	int		isDown(void);
	int		isDownOnce(void);

	void	ClearUpState(void);
};



// Keyboard structure
typedef struct {
	char	text[32];
	int		value;
} keys_t;


// Joystick structure
typedef struct {
	char	text[32];
	int		value;
	int		extra;
} joystick_t;




#endif  //  __CINPUT_H__

