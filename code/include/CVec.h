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


// Vector class
// Created 28/9/01
// By Jason Boettcher


#ifndef __CVEC_H__
#define __CVEC_H__


class CVec {
public:
	// Constructor
	CVec() {
		x=y=z=0;
	}

	CVec(float _x, float _y, float _z) {
		x=_x;
		y=_y;
		z=_z;
	}


private:
	// Attributes

	float x, y, z;


public:
	// Methods

	float	GetX(void)					{ return x; }
	float	GetY(void)					{ return y; }
	float	GetZ(void)					{ return z; }
	void	SetX(float _x)				{ x = _x; }
	void	SetY(float _y)				{ y = _y; }
	void	SetZ(float _z)				{ z = _z; }


	float	GetIndex(int i)				{ if(i==0) return x; if(i==1) return y; return z; }
	void	SetIndex(int i, float v)	{ if(i==0) x=v; if(i==1) y=v; if(i==2) z=v; }


	// Overloads
	CVec operator*(float scalar) {		
		return CVec(x*scalar,y*scalar,z*scalar);
	}
	CVec operator*(CVec vec) {
		return CVec(x*vec.x,y*vec.y,z*vec.z);
	}
	CVec operator/(float scalar) {		
		return CVec(x/scalar,y/scalar,z/scalar);
	}
	CVec operator+(CVec vec) {
		return CVec(x+vec.x,y+vec.y,z+vec.z);
	}
	CVec operator-(CVec vec) {
		return CVec(x-vec.x,y-vec.y,z-vec.z);
	}
	CVec operator-() {
		return CVec(-x,-y,-z);
	}
	CVec operator+=(CVec vec) {
		return CVec(x+=vec.x, y+=vec.y, z+=vec.z);
	}
	CVec operator-=(CVec vec) {
		return CVec(x-=vec.x, y-=vec.y, z-=vec.z);
	}

	bool operator<(CVec op) {
		if(x<op.x && y<op.y && z<op.z)
			return true;
		return false;
	}
	bool operator==(CVec op) {
		if(x==op.x && y==op.y && z==op.z)
			return true;
		return false;
	}
};









#endif  //  __CVEC_H__
