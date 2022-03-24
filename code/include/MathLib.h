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


// Mathematics library
// Created 13/5/02
// Jason Boettcher


#ifndef __MATHLIB_H__
#define __MATHLIB_H__


// PI
#define PI		3.14159265358979323846

#define	EPSILON	0.00001f

// Macros
#define		MIN(a,b)	((a)<(b) ? (a) : (b))
#define		MAX(a,b)	((a)>(b) ? (a) : (b))


// Routines
float	VectorLength(CVec v);
float	VectorNormalize(CVec *v);
float	DotProduct(CVec v1, CVec v2);
CVec	CrossProduct(CVec v1, CVec v2);
CVec	VecAbs(CVec v);
float	GetDistance(CVec v1, CVec v2);
void	GetAngles(float pitch, float roll, float yaw ,CVec *forward, CVec *right, CVec *up);
float	GetRandomNum(void);
int     GetRandomInt(int max);
plane_t CalculatePlane(CVec a, CVec b, CVec c);
void	CreateCDMatrix(float *m, CVec v);
float   ProjectPoint(plane_t *plane, CVec point);

// Game maths
void	UnprojectMouse(CVec *vProjected);
int		MouseinRect(int x1, int y1, int x2, int y2);
CVec    UnprojectPoint(CVec p);


// Driving maths
CVec	RotateAround(CVec a, CVec b);
float	LimitAngle(float angle);

// Tri-Tri collision detection
static void TriTri_Projection(const CVec &rkD, const CVec akV[3], float &rfMin, float &rfMax);
bool        TriTri_Intersection(CVec akU[3],  CVec akV[3]);


#endif  //  __MATHLIB_H__