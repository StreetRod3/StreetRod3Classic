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


// Quaternion class
// Created 20/7/03
// By Jason Boettcher


#ifndef __CQUATERNION_H__
#define __CQUATERNION_H__


class CQuaternion {
protected:
    // Attributes

    float   m_fX, m_fY, m_fZ;
    float   m_fW;

public:
    // Methods

    CQuaternion();
    CQuaternion(float w, float x, float y, float z);
	CQuaternion(CVec cAxis, float fAngle);

    void        createFromEuler(float fPitch, float fRoll, float fYaw);
    CVec        createEuler(void);

    void        createGLMatrix(CVec cPos, float *m);

	CMatrix		getMatrix(void);

    float       magnitude(void);
    float       getScalar(void);

    // Operators
    CQuaternion operator+=(CQuaternion q);
    CQuaternion operator-=(CQuaternion q);
    CQuaternion operator+(CQuaternion q);
    CQuaternion operator-(CQuaternion q);
	CQuaternion operator*(CQuaternion q);
    CQuaternion operator/=(float s);
    CQuaternion operator*=(float s);
    
    CQuaternion operator/(float s);
    CQuaternion operator*(float s);
    
    CQuaternion operator*(CVec v);


};


#endif  //  __CQUATERNION_H__