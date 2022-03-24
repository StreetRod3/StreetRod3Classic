/////////////////////////////////////////
//
//              Car Library
//        Auxiliary Software 2002
//
/////////////////////////////////////////


// Simulate car class
// Created 18/3/02
// By Jason Boettcher


#include "defs.h"
#include "System.h"
#include "Game.h"
//#include "rigid/vitamina.hpp"


#define ABS(x)       (((x) >= 0) ? (x) : (-(x)))     
#define SGN(x)       (((x) >= 0) ? 1 : -1)



///////////////////
// Setup the car sim
void CCarSim::Setup(float _mass, float _massdist)
{
	Mass = _mass;
	MassDist = _massdist;	
	Weight = Mass*SIM_GRAVITY;
	
	Sim.Clear();
	Sim.Setup(Mass,MassDist);

	Command.Accel = false;
	Command.Brake = false;
	Command.Handbrake = false;
	Command.Steering = 0;

	AirDrag = 2;

	double grav[] = {0,0,-SIM_GRAVITY};
//	double toi[3];

	float Dx = 15;
	float Dy = 7;
	float Dz = 10;

	Position_wc = CVec(0,0,2);

//	toi[0]=((real)1.0)/((real)12.0)*_mass*(Dy*Dy+Dz*Dz);
	//toi[1]=((real)1.0)/((real)12.0)*_mass*(Dx*Dx+Dz*Dz);
	//toi[2]=((real)1.0)/((real)12.0)*_mass*(Dx*Dx+Dy*Dy);

	//CuboidMomentsOfInertia(toi,1500, 15, 6, 10);
	//cRBody.Build(grav, toi, _mass);

	//Sim.Rotate(CVec(0,0,-1));

	
}

extern CCar cRaceCar;
void Race_CheckCollisions(CCar *car);

///////////////////
// Simulate the car
void CCarSim::Simulate(float dt)
{
	CVec Grav = CVec(0,0,-SIM_GRAVITY);	


	//if(tContact.Touching)
	//	HandleCollision();

	
	int n;
	for(n=0;n<1;n++) {		
		Race_CheckCollisions(&cRaceCar);
		if(!tContact.Touching)
			break;
		HandleCollision();
	}


	// Add the wheel forces to the car
	CVec Force;
	for(int w=0;w<4;w++) {
		Force = Wheels[w].CalculateForce(Sim.getVelocity(), dt);
		//Sim.AddForce(Wheels[w].getSimBody()->getPosition(),Force);
		//Sim.AddLatForce(Wheels[w].getSimBody()->getPosition(),Wheels[w].getLateralForce());
	}

	//if(Command.Accel /*&& Wheels[3].getContact()->Touching*/)
	//	Sim.AddForce( Sim.getPosition(), Sim.getDir()*100000);
	//if(Command.Brake /*&& Wheels[3].getContact()->Touching*/)
	//	Sim.AddForce( Sim.getPosition(), -Sim.getDir()*100000);

	//if(Command.Steering > 0)
//		Sim.Rotate(CVec(0,0,1)*tMainSR3.fDeltaTime);


	// Gravity
	//Sim.AddForce(Sim.getPosition(),Grav * Sim.getMass());

	DoPhysics();

	// Air drag
	//CVec vel = Sim.getVelocity();
	//Sim.AddForce(Sim.getPosition(), vel * (-AirDrag * VectorLength(vel)));

	// Simulate the simbody
	//Sim.Simulate(dt);

	// Update the wheels
	//for(w=0;w<4;w++)
	//	Wheels[w].Update(&Sim.GetSimBody(Wheels[w].getRelPos()), &Command, dt);

	// Rest the car on the wheels
	/*CVec v = Sim.getPosition();
	Sim.setPosition( CVec(v.GetX(),v.GetY(), Wheels[3].getSuspensionPos().GetZ()) );
	if(Wheels[3].getContact()->Touching)
		Sim.setVelocity(CVec(0,0,0));*/


	
	// Calculate the car position based on the tires
	/*if( Wheels[3].getContact()->Touching ) {
		CVec v = Sim.getPosition();
		Sim.setPosition( CVec(v.GetX(),v.GetY(), Wheels[3].getSimBody()->getPosition().GetZ()+2) );
	}*/

	


	// Hack
	/*CVec v = Sim.getPosition();
	v.SetZ(2.5);
	Sim.setPosition( v );*/
}


///////////////////
// Handle the collision
void CCarSim::HandleCollision(void)
{
	float j;
	float coef = 0.3f;
	CVec v = Sim.getVelocity();	

	// Check to make sure the plane we're rebounding off is facing opposite the velocity
	if(DotProduct(v,tContact.Norm) > 0)
		return;	

	// Check to make sure the car position is the good side of the plane
	if(DotProduct(Sim.getPosition(),tContact.Norm) + tContact.PlaneDist < 0)
		return;



	j = DotProduct(v * -(1+coef),tContact.Norm) / DotProduct(tContact.Norm,tContact.Norm * (1/Mass));
	v = v + tContact.Norm * (j*(1/Mass));

	//Sim.setVelocity(v);

	CVec a = Sim.getAngVelocity();
	//CrossProduct(pos-Sim.getPosition(),j);
	



	/*
		Bounding box method
	*/

	// Find the corner which is intersecting the most
	/*CVec corner;
	float f;

	CVec dir = tContact.Pos - Sim.getPosition();
	VectorNormalize(&dir);
	//dir = tContact.Norm;
	//tContact.Norm = dir;

	// Up/Down
	f = DotProduct(dir, Sim.getUp());
	if(f < 0)
		corner = corner + Sim.getUp()*vDimension.GetZ();
	else
		corner = corner - Sim.getUp()*vDimension.GetZ();

	// Forwards/Backwards
	f = DotProduct(dir, Sim.getDir());
	if(f < 0)
		corner = corner + Sim.getDir()*vDimension.GetY();
	else
		corner = corner - Sim.getDir()*vDimension.GetY();

	// Right/Left
	f = DotProduct(dir, Sim.getRight());
	if(f < 0)
		corner = corner + Sim.getRight()*vDimension.GetX();
	else
		corner = corner - Sim.getRight()*vDimension.GetX();


	corner = corner + Sim.getPosition();

	//	Theoretically, corner should now be the corner that is closest to the plane


	// Find the distance from the plane to the corner
	f = DotProduct(corner,tContact.Norm) + tContact.PlaneDist;
	CVec pos = Sim.getPosition() - tContact.Norm*f;

	tMainSR3.f2 = tContact.Norm.GetZ();
	Sim.setPosition(pos);
	
	// Bounce off the wall
	CVec v = Sim.getVelocity();
	float l = VectorNormalize(&v);
	
	//2N(N*V) - V 
	CVec dr = ((tContact.Norm * 2 * DotProduct(v,tContact.Norm)) - v) * -1;
	VectorNormalize(&dr);

	l *= 0.5f;
	Sim.setVelocity( dr * l);*/
}


//extern float alpha,beta;
///////////////////
// Anti-roll bars
void CCarSim::AntiRollBars(void)
{
	CVec t = Sim.getAngVelocity();

	float max = 0.30f;

}


///////////////////
// Hack
void CCarSim::Hack(void)
{
	
}


#define EPSILON	1.0		/* very low speed */
#define EPS_AV	0.1		/* very low ang. velocity */


/*
Old Values
2.0
20.0
-20.20
-13.5
2.0
*/

#define	DRAG		2.0		 		/* factor for air resistance (drag) 	*/
#define	RESISTANCE	30.0			/* factor for rolling resistance */
#define CA_R		-20.20			/* cornering stiffness */
#define CA_F		-17.5			/* cornering stiffness */
#define MAX_GRIP	4.0			/* maximum (normalised) friction force, =diameter of friction circle */


///////////////////
// Do the physics
void CCarSim::DoPhysics(void)
{
	float	wheelbase;		// wheelbase in m
	float	b;				// in m, distance from CG to front axle
	float	c;				// in m, idem to rear axle
	float	h;				// in m, height of CM from ground
	float	mass;			// in kg
	float	inertia;		// in kg.m
	float	length,width;
	float	wheellength,wheelwidth;

	b = 0.50;					// m							
	c = 0.50;					// m
	wheelbase = b + c;
	h = 1.0;					// m
	mass = 1500;				// kg			
	inertia = 1500;				// kg.m			
	width = 1.5;				// m
	length = 3.0;				// m, must be > wheelbase
	wheellength = 0.7f;
	wheelwidth = 0.3f;

	float Steering = -Command.Steering * 1.1f;

	float DEG = (180/PI);

	
	double sn = sin(Angles.GetZ());
	double cs = cos(Angles.GetZ());

	
	// SAE convention: x is to the front of the car, y is to the right, z is down
	
	float velocity_x, velocity_y;

	// transform velocity in world reference frame to velocity in car reference frame
	velocity_x =  cs * Velocity_wc.GetY() + sn * Velocity_wc.GetX();
	velocity_y = -sn * Velocity_wc.GetY() + cs * Velocity_wc.GetX();
	
// Lateral force on wheels
//	
	// Resulting velocity of the wheels as result of the yaw rate of the car body
	// v = yawrate * r where r is distance of wheel to CG (approx. half wheel base)
	// yawrate (ang.velocity) must be in rad/s
	//
	float yawspeed = wheelbase * 0.5 * AngularVelocity.GetZ();
	float rot_angle = 0;
	float sideslip = 0;
	float slipanglefront = 0;
	float slipanglerear = 0;
	float weight = 0;
	float flatf_x, flatf_y;
	float flatr_x, flatr_y;
	bool front_slip = false;
	bool rear_slip = false;
	float ftraction_x, ftraction_y;
	float rtraction_x, rtraction_y;
	float resistance_x, resistance_y;
	float force_x, force_y;


	//if(System_GetKeyboard()->KeyDown[SDLK_v]) {
		//rear_slip = true;
		front_slip = true;
	//}


	if( fabs(velocity_x) < EPSILON )		// TODO: fix singularity
		rot_angle = 0;
	else
		rot_angle = atan2( yawspeed, velocity_x);

	// Calculate the side slip angle of the car (a.k.a. beta)
	if( fabs(velocity_x) < EPSILON )		// TODO: fix singularity
		sideslip = 0;
	else
		sideslip = atan2( velocity_y, velocity_x);		

	// Calculate slip angles for front and rear wheels (a.k.a. alpha)
	slipanglefront = sideslip + rot_angle - Steering * (PI/180);
	slipanglerear  = sideslip - rot_angle;

	// weight per axle = half car mass times 1G (=9.8m/s^2) 
	weight = mass * 9.8 * 0.5;	



	CVec dir;
	GetAngles(0,0,-Angles.GetZ() * (180/PI), &dir,NULL,NULL);

	CVec v = Velocity_wc;
	VectorNormalize(&v);
	float slippage = DotProduct(v, dir);

	//tMainSR3.f1 = Command.Accel - Command.Brake;	

	slippage = MIN(1,slippage);
	slippage = MAX(0.7,slippage);
	slippage = 1-slippage;

	slippage = 0.9f;


	
	// lateral force on front wheels = (Ca * slip angle) capped to friction circle * load
	flatf_x = 0;
	flatf_y = CA_F * slipanglefront;
	flatf_y = MIN(MAX_GRIP, flatf_y);
	flatf_y = MAX(-MAX_GRIP, flatf_y);
	flatf_y *= weight;
	if(front_slip)
		flatf_y *= slippage;

	// lateral force on rear wheels
	flatr_x = 0;
	flatr_y = CA_R * slipanglerear;
	flatr_y = MIN(MAX_GRIP, flatr_y);
	flatr_y = MAX(-MAX_GRIP, flatr_y);
	flatr_y *= weight;
	if(rear_slip)
		flatr_y *= slippage;

	// longtitudinal force on rear wheels - very simple traction model
	ftraction_x = 10000*(Command.Accel - Command.Brake*SGN(velocity_x));	
	ftraction_y = 0;
	if(rear_slip)
		ftraction_x *= slippage;

	// Prevent spinning on the spot
	if( fabs(velocity_x) < EPSILON) {
		flatf_y = 0;
		flatr_y = 0;
	}


// Forces and torque on body
	
	// drag and rolling resistance
	resistance_x = -( RESISTANCE*velocity_x + DRAG*velocity_x*ABS(velocity_x) );
	resistance_y = -( RESISTANCE*velocity_y + DRAG*velocity_y*ABS(velocity_y) );

	// sum forces	
	force_x = ftraction_x + sin(Steering) * flatf_x + flatr_x + resistance_x;
	force_y = ftraction_y + cos(Steering) * flatf_y + flatr_y + resistance_y;	

	// torque on body from lateral forces
	float torque = b * flatf_y - c * flatr_y;

	
	float delta_t = tMainSR3.fDeltaTime;

// Acceleration
	
	// Newton F = m.a, therefore a = F/m
	float acceleration_x = force_x/mass;
	float acceleration_y = force_y/mass;
	
	float angular_acceleration = torque / inertia;

// Velocity and position
	
	// transform acceleration from car reference frame to world reference frame
	//ma.LoadIdentity();
	//ma.CreateFromAngle(-Angles.GetX() * DEG, -Angles.GetY() * DEG, -Angles.GetZ() * DEG);

	float acceleration_wc_x =  cs * acceleration_y + sn * acceleration_x;
	float acceleration_wc_y = -sn * acceleration_y + cs * acceleration_x;

	// velocity is integrated acceleration
	//	
	Velocity_wc.SetX( Velocity_wc.GetX() + delta_t * acceleration_wc_x );
	Velocity_wc.SetY( Velocity_wc.GetY() + delta_t * acceleration_wc_y );
	
	//car->velocity_wc.x += delta_t * acceleration_wc.x;
	//car->velocity_wc.y += delta_t * acceleration_wc.y;

	// position is integrated velocity
	//
	Position_wc.SetX( Position_wc.GetX() + delta_t * Velocity_wc.GetX() );
	Position_wc.SetY( Position_wc.GetY() + delta_t * Velocity_wc.GetY() );	
	//car->position_wc.x += delta_t * car->velocity_wc.x;
	//car->position_wc.y += delta_t * car->velocity_wc.y;


// Angular velocity and heading

	// integrate angular acceleration to get angular velocity
	//
	AngularVelocity.SetZ( AngularVelocity.GetZ() + delta_t * angular_acceleration );
	//car->angularvelocity += delta_t * angular_acceleration;

	// integrate angular velocity to get angular orientation
	//
	
	Angles.SetZ( Angles.GetZ() + delta_t * AngularVelocity.GetZ() );
	//car->angle += delta_t * car->angularvelocity ;
	


	// Put the wheels in the right position
	

	for(int i=0; i<4; i++) {
		//Wheels[i].getSimBody()->setPosition( m.MultiplyVector(Wheels[i].getRelPos()) + Position_wc );
		
		UpdateWheel(i);
		//sWheels[i].cOrigPos = m.MultiplyVector(Wheels[i].getRelPos()) + Position_wc );

	}

}


///////////////////
// Update the wheel position
void CCarSim::UpdateWheel(int id)
{
	CMatrix m;
	m.CreateFromAngle(-Angles.GetX() * (180/PI), -Angles.GetY() * (180/PI), -Angles.GetZ() * (180/PI));

	wheel_t *w = &sWheels[id];
	w->cOrigPos = Wheels[id].getRelPos();

	// Not touching
	if(!Wheels[id].getContact()->Touching) {
		w->cPos = CVec(0,0,-2);
		w->cPos = m.MultiplyVector(w->cOrigPos+w->cPos) + Position_wc;

		Wheels[id].getSimBody()->setPosition( w->cPos );
		return;
	}


	// Touching
	CVec o = w->cOrigPos;	
	o.SetZ( Wheels[id].getContact()->Pos.GetZ() );
	w->cPos = m.MultiplyVector(o) + Position_wc;

	Wheels[id].getSimBody()->setPosition( w->cPos );		
}


///////////////////
// Get the full stretched suspension wheel position for collision detection
CVec CCarSim::GetWheelCol(int id)
{
	CMatrix m;
	m.CreateFromAngle(0,0, -Angles.GetZ() * (180/PI));

	wheel_t *w = &sWheels[id];
	w->cOrigPos = Wheels[id].getRelPos();

	w->cPos = CVec(0,0,-3);

	return m.MultiplyVector(w->cOrigPos+w->cPos) + Position_wc;
}