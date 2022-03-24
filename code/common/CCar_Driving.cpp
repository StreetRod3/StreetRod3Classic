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


// Car class - Driving routines
// Created 14/7/02
// Jason Boettcher


#include "defs.h"
#include "System.h"
#include "game.h"


#define		ADD_QUAD(x1,y1,z1, x2,y2,z2, x3,y3,z3, x4,y4,z4) \
	        BoundBox->addTriangle(x1,y1,z1, x2,y2,z2, x4,y4,z4); \
			BoundBox->addTriangle(x2,y2,z2, x3,y3,z3, x4,y4,z4);



///////////////////
// Setup the simulation
void CCar::SetupSimulation(void)
{
	Car_SetupCar( &sCarSim, this );


    // Find the bounding box for the car
	CVec min = CVec(9999,9999,9999);
	CVec max = CVec(-9999,-9999,-9999);

	vector_t	*v, *v2;

    v = cModel->getVertexList();

	// Go through each mesh
	mesh_t *m = cModel->getMeshes();
	for( int i=0; i<cModel->getNumMeshes(); i++,m++ ) {

		// Go through each poly
		polygon_t *poly = m->psPolygons;
		for( int p=0; p<m->nNumPolygons; p++,poly++ ) {

			// Go through each vertex
			for(int n=0; n<3; n++) {
				v2 = v+poly->nVertex[n];

				// Min
				if( v2->X < min.GetX() )
					min.SetX(v2->X);
				if( v2->Y < min.GetY() )
					min.SetY(v2->Y);
				if( v2->Z < min.GetZ() )
					min.SetZ(v2->Z);

				// Max
				if( v2->X > max.GetX() )
					max.SetX(v2->X);
				if( v2->Y > max.GetY() )
					max.SetY(v2->Y);
				if( v2->Z > max.GetZ() )
					max.SetZ(v2->Z);
			}
		}
	}

	cDim[0] = min;
	cDim[1] = max;

    // Setup the bounding box
    BoundBox = newCollisionModel3D(false);


	// Top & Bottom
	ADD_QUAD(min.GetX(),min.GetY(),max.GetZ(),
			 max.GetX(),min.GetY(),max.GetZ(),
			 max.GetX(),max.GetY(),max.GetZ(),
			 min.GetX(),max.GetY(),max.GetZ())
    ADD_QUAD(min.GetX(),min.GetY(),min.GetZ(),
			 max.GetX(),min.GetY(),min.GetZ(),
			 max.GetX(),max.GetY(),min.GetZ(),
			 min.GetX(),max.GetY(),min.GetZ())

	// Left & Right
	ADD_QUAD(min.GetX(),min.GetY(),min.GetZ(),
			 min.GetX(),min.GetY(),max.GetZ(),
			 min.GetX(),max.GetY(),max.GetZ(),
			 min.GetX(),max.GetY(),min.GetZ())
    ADD_QUAD(max.GetX(),min.GetY(),min.GetZ(),
			 max.GetX(),min.GetY(),max.GetZ(),
			 max.GetX(),max.GetY(),max.GetZ(),
			 max.GetX(),max.GetY(),min.GetZ())

	// Front & Back
	ADD_QUAD(min.GetX(),min.GetY(),min.GetZ(),
			 max.GetX(),min.GetY(),min.GetZ(),
			 max.GetX(),min.GetY(),max.GetZ(),
			 min.GetX(),min.GetY(),max.GetZ())
    ADD_QUAD(min.GetX(),max.GetY(),min.GetZ(),
			 max.GetX(),max.GetY(),min.GetZ(),
			 max.GetX(),max.GetY(),max.GetZ(),
			 min.GetX(),max.GetY(),max.GetZ())

	BoundBox->finalize();	
}


///////////////////
// Simulate the driving
void CCar::SimulateDriving(float dt)
{
	/*keyboard_t *kb = System_GetKeyboard();
	
	// Car commands
	car_command_t *cmd = cCarSim.getCommand();

	// Clear the command
	cmd->Accel = cmd->Brake = cmd->Handbrake = false;
	

	if(kb->KeyDown[SDLK_UP])
		cmd->Accel = true;
	if(kb->KeyDown[SDLK_DOWN])
		cmd->Brake = true;
	if(kb->KeyDown[SDLK_SPACE])
		cmd->Handbrake = true;

	float Steering = 5;
	float Steer_Springback = 3;
	
	// Steering
	if(kb->KeyDown[SDLK_LEFT])
		cmd->Steering += Steering*dt;
	else if(kb->KeyDown[SDLK_RIGHT])
		cmd->Steering -= Steering*dt;
	else {
		if(cmd->Steering > 0)
			cmd->Steering -= Steer_Springback*dt;
		if(cmd->Steering < 0)
			cmd->Steering += Steer_Springback*dt;

		// Default position
		if(fabs(cmd->Steering) < 0.1f)
			cmd->Steering = 0;
	}

	// Clamp the steering
	cmd->Steering = MAX(cmd->Steering, -1);
	cmd->Steering = MIN(cmd->Steering, 1);


	// Step
	//if(System_GetKeyboard()->KeyDown[SDLK_s])
		cCarSim.Simulate(dt);

	if(System_GetKeyboard()->KeyDown[SDLK_x])
		cCarSim.getSimBody()->setAngVelocity(CVec(0,0,10));

	if(System_GetKeyboard()->KeyDown[SDLK_v])
		cCarSim.getSimBody()->setVelocity(CVec(0,10,0));

	// Reset
	if(System_GetKeyboard()->KeyUp[SDLK_r]) {
		
		cCarSim.Setup(1000,1500);
		cCarSim.getContact()->Touching = false;
	
		cCarSim.getSimBody()->setPosition(CVec(10,760,10));
		
		//cCarSim.getSimBody()->Rotate(CVec(0,-3.14,0));
		//cCarSim.getSimBody()->setVelocity(CVec(0,300,0));
	}


	if(System_GetKeyboard()->KeyDown[SDLK_t])
		cCarSim.getSimBody()->setAngVelocity( CVec(0,0,0) );

*/

}

