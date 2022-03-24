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


#ifndef __CRIGIDBODY_H__
#define __CRIGIDBODY_H__


// Collision types
#define     RBCOL_MESH      0
#define     RBCOL_BODY      1


// Collision structure
typedef struct rbcol_s {

    int                 nType;
	int					nID;	
    
    // Pointer to _other_ body
    class cRigidBody    *pcBody;

    CVec                cNormal;
    CVec                cPoint;
    CVec                cRelVelocity;

    struct rbcol_s      *psNext;

} rbcol_t;



class CRigidBody {
private:
    // Attributes

    // Mass & Inertia
    float       m_fCoefRest;
    float       m_fMass;
	float		m_fInvMass;
    CMatrix     m_cInertia;
    CMatrix     m_cInertiaInverse;
    CMatrix     m_cWorldInertiaInverse;

    // Linear    
    CVec        m_cPosition;
    CVec        m_cVelocity;

    // Angular
    CQuaternion m_cOrientation;
	CMatrix		m_cMatOri;

    CVec        m_cAngularVelocity;
    CVec        m_cAngularMomentum;

    // Forces
    CVec        m_cForce;
    CVec        m_cTorque;

    // Model
    CModel      *m_pcModel;

    // Collision list
    rbcol_t     *m_psCollisionList;

	double		m_dTime;


public:
    // Methods

    CRigidBody();

    void        reset(void);
    void        createFromModel(float fMass, float fCoef, CModel *pcModel);

    
    // Simulation
    void        simulate(float fDelta, CModel *pcMesh);    
    void        checkCollisions(CModel *pcMesh);
    void        checkCollisions(CRigidBody *pcBody);
    void        resolveCollisions(void);

	void		createDrawMatrix(float *m);


    // Force
    void        applyForce(CVec cForce);
    void        applyTorque(CVec cTorque);
    void        applyPosTorque(CVec cPos, CVec cDir);
    void        applyDampening(float fLinear, float fAngular);


    // Variables
    CVec        getPosition(void)           { return m_cPosition; }
    void        setPosition(CVec p)         { m_cPosition = p; }
    CQuaternion getOrientation(void)        { return m_cOrientation; }

	


private:
    // Private methods

    void        _simulate(float fDelta);

    // Collision
    rbcol_t     *addNewCollision(void);
    void        freeCollisions(void);
};





#endif  //  __CRIGIDBODY_H__