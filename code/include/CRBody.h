/////////////////////////////////////////
//
//             Street Rod 3
//
//      Copyright Auxiliary Software 2000
//
//
/////////////////////////////////////////


// Rigid body class
// Created 22/10/02
// Jason Boettcher


#ifndef __CRBODY_H__
#define __CRBODY_H__


// Collision structure
typedef struct {
	int		iTouching;
	CVec	vPos;
	CVec	vNormal;
} collision_t;



class CRBody {
public:
	// Constructor
	CRBody() {
		ColModel = NULL;
		tCollision.iTouching = false;

		Mass = 1000;
		OneoverMass = 1 / Mass;
		CoefficientofRestitution = 0.5f;
	}


private:
	// Attributes

	// Entity attributes
	float		OneoverMass;
	float		Mass;
	CMatrix		InverseBodyInertia;
	float		CoefficientofRestitution;	
	CollisionModel3D *ColModel;


	// Rigid body attributes
	CVec		CMPosition;
	CMatrix		Orientation;
	CVec		CMVelocity;
	CVec		CMForce;

	CVec		AngularVelocity;
	CVec		AngularMomentum;
	CVec		Torque;

	CMatrix		InverseWorldInertia;


	// Collision
	collision_t	tCollision;


public:
	// Methods


	void		Setup(float _mass, CModel *model);

	void		Simulate(float dt);
	void		ComputeForces(float dt);
	void		Integrate(float dt);

	void		ResolveCollisions(void);

	void		AddForce(CVec v)			{ CMForce = CMForce + v; }

	void		setDrawingMatrix(float *oglmat);



	// Variables
	CVec		getCMPosition(void)			{ return CMPosition; }
	CMatrix		getOrientation(void)		{ return Orientation; }
	CVec		getCMVelocity(void)			{ return CMVelocity; }
	CVec		getCMForce(void)			{ return CMForce; }
	collision_t *getCollision(void)			{ return &tCollision; }
	float		getOneoverMass(void)		{ return OneoverMass; }


	void		setCollisionMod(CollisionModel3D *c)		{ ColModel = c; }
	void		setCMPosition(CVec p)		{ CMPosition = p; }

	
};





#endif  //  __CRBODY_H__