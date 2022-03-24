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


// Nut class
// Created 30/5/02
// Jason Boettcher


#ifndef __CNUT_H__
#define __CNUT_H__


class CNut {
public:
	// Constructor
	CNut() {
		iIn = true;
        cNut = NULL;

	}


private:
	// Attributes

	// 3D
    CModel      *cNut;
	CVec		vOrigPos;
	CVec		vPos;
	CMatrix		cMatrix;
	CMatrix		cOrigMatrix;

	// Game
	int			iIn;
	int			iHighlight;


public:
	// Methods

	void		Setup(CVec pos, CMatrix mat, CMatrix pmat, CVec partpos);

	void		Draw(void);
	void		DrawHighlight(void);
	int			LineIntersect(CVec p1, CVec dir);


	void		setIn(int _i)		{ iIn = _i; }
	int			getIn(void)			{ return iIn; }


	void		setOrigPos(CVec _p)		{ vOrigPos = _p; }
	CVec		getOrigPos(void)		{ return vOrigPos; }
	void		setPos(CVec _p)			{ vPos = _p; }
	CVec		getPos(void)			{ return vPos; }

	void		setOrigMat(CMatrix _m)	{ cOrigMatrix = _m; }	
	CMatrix		getOrigMat(void)		{ return cOrigMatrix; }
	void		setMat(CMatrix _m)		{ cMatrix = _m; }	
	CMatrix		getMat(void)			{ return cMatrix; }

	void		setHighlight(int _h)	{ iHighlight = _h; }

};



#endif  //  __CNUT_H__