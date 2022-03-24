/////////////////////////////////////////
//
//             Street Rod 3
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Car simulation
// Created 23/02/03
// Jason Boettcher


#include "defs.h"
#include "System.h"
#include "game.h"
#include "CarSimulation.h"
#include "Entity.h"


// Temp until we get the transmission into the game
float	gearRatios[5] = {2, 2.55f, 1.5f, 1.15f, 1.0f};


///////////////////
// Setup a car
void Car_SetupCar(carsim_t *psCar, CCar *pcCar)
{
	int			i;
	gobject_t	*g;
	char		*sTyres[] = {"$frwheel","$brwheel","$blwheel","$flwheel"};

	// Identity orientation
	psCar->X = CVec(1,0,0);
	psCar->Y = CVec(0,1,0);
	psCar->Z = CVec(0,0,1);

	psCar->cPos = CVec(0,0,2);
    psCar->cVelocity = CVec(0,0,0);

    // Camera
    psCar->nCameraType = CAM_INCAR;
    psCar->cInCarCamera = pcCar->getInCarCamera();

	// Engine
    psCar->fAcceleration = 0;
	psCar->nGear = 0;       // Neutral
	psCar->nRPM = 1000;
    psCar->dTime = (double)SDL_GetTicks() * 0.001;

	// Collision
	psCar->bCollision = false;
	psCar->bFlipping = false;
	psCar->fSteerAngle = 0;

    // Stats
    psCar->fMaxSpeed = 0;
    psCar->fRaceTime = 0;
        

	// Wheels
    float c = 0;
	for(i=0; i<4; i++) {
		psCar->sWheels[i].bCollision = false;
		psCar->sWheels[i].fSpin = 0;
		psCar->sWheels[i].fSpeed = 0;
		psCar->sWheels[i].fPistonVelocity = 0;
		psCar->sWheels[i].fWheelZ = 0;
        psCar->sWheels[i].fTorque = 0;
        psCar->sWheels[i].fEngineLoad = 0;
        psCar->sWheels[i].bSlip = false;

        // Defaults
        psCar->sWheels[i].fSpring = 400;
        psCar->sWheels[i].fDampening = 15;
        psCar->sWheels[i].fCornering = 1;

        if( pcCar->getShock(i) ) {
            psCar->sWheels[i].fSpring = pcCar->getShock(i)->getSpring();
            psCar->sWheels[i].fDampening = pcCar->getShock(i)->getDampening();
            psCar->sWheels[i].fRestLength = pcCar->getShock(i)->getRestLength();
        }

        if( pcCar->getTire(i) ) {
            psCar->sWheels[i].fCornering = pcCar->getTire(i)->getCornering();
            psCar->sWheels[i].fRadius = pcCar->getTire(i)->getDimensions().GetZ()/2;
        }

        c += psCar->sWheels[i].fCornering;
	}

    // Calculate the average cornering value for the 4 wheels
    psCar->fCornering = c / 4;

	// Setup the wheels from the parts
	/*CModel *pcModel = pcCar->getModel();
	g = pcModel->findGameObject(sTyres[0]);
	CVec FR = g->vPosition + CVec(0,0,2);
	

	// Only use 1 tyre, and base the other positions from that
	// This maintains that the tyre positions are equal distance from the center of gravity
	psCar->sWheels[0].cRelPos = FR;											// Front Right
	psCar->sWheels[1].cRelPos = CVec(FR.GetX(), -FR.GetY(), FR.GetZ());		// Back Right
	psCar->sWheels[2].cRelPos = CVec(-FR.GetX(), -FR.GetY(), FR.GetZ());	// Back Left
	psCar->sWheels[3].cRelPos = CVec(-FR.GetX(), FR.GetY(), FR.GetZ());		// Front Left*/

    for(i=0; i<4; i++) {
        g = pcCar->getModel()->findGameObject(sTyres[i]);
        psCar->sWheels[i].cRelPos = g->vPosition + CVec(0,0,2);
    }
}


///////////////////
// Process the car
void Car_ProcessCar(carsim_t *psCar, CModel *pcTrack, CCar *pcCar, CCamera *pcCam, double dt, bool bPaused)
{
	double fixed = 0.025f;
	double t=0;

    double time = (double)SDL_GetTicks() * 0.001;

    // Fixed time-step physics simulation
    while(psCar->dTime < time) {

        dt = time - psCar->dTime;
        if(dt > fixed)
            dt = fixed;

        // Don't simulate cars when paused (only the camera)
        if(!bPaused)
		    Car_SimulateCar(psCar, pcTrack, dt);

		Car_SetupCamera(psCar, pcCam, dt);
        
        psCar->dTime += dt;
    }
    psCar->dTime = time;

   
    if(!bPaused)
        Car_CheckCollisions(psCar, pcCar, pcTrack);

    // Do some sparks
    if( psCar->bCollision ) {
        for(int i=0;i<5;i++)
            SpawnEntity(ent_spark, 0, psCar->cColPoint, CVec(GetRandomNum()*15,GetRandomNum()*15,GetRandomNum()*15), 2);
    }

}


///////////////////
// Simulate a car
void Car_SimulateCar(carsim_t *psCar, CModel *pcTrack, float dt)
{
	float	angSpeed = 50;	// 110
	float	rotInertia = 45; // 45
	float	gravity = 100;	// 200
	float	AirDrag = 0.001f;
	float	TyreFriction = 350;	// 100
	float	brakeForce = 70;
	float	handbrakeForce = 70;

	CVec	force, torque;
	CVec	dir, v;
	int		i;

	// Wheels on the ground
	bool	aWheel = false;
	bool	rearWheels = false;


	for(i=0; i<4; i++) {
		if(psCar->sWheels[i].bCollision)
			aWheel = true;
		if( (i==1 || i==2) && psCar->sWheels[i].bCollision)
			rearWheels = true;
	}

	// Gravity
	force += CVec(0,0,-gravity);

	// Ground force
	if(psCar->bCollision)
		force += psCar->cColNormal*gravity;

	// Dampening
	torque -= psCar->cAngVelocity*50;

	// Air drag	
	force += psCar->cVelocity * (-AirDrag * VectorLength(psCar->cVelocity));

	// Get the force from the wheels
	for(i=0; i<4; i++) {
		CVec f = Car_GetWheelForce(psCar, i);

		// Force
		force += f;

		// Torque
		torque -= CrossProduct(psCar->cPos-psCar->sWheels[i].cPos, f);
	}


	// Engine
	//if(psCar->bAccelerating)
	//	force += psCar->Y*100;

    float	diffRatio = 3.6f;
	float	gearEfficiency = 0.7f;	

	float	wheelAV = ((psCar->sWheels[1].fSpeed) / psCar->sWheels[1].fRadius);
    float   gearRatio = 0;

    if(psCar->nGear >= 0)
        gearRatio = gearRatios[psCar->nGear];
    else
        // Reverse
        gearRatio = gearRatios[1];

    if(rearWheels && psCar->nGear > 0) {
        //if(psCar->sWheels[1].bSlip)
            //psCar->nRPM -= psCar->sWheels[1].fEngineLoad;
        //else
		    psCar->nRPM = wheelAV  * 60 * gearRatio * diffRatio / (2.0*PI);
    }
	psCar->nRPM = MAX(psCar->nRPM,1000);

    
	// Acceleration
	float EngineForce = 0;
	if( !psCar->bHandBrake ) {

		float t = 0;

		float mult = 20;
        //mult = 40;

		// Lookup the torque
		if(psCar->nRPM < 5000)
			t = (float)psCar->nRPM/5000 * mult;
		else if(psCar->nRPM < 6000)
			t = mult*(1000 - ((float)psCar->nRPM - 5000))*0.001;
		else
			t = 0;

		EngineForce = t * gearRatio * diffRatio * gearEfficiency * psCar->fAcceleration;

        // Reverse
        if(psCar->nGear == -1)
            EngineForce = -EngineForce;

        // If we're in neutral, the rpm is directly from the Engine Force
        if( psCar->nGear == 0 ) {
            psCar->nRPM += EngineForce;
            EngineForce = 0;
        } else {

            // Apply torque to the rear wheels
            psCar->sWheels[1].fTorque = EngineForce;
            psCar->sWheels[2].fTorque = EngineForce;
        }
	
		//EngineForce = 50;

		//EngineForce /= 0.05f;
		//EngineForce -= car->nRPM / 100;

		//EngineForce = 90;
	}

    // If we're in neutral & not accelerating lower the rpm
    if( psCar->nGear == 0 && psCar->fAcceleration == 0 ) {
        psCar->nRPM -= 2000*dt;
    }

    // Back hack
    if(psCar->nGear == -1)
        psCar->nRPM = 1000;

    // Apply force to the rear wheels
    if(rearWheels) {
        force += psCar->Y*EngineForce;
        float e = MIN(EngineForce,30);
        e = MAX(e,-30);
        torque += CrossProduct(psCar->Y*10, CVec(0,0,e));
    }


	// Braking
	v = psCar->cVelocity;
	float spd = VectorNormalize(&v);
    if(psCar->bBraking && rearWheels) {
		force -= v*75;
        torque -= CrossProduct(psCar->Y*10, CVec(0,0,MIN(spd,30)));

    }

	// Steering
    
    // Reverse the angle if the car is going backwards
	float ang = psCar->fSteerAngle;
	if( DotProduct(psCar->cVelocity,psCar->Y) < 0)
		ang = -psCar->fSteerAngle;

    
    float steering = 60;
    float vl = VectorLength(psCar->cVelocity);
    if( vl < 60 )
        steering = vl*2;

    steering *= psCar->fCornering;

	float d = steering*ang;

	psCar->Y = RotateAround(psCar->Y,CVec(0,0,d * (PI/180))*dt);
	psCar->Z = RotateAround(psCar->Z,CVec(0,0,d * (PI/180))*dt);
	Car_Straighten(psCar);


	// Tyre physics
	CVec oneUp = CVec(1-psCar->Z.GetX(), 1-psCar->Z.GetY(), 1-psCar->Z.GetZ());
	v = psCar->cVelocity*oneUp;
	float length = VectorNormalize(&v);
	float side = DotProduct(v, psCar->X);

	// Traction multiplyer
	// (Based on lateral slip)
	float traction = (1-side);
	traction = MAX(0.75f,traction);

	//if(rearWheels) {
	//	force -= psCar->Y * (traction*EngineForce);
	//}

	// Friction on the tyres
	float friction = side * TyreFriction;
	if(length > 1 && aWheel) {
		CVec t = psCar->X * friction;
		force -= t;

        // Anti-roll bar to limit the torque on the side friction
        float s = MIN(side,20);
		//if(fabs(DotProduct(psCar->cAngVelocity, psCar->X)) < 1)
			//torque -= CrossProduct(psCar->X*(s*30), CVec(0,0,90));
	}


	// Flip the car back onto its tyres
	if(System_GetKeyboard()->KeyDown[SDLK_f] && psCar->bCollision) {
		psCar->bFlipping = true;
		psCar->cFlipNormal = psCar->cColNormal;
	}


	// Flipping the car
	if(psCar->bFlipping) {
		torque += CrossProduct(psCar->X*5, psCar->Z*50);

		// If the car UP vector is within a threshold of the car's ground normal, stop flipping
		if(DotProduct(psCar->Z, psCar->cFlipNormal) > 0.5f)
			psCar->bFlipping = false;
	}


	// Linear
	psCar->cVelocity += force*dt;
	psCar->cPos += psCar->cVelocity*dt;

	// Angular
	psCar->cAngVelocity = psCar->cAngVelocity + (torque/rotInertia) * dt;
	psCar->Y = RotateAround(psCar->Y,psCar->cAngVelocity*dt);
	psCar->Z = RotateAround(psCar->Z,psCar->cAngVelocity*dt);
	Car_Straighten(psCar);


	// Update the wheels
	for(i=0; i<4; i++)
		Car_UpdateWheel(psCar, pcTrack, i);
}


///////////////////
// Update the wheel position
void Car_UpdateWheel(carsim_t *psCar, CModel *pcTrack, int id)
{
	wheel_t *w = &psCar->sWheels[id];

	CVec	relpos = w->cRelPos;
	float	p[3], t1[9], t2[9];
	plane_t	plane;

	// Calculate the wheel 'top' pos
	CVec wheelPos = psCar->X*relpos.GetX() + psCar->Y*relpos.GetY() + psCar->Z*relpos.GetZ() + psCar->cPos;

	// Convert the wheel in car coords
	relpos -= CVec(0,0,w->fRestLength);	
	w->cPos = psCar->X*relpos.GetX() + psCar->Y*relpos.GetY() + psCar->Z*relpos.GetZ() + psCar->cPos;	
	
	// This is for the rendering
	w->fWheelZ = -w->fRestLength;	

	// Defaults
	w->fSuspLength = w->fRestLength;
	w->fPistonVelocity = 0;


	// Check for collisions
	p[0] = w->cPos.GetX();	p[1] = w->cPos.GetY();	p[2] = w->cPos.GetZ();

	w->bCollision = false;
	float	pDist = 0;

	if( pcTrack->getCDModel()->sphereCollision( p, w->fRadius ) ) {

		float top=999999;
		for(int n=0; n<pcTrack->getCDModel()->getNumCollisions(); n++) {

			pcTrack->getCDModel()->getCollidingTriangles(n, t1,t2,false);		

			plane = CalculatePlane(CVec(t1[0],t1[1],t1[2]),
										   CVec(t1[3],t1[4],t1[5]),
										   CVec(t1[6],t1[7],t1[8]));			

			// If the normal is too great for the tyre, ignore the collision
			if(DotProduct(plane.vNormal,psCar->Z) < 0.5f)
				continue;

			// Find the plane that is raised towards the wheel center the most
			if( DotProduct(wheelPos, plane.vNormal) + plane.fDistance < top) {

				// TODO: Check if the tyre is not too high (ie, over the suspension joint)
				top = DotProduct(wheelPos, plane.vNormal) + plane.fDistance;
				w->cNormal = plane.vNormal;
				pDist = plane.fDistance;
			}
						
			w->bCollision = true;
		}
	}

	w->fSpeed = 0;

	// If not colliding, just leave
	if(!w->bCollision)
		return;


	// Make the tyre sit on top of the road
	float d = DotProduct(w->cPos, w->cNormal) + pDist;
	w->fSuspLength = d;

	relpos = w->cRelPos - CVec(0,0,w->fSuspLength);
	w->fWheelZ = -w->fSuspLength;
	w->cPos = psCar->X*relpos.GetX() + psCar->Y*relpos.GetY() + psCar->Z*relpos.GetZ() + psCar->cPos;

	// Get the piston velocity for the suspension
	CVec vel = CrossProduct(psCar->cPos-w->cPos, psCar->cAngVelocity) + psCar->cVelocity;

	w->fPistonVelocity = (psCar->Z * DotProduct(psCar->Z,vel)).GetZ();


	// Calculate the rotation speed of the tyre
	CVec s = psCar->Y * DotProduct(psCar->cVelocity, psCar->Y);
	w->fSpeed = VectorLength(s);    

	if(DotProduct(psCar->cVelocity, psCar->Y) > 0)
		w->fSpin += w->fSpeed / w->fRadius;
	else
		w->fSpin -= w->fSpeed / w->fRadius;


    // Rotation is based on the Torque from the engine
    //w->fSpin += w->fTorque / w->fRadius;

    // If the torque is greater then the slip torque for the surface, the wheel is slipping and the engine force
    // is less
    // Note: The slip torque is constant for now
    if(id == 1) {
        //tMainSR3.f1 = w->fTorque;
    }

    if(w->fTorque > 100) {
        // Slipping
        w->fEngineLoad = w->fTorque/50;
        w->bSlip = true;
    } else {
        w->bSlip = false;
        w->fEngineLoad = 0;
    }


	w->fSpin = LimitAngle(w->fSpin);
}

///////////////////
// Get the force from the tyre
CVec Car_GetWheelForce(carsim_t *psCar, int i)
{
	CVec		force;
	wheel_t		*w = &psCar->sWheels[i];

	float		k = w->fSpring;
	float		Dampening = w->fDampening;

	if(!w->bCollision)
		return CVec(0,0,0);

    

	float f = k * (w->fRestLength - w->fSuspLength);
	f -= w->fPistonVelocity * Dampening;
	//f += 200;		// Ground force (-gravity)

	f = MAX(0,f);	// No negative forces

	force = (w->cNormal * f) / 4;

	return force;
}



///////////////////
// Check collisions between the car & track
void Car_CheckCollisions(carsim_t *psCar, CCar *pcCar, CModel *pcTrack)
{
	int		i;	
	float	m[16], t1[9], t2[9];
    float   p[3];

	// Clear it
	for(i=0;i<16;i++)
		m[i] = 0;

	for(i=0;i<3;i++)
		m[i] = psCar->X.GetIndex(i);
	for(i=4;i<7;i++)
		m[i] = psCar->Y.GetIndex(i-4);
	for(i=8;i<11;i++)
		m[i] = psCar->Z.GetIndex(i-8);
	for(i=12;i<15;i++)
		m[i] = psCar->cPos.GetIndex(i-12);
	m[15] = 1;

	// Raise the car up a bit
	m[14] += 2;


	psCar->bCollision = false;

	CollisionModel3D *col = pcCar->getColMod();//pcCar->getModel()->GetCDModel();

	col->setTransform(m);

	if(col->collision( pcTrack->getCDModel(),-1,500 )) {

		// Get collision point
		//carcol->getCollisionPoint(v1);
		//for(i=0;i<3;i++) cont->Pos.SetIndex(i,v1[i]);

		//cont->Pos = cont->Pos + offset;

		CVec oldVel = psCar->cVelocity;
		CVec oldAng = psCar->cAngVelocity;

		for(int n=0; n<col->getNumCollisions(); n++) {

			// Get collision normal
			col->getCollidingTriangles(n,t1,t2,false);
            col->getCollisionPoint(n,p,false);

			plane_t plane = CalculatePlane(CVec(t2[0],t2[1],t2[2]),
										   CVec(t2[3],t2[4],t2[5]),
										   CVec(t2[6],t2[7],t2[8]));            

			// Resolve the collision

			// Check to make sure the plane we're rebounding off is facing opposite the velocity
			if(DotProduct(psCar->cVelocity, plane.vNormal) > 0)
				continue;

			// Check to make sure the car position is the good side of the plane
			if(DotProduct(psCar->cPos,plane.vNormal) + plane.fDistance < 0)
				continue;
		
			float Mass = 2000;
			float coef = 0.2f;

			// If the velocity against the mesh is too low, make sure we fully bounce back so we don't slowly
			// go into the mesh
			if( DotProduct(psCar->cVelocity, plane.vNormal) > -1)
				coef = 1.0f;

			psCar->bCollision = true;
			psCar->cColNormal = plane.vNormal;            
            psCar->cColPoint = CVec(p[0], p[1], p[2]);

			// Linear velocity rebound
			float j = DotProduct(psCar->cVelocity * -(1+coef),plane.vNormal) / DotProduct(plane.vNormal,plane.vNormal * (1/Mass));
			psCar->cVelocity += plane.vNormal * (j*(1/Mass));



			// Angular velocity rebound
			float pnt[3];
			col->getCollisionPoint(n, pnt);
			//CVec p = CVec(pnt[0],pnt[1],pnt[2]);
			CVec p = psCar->X*pnt[0] + psCar->Y*pnt[1] + psCar->Z*pnt[2];
			CVec v = CrossProduct(oldAng, p) + oldVel;

			if(DotProduct(plane.vNormal,v) > EPSILON)
				continue;

			if(DotProduct(p+psCar->cPos,plane.vNormal) + plane.fDistance > EPSILON)
				continue;

			v = CrossProduct(p, plane.vNormal*-DotProduct(v,plane.vNormal) * 0.01f);
			
			//psCar->cAngVelocity += v;
		}

		//rbody.ResolveCollisions();

		//return true;
	}
}


///////////////////
// Straighten up the car matrix
void Car_Straighten(carsim_t *psCar)
{
	psCar->X = CrossProduct(psCar->Y,psCar->Z);
	psCar->Z = CrossProduct(psCar->X,psCar->Y);

	VectorNormalize(&psCar->X);
	VectorNormalize(&psCar->Y);
	VectorNormalize(&psCar->Z);
}


///////////////////
// Check Resolve a collision between 2 cars
void Car_CheckCarCollisions(CCar *pcCar1, CCar *pcCar2)
{
    float m1[16], m2[16];
    carsim_t *psCar1 = pcCar1->getCarSim();
    carsim_t *psCar2 = pcCar2->getCarSim();


    // First, do a quick check to make sure the cars are within a small 
    // distance of each other using a seperating axis check
    if( fabs(psCar1->cPos.GetX() - psCar2->cPos.GetX()) > 50 )
        return;
    if( fabs(psCar1->cPos.GetY() - psCar2->cPos.GetY()) > 50 )
        return;
    if( fabs(psCar1->cPos.GetZ() - psCar2->cPos.GetZ()) > 50 )
        return;

    // Clear it
    for(int i=0;i<16;i++) {
		m1[i] = 0;
        m2[i] = 0;
    }
    for(i=0;i<3;i++) {
		m1[i] = psCar1->X.GetIndex(i);
        m2[i] = psCar2->X.GetIndex(i);
    }
    for(i=4;i<7;i++) {
		m1[i] = psCar1->Y.GetIndex(i-4);
        m2[i] = psCar2->Y.GetIndex(i-4);
    }
    for(i=8;i<11;i++) {
		m1[i] = psCar1->Z.GetIndex(i-8);
        m2[i] = psCar2->Z.GetIndex(i-8);
    }
    for(i=12;i<15;i++) {
		m1[i] = psCar1->cPos.GetIndex(i-12);
        m2[i] = psCar2->cPos.GetIndex(i-12);
    }
	m1[15] = 1;
    m2[15] = 1;
    // Rase it a bit
	m1[14] += 2;
    m2[14] += 2;

    CollisionModel3D *col = pcCar1->getColMod();
    CollisionModel3D *col2 = pcCar2->getColMod();


   	col->setTransform( m1 );
    //col->setTransform( m2 );

	CVec relVel = psCar1->cVelocity - psCar2->cVelocity;
	CVec posNorm = psCar1->cPos - psCar2->cPos;
	VectorNormalize(&posNorm);


    // Check for collision
	if(col->collision( col2, -1, 0, m2 )) {

        float t1[9], t2[9];
        plane_t plane1, plane2;

        for(int n=0; n<col->getNumCollisions(); n++) {

            // Get collision normal
		    col->getCollidingTriangles(n,t1,t2,false);

		    plane2 = CalculatePlane(CVec(t1[0],t1[1],t1[2]),
			    						   CVec(t1[3],t1[4],t1[5]),
				    					   CVec(t1[6],t1[7],t1[8]));
            plane1 = CalculatePlane(CVec(t2[0],t2[1],t2[2]),
			    						   CVec(t2[3],t2[4],t2[5]),
				    					   CVec(t2[6],t2[7],t2[8]));

            //plane1.vNormal.SetZ( 0 );//MIN(plane1.vNormal.GetZ(), 0) );
            //plane2.vNormal.SetZ( 0 );//MIN(plane2.vNormal.GetZ(), 0) );

            // Make sure we are going into the plane
            //if(DotProduct(psCar1->cVelocity, plane1.vNormal) > 0)
				//continue;

            // Check to make sure the car position is the good side of the plane
			//if(DotProduct(psCar1->cPos,plane1.vNormal) + plane1.fDistance < 0)
				//continue;

			//float nRV = DotProduct(relVel, posNorm);
			//if(nRV > 0)
				//continue;



            break;
        }

        // Car 1
        CVec norm = plane1.vNormal;
	    float dot = DotProduct(plane1.vNormal, psCar1->cVelocity);
	    norm = norm * dot;
	    CVec vt = psCar1->cVelocity - norm;

        float coef = 0.01f;
		
	    // Ball-on-ball collision
	    CVec norm2 = norm * coef;
		psCar1->cVelocity = vt + norm2;
		norm2 = norm * (1.0f - coef);
		psCar2->cVelocity += norm2;


        // Car 2
        /*norm = plane2.vNormal;
	    dot = DotProduct(plane2.vNormal, psCar2->cVelocity);
	    norm = norm * dot;
	    vt = psCar2->cVelocity - norm;

        coef = 0.01f;
		
	    // Ball-on-ball collision
	    norm2 = norm * coef;
		psCar2->cVelocity = vt + norm2;
		norm2 = norm * (1.0f - coef);
		psCar1->cVelocity += norm2;*/
    }
}


///////////////////
// Setup the camera for the car
void Car_SetupCamera(carsim_t *psCar, CCamera *pcCam, float dt)
{
	float length = 40;

	if(!pcCam)
		return;

	CVec pos = psCar->cPos;
	
	CVec up = psCar->Z;
	// Limit the UP vector to a certain degree
	up.SetZ( MAX(0.5f, up.GetZ()) );

	psCar->cDestCamPos = pos - psCar->Y * length + up * 10;

	CVec v = pcCam->getPos() - psCar->cDestCamPos;
	float dist = VectorNormalize(&v);
	
	// Spring to the 'normal' position
	CVec campos = pcCam->getPos() - v * CVec(10,10,5) * (dist*dt);

	// Follow car camera
	pcCam->Setup( campos, pos+psCar->Y*8 );

    //
    // DEVELOPER: side view
    //
    keyboard_t *kb = System_GetKeyboard();
	if(psOptions->nDeveloper) {
		if(kb->keys[SDLK_q])
			pcCam->Setup( pos - psCar->X*35 + psCar->Z*5, pos);
		if(kb->keys[SDLK_e])
			pcCam->Setup( pos + psCar->X*35 + psCar->Z*5, pos);
		if(kb->keys[SDLK_w])
			pcCam->Setup( pos + psCar->Y*35 + psCar->Z*5, pos);
	}




	// Stationary (DEBUG) camera
	campos = CVec(30,30,420);
	//pcCam->Setup( campos, pos );



	// Hood camera
    if( psCar->nCameraType == CAM_INCAR ) {
        pos = psCar->X*psCar->cInCarCamera.GetX() + psCar->Y*psCar->cInCarCamera.GetY() + psCar->Z*psCar->cInCarCamera.GetZ() + psCar->cPos;
        pcCam->Setup( pos, pos+psCar->Y*5);

        // Find the roll angle of the car
        // TODO: Do this better
        float roll = -atanf( psCar->X.GetZ() ) * (180/PI);
        pcCam->setRoll( roll );
    } else 
        pcCam->setRoll( 0 );
}


///////////////////
// Setup the camera for the rearview mirror
void Car_SetRearviewCam(CCar *pcCar, CCamera *pcCam)
{
    carsim_t *psCar = pcCar->getCarSim();

    // Hood camera
	CVec pos = psCar->X*psCar->cInCarCamera.GetX() + psCar->Y*psCar->cInCarCamera.GetY() + psCar->Z*psCar->cInCarCamera.GetZ() + psCar->cPos;
	pcCam->Setup( pos, pos-psCar->Y*5);
}