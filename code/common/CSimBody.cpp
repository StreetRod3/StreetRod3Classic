/////////////////////////////////////////
//
//              Car Library
//        Auxiliary Software 2002
//
/////////////////////////////////////////


// Simulation body class
// Created 17/3/02
// By Jason Boettcher


#include "defs.h"
#include "Game.h"


///////////////////
// Clear the sim body
void CSimBody::Clear(void)
{
	Position = CVec(0,0,0);
	Velocity = AngVelocity = CVec(0,0,0);
	Force = Torque = CVec(0,0,0);
	Mass = RotInertia = 0;

	X = CVec(1,0,0);
	Y = CVec(0,1,0);
	Z = CVec(0,0,1);
}


///////////////////
// Setup the sim body
void CSimBody::Setup(float _mass, float _rotinertia)
{
	Mass = _mass;
	RotInertia = _rotinertia;
}


///////////////////
// Add a force to the body
void CSimBody::AddForce(CVec pos, CVec mag)
{
	Force = Force + mag;
	Torque = Torque + CrossProduct(pos-Position,mag);
}

///////////////////
// Add a lateral force (from sideways action) to the body
void CSimBody::AddLatForce(CVec pos, CVec mag)
{
	CVec t;
	Force = Force + mag;
	
	t = CrossProduct(pos-Position,mag);

	//t.SetY(0);

	// Limit the roll
	Torque = Torque + t;
}


///////////////////
// Simulate the body
void CSimBody::Simulate(float dt)
{
	// Dampening
	Torque = Torque + AngVelocity * -0.4f;
	Force = Force + Velocity * -0.4f;

	Velocity = Velocity + (Force/Mass) * dt;
	AngVelocity = AngVelocity + (Torque/RotInertia) * dt;

	
	// Clear forces
	Force = Torque = CVec(0,0,0);	

	// Integrate into actual position/rotation
	Position = Position + Velocity*dt;
	Rotate(AngVelocity * dt);	
}


///////////////////
// Rotate around an axis
void CSimBody::Rotate(CVec axis)
{
	Y = RotateAround(Y,axis);
	Z = RotateAround(Z,axis);
	Straighten();
}


///////////////////
// Straight the sim body
void CSimBody::Straighten(void)
{
	X = CrossProduct(Y,Z);
	Z = CrossProduct(X,Y);

	VectorNormalize(&X);
	VectorNormalize(&Y);
	VectorNormalize(&Z);
}


///////////////////
// Create a drawing matrix
void CSimBody::createDrawMatrix(float *m)
{
	int i;

	// Clear it
	for(i=0;i<16;i++)
		m[i] = 0;

	for(i=0;i<3;i++)
		m[i] = X.GetIndex(i);
	for(i=4;i<7;i++)
		m[i] = Y.GetIndex(i-4);
	for(i=8;i<11;i++)
		m[i] = Z.GetIndex(i-8);
	for(i=12;i<15;i++)
		m[i] = Position.GetIndex(i-12);
	m[15] = 1;
}


///////////////////
// Create a collision detection matrix
void CSimBody::createCDMatrix(float *m)
{
	int i;

	// Clear it
	for(i=0;i<16;i++)
		m[i] = 0;

	// Position
	for(i=12;i<15;i++)
		m[i] = Position.GetIndex(i-12);



}


///////////////////
// Get the absolute coord from a relative position
CVec CSimBody::GetAbsCoord(CVec RelPos)
{
	return X*RelPos.GetX() + Y*RelPos.GetY() + Z*RelPos.GetZ() + Position;
}


///////////////////
// Get the simbody with an edited speed at a pos on the body
CSimBody CSimBody::GetSimBody(CVec RelPos)
{
	CSimBody tmp;

	tmp.Clear();
	tmp.setPosition(GetAbsCoord(RelPos));
	tmp.setY(Y);
	tmp.setZ(Z);
	tmp.Straighten();

	tmp.setVelocity( Velocity + CrossProduct(AngVelocity,tmp.getPosition()-Position));

	return tmp;
}
