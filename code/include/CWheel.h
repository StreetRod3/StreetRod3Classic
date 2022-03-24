/////////////////////////////////////////
//
//              Car Library
//        Auxiliary Software 2002
//
/////////////////////////////////////////


// Wheel class
// Created 17/3/02
// By Jason Boettcher


#ifndef __CWHEEL_H__
#define __CWHEEL_H__


// Contact with ground structure
typedef struct {
	int		Touching;
	CVec	modPos;
	CVec	Pos;
	CVec	Norm;
	float	PlaneDist;
	int		SurfType;

} contact_t;


class CWheel {
public:
	// Constructor
	CWheel() {
		id= 0;
		Clear();
		Default(false);

	}


private:
	// Attributes

	int			id;

	CVec		RelPos;
	CVec		OriginalPos;
	float		SpringStiffness;
	float		MaxTravel;
	float		MaxLoad;			//load beyond which the tyre is no longer efficient
	float		WheelRadius;
	float		DamperCompression;
	float		DamperRebound;
	float		Grip;
	float		AccelFactor;
	float		BrakeFactor;
	float		SteerFactor;
	int			LockUp;

	float		Thrust;
	float		RollingAngle;
	int			Lock;
	CVec		Accel;
	CSimBody	Sim;
	CVec		Speed,Direction;
	float		Gs;
	float		Travel, DeltaTravel;
	contact_t	Contact;
	float		Length;

	// New variables
	float		fSuspLength;
	float		fRestLength;
	float		fMinLength;
	float		fMaxLength;
	CVec		vPistonVelocity;
	CVec		vSuspensionPosition;

	int			iDrawSpecial;			// Usefull with debugging


	// Forces
	CVec		LateralForce;
	CVec		LongForce;


public:
	// Methods

	void		Clear(void);
	void		Default(int frontwheel);
	void		Setup(CVec _relpos, int frontwheel, CVec CarPos, int _id);

	CVec		CalculateForce(CVec normalpos, float dt);
	void		Update(CSimBody *_sim, car_command_t *Command, float dt);

	CSimBody	*getSimBody(void)						{ return &Sim; }
	CVec		getRelPos(void)							{ return RelPos; }

	CVec		getLateralForce(void)					{ return LateralForce; }
	
	void		getContactRay(CSimBody sim, CVec *pos, CVec *mag);
	void		setContact(contact_t *Cont)				{ Contact = *Cont; }
	contact_t	*getContact(void)						{ return &Contact; }

	float		getAccel(void)							{ return AccelFactor; }
	
	int			drawSpecial(void)						{ return iDrawSpecial; }

	CVec		getSuspensionPos(void)					{ return vSuspensionPosition; }



};





#endif  //  __CWHEEL_H__