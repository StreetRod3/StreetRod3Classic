/////////////////////////////////////////
//
//              Car Library
//        Auxiliary Software 2002
//
/////////////////////////////////////////


// Simulation body class
// Created 17/3/02
// By Jason Boettcher


#ifndef __CSIMBODY_H__
#define __CSIMBODY_H__


class CSimBody {
public:
	// Constructor
	CSimBody() {
		Clear();

	}



private:
	// Attributes
	CVec	X,Y,Z;
	CVec	Angles;
	CVec	Position;
	CVec	Velocity;
	CVec	AngVelocity;

	CVec	Force,Torque;
	float	Mass;
	float	RotInertia;



public:
	// Methods

	void	Clear(void);
	void	Setup(float _mass, float _rotinertia);

	void	AddForce(CVec pos, CVec mag);
	void	AddLatForce(CVec pos, CVec mag);	

	void	Simulate(float dt);

	CVec	GetAbsCoord(CVec RelPos);
	CSimBody GetSimBody(CVec RelPos);

	void	Rotate(CVec axis);
	void	Straighten(void);

	void	createDrawMatrix(float *m);
	void	createCDMatrix(float *m);

	void	setPosition(CVec _pos)		{ Position = _pos; }
	CVec	getPosition(void)			{ return Position; }

	void	setVelocity(CVec _v)		{ Velocity = _v; }	
	CVec	getVelocity(void)			{ return Velocity; }

	CVec	getTorque(void)				{ return Torque; }
	void	setTorque(CVec _t)			{ Torque = _t; }

	CVec	getAngVelocity(void)		{ return AngVelocity; }
	void	setAngVelocity(CVec _a)		{ AngVelocity = _a; }

	CVec	getX(void)					{ return X; }
	CVec	getY(void)					{ return Y; }
	CVec	getZ(void)					{ return Z; }

	void	setX(CVec _x)				{ X=_x; }
	void	setY(CVec _y)				{ Y=_y; }
	void	setZ(CVec _z)				{ Z=_z; }

	CVec	getDir(void)				{ return Y; }
	CVec	getUp(void)					{ return Z; }
	CVec	getRight(void)				{ return X; }

	float	getMass(void)				{ return Mass; }
};





#endif  //  __CSIMBODY_H__
