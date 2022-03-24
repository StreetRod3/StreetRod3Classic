/////////////////////////////////////////
//
//              Car Library
//        Auxiliary Software 2002
//
/////////////////////////////////////////


// Simulate car class
// Created 18/3/02
// By Jason Boettcher


#ifndef __CCARSIM_H__
#define __CCARSIM_H__


// Simple wheel structure
typedef struct {
	CVec	cOrigPos;
	CVec	cPos;


} wheel_t;


class CCarSim {
public:
	// Constructor
	CCarSim() {

		Mass = 1000;
		MassDist = 1500;
		AirDrag = 8;
	}


private:
	// Attributes

	float			Mass;		// kg
	float			Weight;		// N
	float			MassDist;	// kg.m
	float			AirDrag;


	CVec			Position_wc;
	CVec			Velocity_wc;

	CVec			Angles;
	CVec			AngularVelocity;

	CVec			vDimension;

	car_command_t	Command;

	//CRigidBody		cRBody;
	CSimBody		Sim;

	CWheel			Wheels[4];
	wheel_t			sWheels[4];

	contact_t		tContact;	


public:
	// Methods

	void			Setup(float _mass, float _massdist);

	void			Simulate(float dt);
	void			HandleCollision(void);

	void			DoPhysics(void);
	void			UpdateWheel(int id);

	CVec			GetWheelCol(int id);

	void			AntiRollBars(void);

	void			createDrawMatrix(float *m)		{ Sim.createDrawMatrix(m); }

	CWheel			*getWheel(int w)				{ return &Wheels[w]; }
	wheel_t			*getWheel2(int w)				{ return &sWheels[w]; }
	CSimBody		*getSimBody(void)				{ return &Sim; }
	car_command_t	*getCommand(void)				{ return &Command; }
	contact_t		*getContact(void)				{ return &tContact; }

	void			setDimension(CVec d)			{ vDimension = d; }

	CVec			getPosition(void)				{ return Position_wc; }
	CVec			getAngles(void)					{ return Angles; }


	void			Hack(void);




};



#endif  //  __CCARSIM_H__