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


// Camera class
// Created 8/5/02
// Jason Boettcher


#ifndef __CCLASS_H__
#define __CCLASS_H__


class CCamera {
public:
	// Constructor
	CCamera() {
		fPitch = fRoll = fYaw = 0;
		fCurYaw = fGotoYaw = 0;
	}


private:
	// Attributes

	CVec		vPos;
	CVec		vLookAt;

	float		fPitch, fRoll, fYaw;
	float		fCurYaw, fGotoYaw;


public:
	// Methods

	void		Setup(CVec pos, CVec lookat);
	void		LookAt(CVec lookat);
	void		Show(void);
    void        Reverse(void);

	void		FollowCar(void);	// Add car to parameter

	

	CVec		getPos(void)		{ return vPos; }
 	void		setPos(CVec p)		{ vPos = p; }

    CVec        getLook(void)       { return vLookAt; }

	float		getPitch(void)		{ return fPitch; }
	float		getYaw(void)		{ return fYaw; }
    float       getRoll(void)       { return fRoll; }

	void		setPitch(float _p)	{ fPitch = _p; }
	void		setYaw(float _y)	{ fYaw = _y; }
    void        setRoll(float _r)   { fRoll = _r; }





};




#endif  //  __CCLASS_H__