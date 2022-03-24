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


// Lite Matrix 3x3 class
// Created 18/2/02
// By Jason Boettcher


#include "defs.h"
#include "3D.h"

const float	R2D(5.729578e+1f);

///////////////////
// Load from a file
void CMatrix::Load(FILE *fp)
{
	for(int i=0;i<3;i++)
		for(int j=0;j<3;j++) {
			fread(&mat[j][i],sizeof(float),1,fp);
		}
}


///////////////////
// Save the matrix to a file
void CMatrix::Save(FILE *fp)
{
    for(int i=0;i<3;i++)
		for(int j=0;j<3;j++) {
			fwrite(&mat[j][i],sizeof(float),1,fp);
		}
}


///////////////////
// Print out the matrix
void CMatrix::Print(void)
{
    printf("%f %f %f\n", mat[0][0], mat[1][0], mat[2][0]);
    printf("%f %f %f\n", mat[0][1], mat[1][1], mat[2][1]);
    printf("%f %f %f\n", mat[0][2], mat[1][2], mat[2][2]);
}


///////////////////
// Create the matrix from an array
void CMatrix::createFromArray(float *m)
{
    //memcpy(mat,m,sizeof(float)*9);
    int n=0;
    for(int i=0;i<3;i++) {
		for(int j=0;j<3;j++) {
            mat[j][i] = m[n++];
        }
    }
}


///////////////////
// Load an identy matrix
void CMatrix::LoadIdentity(void)
{
	memset(mat,0,sizeof(mat));

	mat[0][0] = mat[1][1] = mat[2][2] = 1.0f;
}


///////////////////
// Create a matrix from angles
void CMatrix::CreateFromAngle(float pitch, float roll, float yaw)
{
	CVec forward, right, up;
	int i;

	memset(mat,0,sizeof(mat));

	GetAngles(pitch,roll,yaw,&forward,&right,&up);

	for (i=0;i<3;i++) {
		mat[i][0] =  forward.GetIndex(i);
		mat[i][1] = -right.GetIndex(i);
		mat[i][2] =  up.GetIndex(i);
	}
}


///////////////////
// Multiply a vector by the matrix
CVec CMatrix::MultiplyVector(CVec vec)
{
	CVec v;
	int i,k;
	float value;

    for(i=0;i<3;i++)
    {
        value = 0;
        for(k=0;k<3;k++)        
            value += mat[i][k] * vec.GetIndex(k);        

        v.SetIndex(i,value);
    }

	return v;
}


///////////////////
// Multiply a vector by the matrix
CVec CMatrix::MultiplyVector2(CVec vec)
{
	CVec v;
	int i,k;
	float value;

    for(i=0;i<3;i++)
    {
        value = 0;
        for(k=0;k<3;k++)        
            value += mat[k][i] * vec.GetIndex(k);        

        v.SetIndex(i,value);
    }

	return v;
}


///////////////////
// Create an OpenGL matrix from this matrix
void CMatrix::CreateGLMatrix(CVec translate, float *OGL)
{
	float (*OGL44)[4] = (float(*)[4])OGL;
	int row,col;	

	// this is here because OGL matrix's are transposed
	for(row=0;row<3;row++)
    {
        for(col=0;col<3;col++)        
            OGL44[col][row] = mat[row][col];
	}

	OGL44[0][3] = OGL44[1][3] = OGL44[2][3] = 0.0f;
	OGL44[3][3] = 1.0f;

	// set translation
	for(row=0;row<3;row++)
        OGL44[3][row] = translate.GetIndex(row);
}


///////////////////
// Create a straight 16 matrix (for the Collision Detection system)
void CMatrix::CreateCDMatrix(CVec translate, float *m)
{
	// Transposed
	m[0] =  mat[0][0]; m[1] =  mat[1][0]; m[2] =  mat[2][0]; m[3] =  0.0f;
	m[4] =  mat[0][1]; m[5] =  mat[1][1]; m[6] =  mat[2][1]; m[7] =  0.0f;
    m[8] =  mat[0][2]; m[9] =  mat[1][2]; m[10] = mat[2][2]; m[11] = 0.0f;
															 m[15] = 1.0f;

	/*
	m[0] =  mat[0][0]; m[1] =  mat[0][1]; m[2] =  mat[0][2]; m[3] =  0.0f;
	m[4] =  mat[1][0]; m[5] =  mat[1][1]; m[6] =  mat[1][2]; m[7] =  0.0f;
    m[8] =  mat[2][0]; m[9] =  mat[2][1]; m[10] = mat[2][2]; m[11] = 0.0f;
															 m[15] = 1.0f;
	*/

	// Set the translation
	m[12] = translate.GetX();
	m[13] = translate.GetY();
	m[14] = translate.GetZ();
}


///////////////////
// Multiple a matrix by this matrix
CMatrix CMatrix::operator *(CMatrix m)
{
	CMatrix m2;
	float value;
	int i,j,k;

	m2.LoadIdentity();	
	
	for(i=0;i<3;i++) {
        for(j=0;j<3;j++) {
            value = 0;
            for(k=0;k<3;k++)            
                value += mat[i][k] * m.GetValue(k,j);
            
			m2.SetValue(i,j,value);
        }
    }

	return m2;
}


///////////////////
// Scale the matrix by a scalar
CMatrix CMatrix::operator *(float s)
{
	CMatrix m2;

	for(int n=0;n<3;n++) {
		m2.SetValue(0,n, mat[0][n]);
		m2.SetValue(1,n, mat[1][n]);
		m2.SetValue(2,n, mat[2][n]);
	}

	return m2;
}


///////////////////
// Add a matrix with this matrix
CMatrix CMatrix::operator +(CMatrix m)
{
	CMatrix m2;
	int n;

	for(n=0;n<3;n++) {
		m2.SetValue(0,n, mat[0][n] + m.GetValue(0,n) );
		m2.SetValue(1,n, mat[1][n] + m.GetValue(1,n) );
		m2.SetValue(2,n, mat[2][n] + m.GetValue(2,n) );
	}

	return m2;
}


///////////////////
// Multiply a vector with this matrix
CVec CMatrix::operator *(CVec v)
{
    return MultiplyVector(v);
}


///////////////////
// Transpose this matrix
void CMatrix::Transpose(void)
{
	int n;
	CMatrix m;

	for(n=0;n<3;n++) {
		m.SetValue(0,n, mat[n][0]);
		m.SetValue(1,n, mat[n][1]);
		m.SetValue(2,n, mat[n][2]);        
    }

	*this = m;
}


///////////////////
// Skew the matrix symetrically by a vector
void CMatrix::SkewSymmetric(CVec Crossv)
{
	mat[0][0] = 0.0f;			mat[0][1] = -Crossv.GetZ();	mat[0][2] = Crossv.GetY();
	mat[1][0] = Crossv.GetZ();	mat[1][1] = 0.0f;			mat[1][2] = -Crossv.GetX();
	mat[2][0] = -Crossv.GetY();	mat[2][1] = Crossv.GetX();	mat[2][2] = 0.0f;
}


///////////////////
// Orthogonically normalize this matrix
void CMatrix::OrthoNormalize(void)
{
	CVec x,y,z;
	
	x.SetX( mat[0][0] );
	x.SetY( mat[1][0] );
	x.SetZ( mat[2][0] );
	y.SetX( mat[0][1] );
	y.SetY( mat[1][1] );
	y.SetZ( mat[2][1] );

	VectorNormalize(&x);
	z = CrossProduct(x,y);	VectorNormalize(&z);
	y = CrossProduct(z,x);	VectorNormalize(&y);

	for(int n=0; n<3; n++) {
		mat[n][0] = x.GetIndex(n);
		mat[n][1] = y.GetIndex(n);
		mat[n][2] = z.GetIndex(n);
	}	
}


///////////////////
// Rotate the matrix
void CMatrix::Rotate(CVec axis, float angle)
{
	float angleRad = angle * (PI/180.0);
	float c = cos(angleRad);
	float s = sin(angleRad);
	float t = 1.0 - c;

	LoadIdentity();

	VectorNormalize(&axis);

	mat[0][0] = t * axis.GetX() * axis.GetX() + c;
	mat[1][0] = t * axis.GetX() * axis.GetY() - s * axis.GetZ();
	mat[2][0] = t * axis.GetX() * axis.GetZ() + s * axis.GetY();

	mat[0][1] = t * axis.GetX() * axis.GetY() + s * axis.GetZ();
	mat[1][1] = t * axis.GetY() * axis.GetY() + c;
	mat[2][1] = t * axis.GetY() * axis.GetZ() - s * axis.GetX();

	mat[0][2] = t * axis.GetX() * axis.GetZ() - s * axis.GetY();
	mat[1][2] = t * axis.GetY() * axis.GetZ() + s * axis.GetX();
	mat[2][2] = t * axis.GetZ() * axis.GetZ() + c;
}


///////////////////
// Inverse the matrix
void CMatrix::Inverse(void)
{
    float det = mat[0][0]*mat[1][1]*mat[2][2] -
                mat[0][0]*mat[2][1]*mat[1][2] +
                mat[1][0]*mat[2][1]*mat[0][2] -
                mat[1][0]*mat[0][1]*mat[2][2] +
                mat[2][0]*mat[0][1]*mat[1][2] -
                mat[2][0]*mat[1][1]*mat[0][2];

    if( det == 0 )
        det = 1;

    // Create a new temp array
    float m[3][3];

    m[0][0] =  (mat[1][1]*mat[2][2] - mat[1][2]*mat[2][1])/det;
    m[1][0] = -(mat[0][1]*mat[2][2] - mat[0][2]*mat[2][1])/det;
    m[2][0] =  (mat[0][1]*mat[1][2] - mat[0][2]*mat[1][1])/det;
    m[0][1] = -(mat[1][0]*mat[2][2] - mat[1][2]*mat[2][0])/det;
    m[1][1] =  (mat[0][0]*mat[2][2] - mat[0][2]*mat[2][0])/det;
    m[2][1] = -(mat[0][0]*mat[1][2] - mat[0][2]*mat[1][0])/det;
    m[0][2] =  (mat[1][0]*mat[2][1] - mat[1][1]*mat[2][0])/det;
    m[1][2] = -(mat[0][0]*mat[2][1] - mat[0][1]*mat[2][0])/det;
    m[2][2] =  (mat[0][0]*mat[1][1] - mat[0][1]*mat[1][0])/det;


    // Copy back into our member
    for( int i=0; i<3; i++ ) {
        for( int j=0; j<3; j++ ) {
            mat[j][i] = m[j][i];
        }
    }
}