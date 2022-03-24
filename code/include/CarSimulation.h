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



// Car simulation header file
// Created 23/02/03
// Jason Boettcher


#ifndef __CARSIM_H__
#define __CARSIM_H__


// Camera positions
#define     CAM_OUTSIDE     0
#define     CAM_INCAR       1


// Wheel structure
typedef struct {

	CVec	cRelPos;
	CVec	cPos;
	CVec	cNormal;

	float	fPistonVelocity;
	float	fSuspLength;
	bool	bCollision;

    // Performance
    float   fSpring;
    float   fDampening;
    float   fRestLength;
    float   fCornering;
    
    float   fTorque;
    float   fEngineLoad;
    bool    bSlip;

	float	fSpeed;
	float	fSpin;
    float   fRadius;

	float	fWheelZ;

} wheel_t;



// carsim structure
typedef struct carsim_s {
	// Simulation

	CVec	cPos;
	CVec	cVelocity;
	CVec	cAngle;
	CVec	cAngVelocity;
    double  dTime;

	CVec	X,Y,Z;


	// Input
	float   fAcceleration;
	bool	bBraking;
	bool	bHandBrake;
	

	// Engine
	int		nRPM;
	int		nGear;
	bool	bReversing;

	// Transmission
	int		bAutomatic;
	int		iNumGears;
	float	fGearRatio[33]; // Allow for up to 32 gears plus neutral
	
	// Wheels
	float	fSteerAngle;
    float   fCornering;
	wheel_t	sWheels[4];

	// Collision
	bool	bCollision;
	CVec	cColNormal;
    CVec    cColPoint;
	bool	bFlipping;
	CVec	cFlipNormal;
	bool	bCarCol;

	CVec	cColTemp[2];
	
	// Camera stuff	
    int     nCameraType;
	CVec	cDestCamPos;
    CVec    cInCarCamera;

    // Game
    bool    bFinished;

    // Stats
    float   fRaceTime;
    float   fMaxSpeed;
	
} carsim_t;

class CCar;


// Functions
void	Car_SetupCar(carsim_t *psCar, CCar *pcCar);
void	Car_ProcessCar(carsim_t *psCar, CModel *pcTrack, CCar *pcCar, CCamera *pcCam, double dt, bool bPaused);
void	Car_SimulateCar(carsim_t *psCar, CModel *pcTrack,  float dt);

void	Car_SimulateEngine(carsim_t *psCar, int dt, CVec *pForce, CVec *pTorque);

void	Car_UpdateWheel(carsim_t *psCar, CModel *pcTrack, int id);
CVec	Car_GetWheelForce(carsim_t *psCar, int i);
void	Car_CheckCollisions(carsim_t *psCar, CCar *pcCar, CModel *pcTrack);
void	Car_Straighten(carsim_t *psCar);

void    Car_CheckCarCollisions(CCar *pcCar1, CCar *pcCar2);

void	Car_SetupCamera(carsim_t *psCar, CCamera *pcCam, float dt);
void    Car_SetRearviewCam(CCar *pcCar, CCamera *pcCam);




#endif  //  __CARSIM_H__