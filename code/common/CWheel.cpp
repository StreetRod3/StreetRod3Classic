/////////////////////////////////////////
//
//              Car Library
//        Auxiliary Software 2002
//
/////////////////////////////////////////


// Wheel class
// Created 18/3/02
// By Jason Boettcher


#include "defs.h"
#include "System.h"
#include "Game.h"


const float	D2R(1.745329e-2f); // degrees to radians
const float	R2D(5.729578e+1f); // radians to degrees

#define DEG2RAD(a)  (a * D2R)
#define RAD2DEG(a)  (a * R2D)



// Lateral Force curve
double LateralCurve[] = {

//  Angle	    Force scalar

	0,			0,
	2.5,		0.788571,
	3.450000,	0.905714,
	4.750000,	0.971429,
	6.000000,	1.000000,
	8.050000,	0.991429,
	10.550000,	0.962857,
	30.000000,	0.750000,
	90.000000,	0.628571,
	153.899994, 0.757143,
	165.000000, 0.960000,
	167.000000,	0.990000,
	170.000000,	1.000000,
	173.000000,	0.950000,
	180.000000,	0.000000

};


///////////////////
// Clear the wheel info
void CWheel::Clear(void)
{
	Accel = CVec(0,0,0);
	RollingAngle = 0;
	DeltaTravel = 0;
	Travel = 0;
	Gs = 0;
	Direction = CVec(0,0,0);
	Speed = CVec(0,0,0);
	Lock = false;
	Thrust = 0;
	Length = 0;
	iDrawSpecial = false;

	Contact.Touching = false;
}


///////////////////
// Give the wheel default values
void CWheel::Default(int frontwheel)
{
	RelPos = CVec(0,0,0);
	SpringStiffness = 500;		// 30000
	MaxTravel = -0.7f;
	MaxLoad = 5000;
	WheelRadius = 0.4f;
	DamperCompression = 8000;
	DamperRebound = 5000;
	Grip = 1.0f;
	LockUp = true;

	// Suspension	
	fRestLength = 2.5f;
	fMinLength = 0.5f;
	fMaxLength = 3;
	fSuspLength = fRestLength;

	
	if(frontwheel) {
		Grip = 1.0f;
		AccelFactor = 0;
		BrakeFactor = 4000;
		SteerFactor = 0.55f;
	} else {
		Grip = 1.0f;
		AccelFactor = 6000;
		BrakeFactor = 2000;
		SteerFactor = 0.0f;
	}
}


///////////////////
// Setup the wheel
void CWheel::Setup(CVec _relpos, int frontwheel, CVec CarPos, int _id)
{
	id = _id;
	Clear();
	Default(frontwheel);

	RelPos = _relpos;
	Sim.setPosition(CarPos + RelPos);
	OriginalPos = Sim.getPosition();
}


///////////////////
// Get a ray for the contact detected
void CWheel::getContactRay(CSimBody sim, CVec *pos, CVec *mag)
{
	//*pos = sim.getPosition() - sim.getUp() * fRestLength + CVec(0,0,1);
	*pos = sim.getPosition() - sim.getUp() * fRestLength;
	*mag = -sim.getUp();
}


///////////////////
// Calculate the force on the car
CVec CWheel::CalculateForce(CVec normalpos, float dt)
{
	CVec Force;
	CVec Traction;
	CVec TyreTraction;

	LateralForce = CVec(0,0,0);



	// Not touching
	if(!Contact.Touching)
		return CVec(0,0,0);


	
	float Load = 0;

	CVec f;
	float damperRate = 0;

	// Spring constant
	float k = 65000;	// old = 15000

	// Spring force
	f.SetZ( k * (fRestLength-fSuspLength) );	

	// Dampening forces
	if(vPistonVelocity.GetZ() > 0)
		// Stretching
		damperRate = DamperRebound;
	else
		// Compressing
		damperRate = DamperCompression;

	//////////////////
	Load = f.GetZ() - ( damperRate * vPistonVelocity.GetZ() );

	
	float tanO = VectorLength( CrossProduct(Contact.Norm, Sim.getUp()))
			/ DotProduct(Contact.Norm,Sim.getUp());
	tanO = LIMIT(0.f,tanO,1.f);
	Load = (1+tanO)*Load;
	f = Contact.Norm * Load;

	//f.SetZ(f.GetZ() - ( damperRate * vPistonVelocity.GetZ() ));

	// Divide the force by 4 (spread the force over the 4 wheels)
	Force  = f / 4;

	// Make sure we don't give negative forces
	Force.SetZ( MAX(0,Force.GetZ()) );





	
	//calculate where the wheel is pushing:
	Load = Force.GetZ();



	CVec sp = Speed;
	VectorNormalize(&sp);

	float spDir = atan2(sp.GetX(), sp.GetY());
	float tyDir = atan2(Direction.GetX(), Direction.GetY());

	float slipAngle = spDir - tyDir;


	// No slip angle at low speeds
	if(fabs(Speed.GetX()) < 1.0f && fabs(Speed.GetY()) < 1.0f)
		slipAngle = 0;	


	// Keep slipAngle between -180..180 degrees
    if(slipAngle<-PI)slipAngle+=2*PI;
    else if(slipAngle>PI)slipAngle-=2*PI;

	float angle = RAD2DEG(slipAngle);
	float force = 2000;
	int minus = false;

	if(angle < 0) {
		angle = -angle;
		minus = true;
	}



	// Find the appropriate force on the lateral curve
	float a = angle;
	float an = 0;
	
	for(int i=0;i<sizeof(LateralCurve)-2;i+=2) {

		// Found the region the point is in, interpolate to find the matching point
		if(a > LateralCurve[i] && a < LateralCurve[i+2]) {

			float dist = LateralCurve[i+2] - LateralCurve[i];
			float p = (a-LateralCurve[i]) / dist;

			an = LateralCurve[i+1] + (LateralCurve[i+3] - LateralCurve[i+1]) * p;
			break;
		}
	}


	if(slipAngle == 0)
		an = 0;

	if(minus)
		an=-an;

	
	force = 3000 * an;

	


	//LateralForce = -Sim.getX() /** slipAngle)*/ * force;
	//if(id == 0 || id == 2)
	LateralForce = -Sim.getX() * an * 5000;
	//LateralForce = -Sim.getX() * slipAngle * 1000;
	//Load /= 10;
	LateralForce = (Direction*DotProduct(Speed,Direction)-Speed);//*(Load);
	LateralForce = LateralForce*5000;

	float l = VectorNormalize(&LateralForce);
	if(l > 4000)
		l=4000;
	LateralForce = LateralForce * l;



	if(id == 1) {
		//tMainSR3.f1 = Direction.GetY();
        //tMainSR3.f2 = l;

		tMainSR3.v1 = Direction;
		tMainSR3.v2 = sp;
	}

	

	//laterally
	//LateralForce = (Direction*DotProduct(Speed,Direction)-Speed)*(Load);


	//LateralForce = Sim.getX()*grip;

	

	//if(id == 2 || id == 3)
	//	LateralForce = LateralForce * 0.5f;
	//ForceFeedback = Traction.norm()/

	

	//alpha = Direction.GetY();
	//beta = Speed.GetY();

		
	//Longitudinally:
	//Traction = Direction*Thrust;
	//if (LockUp && Lock)
	//	Traction = -Speed*Load;

	
	//try too much traction and you slip:
	//max traction depends on load and grip
	//but the load is limited to MaxLoad
	//to avoid wierd reactions
	//ex: at jump receptions

	/*Grip = 100;
	MaxLoad = 200;
		
	double MaxTraction = MIN(Load*Grip,MaxLoad*Grip);	

	float length = VectorLength(LateralForce);
	if(length > MaxTraction) {
		VectorNormalize(&LateralForce);
		LateralForce = LateralForce * (float)MaxTraction;
	}*/

	// Get rid of lateral force & traction for now
	//LateralForce = CVec(0,0,0);
	//Traction = CVec(0,0,0);

	Force = Force + Traction;// + LateralForce;	

	return Force;
}


///////////////////
// Update the wheel
void CWheel::Update(CSimBody *_sim, car_command_t *Command, float dt)
{
	CVec before = Sim.getPosition();
	Sim = *_sim;

	// Rotate the wheel
	Sim.Rotate(Sim.getUp() * (Command->Steering * SteerFactor));
	
	Lock = Command->Handbrake;
	Thrust = 0;

	if(Command->Accel)
		Thrust = AccelFactor;
	if(Command->Brake)
		Thrust -= BrakeFactor;
	
	// BAD!! Only accounts for Z
	//vSuspensionPosition = Sim.getPosition()-RelPos;



	// Make the wheel at a restlength below the car
	CVec oldpos = Sim.getPosition();
	Sim.setPosition( Sim.getPosition() - Sim.getUp() * fRestLength );	

	// Not touching
	if(!Contact.Touching) {
		Speed = CVec(0,0,0);
		Direction = CVec(0,0,0);
		vPistonVelocity = CVec(0,0,0);
		Travel = 0;
		fSuspLength = fRestLength;
		return;
	}

	// Make sure the contact normal is within a good angle
	if( fabs(DotProduct(Contact.Norm,Sim.getUp())) < 0.7) {
		return;
	}
	

	/*	
		Calculate some information about the wheel
	*/

	// Calculate the direction of the wheel
	CVec W = CrossProduct( Sim.getUp(), Sim.getDir() );
	Direction = CrossProduct(W, Contact.Norm);
	VectorNormalize(&Direction);

	// Calculate the speed of the wheel
	Speed = CrossProduct( Contact.Norm, CrossProduct(Sim.getVelocity(), Contact.Norm) );





	/*
		--------------------------------

		Make the wheel sit on the ground

		--------------------------------
	*/
	/*CVec newpos = Sim.getPosition();
	CVec v = Contact.Pos + Contact.Norm*1.7f;
	float l = GetDistance(Sim.getPosition(),v);
	newpos = Sim.getPosition()+Sim.getUp()*l;*/

	/*
	   Possible problem:
	   (Assuming on flat ground (0,0,1))
	   Contact.Pos's z value is good, but the x & y values could be outdated (ie, in previous position)
	   AND x & y could be anywhere on the plane but within the sphere
	*/


	CVec newpos = Sim.getPosition();
	CVec cpos = Contact.Pos;
	CVec v = cpos + Contact.Norm*1.7f;
	float l = GetDistance(Sim.getPosition(),v);
	newpos = Sim.getPosition()+Sim.getUp()*l;

	

	/*CVec orig = Sim.getPosition() + Sim.getUp()*fRestLength;
	float d = DotProduct(Contact.Norm,orig) + Contact.PlaneDist;
	float l = d - 1.4f;
	CVec newpos = orig - Sim.getUp()*l;*/


	/*CVec p = Sim.getPosition() + Sim.getUp() * fRestLength;
	float d = DotProduct(Contact.Norm,p) + Contact.PlaneDist;
	//newpos = p - Sim.getUp() * (fRestLength+d-2);
	*/

	


	/*	
		Calculate the suspension compression, and the point at which the car is connected to the suspension
	*/
	vPistonVelocity = Sim.getUp() * DotProduct(Sim.getUp(),Sim.getVelocity());
	fSuspLength = GetDistance(oldpos,newpos);
	//tMainSR3.f2 = vPistonVelocity.GetZ();

	/*float vel = vPistonVelocity.GetZ()*2;

	if(id == 3)
		tMainSR3.f1 = vel;

	// Compression
	if(vel < 0)
		fSuspLength += vel * dt;
	//else
	// Rebound
	//	fSuspLength += 100 * dt;


	// Make the suspension spring back to it's resting position
	if(fSuspLength < fRestLength)
		fSuspLength += (fRestLength-fSuspLength)*20 * dt;



	// Clamp the suspension
	if(fSuspLength < fMinLength)
		fSuspLength = fMinLength;
	
	if(fSuspLength > fMaxLength)
		fSuspLength = fMaxLength;
		
	
	// BAD!! Only accounts for Z
	vSuspensionPosition = newpos-RelPos + Sim.getUp() * fSuspLength;*/
	



	/*CVec topSusp = Sim.getPosition() + Sim.getUp() * fRestLength;
	CVec toGround = Contact.Pos - topSusp;

	CVec v = Sim.getPosition() - Contact.Pos;

	float l = VectorLength(v);
	

	// Set the wheel at the contact position
	CVec newpos = topSusp + (Sim.getUp() * -VectorLength(toGround));
		
	//newpos.SetZ(Contact.Pos.GetZ());	
	newpos = Sim.getPosition()-v;
	//newpos = topSusp;
	//newpos = Contact.Pos;
	
	// ASSUMING STRAIGHT DROP!! BAD!!!
	//fSuspLength = topSusp.GetZ() - newpos.GetZ();		// !!!!! Must change to use all axis

	fSuspLength = GetDistance(topSusp,newpos);
	//fSuspLength = l;

	tMainSR3.f1 = l;

	if(fSuspLength > fRestLength)
		fSuspLength = fRestLength;*/

	/*CVec newpos = Contact.Pos + Sim.getUp()*1.8f;
	newpos = Contact.Pos + CVec(0,0,1.8f);
	fSuspLength = GetDistance( Sim.getPosition(), newpos);
	
	fSuspLength = MIN(fSuspLength, fRestLength);
	tMainSR3.f1 = fSuspLength;*/

	

	// Change so only the velocity in the down/up direction (relative to the wheels) is used
	// Otherwise high-speed driving may confuse the suspension
	

	// Check if the suspension length is smaller then the lower limit (compressed beyond limit)
	if(fSuspLength < fMinLength) {
		fSuspLength = fMinLength;
		vPistonVelocity = CVec(0,0,0);
		return;
	}

	// Check if the suspension length is larger then the maximum limit (rebounded beyond limit)
	if(fSuspLength > fMaxLength) {
		fSuspLength = fMaxLength;
		vPistonVelocity = CVec(0,0,0);
		return;
	}


	// Within the limits
	Sim.setPosition( newpos );
}