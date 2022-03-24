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
// Created 15/5/02
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "System.h"

const int HALF_RAND = (RAND_MAX / 2);


///////////////////
// Vector length
float VectorLength(CVec v)
{
	return (float)sqrt(v.GetX()*v.GetX() + v.GetY()*v.GetY() + v.GetZ()*v.GetZ());
}


///////////////////
// Vector normalize
float VectorNormalize(CVec *v)
{
	float length = VectorLength(*v);

	if(length == 0)
		return length;

	v->SetX(v->GetX()/length);
	v->SetY(v->GetY()/length);
	v->SetZ(v->GetZ()/length);

	return length;
}


///////////////////
// Dot Product
float DotProduct(CVec v1, CVec v2)
{
	return v1.GetX()*v2.GetX() + v1.GetY()*v2.GetY() + v1.GetZ()*v2.GetZ();
}


///////////////////
// Cross Product
CVec CrossProduct(CVec v1, CVec v2)
{
	CVec cross;

	cross.SetX( v1.GetY()*v2.GetZ() - v1.GetZ()*v2.GetY());
	cross.SetY( v1.GetZ()*v2.GetX() - v1.GetX()*v2.GetZ());
	cross.SetZ( v1.GetX()*v2.GetY() - v1.GetY()*v2.GetX());

	return cross;
}


///////////////////
// Return an absolute vector
CVec VecAbs(CVec v)
{
	return CVec( (float)fabs(v.GetX()), (float)fabs(v.GetY()), (float)fabs(v.GetZ()));
}


///////////////////
// Get the distance between 2 vectors
float GetDistance(CVec v1, CVec v2)
{
	CVec dif = v2-v1;

	return (float)sqrt((float)( dif.GetX() * dif.GetX() + dif.GetY() * dif.GetY() + dif.GetZ() * dif.GetZ() ));
}


///////////////////
// Creates vectors based on angles
void GetAngles(float pitch, float roll, float yaw ,CVec *forward, CVec *right, CVec *up)
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;
	
	angle = yaw * (float)(PI*2 / 360);
	sy = (float)sin(angle);
	cy = (float)cos(angle);	

	angle = pitch * (float)(PI*2 / 360);
	sp = (float)sin(angle);
	cp = (float)cos(angle);	

	angle = roll * (float)(PI*2 / 360);
	sr = (float)sin(angle);
	cr = (float)cos(angle);

	if(forward)
		*forward = CVec(cp*cy,cp*sy,-sp);
	if(right)
		*right = CVec((-1*sr*sp*cy+-1*cr*-sy),(-1*sr*sp*sy+-1*cr*cy),-1*sr*cp);
	if(up)
		*up = CVec((cr*sp*cy+-sr*-sy), (cr*sp*sy+-sr*cy), cr*cp);
}


///////////////////
// Unproject the mouse pointer into 3D space
void UnprojectMouse(CVec *vProjected)
{
	GLint		viewport[4];
	GLdouble	mvmatrix[16],projmatrix[16];
	GLdouble	wx,wy,wz;
	int			realy;
	
	mouse_t		*mouse = System_GetMouse();

	// First unproject the 2D point into 3D space, and onto the plane
	glGetIntegerv(GL_VIEWPORT,viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX,mvmatrix);
	glGetDoublev(GL_PROJECTION_MATRIX,projmatrix);

	// Re-calculate position to account for viewport position
	realy = viewport[3] - mouse->Y;

	gluUnProject((GLdouble)mouse->X,(GLdouble)realy,1.0,mvmatrix,projmatrix,viewport,&wx,&wy,&wz);

	if(vProjected)
		*vProjected = CVec((float)wx, (float)wy, (float)wz);
}


///////////////////
// Unproject a 2D point into 3D space
CVec UnprojectPoint(CVec p)
{
    GLint		viewport[4];
	GLdouble	mvmatrix[16],projmatrix[16];
	GLdouble	wx,wy,wz;
	int			realy;
	
	mouse_t		*mouse = System_GetMouse();

	// First unproject the 2D point into 3D space, and onto the plane
	glGetIntegerv(GL_VIEWPORT,viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX,mvmatrix);
	glGetDoublev(GL_PROJECTION_MATRIX,projmatrix);

	// Re-calculate position to account for viewport position
	realy = viewport[3] - p.GetY();

	gluUnProject((GLdouble)p.GetX(),(GLdouble)realy,1.0,mvmatrix,projmatrix,viewport,&wx,&wy,&wz);

	return CVec((float)wx, (float)wy, (float)wz);
}


///////////////////
// Get a random number
float GetRandomNum(void)
{
	int rn;
	rn = rand();
	return ((float)(rn - HALF_RAND) / (float)HALF_RAND);
}


///////////////////
// Get a random integer with a max value
int GetRandomInt(int max)
{
	float f = GetRandomNum()*(float)(max+1);

	return MIN(max, abs((int)f) );
}



///////////////////
// Rotate a point
CVec RotateAround(CVec a, CVec b)
{
	CVec ret = a;
	
	float angle = VectorNormalize(&b);
	float s, c;
	float xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;
	float x,y,z;

	s = (float)sin((double)angle);
	c = (float)cos((double)angle);	

	if (angle>0) {
		xx = b.GetX() * b.GetX();
		yy = b.GetY() * b.GetY();
		zz = b.GetZ() * b.GetZ();
		xy = b.GetX() * b.GetY();
		yz = b.GetY() * b.GetZ();
		zx = b.GetZ() * b.GetX();
		xs = b.GetX() * s;
		ys = b.GetY() * s;
		zs = b.GetZ() * s;
		one_c = 1 - c;

		x = ((one_c * xx) + c)*a.GetX() + ((one_c * xy) - zs)*a.GetY() + ((one_c * zx) + ys)*a.GetZ();
		y = ((one_c * xy) + zs)*a.GetX() + ((one_c * yy) + c)*a.GetY() + ((one_c * yz) - xs)*a.GetZ();
		z = ((one_c * zx) - ys)*a.GetX() + ((one_c * yz) + xs)*a.GetY() + ((one_c * zz) + c)*a.GetZ();

		ret = CVec(x,y,z);
	}

	return ret;
}


///////////////////
// Calculate a plane based on 3 points
plane_t CalculatePlane(CVec a, CVec b, CVec c)
{
	plane_t p;

	p.vNormal = CrossProduct(b - a, c - a);
	VectorNormalize(&p.vNormal);
	p.fDistance = DotProduct(-p.vNormal,a);

	return p;
}


///////////////////
// Create a CD matrix based on position
void CreateCDMatrix(float *m, CVec v)
{
	memset(m,0,sizeof(float) * 16);

	m[0] =  1.0f;
	m[5] =  1.0f;
    m[10] = 1.0f;
	m[15] = 1.0f;

	// Set the translation
	m[12] = v.GetX();
	m[13] = v.GetY();
	m[14] = v.GetZ();
}



///////////////////
// Checks if the mouse point is inside a rectangle
int MouseinRect(int x1, int y1, int x2, int y2)
{
	mouse_t *Mouse = System_GetMouse();

	if(Mouse->X >= x1 && Mouse->X <= x2)
		if(Mouse->Y >= y1 && Mouse->Y <= y2)
			return true;

	return false;
}


///////////////////
// Limit an angle
float LimitAngle(float angle)
{
	while(angle>360)
		angle-=360;
	while(angle<0)
		angle+=360;

	return angle;
}


///////////////////
// Get the distance from a point to a plane
float ProjectPoint(plane_t *plane, CVec point)
{
    return DotProduct( plane->vNormal, point ) + plane->fDistance;
}





/*
   ----------------------------------------------------------------------------

   Check for Triangle-Triangle collision
   
   Obtained from Magic-Software.com

   ----------------------------------------------------------------------------
*/
static void TriTri_Projection(const CVec &rkD, const CVec akV[3], float &rfMin, float &rfMax)
{
    float afDot[3] = { DotProduct(rkD,akV[0]), DotProduct(rkD,akV[1]), DotProduct(rkD,akV[2]) };

    rfMin = afDot[0];
    rfMax = rfMin;

    if ( afDot[1] < rfMin )
        rfMin = afDot[1];
    else if ( afDot[1] > rfMax )
        rfMax = afDot[1];

    if ( afDot[2] < rfMin )
        rfMin = afDot[2];
    else if ( afDot[2] > rfMax )
        rfMax = afDot[2];
}



bool TriTri_Intersection(CVec akU[3],  CVec akV[3])
{
    CVec kDir;
    float fUMin, fUMax, fVMin, fVMax;
    int i0, i1;

    // direction N
    CVec akE[3] = { akU[1]-akU[0], akU[2]-akU[1], akU[0]-akU[2] };
    CVec kN = CrossProduct(akE[0], akE[1]);
    float fNdU0 = DotProduct(kN, akU[0]);
    TriTri_Projection(kN,akV,fVMin,fVMax);
    if ( fNdU0 < fVMin || fNdU0 > fVMax )
        return false;

    // direction M
    CVec akF[3] = { akV[1]-akV[0], akV[2]-akV[1], akV[0]-akV[2] };
    CVec kM = CrossProduct(akF[0], akF[1]);

    const float fEpsilon = 1e-06f;  // cutoff: sin(Angle(N,M)) < epsilon
    CVec kNxM = CrossProduct(kN, kM);
    if ( DotProduct(kNxM, kNxM) >= fEpsilon*(DotProduct(kN, kN) * DotProduct(kM, kM)) )
    {
        // triangles are not parallel
        float fMdV0 = DotProduct(kM, akV[0]);
        TriTri_Projection(kM,akU,fUMin,fUMax);
        if ( fMdV0 < fUMin || fMdV0 > fUMax )
            return false;

        // directions E[i0]xF[i1]
        for (i1 = 0; i1 < 3; i1++)
        {
            for (i0 = 0; i0 < 3; i0++)
            {
                kDir = CrossProduct(akE[i0], akF[i1]);
                TriTri_Projection(kDir,akU,fUMin,fUMax);
                TriTri_Projection(kDir,akV,fVMin,fVMax);
                if ( fUMax < fVMin || fVMax < fUMin )
                    return false;
            }
        }
    }
    else  // triangles are parallel (and, in fact, coplanar)
    {
        // directions NxE[i0]
        for (i0 = 0; i0 < 3; i0++)
        {
            kDir = CrossProduct(kN, akE[i0]);
            TriTri_Projection(kDir,akU,fUMin,fUMax);
            TriTri_Projection(kDir,akV,fVMin,fVMax);
            if ( fUMax < fVMin || fVMax < fUMin )
                return false;
        }

        // directions NxF[i1]
        for (i1 = 0; i1 < 3; i1++)
        {
            kDir = CrossProduct(kM, akF[i1]);
            TriTri_Projection(kDir,akU,fUMin,fUMax);
            TriTri_Projection(kDir,akV,fVMin,fVMax);
            if ( fUMax < fVMin || fVMax < fUMin )
                return false;
        }
    }

    return true;
}