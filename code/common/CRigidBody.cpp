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


// Rigid Body class
// Created 10/6/03
// By Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "CRigidBody.h"
#include "System.h"


///////////////////
// CRigidBody Constructor
CRigidBody::CRigidBody()
{
    m_pcModel = NULL;
    m_psCollisionList = NULL;

    reset();
}


///////////////////
// Reset the rigid body
void CRigidBody::reset(void)
{
    // Mass
    m_fMass = 0;
    m_cInertia.LoadIdentity();
    m_cInertiaInverse.LoadIdentity();
    m_cWorldInertiaInverse.LoadIdentity();

    // Linear    
    m_cPosition = CVec(0,0,0);
    m_cVelocity = CVec(0,0,0);

    // Angular
    m_cOrientation.createFromEuler(0,0,0);
	m_cMatOri.LoadIdentity();
    m_cAngularVelocity = CVec(0,0,0);
    m_cAngularMomentum = CVec(0,0,0);

    // Forces
    m_cForce = CVec(0,0,0);
    m_cTorque = CVec(0,0,0);

	m_dTime = (double)SDL_GetTicks() * 0.001;

    freeCollisions();

	m_cOrientation.createFromEuler(0,-180,0);
	m_cMatOri.CreateFromAngle(20,20,0);
}


///////////////////
// Setup the rigid body from a model
void CRigidBody::createFromModel(float fMass, float fCoef, CModel *pcModel)
{
    assert( pcModel );
    m_pcModel = pcModel;
    //m_fMass = fMass/-(-9.8f);
    m_fCoefRest = fCoef;

    // Calculate the Inverse Inertia from the model's dimensions
    CVec dim = pcModel->calculateDimensions();	

	float fDensity = 1.0f;
	m_fMass = fDensity * dim.GetX() * dim.GetY() * dim.GetZ();	
	m_fInvMass = 1.0f / m_fMass;

	dim = dim/2;
	

    m_cInertia.LoadIdentity();
    m_cInertia.SetValue(0,0, 3.0f / (m_fMass * (dim.GetY()*dim.GetY() + dim.GetZ()*dim.GetZ())) );
    m_cInertia.SetValue(1,1, 3.0f / (m_fMass * (dim.GetX()*dim.GetX() + dim.GetZ()*dim.GetZ())) );
    m_cInertia.SetValue(2,2, 3.0f / (m_fMass * (dim.GetX()*dim.GetX() + dim.GetY()*dim.GetY())) );

    m_cInertiaInverse = m_cInertia;    
}


///////////////////
// Create a GL matrix
void CRigidBody::createDrawMatrix(float *m)
{
	//m_cOrientation.createGLMatrix(m_cPosition, m);
	CMatrix mat = m_cOrientation.getMatrix();
	mat.Transpose();
	mat.CreateGLMatrix(m_cPosition, m);
	//m_cMatOri.CreateGLMatrix(m_cPosition, m);
}


///////////////////
// Simulate the rigid body
void CRigidBody::simulate(float fDelta, CModel *pcMesh)
{
    /*float fFixed = 0.01f;
    float t = 0;

    // Fixed time step simulation
	while(t < fDelta) {
		t += fFixed;

        if(fFixed > fDelta)
            fFixed = t = fDelta;
		else if(t > fDelta)
		    fFixed = t - fDelta;
		
        _simulate(fFixed);
	}*/
	

	double fixed = 0.001f;
	double t=0;
	double dt = fDelta;

    double time = (double)SDL_GetTicks() * 0.001;

    // Fixed time-step physics simulation
    while(m_dTime < time) {

        dt = time - m_dTime;
        if(dt > fixed)
            dt = fixed;

		if(!System_GetKeyboard()->keys[SDLK_SPACE]) {
			_simulate(dt);
			checkCollisions(pcMesh);
			resolveCollisions();
		}
		
        
        m_dTime += dt;
    }
    m_dTime = time;

	
	
	
}


///////////////////
// Internal Simulate
void CRigidBody::_simulate(float fDelta)
{
    //
    // Linear
    //

	// Velocity changes the body's position
	m_cPosition += m_cVelocity * fDelta;

    // Forces create acceleration, which alters velocity
    CVec acc = m_cForce * (m_fInvMass * fDelta);
    m_cVelocity += acc;
	
   

    //
    // Angular
    //
    //CVec v2 = CrossProduct(m_cAngularVelocity, (m_cInertia * m_cAngularVelocity));
    //CVec ang_acc = m_cInertiaInverse * (m_cTorque - v2);

    /*CVec v1 = m_cInertia.MultiplyVector(m_cAngularVelocity);
    CVec v2 = CrossProduct(m_cAngularVelocity, v1);
    CVec angacc = m_cInertiaInverse.MultiplyVector( m_cTorque - v2 );
    m_cAngularVelocity += angacc * fDelta;*/

	/*CMatrix m;
	m.SkewSymmetric(m_cAngularVelocity);
	m = m * m_cMatOri;
	m = m * fDelta;
	m_cMatOri = m_cMatOri + m;

	m_cMatOri.OrthoNormalize();*/

    /*CMatrix t1;
    t1.SkewSymmetric(m_cAngularVelocity);
    t1 = t1 * m_cOrientation;
    t1 = t1 * fDelta;
    m_cOrientation = m_cOrientation + t1;*/
    m_cOrientation += CQuaternion(m_cAngularVelocity,0) * m_cOrientation * fDelta * 0.5f;
	//m_cOrientation += (m_cOrientation * m_cAngularVelocity) * (fDelta * 0.5f);
	//m_cOrientation = m_cOrientation + CQuaternion(CVec(0,1,0),0) * fDelta;

    // Normalize the orientation
    float mag = m_cOrientation.magnitude();
    if(mag != 0)
        m_cOrientation /= mag;


    m_cAngularMomentum += m_cTorque * fDelta;

    CMatrix t1 = m_cOrientation.getMatrix();	
    CMatrix t2 = t1;
    t2.Transpose();
	

    m_cWorldInertiaInverse = t1 * m_cInertiaInverse * t2;

	/*m = m_cMatOri * m_cInertiaInverse;
	t1 = m_cMatOri;
	t1.Transpose();
	m_cWorldInertiaInverse = m * t1;*/




	t1 = m_cWorldInertiaInverse;	
    m_cAngularVelocity = t1.MultiplyVector2(m_cAngularMomentum);



    //
    // Clear forces
    //
    m_cForce = CVec(0,0,0);
    m_cTorque = CVec(0,0,0);
}


///////////////////
// Apply a force to the body
void CRigidBody::applyForce(CVec cForce)
{
    m_cForce += cForce;
}


///////////////////
// Apply torque to the body
void CRigidBody::applyTorque(CVec cTorque)
{
	m_cTorque += cTorque;
}


///////////////////
// Apply torque at a position
void CRigidBody::applyPosTorque(CVec cPos, CVec cDir)
{
    m_cTorque += CrossProduct(m_cPosition-cPos, cDir);
}


///////////////////
// Apply dampening
void CRigidBody::applyDampening(float fLinear, float fAngular)
{
    m_cForce -= m_cVelocity * fLinear;
    m_cTorque -= m_cAngularVelocity * fAngular;
}


///////////////////
// Check for collisions with a static mesh
void CRigidBody::checkCollisions(CModel *pcMesh)
{
    assert(pcMesh);
    assert(m_pcModel);

    float matrix[16];
    float p[3], t1[9], t2[9];
	int nID = 0;

    // Setup the Collision Detection matrix
	createDrawMatrix(matrix);
    

    CollisionModel3D *col = m_pcModel->getCDModel();
    col->setTransform(matrix);

    if( pcMesh->getCDModel()->collision( col, -1, 500 ) ) {

        for(int n=0; n<pcMesh->getCDModel()->getNumCollisions(); n++) {

			// Get collision normal
			pcMesh->getCDModel()->getCollidingTriangles(n,t2,t1,false);
            pcMesh->getCDModel()->getCollisionPoint(n,p,false);
            
			plane_t plane = CalculatePlane(CVec(t2[0],t2[1],t2[2]),
										   CVec(t2[3],t2[4],t2[5]),
										   CVec(t2[6],t2[7],t2[8]));

			
            CVec P = CVec(p[0], p[1], p[2]);
			

            // Calculate the relative velocity
            CVec pos = P - m_cPosition;
            CVec rel = CrossProduct(m_cAngularVelocity,pos) + m_cVelocity;
            //rel = m_cOrientation.MultiplyVector(rel);

            // If the relative velocity is not going into the plane, ignore this collision
            if( DotProduct(rel, plane.vNormal) > 0 )
                continue;

			// CM position past plane? if so don't use the collision
            if(DotProduct(m_cPosition,plane.vNormal) + plane.fDistance < 0)
				continue;


			// Check if we havn't already added the same collision in
			bool ignore = false;
			
			rbcol_t *psCol2 = m_psCollisionList;
			for(; psCol2; psCol2=psCol2->psNext) {
				
				if(P == psCol2->cPoint) {
					ignore = true;
					break;
				}
			}
			
			if(ignore)
				continue;


            // Create a new collision
            rbcol_t *psCol = addNewCollision();

            psCol->nType = RBCOL_MESH;
            psCol->cPoint = P;
            psCol->cNormal = plane.vNormal;
            psCol->cRelVelocity = rel;
			psCol->nID = nID++;


		
			//break;
        }
    }
}


///////////////////
// Resolve any collisions
void CRigidBody::resolveCollisions(void)
{


	//return;
    rbcol_t *psCol = m_psCollisionList;
    for(; psCol; psCol=psCol->psNext) {

		bool ignore = false;

		// TEMP: Check for any double ups
		rbcol_t *psCol2 = m_psCollisionList;
		for(; psCol2; psCol2=psCol2->psNext) {
			if(psCol2->nID == psCol->nID)
				continue;

			if(psCol->cPoint == psCol2->cPoint && psCol->nID > psCol2->nID ) {
				ignore = true;
				break;
			}
		}

		if(ignore)
			continue;


        // Body->Mesh collision
        if( psCol->nType == RBCOL_MESH ) {

            CVec p = psCol->cPoint - m_cPosition;

			CVec rel = CrossProduct(m_cAngularVelocity,p) + m_cVelocity;

            // Calculate the numerator
            //float Num = -(1+m_fCoefRest) * DotProduct(psCol->cRelVelocity,psCol->cNormal);
			float Num = -(1.0f + m_fCoefRest) * DotProduct(rel, psCol->cNormal);
			
            // Caluclate the denominator
            float Denom = (m_fInvMass) + DotProduct( CrossProduct( m_cWorldInertiaInverse.MultiplyVector(CrossProduct(p,psCol->cNormal)), p ), psCol->cNormal );

            // Impulse = numerator / denominator
            float j = Num / Denom;
			CVec cImpulse = psCol->cNormal * j;

            // Apply the linear impulse
            m_cVelocity += cImpulse * m_fInvMass;

			CMatrix t1 = m_cOrientation.getMatrix();	
			CMatrix t2 = t1;
			t2.Transpose();
	

			m_cWorldInertiaInverse = t1 * m_cInertiaInverse * t2;

            // Apply the angular impulse
            m_cAngularMomentum += CrossProduct(p, cImpulse);
            m_cAngularVelocity += m_cWorldInertiaInverse.MultiplyVector(m_cAngularMomentum);




			// Draw a debug vector
			/*ec cRight = CrossProduct(psCol->cNormal, CVec(1,0,0));
			CVec cStart = psCol->cPoint;
			CVec cEnd1 = (cStart + psCol->cNormal*5) + cRight*1.0f;
			CVec cEnd2 = (cStart + psCol->cNormal*5) - cRight*1.0f;
			
			State_Disable(GL_TEXTURE_2D);
			State_Disable(GL_BLEND);
			State_Disable(GL_CULL_FACE);
			
			glBegin(GL_TRIANGLES);
			glColor4f(0,0,1,1);
			glVertex3f(cStart.GetX(), cStart.GetY(), cStart.GetZ());
			glColor4f(0,1,0,1);
			glVertex3f(cEnd1.GetX(), cEnd1.GetY(), cEnd1.GetZ());
			glVertex3f(cEnd2.GetX(), cEnd2.GetY(), cEnd2.GetZ());
			glEnd();*/


            //break;







        }
    }

    // Free the collision list
    freeCollisions();
}


///////////////////
// Add a new collision to the list
// Returns the pointer to the collision to be filled in
rbcol_t *CRigidBody::addNewCollision(void)
{
    rbcol_t *psCol = new rbcol_t;
    if( !psCol )
        return NULL;

    // Link it in
    psCol->psNext = m_psCollisionList;
    m_psCollisionList = psCol;

    return psCol;
}


///////////////////
// Free the collisions list
void CRigidBody::freeCollisions(void)
{
    rbcol_t *psCol = m_psCollisionList;
    rbcol_t *nt = NULL;

    for(; psCol; psCol=nt ) {
        nt = psCol->psNext;

        delete psCol;
    }

    m_psCollisionList = NULL;
}
