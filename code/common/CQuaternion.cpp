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


#include "defs.h"
#include "3D.h"


///////////////////
// CQuaternion Constructor
CQuaternion::CQuaternion()
{
    m_fX = m_fY = m_fZ = m_fW = 0;
}


///////////////////
// CQuaternion Constructor
CQuaternion::CQuaternion(float w, float x, float y, float z)
{
    m_fX = x;
    m_fY = y;
    m_fZ = z;
    m_fW = w;
}


///////////////////
// CQuaternion Constructor
CQuaternion::CQuaternion(CVec cAxis, float fAngle)
{
	m_fX = cAxis.GetX();
	m_fY = cAxis.GetY();
	m_fZ = cAxis.GetZ();
	m_fW = fAngle;
}


///////////////////
// Create the quaternion from Euler angles
void CQuaternion::createFromEuler(float fPitch, float fRoll, float fYaw)
{
    double D2R = (PI / 180);
    
    fPitch *= D2R;
    fRoll *= D2R;
    fYaw *= D2R;

    double	cyaw, cpitch, croll, syaw, spitch, sroll;
	double	cyawcpitch, syawspitch, cyawspitch, syawcpitch;

	cyaw = cos(0.5f * fYaw);
	cpitch = cos(0.5f * fPitch);
	croll = cos(0.5f * fRoll);
	syaw = sin(0.5f * fYaw);
	spitch = sin(0.5f * fPitch);
	sroll = sin(0.5f * fRoll);

	cyawcpitch = cyaw*cpitch;
	syawspitch = syaw*spitch;
	cyawspitch = cyaw*spitch;
	syawcpitch = syaw*cpitch;

	m_fW = (float) (cyawcpitch * croll + syawspitch * sroll);
	m_fX = (float) (cyawcpitch * sroll - syawspitch * croll);
    m_fY = (float) (cyawspitch * croll + syawcpitch * sroll);
	m_fZ = (float) (syawcpitch * croll - cyawspitch * sroll);        
}


///////////////////
// Get the magnitude
float CQuaternion::magnitude(void)
{
    return (float)sqrt(m_fW*m_fW + m_fX*m_fX + m_fY*m_fY + m_fZ*m_fZ);
}


///////////////////
// += Operator
CQuaternion CQuaternion::operator+=(CQuaternion q)
{
    m_fW += q.m_fW;
    m_fX += q.m_fX;
    m_fY += q.m_fY;
    m_fZ += q.m_fZ;

    return *this;
}


///////////////////
// -= Operator
CQuaternion CQuaternion::operator-=(CQuaternion q)
{
    m_fW -= q.m_fW;
    m_fX -= q.m_fX;
    m_fY -= q.m_fY;
    m_fZ -= q.m_fZ;

    return *this;
}


///////////////////
// *= Operator
CQuaternion CQuaternion::operator*=(float s)
{
    m_fW *= s;
    m_fX *= s;
    m_fY *= s;
    m_fZ *= s;

    return *this;
}


///////////////////
// * Operator
CQuaternion CQuaternion::operator*(float s)
{
    return CQuaternion(m_fW * s,
                       m_fX * s,
                       m_fY * s,
                       m_fZ * s);
}


///////////////////
// /= Operator
CQuaternion CQuaternion::operator/=(float s)
{
    m_fW /= s;
    m_fX /= s;
    m_fY /= s;
    m_fZ /= s;

    return *this;
}


///////////////////
// * Operator
CQuaternion CQuaternion::operator*(CQuaternion q)
{
	//CQuaternion cOut;
	
	/*cOut.m_fW = m_fW*q.m_fW - m_fX*q.m_fX - m_fY*q.m_fY - m_fZ*q.m_fZ;
	cOut.m_fX = m_fW*q.m_fX + m_fX*q.m_fW + m_fY*q.m_fZ - m_fZ*q.m_fY;
	cOut.m_fY = m_fW*q.m_fY + m_fY*q.m_fW + m_fZ*q.m_fX - m_fX*q.m_fZ;
	cOut.m_fZ = m_fW*q.m_fZ + m_fZ*q.m_fW + m_fX*q.m_fY - m_fY*q.m_fX;*/

	CVec q1 = CVec(m_fX, m_fY, m_fZ);
	CVec q2 = CVec(q.m_fX, q.m_fY, q.m_fZ);

	CVec a = CrossProduct(q1,q2) + (q1*q.m_fW ) + (q1*m_fW);	
    
	float w = (m_fW*q.m_fW) - DotProduct(q1,q2);
	
	return CQuaternion(w, a.GetX(), a.GetY(), a.GetZ());

}


///////////////////
// + Operator
CQuaternion CQuaternion::operator+(CQuaternion q)
{
    return CQuaternion(m_fW + q.m_fW,
                       m_fX + q.m_fX,
                       m_fY + q.m_fY,
                       m_fZ + q.m_fZ);
}


///////////////////
// - Operator
CQuaternion CQuaternion::operator-(CQuaternion q)
{
    return CQuaternion(m_fW - q.m_fW,
                       m_fX - q.m_fX,
                       m_fY - q.m_fY,
                       m_fZ - q.m_fZ);
}


///////////////////
// * Operator
CQuaternion CQuaternion::operator*(CVec v)
{
    return CQuaternion(-(m_fX*v.GetX() + m_fY*v.GetY() + m_fZ*v.GetX()),
					     m_fW*v.GetX() + m_fY*v.GetZ() - m_fZ*v.GetY(),
					 	 m_fW*v.GetY() + m_fZ*v.GetX() - m_fX*v.GetZ(),
					     m_fW*v.GetZ() + m_fX*v.GetY() - m_fY*v.GetX());
}


///////////////////
// Create an OpenGL matrix based from this quaternion
void CQuaternion::createGLMatrix(CVec cPos, float *m)
{
    CVec angle = createEuler();
    CMatrix mat;
    mat.CreateFromAngle(angle.GetX(), angle.GetY(), angle.GetZ());

    mat.CreateGLMatrix(cPos,m);
}



///////////////////
// Create Euler angles from this quaternion
CVec CQuaternion::createEuler(void)
{
    double	r11, r21, r31, r32, r33, r12, r13;
	double	q00, q11, q22, q33;
	double	tmp;
    CVec u;

    double R2D = (180 / PI);

	q00 = m_fW * m_fW;
	q11 = m_fX * m_fX;
	q22 = m_fY * m_fY;
	q33 = m_fZ * m_fZ;

	r11 = q00 + q11 - q22 - q33;
	r21 = 2 * (m_fX*m_fY + m_fW*m_fZ);
	r31 = 2 * (m_fX*m_fZ - m_fW*m_fY);
	r32 = 2 * (m_fY*m_fZ + m_fW*m_fX);
	r33 = q00 - q11 - q22 + q33;

	tmp = fabs(r31);
	if(tmp > 0.999999)
	{
		r12 = 2 * (m_fX*m_fY - m_fW*m_fZ);
		r13 = 2 * (m_fX*m_fZ + m_fW*m_fY);

		u.SetX( 0.0f ); //roll
		u.SetY( R2D * ((float)(-(PI/2) * r31/tmp)) ); // pitch
		u.SetZ( R2D * ((float) atan2(-r12, -r31*r13)) ); // yaw
		return u;
	}

	u.SetX( R2D * ((float) atan2(r32, r33)) ); // roll
	u.SetY( R2D * ((float) asin(-r31)) );		 // pitch
	u.SetZ( R2D * ((float) atan2(r21, r11)) ); // yaw
	return u;
}


///////////////////
// Create a matrix from this quaternion
CMatrix CQuaternion::getMatrix(void)
{
	CMatrix		cMat;

	

	cMat.SetValue(0,0, 1 - 2*( (m_fY*m_fY) + (m_fZ*m_fZ) ));
	cMat.SetValue(0,1, 2*( (m_fX*m_fY) + (m_fW*m_fZ) ));
    cMat.SetValue(0,2, 2*( (m_fX*m_fZ) - (m_fW*m_fY) ));

    cMat.SetValue(1,0, 2*( (m_fX*m_fY) - (m_fW*m_fZ) ));
    cMat.SetValue(1,1, 1 - 2*( (m_fX*m_fX) + (m_fZ*m_fZ) ));
    cMat.SetValue(1,2, 2*( (m_fY*m_fZ) + (m_fW*m_fX) ));

    cMat.SetValue(2,0, 2*( (m_fX*m_fZ) + (m_fW*m_fY) ));
    cMat.SetValue(2,1, 2*( (m_fY*m_fZ) - (m_fW*m_fX) ));
    cMat.SetValue(2,2, 1 - 2*( (m_fX*m_fX) + (m_fY*m_fY) ));

	//CVec angle = createEuler();    
    //cMat.CreateFromAngle(angle.GetX(), angle.GetY(), angle.GetZ());

	return cMat;
}