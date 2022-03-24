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


#ifndef __CMATRIX_H__
#define __CMATRIX_H__


class CMatrix {
public:
	// Constructor
	CMatrix() {
		LoadIdentity();
	}

private:
	// Attributes

	float	mat[3][3];

public:
	// Methods

	void	Load(FILE *fp);
    void    Save(FILE *fp);
    void    Print(void);

	void	LoadIdentity(void);

	void	CreateFromAngle(float pitch, float roll, float yaw);
    void    createFromArray(float *m);

	CVec	MultiplyVector(CVec vec);
	CVec	MultiplyVector2(CVec vec);
	void	Transpose(void);
	void	Rotate(CVec axis, float angle);

	void	CreateGLMatrix(CVec translate, float *OGL);
	void	CreateCDMatrix(CVec translate, float *mat);
    

	void	SkewSymmetric(CVec Crossv);
	void	OrthoNormalize(void);

    void    Inverse(void);

	
	float	GetValue(int i, int j)				{ return mat[i][j]; }
	void	SetValue(int i, int j, float v)		{ mat[i][j] = v; }


	// Operators
	CMatrix	operator*(CMatrix m);
	CMatrix operator*(float s);
	CMatrix operator+(CMatrix m);
    CVec    operator*(CVec v);


};




#endif  //  __CMATRIX_H__