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


// Car class
// Created 23/5/02
// Jason Boettcher


#include "defs.h"
#include "System.h"
#include "game.h"


using namespace std;


///////////////////
// Load the car model
int CCar::LoadModel(char *sCarDir)
{
	char buf[256];

    strcpy(szCarDir,sCarDir);

    // Add the car dir to the texture dir list
    sprintf(buf,"data/cars/%s",sCarDir);
    Tex_AddDir(buf);	

	tShadow = Cache_LoadTexture("data/cars/shadow3.png");
	Tex_Upload(tShadow);
    psCorona = Cache_LoadTexture("data/textures/corona.png");
    Tex_Upload(psCorona);


	pcBox = Cache_LoadModel("data/temp/box");


	// Note: We don't load the model from the cache because we may want to change some of the models
	//       variables. Like the car material colour/texture, etc
	cModel = new CModel;
    
	if(cModel == NULL) {
		return false;
	}
    sprintf(buf,"data/cars/%s/car",sCarDir);    
    if(!cModel->load(buf)) {
        // Remove the car dir from the texture dir list
        sprintf(buf,"data/cars/%s",sCarDir);
        Tex_RemoveDir(buf);
		return false;
    }

    // Remove the car dir from the texture dir list
    sprintf(buf,"data/cars/%s",sCarDir);
    Tex_RemoveDir(buf);

    // Get the colour of the car
    int num = cModel->getNumMaterials();
	for(int m=0;m<num;m++) {
		CMaterial *mat = cModel->getMaterial(m);

		if(stricmp(mat->getName(),"CarMaterial") == 0) {
            tColour = mat->getPass(0)->tColour;
			// 17/09/2005
			m=num+1;	// breaks for
			//
        }
    }

    // Initialize some stuff
    psStickers = NULL;

    // Get the light positions
	LoadLightPositions();

    // Camera position
    gobject_t *g = cModel->findGameObject("$carcam","$incar_cam");
    if(g)
		cInCarCamera = g->vPosition;
	// 17/06/05
	else	{
		//cInCarCamera = CVec(-1.369f,1.83f,2.7f);
		writeLog("No Internal Car Camera found for car %s.",sCarDir);
	}
	//

	return true;
}


///////////////////
// Load a setup from a file
int CCar::LoadSetup(char *sSetupfile)
{
	char	sCardir[64], sbuf[64];
	int		i;
	gobject_t *g;

	char	*sTyres[] =  {"$flwheel","$frwheel","$brwheel","$blwheel"};
    char	*sShocks[] = {"$flshock","$frshock","$brshock","$blshock"};


	// General details
	ReadString(sSetupfile,  "Car",  "Car",sCardir,"");
	ReadString(sSetupfile,  "Car",  "Name",sName,"");
    ReadKeyword(sSetupfile, "Car",  "EngineBaySize", &iEngineBaySize, SZ_LARGE);
    ReadKeyword(sSetupfile, "Car",  "HoodType", &iHoodType, HOOD_NORMAL);
    ReadKeyword(sSetupfile, "Car",  "FrontBumper", &nFrontBumper, true);
    ReadKeyword(sSetupfile, "Car",  "RearBumper", &nRearBumper, true);

	// Price
	ReadInteger(sSetupfile,"Price","MinPrice", &iMinPrice, 0);
	ReadInteger(sSetupfile,"Price","MaxPrice", &iMaxPrice, 0);
	ReadInteger(sSetupfile,"Price","SellingPrice", &iSellingPrice, 0);

	if(sCardir) {

		// Load the model
		if(!LoadModel(sCardir))
			return false;
	} else
		return false;


    // Set the hood type
    setHoodType(iHoodType);
    
    // Remove bumpers?
    if( !nFrontBumper )
        removeBumper(0);
    if( !nRearBumper )
        removeBumper(1);


	// Tires
	for(i=0;i<4;i++) {
		sprintf(sbuf,"Tire%d",i+1);

		// Create the part
		cTires[i] = newPart();
		if(cTires[i] == NULL)
			return false;

        if(!cTires[i]->LoadSetup(sSetupfile,sbuf)) {
            // If we couldn't load the part, remove it and keep going
            deletePart(cTires[i]);
            cTires[i] = NULL;
            continue;
        }

		// Tires need to have an ID so the tire knows where it is on the car
		cTires[i]->setPartID(i);

		// Find the game object
		g = cModel->findGameObject(sTyres[i]);
		if(g)
			cTires[i]->Setup(cMatrix,vPos,g);
        else {
			// Warning
            writeLog("Modeller Warning: Could not find the object '%s' on the car\n",sTyres[i]);

            // Remove the tire
            deletePart(cTires[i]);
            cTires[i] = NULL;
            continue;
        }
	}


    // Shocks
    for(i=0; i<4; i++) {
        sprintf(sbuf,"Shock%d",i+1);

        cShocks[i] = newPart();
        if(cShocks[i] == NULL)
            return false;

        if(!cShocks[i]->LoadSetup(sSetupfile,sbuf)) {
            deletePart(cShocks[i]);
            cShocks[i] = NULL;
            continue;
        }         

        // Shocks need to have an ID so the shock knows where it is on the car
        cShocks[i]->setPartID(i);

        // Find the game object
		g = cModel->findGameObject(sShocks[i]);
		if(g)
			cShocks[i]->Setup(cMatrix,vPos,g);
        else {
			// Warning
            writeLog("Modeller Warning: Could not find the object '%s' on the car\n",sShocks[i]);

            // Remove the shock
            deletePart(cShocks[i]);
            cShocks[i] = NULL;
            continue;
		}

    }


	// LEFT Muffler
	cMufflers[LEFT_MUFFLER] = newPart();

	if(cMufflers[LEFT_MUFFLER]) {
		cMufflers[LEFT_MUFFLER]->setPartID(LEFT_MUFFLER);
		
        if(!cMufflers[LEFT_MUFFLER]->LoadSetup(sSetupfile,"LMuffler")) {
            deletePart(cMufflers[LEFT_MUFFLER]);
            cMufflers[LEFT_MUFFLER] = NULL;
        } else {
		
            g = cModel->findGameObject("$lmuffler");
            if(g) {
                cMufflers[LEFT_MUFFLER]->Setup(cMatrix,vPos,g);
            } else {
                // Just clean up the part
                deletePart( cMufflers[LEFT_MUFFLER] );
                cMufflers[LEFT_MUFFLER] = NULL;
                
                writeLog("Modeller Warning: Could not find the object '$lmuffler' on the car\n");        
            }
        }
	}


	// RIGHT Muffler
	cMufflers[RIGHT_MUFFLER] = newPart();
	if(cMufflers[RIGHT_MUFFLER]) {
		cMufflers[RIGHT_MUFFLER]->setPartID(RIGHT_MUFFLER);

        if(!cMufflers[RIGHT_MUFFLER]->LoadSetup(sSetupfile,"RMuffler")) {
            deletePart(cMufflers[RIGHT_MUFFLER]);
            cMufflers[RIGHT_MUFFLER] = NULL;
        } else {
		
            g = cModel->findGameObject("$rmuffler");
            if(g) {
                cMufflers[RIGHT_MUFFLER]->Setup(cMatrix,vPos,g);
            } else {
                // Just clean up the part
                deletePart( cMufflers[RIGHT_MUFFLER] );
                cMufflers[RIGHT_MUFFLER] = NULL;
                
                writeLog("Modeller Warning: Could not find the object '$rmuffler' on the car\n");
            }
        }
	}

	// Block
	cBlock = newPart();
	if(cBlock == NULL)
		return false;

    if(!cBlock->LoadSetup(sSetupfile,"block")) {
        deletePart(cBlock);
        cBlock = NULL;
    } else {
	
        // Find the game object
        g = cModel->findGameObject("$block");
        if(g) {
            cBlock->Setup(cMatrix,vPos,g);
            cBlock->LoadBlockParts(sSetupfile);
        }
        else {
            writeLog("Modeller Warning: Could not find the object '$block' on the car\n");
            deletePart(cBlock);
            cBlock = NULL;
        }
    }


    // Differential
    cDiff = newPart();
    if( !cDiff )
        return false;

    if(!cDiff->LoadSetup(sSetupfile,"Diff")) {
        deletePart(cDiff);
        cDiff = NULL;
    } else {        
        g = cModel->findGameObject("$diff");
        if(g)
            cDiff->Setup(cMatrix,vPos,g);
        else {
            writeLog("Modeller Warning: Could not find the object '$diff' on the car\n");
            deletePart(cDiff);
            cDiff = NULL;
        }
    }


    // Transmission
    cTrans = newPart();
    if( !cTrans )
        return false;

    if( !cTrans->LoadSetup(sSetupfile,"Trans")) {
        deletePart(cTrans);
        cTrans = NULL;
    } else {
        g = cModel->findGameObject("$trans");
        if(g)
            cTrans->Setup(cMatrix,vPos,g);
        else {
            writeLog("Modeller Warning: Could not find the object '$trans' on the car\n");
            deletePart(cTrans);
            cTrans = NULL;
        }
    }

	return true;
}


///////////////////
// Load the light positions from the car model
void CCar::LoadLightPositions(void)
{
	// Front lights
	m_FrontLights.clear();
	int Count = 1;
	
	// There shouldn't be any more than 99 lights
	while(Count < 100) {
		char	szBuffer[64];
		sprintf(szBuffer, "$hlite%s%d", Count < 10 ? "0" : "", Count++);

		gobject_t *g = cModel->findGameObject(szBuffer);
		if(!g)
			break;

		m_FrontLights.push_back(g->vPosition);
	}

	// Rear lights
	m_RearLights.clear();
	Count = 1;

	// There shouldn't be any more than 99 lights
	while(Count < 100) {
		char	szBuffer[64];
		sprintf(szBuffer, "$tlite%s%d", Count < 10 ? "0" : "", Count++);

		gobject_t *g = cModel->findGameObject(szBuffer);
		if(!g)
			break;

		m_RearLights.push_back(g->vPosition);
	}

}


///////////////////
// Calculate the rest angle & height
void CCar::calculateRest(CVec pos)
{
    int i;

    // Clear the position & rotation
    cMatrix.LoadIdentity();
    setPosition(CVec(0,0,0));

    // If any of the tires are missing, just rest at a default position
    for(i=0; i<4; i++) {
        if(!cTires[i]) {
            setPosition(pos-CVec(0,0,-3));
            return;
        }
    }


    // Calculate the appropriate rest height & angle based on tyre size & positions
    fRestHeight = 9999;
    float heights[4];
    float lowX[2];
    float lowY[2];
    float wheelBase = 1;
    float wheelSide = 1;
    for(i=0; i<4; i++) {
        if(!cTires[i]) {
            heights[i] = 0;
            continue;
        }
        heights[i] = (cTires[i]->getPos() - cTires[i]->getDimensions()/2).GetZ();
    }    
    // Lows on each axis
    lowX[0] = MIN(heights[0], heights[3]);
    lowX[1] = MIN(heights[1], heights[2]);
    lowY[0] = MIN(heights[0], heights[1]);
    lowY[1] = MIN(heights[2], heights[3]);
    
    // Pitch
    if(cTires[0] && cTires[2]) {
        wheelBase = (cTires[2]->getPos()-cTires[0]->getPos()).GetY();
    }
    cRestAngle.SetX( atan((lowY[0] - lowY[1]) / wheelBase) * (180.0f/PI) );
    // Roll
    if(cTires[0] && cTires[1]) {
        wheelSide = (cTires[0]->getPos()-cTires[1]->getPos()).GetX();
    }
    cRestAngle.SetY( atan((lowX[0] - lowX[1]) / wheelSide) * (180.0f/PI) );


    cMatrix.CreateFromAngle(cRestAngle.GetY(),cRestAngle.GetX(),0);
    setPosition(CVec(0,0,0));

    for(i=0; i<4; i++) {
        if(!cTires[i])
            continue;
        float h = (cTires[i]->getPos() - cTires[i]->getDimensions()/2).GetZ();
        fRestHeight = MIN(fRestHeight,h);
    }
    setPosition(pos-CVec(0,0,fRestHeight));

}


///////////////////
// Get a mesh index based on name
int CCar::getMeshi(char *sName)
{
	mesh_t *m = cModel->getMeshes();	
	for( uint i=0; i<cModel->getNumMeshes(); i++,m++ ) {
		if(stricmp(m->szName,sName) == 0)
			return i;
	}

	return -1;
}


///////////////////
// Get a mesh pointer based on name
mesh_t *CCar::getMeshs(char *sName)
{
	int index = getMeshi(sName);
	if(index == -1)
		return NULL;

	if(index < 0 || index >= cModel->getNumMeshes())
		return NULL;

	return cModel->getMeshes()+index;
}


///////////////////
// Checks for an intersection between a ray and the car
int CCar::LineIntersect(CVec p1, CVec dir, bool closest)
{
	float m[16];
	float p[3],d[3];
	CollisionModel3D *cd = cModel->getCDModel();

	// Set the matrix for the part
	cMatrix.CreateCDMatrix(vPos,m);
	cd->setTransform(m);

	for(int i=0;i<3;i++) {
		p[i] = p1.GetIndex(i);
		d[i] = dir.GetIndex(i);
	}

	return cd->rayCollision(p,d,closest);
}


///////////////////
// Apply a matrix to the car
void CCar::applyMatrix(CMatrix m)
{
    cMatrix = cMatrix * m;

    // Re-adjust all the parts
    setPosition(vPos);
}


///////////////////
// Set the position of the car & all the parts
void CCar::setPosition(CVec pos)
{
	int i;

	vPos = pos;

	// Tires
	for(i=0; i<4; i++) {
		if(cTires[i])
			cTires[i]->Setup( cMatrix, vPos, cTires[i]->getGameObject() );
	}

    // Shocks
    for(i=0; i<4; i++) {
        if(cShocks[i])
            cShocks[i]->Setup( cMatrix, vPos, cShocks[i]->getGameObject() );

    }

	// Mufflers
	for(i=0; i<2; i++) {
		if(cMufflers[i])
			cMufflers[i]->Setup( cMatrix, vPos, cMufflers[i]->getGameObject() );
	}

	// Block
	if(cBlock)
		cBlock->setPosition( cMatrix, vPos );

    // Diff
    if(cDiff)
        cDiff->setPosition( cMatrix, vPos );

    // Trans
    if(cTrans)
        cTrans->setPosition( cMatrix, vPos );

}


///////////////////
// Draw the car for the garage, restaurant
void CCar::Draw(void)
{
	int		i;
	float	mat[16];


    //
    // Draw the shadow
    //
    glDepthMask(0);
	State_Disable(GL_TEXTURE_GEN_S);
	State_Disable(GL_TEXTURE_GEN_T);
	State_Disable(GL_LIGHTING);
	State_Enable(GL_TEXTURE_2D);
	State_Enable(GL_BLEND);	
	glBlendFunc(GL_ZERO, GL_SRC_COLOR);
    
    glPushMatrix();			// <-- Push is here!
	Tex_Bind(tShadow);

	glColor4f(1,1,1,1);    
	glTranslatef(vPos.GetX(),vPos.GetY(),vPos.GetZ()+fRestHeight);

    // Get the yaw for the shadow
    CVec v = CVec(0,1,0);
    v = cMatrix.MultiplyVector(v);
    glRotatef(atan2(v.GetX(),v.GetY()) * (180/PI),0,0,1);

	glBegin(GL_QUADS);
	glTexCoord2i(0,0);		glVertex2f(-5,-12.5);
	glTexCoord2i(1,0);		glVertex2f(5,-12.5);
	glTexCoord2i(1,1);		glVertex2f(5,12.5);
	glTexCoord2i(0,1);		glVertex2f(-5,12.5);
	glEnd();

    glPopMatrix();

    glDepthMask(1);
    State_Enable(GL_LIGHTING);


     //
    // Draw the parts
    //

    // Set the specular lighting to zero
    float specz[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, specz);

	// For some reason, this had to be here
	// Part drawing in special mode can't set their blendfunc properly ????
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    State_Enable(GL_LIGHTING);

	// Draw the tyres
	for(i=0;i<4;i++)
		if(cTires[i])
			cTires[i]->Draw();

    // Draw the shocks
	for(i=0;i<4;i++)
		if(cShocks[i])
			cShocks[i]->Draw();

	// Draw the mufflers
	for(i=0;i<2;i++)
		if(cMufflers[i])
			cMufflers[i]->Draw();

    // Draw the diff
    if(cDiff)
        cDiff->Draw();

    // Draw the transmission
    if(cTrans)
        cTrans->Draw();
	

    //
    // Draw the car
    //
	glPushMatrix();

	createMatrix(mat);
	glMultMatrixf(mat);

    // Set the specular highlighting
    float spec[4] = {0.3f, 0.3f, 0.3f, 1.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);

	cModel->render();
    	

    //
    // Draw the stickers
    //
    State_Enable(GL_TEXTURE_2D);
    State_Disable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);    

    sticker_t *decal = psStickers;
    for(; decal; decal=decal->psNext ) {
        Tex_Bind(decal->psTexture);
        glColor4f(decal->cColour.GetX(), decal->cColour.GetY(), decal->cColour.GetZ(), 1.0f);
        cModel->renderDecal(decal->psPolygons, decal->nNumPolygons);
    }

	glPopMatrix();

}


///////////////////
// Draw the driving version of the car
void CCar::DrawDriving(CCamera *pcCamera)
{
	carsim_t	*car = &sCarSim;

	// Car
	glPushMatrix();

	// Apply the matrix
	int		i;
	float	m[16];

	// Clear it
	for(i=0;i<16;i++)
		m[i] = 0;

	for(i=0;i<3;i++)
		m[i] = car->X.GetIndex(i);
	for(i=4;i<7;i++)
		m[i] = car->Y.GetIndex(i-4);
	for(i=8;i<11;i++)
		m[i] = car->Z.GetIndex(i-8);
	for(i=12;i<15;i++)
		m[i] = car->cPos.GetIndex(i-12);
	m[15] = 1;

	glMultMatrixf(m);
    CMatrix curMat;
    curMat.createFromArray(m);


	// TEMP
	glPushMatrix();
	CVec t = cDim[1] - cDim[0];
	//glTranslatef(t.GetX(), t.GetY(), t.GetZ());
	glScalef(t.GetX(), t.GetY(), t.GetZ());
	//pcBox->render();
	glPopMatrix();

	cModel->render();


	// Tyres
	for(i=0;i<4;i++) {
		if(cTires[i]) {

			glPushMatrix();

			glTranslatef( sCarSim.sWheels[i].cRelPos.GetX(), sCarSim.sWheels[i].cRelPos.GetY(), sCarSim.sWheels[i].cRelPos.GetZ()+sCarSim.sWheels[i].fWheelZ );
			glRotatef(90,0,0,1);
			
			if(i==0 || i==3)
				glRotatef(sCarSim.fSteerAngle*30, 0,0,1);

			glRotatef( sCarSim.sWheels[i].fSpin, 0,1,0);
			if(i<=1)
				glRotatef(180,0,0,1);

			cTires[i]->getModel()->render();

			glPopMatrix();
		}
	}

    // Draw the engine
    CMatrix mat;
    if(cBlock)
        cBlock->setPosition(mat,CVec(0,0,0));
    glPushMatrix();
    float a = 0.5+0.5*sin( tMainSR3.fCurTime / (0.01f) );
    if(a>1) a=1;
    if(a<0) a=0;
    glRotatef((a-0.5f) / 2.5f,0,1,0);
    DrawEngine();
    glPopMatrix();

	glPopMatrix();
    

	// Draw the front and rear lights
	DrawLights(pcCamera);
	
    State_Enable(GL_DEPTH_TEST);
    State_Enable(GL_LIGHTING);


	// Draw a debug vector
	CVec cRight = CrossProduct(car->cColTemp[1], CVec(0,0,1));
	CVec cStart = car->cColTemp[0];
	CVec cEnd1 = (car->cColTemp[0] + car->cColTemp[1]*5) + cRight*0.5f;
	CVec cEnd2 = (car->cColTemp[0] + car->cColTemp[1]*5) - cRight*0.5f;

	State_Disable(GL_TEXTURE_2D);
	State_Disable(GL_BLEND);
	State_Disable(GL_CULL_FACE);

	glBegin(GL_TRIANGLES);
		glColor4f(0,0,1,1);
		glVertex3f(cStart.GetX(), cStart.GetY(), cStart.GetZ());
		glColor4f(0,1,0,1);
		glVertex3f(cEnd1.GetX(), cEnd1.GetY(), cEnd1.GetZ());
		glVertex3f(cEnd2.GetX(), cEnd2.GetY(), cEnd2.GetZ());
	glEnd();
}


///////////////////
// Draw the engine parts
void CCar::DrawEngine(void)
{
    // Set the specular lighting to zero
    float spec[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);

	if(cBlock) {
		cBlock->Draw();
		cBlock->DrawAttached();
	}
}


///////////////////
// Draw the front & rear lights
void CCar::DrawLights(CCamera *pcCamera)
{
	carsim_t	*car = &sCarSim;

	/*
	 * Setup the drawing
	 */
	glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_COLOR);
    State_Enable(GL_BLEND);
    State_Disable(GL_LIGHTING);
    State_Enable(GL_TEXTURE_2D);
    State_Disable(GL_DEPTH_TEST);


	/*
	 * Front lights
	 */
	vector<CVec>::iterator it;

	for(it = m_FrontLights.begin(); it != m_FrontLights.end(); it++) {
		CVec Position = *it;

		// Transform the light position
		CVec Transform = car->X*Position.GetX() + car->Y*Position.GetY() + car->Z*Position.GetZ() + car->cPos;

		// Can we see the light?
		if(DotProduct(car->Y, pcCamera->getPos()) + DotProduct(-car->Y, Transform) <= 0)
			continue;

		// Draw it
		glColor4f(1,1,1,1);
		RenderBillboardSprite(Transform, psCorona, pcCamera, 3, 0);
	}


	/*
	 * Rear lights
	 */
	for(it = m_RearLights.begin(); it != m_RearLights.end(); it++) {
		CVec Position = *it;

		// Transform the light position
		CVec Transform = car->X*Position.GetX() + car->Y*Position.GetY() + car->Z*Position.GetZ() + car->cPos;

		// Can we see the light?
		if(DotProduct(car->Y, pcCamera->getPos()) + DotProduct(-car->Y, Transform) > 0)
			continue;

		float Red = 0.5f;
		float Size = 2.0f;

		// If we're braking, draw it at full-brightness
		if(car->bBraking) {
			Red = 1.0f;
			Size = 2.0f;
		}

		// Draw it
		glColor4f(Red,0,0,1);
		RenderBillboardSprite(Transform, psCorona, pcCamera, Size, 0);
	}
}


///////////////////
// Create the matrix
void CCar::createMatrix(float *m)
{
	cMatrix.CreateGLMatrix(vPos,m);
}


///////////////////
// Translates a vector in car-coords to a vector in world-coords
CVec CCar::TranslateVector(CVec v)
{
	return cMatrix.MultiplyVector(v) + vPos;
}


///////////////////
// Checks if the part can be removed from the engine
int CCar::checkPartRemoval(int type)
{
	// Check dependencies
	switch(type) {
		case PRT_BLOCK:
		case PRT_ENGINE:		// Fall through
			return true;
			break;


		// Intake manifold
		case PRT_INTAKEMAN:
			if(cBlock) {

                // Check that ALL the carby's have been removed
                CPart *man = cBlock->FindPart(PRT_INTAKEMAN,-1);
                if( !man )
                    return false;

                for( int i=0; i<man->getNumCarbys(); i++ ) {
                    CPart *carb = cBlock->FindPart(PRT_CARBY,i);
                    if( carb )
                        return false;
                }

                // If we got here, all carby's must be off
                return true;
			}

			return false;
			break;


		// Carby
		case PRT_CARBY:
			if(cBlock) {
				if(!cBlock->FindAttached(PRT_AIRFILTER))
					return true;
			}
			return false;
			break;
	}


	return true;
}


///////////////////
// Add a part into the engine bay
int CCar::AddEnginePart(CPart *part, char *msg)
{
    gobject_t *g = NULL;

    msg[0] = '\0';
    
	if(part == NULL)
		return false;
	

	switch(part->getType()) {

		// Block
		case PRT_BLOCK:
			if(cBlock)
				return false;
            // Does the block match the transmission brand
            if( cTrans ) {
                if( !part->checkManufacturer(cTrans) ) {
                    strcpy(msg,"Block doesn't match transmission");
                    return false;
                }
            }

            cBlock = part;

            // Find the game object
			g = cModel->findGameObject("$block");
			if(g) {				
				cBlock->Setup(cMatrix, vPos, g);
				cBlock->setPosition(cMatrix, vPos);
				return true;
            }
			return true;
			break;

		// Parts attached to the block
		case PRT_INTAKEMAN:
		case PRT_STARTER:
		case PRT_ALTER:
		case PRT_FAN:
		case PRT_CARBY:
		case PRT_AIRFILTER:
			if(cBlock)
				return cBlock->AddPart(part,msg);
			return false;
			break;

        default:
            printf("Error: CCar()::AddEnginePart(): Unknown part type %d\n",part->getType());
            break;
	}


	return false;
}


///////////////////
// Add a part onto the car
int CCar::AddPart(CPart *part)
{
	return false;
}


///////////////////
// Add a part onto the running gear
int CCar::AddRunningGPart(CPart *part, char *msg)
{
	char	*sTyres[] = {"$flwheel","$frwheel","$brwheel","$blwheel"};
    char	*sShocks[] = {"$flshock","$frshock","$brshock","$blshock"};
	int		i;

    msg[0] = '\0';

	assert(part);

	switch(part->getType()) {

		// Tire
		case PRT_TIRE:
			for(i=0; i<4; i++) {
				if(!cTires[i]) {
					cTires[i] = part;
                    cTires[i]->setPartID(i);

					// Find the game object
					gobject_t *g = cModel->findGameObject(sTyres[i]);
					if(g) {
						cTires[i]->Setup( cMatrix, vPos, g);
						return true;
					}
				}
			}
			break;

        // Shock
        case PRT_SHOCK:
            for(i=0; i<4; i++) {
				if(!cShocks[i]) {
					cShocks[i] = part;
                    cShocks[i]->setPartID(i);

					// Find the game object					
					gobject_t *g = cModel->findGameObject(sShocks[i]);
					if(g) {
						cShocks[i]->Setup( cMatrix, vPos, g );
						return true;
					}
				}
			}
            break;


		// Muffler
		case PRT_LMUFFLER:
		case PRT_RMUFFLER:
			
			if(part->getType() == PRT_LMUFFLER)
				i = LEFT_MUFFLER;
			else
				i = RIGHT_MUFFLER;

			if(!cMufflers[i]) {
				cMufflers[i] = part;
                cMufflers[i]->setPartID(i);

				// Find the game object
				gobject_t *g = cModel->findGameObject(i == LEFT_MUFFLER ? "$lmuffler" : "$rmuffler");
				if(g) {
					cMufflers[i]->Setup( cMatrix, vPos, g);
					return true;
				}
			}
			break;

        // Differential
        case PRT_DIFF:

            if( !cDiff ) {
                cDiff = part;
                // Find the game object
			    gobject_t *g = cModel->findGameObject("$diff");
				if(g) {
				    cDiff->Setup( cMatrix, vPos, g);
					return true;
				}
            }
            break;

        // Transmission
        case PRT_TRANS:

            if( !cTrans ) {

                // Check that the transmission matches the block
                if( cBlock ) {
                    if( !cBlock->checkManufacturer(part) ) {
                        strcpy(msg,"Transmission doesn't match block");
                        return false;
                    }
                }

                cTrans = part;
                // Find the game object
			    gobject_t *g = cModel->findGameObject("$trans");
				if(g) {
				    cTrans->Setup( cMatrix, vPos, g);
					return true;
				}
            }
            break;
	}

	return false;
}


///////////////////
// Remove an engine part
void CCar::removeEnginePart(CPart *part)
{
    assert(part);

    if( cBlock ) {
        if( part->getUniqueID() == cBlock->getUniqueID() )
            cBlock = NULL;
    }

}


///////////////////
// Remove a running gear part
void CCar::removeRunningGearPart(CPart *part)
{
	assert(part);
	int id = 0;


	switch(part->getType()) {

		// Muffler
		case PRT_LMUFFLER:
		case PRT_RMUFFLER:
			id = part->getPartID();
			if(id >= 0 && id < 2) {
				cMufflers[id] = NULL;
			}
			break;

		// Tire
		case PRT_TIRE:
			id = part->getPartID();
			if(id >= 0 && id < 4) {
				cTires[id] = NULL;
			}
			break;

        // Shock
		case PRT_SHOCK:
			id = part->getPartID();
			if(id >= 0 && id < 4) {
                cShocks[id] = NULL;
			}
			break;

        // Differential
        case PRT_DIFF:
            if( cDiff )
                cDiff = NULL;
            break;

        // Transmission
        case PRT_TRANS:
            if( cTrans )
                cTrans = NULL;
            break;
	}

}


///////////////////
// Paint the car
void CCar::PaintCar(CVec colour, bool bClearDecals)
{
	tColour.R = colour.GetX();
	tColour.G = colour.GetY();
	tColour.B = colour.GetZ();

	// Find the material named 'CarMaterial' and change it's colour property (in all passes)
	int num = cModel->getNumMaterials();
	for(int m=0;m<num;m++) {
		CMaterial *mat = cModel->getMaterial(m);

		if(stricmp(mat->getName(),"CarMaterial") == 0) {

			// Go through each pass
            for(int p=0;p<mat->getNumPasses();p++) {
				mat->getPass(p)->tColour = tColour;
                mat->getPass(p)->iUseColour = true;
            }
		}
	}

    // Clear the decals from the car
    if( bClearDecals )
        clearDecals();
}


///////////////////
// Start the engine
void CCar::StartEngine(void)
{
	// TODO: Check if the engine can start

	if(cBlock) {
		iEngineRunning = true;
		cBlock->StartEngine();
	}
}


///////////////////
// Switch the engine off
void CCar::SwitchEngineOff(void)
{
	if(cBlock) {
		iEngineRunning = false;
		cBlock->SwitchEngineOff();
	}
}


///////////////////
// Simulate the engine
void CCar::SimulateEngine(void)
{
	if(cBlock && iEngineRunning)
		cBlock->SimulateEngine();
}


///////////////////
// This calculates the rough price of the car
int CCar::calculatePrice(void)
{
	int price = 0;
	int n;

	// Add the price of the car body
	price += iMaxPrice;

	// Add the car parts
	//

	// Block
	if(cBlock)
		price += cBlock->CalculateSelling(10.0f);

	// Tyres
	for(n=0;n<4;n++) {
		if(cTires[n])
			price += cTires[n]->CalculateSelling(10.0f);
	}

    // Shocks
	for(n=0;n<4;n++) {
		if(cShocks[n])
			price += cShocks[n]->CalculateSelling(10.0f);
	}

	// Mufflers
	for(n=0;n<2;n++) {
		if(cMufflers[n])
			price += cMufflers[n]->CalculateSelling(10.0f);
	}

    // Diff
    if(cDiff)
        price += cDiff->CalculateSelling(10.0f);

    // Transmission
    if(cTrans)
        price += cTrans->CalculateSelling(10.0f);
	
	return price;
}


///////////////////
// Calculate the selling price
int CCar::calculateSellingPrice(void)
{
    int price = 0;

    price = calculatePrice();


    return price;
}


///////////////////
// Start the garage roll-in
void CCar::startGarageRollIn(void)
{
	iRollingIn = true;
	iRollingOut = false;
    calculateRest(CVec(4,-45,-4.61f));
}


///////////////////
// Process the garage roll-in
void CCar::processGarageRollIn(void)
{
	float dt = tMainSR3.fDeltaTime;

	vPos = vPos + CVec(0,10*dt,0);
	setPosition(vPos);

	// Spin the tyres
	for(int i=0;i<4;i++)
		if(cTires[i])
			cTires[i]->SpinTyre(1);

	if(vPos.GetY() > 5)
        finishGarageRollIn();
}


///////////////////
// This finishes the garage roll-in (useful if escape was pressed)
void CCar::finishGarageRollIn(void)
{
    //setPosition( CVec(4,5,-1.4f) );
    calculateRest(CVec(4,5,-4.61f));
	iRollingIn = false;
    iRollingOut = false;
}


///////////////////
// Start the garage roll-out
void CCar::startGarageRollOut(void)
{
	iRollingOut = true;
	iRollingIn = false;
    calculateRest(CVec(4,5,-4.61f));
}


///////////////////
// Process the garage roll-out
void CCar::processGarageRollOut(void)
{
	float dt = tMainSR3.fDeltaTime;

	vPos = vPos - CVec(0,15*dt,0);
	setPosition(vPos);

	// Spin the tyres
	for(int i=0;i<4;i++)
		if(cTires[i])
			cTires[i]->SpinTyre(-1);

	if(vPos.GetY() < -45)
        finishGarageRollOut();
}


///////////////////
// This finishes the garage roll-out (useful if escape was pressed)
void CCar::finishGarageRollOut(void)
{
    setPosition( CVec(4,-45,-1.4f) );
	iRollingOut = false;
	iRollingIn = false;
}


///////////////////
// Set the hood type
void CCar::setHoodType(int nType)
{
    iHoodType = nType;

    setupHood();
}


///////////////////
// Show/Hide the hoods
void CCar::setupHood(void)
{
    mesh_t *m = NULL;

    // Unhide both hoods
    m = cModel->findMesh("hood");
    if(m)   m->nFlags &= ~MSH_HIDDEN;
    m = cModel->findMesh("perf_hood");
    if(m)   m->nFlags &= ~MSH_HIDDEN;


    // Hide the other hood
    if(iHoodType == HOOD_NORMAL) {
        m = cModel->findMesh("perf_hood");
        if(m)   m->nFlags |= MSH_HIDDEN;
    }
    if(iHoodType == HOOD_PERFORMANCE) {
        m = cModel->findMesh("hood");
        if(m)   m->nFlags |= MSH_HIDDEN;
    }
}


///////////////////
// Remove a bumper-bar
// bump: 0 = Front
//       1 = Rear
void CCar::removeBumper(int bump)
{
    mesh_t *m = NULL;

    if( bump == 0 ) {
        m = cModel->findMesh("fbumper");
        nFrontBumper = false;
    }
    else if( bump == 1 ) {
        m = cModel->findMesh("rbumper");
        nRearBumper = false;
    }

    // Set the hidden flag
    if( m )        
        m->nFlags |= MSH_HIDDEN;
}


///////////////////
// Repair a bumper-bar
// bump: 0 = Front
//       1 = Rear
void CCar::repairBumper(int bump)
{
    mesh_t *m = NULL;

    if( bump == 0 ) {
        m = cModel->findMesh("fbumper");
        nFrontBumper = true;
    }
    else if( bump == 1 ) {
        m = cModel->findMesh("rbumper");
        nRearBumper = true;
    }

    // Remove the hidden flag
    if( m )
        m->nFlags &= ~MSH_HIDDEN;
}


///////////////////
// Set the visibility of the running gear
void CCar::setRunningGearVis(int nVis)
{
    int i=0;

    // Wheels & Shocks
    for( i=0; i<4; i++ ) {
        if( cTires[i] )
            cTires[i]->setHidden(!nVis);
        if( cShocks[i] )
            cShocks[i]->setHidden(!nVis);
    }

    // Mufflers
    for( i=0; i<2; i++ ) {
        if( cMufflers[i] )
            cMufflers[i]->setHidden(!nVis);
    }

    // Diff
    if( cDiff )
        cDiff->setHidden(!nVis);

    // Trans
    if( cTrans )
        cTrans->setHidden(!nVis);
}


///////////////////
// Set the visibility of the engine
void CCar::setEngineVis(int nVis)
{
    if( cBlock )
        cBlock->setHidden(!nVis);
}


///////////////////
// Create a filter of what parts are needed
void CCar::createEnginePartFilter(bool *pbFilter)
{
    assert( pbFilter );

    // Initialize it to zero
    memset( pbFilter, false, sizeof(bool) * PRT_LAST );

    // Is the block available?
    if( !cBlock ) {
        pbFilter[PRT_BLOCK] = true;
        return;
    }


    // Go through the engine parts
    if( !cBlock->FindPart(PRT_INTAKEMAN,-1) )
        // Intake manifold
        pbFilter[PRT_INTAKEMAN] = true;
    else {

        // Carbys
        CPart *man = cBlock->FindPart(PRT_INTAKEMAN,-1);
        for( int i=0; i<man->getNumCarbys(); i++ ) {
            if( !cBlock->FindPart(PRT_CARBY,i) )
                pbFilter[PRT_CARBY] = true;
        }

        // Air filter
        if( !pbFilter[PRT_CARBY] ) {
            if( !cBlock->FindPart(PRT_AIRFILTER,-1) )
                pbFilter[PRT_AIRFILTER] = true;
        }
    }

    // And the rest
    for( int n=PRT_ALTER; n<=PRT_FAN; n++ ) {
        if( !cBlock->FindPart(n,-1) )
            pbFilter[n] = true;
    }
}


///////////////////
// Create a filter of what parts are needed
void CCar::createRunningGFilter(bool *pbFilter)
{
    assert( pbFilter );

    // Initialize it to zero
    memset( pbFilter, false, sizeof(bool) * PRT_LAST );


    // 4 Wheels & 4 Shocks
    for( int i=0; i<4; i++ ) {
        if( !cTires[i] )
            pbFilter[PRT_TIRE] = true;
        if( !cShocks[i] )
            pbFilter[PRT_SHOCK] = true;
    }

    // 2 Mufflers    
    if( !cMufflers[LEFT_MUFFLER] )
        pbFilter[PRT_LMUFFLER] = true;
    if( !cMufflers[RIGHT_MUFFLER] )
        pbFilter[PRT_RMUFFLER] = true;

    // Diff & Trabs
    if( !cDiff )
        pbFilter[PRT_DIFF] = true;
    if( !cTrans )
        pbFilter[PRT_TRANS] = true;
}


// Temp decal poly structure
typedef struct dec_poly_s {

    uint        nVertexNormal[3];
    uint        nVertex[3];
    texcoord_t  sTexCoord[3];

    mesh_t      *psMesh;
    struct dec_poly_s   *psNext;

} dec_poly_t;



///////////////////
// Apply a decal to the car
void CCar::applyDecal(char *szFilename, CCamera *psCam, int Rx, int Ry, int Rw, int Rh, CVec cColour)
{
    CVec f, r, u;
    plane_t planes[4];  // Top, Right, Bottom, Left
    int i,p,n, j;
    float mtx[16];
	float o[3],d[3];
	CollisionModel3D *cd = cModel->getCDModel();

	// Set the matrix for the car
	cMatrix.CreateCDMatrix(vPos,mtx);
	cd->setTransform(mtx);

	
    // 4 rectangle corners un-projected into 3d space
    CVec pnt[4];
    pnt[0] = UnprojectPoint(CVec(Rx,Ry,0));
    pnt[1] = UnprojectPoint(CVec(Rx+Rw,Ry,0));
    pnt[2] = UnprojectPoint(CVec(Rx+Rw,Ry+Rh,0));
    pnt[3] = UnprojectPoint(CVec(Rx,Ry+Rh,0));

    // Top
    planes[0] = CalculatePlane(pnt[0], pnt[1], psCam->getPos());
    // Right
    planes[1] = CalculatePlane(pnt[1], pnt[2], psCam->getPos());
    // Bottom
    planes[2] = CalculatePlane(pnt[2], pnt[3], psCam->getPos());
    // Left
    planes[3] = CalculatePlane(pnt[3], pnt[0], psCam->getPos());


    // Create a new sticker
    sticker_t *decal = new sticker_t;
    if( !decal )
        return;

    dec_poly_t *list = NULL;
    int count = 0;


    // Check what polygons are inside the volume
    mesh_t *m = cModel->getMeshes();
    for( i=0; i<cModel->getNumMeshes(); i++, m++ ) {        

        polygon_t *poly = m->psPolygons;
        for( p=0; p<m->nNumPolygons; p++, poly++ ) {
            CVec tri[3];

            // Do checks to see if the polygon is inside the volume
            bool inside = false;

            // Ignore backfacing polygons
            if( ProjectPoint(&poly->sPlane, psCam->getPos()) < -EPSILON )
                continue;

            // This polygon _must_ be textured with the 'carmaterial' mat            
            CMaterial *mat = cModel->findMaterial(poly->nTextureID);
            if( mat ) {
                if( stricmp(mat->getName(),"CarMaterial") != 0 )
                    continue;
            }
            
            // Check if any of the vertices are inside the volume
            for( n=0; n<3; n++ ) {
                CVec vert = cMatrix.MultiplyVector(CVec( (cModel->getVertexList()+poly->nVertex[n])->X,
                                                         (cModel->getVertexList()+poly->nVertex[n])->Y,
                                                         (cModel->getVertexList()+poly->nVertex[n])->Z )) + vPos;
                tri[n] = vert;
                if( ProjectPoint(&planes[0], vert) > EPSILON &&
                    ProjectPoint(&planes[1], vert) > EPSILON &&
                    ProjectPoint(&planes[2], vert) > EPSILON &&
                    ProjectPoint(&planes[3], vert) > EPSILON)
                    inside = true;
            }

            // If none of the vertices are inside, do another check to see if the poly intersects with the 4
            // triangles made from the frustum
            if( !inside ) {
                CVec t1[3] = { pnt[0], pnt[1], psCam->getPos() };
                CVec t2[3] = { pnt[1], pnt[2], psCam->getPos() };
                CVec t3[3] = { pnt[2], pnt[3], psCam->getPos() };
                CVec t4[3] = { pnt[3], pnt[0], psCam->getPos() };

                if( TriTri_Intersection( tri, t1 ) || TriTri_Intersection( tri, t2 ) || TriTri_Intersection( tri, t3 ) || TriTri_Intersection( tri, t4 ))
                    inside = true;
            }


            // If rays from all 3 vertices of the triangle, to the camera are blocked, don't apply a decal
            // to this triangle
            if( inside ) {
                int vs = 0;
                for( n=0; n<3; n++ ) {
                    CVec dir = tri[n] - psCam->getPos();
                    float l = VectorNormalize(&dir);
                    for( j=0; j<3; j++ ) {
		                o[j] = psCam->getPos().GetIndex(j);
		                d[j] = dir.GetIndex(j);
                    }
                    if( cd->rayCollision(o, d, true, 0, l-1 ) )
                        vs++;
	            }

                // Are all 3 verts occluded?
                if( vs > 0 )
                    inside = false;
            }


            // Was this triangle in the projected volume??
            // If so, create a new triangle from this triangle
            if( inside ) {
                count++;
                
                // Add the polygon to the list
                dec_poly_t *dp = new dec_poly_t;
                if( !dp ) {
                    writeLog("BAD BAD! No memory for extra sticker polygons\n");
                    return;
                }
                for( n=0; n<3; n++ ) {
                    dp->nVertex[n] = poly->nVertex[n];
                    dp->nVertexNormal[n] = poly->nVertexNormal[n];
                    
                    CVec vert = cMatrix.MultiplyVector(CVec( (cModel->getVertexList()+poly->nVertex[n])->X,
                                                             (cModel->getVertexList()+poly->nVertex[n])->Y,
                                                             (cModel->getVertexList()+poly->nVertex[n])->Z )) + vPos;

                    // Set the UV coordinates based on vertex positions
                    float l = ProjectPoint(&planes[3], vert);
                    float r = ProjectPoint(&planes[1], vert);
                    float t = ProjectPoint(&planes[0], vert);
                    float b = ProjectPoint(&planes[2], vert);
                    dp->sTexCoord[n].U = l / (r+l);
                    dp->sTexCoord[n].V = t / (b+t);
                    dp->psMesh = m;
                }

                // Link it at the end
                dp->psNext = NULL;
                dec_poly_t *end = list;
                if( list ) {
                    for(; end; end=end->psNext ) {
                        if( end->psNext == NULL ) {
                            end->psNext = dp;
                            break;
                        }
                    }
                } else
                    list = dp;
            }
        }
    }


    // Allocate the polygons for the proper sticker structure
    decal->psNext = NULL;
    decal->nNumPolygons = count;
    decal->cColour = cColour;
    decal->psPolygons = new polygon_t[decal->nNumPolygons];
    strcpy(decal->szFilename, szFilename);
    if( !decal->psPolygons ) {
        writeLog("BAD BAD! No memory for extra sticker polygons\n");
        return;
    }

    // Load the texture
    decal->psTexture = Cache_LoadTexture(decal->szFilename);
    if( decal->psTexture ) {
        decal->psTexture->nWrap_S = GL_CLAMP;
        decal->psTexture->nWrap_T = GL_CLAMP;
        decal->psTexture->iMipmaps = false;
        Tex_Upload(decal->psTexture,true);
    }

    // Fill in the polygons
    dec_poly_t *dp = list;
    count = 0;
    for(; dp; dp=dp->psNext ) {
        polygon_t *poly = &decal->psPolygons[count++];

        for( n=0; n<3; n++ ) {
            poly->nVertex[n] = dp->nVertex[n];
            poly->nVertexNormal[n] = dp->nVertexNormal[n];
            poly->sDecalCoords[n] = dp->sTexCoord[n];
            poly->psMesh = dp->psMesh;
        }
    }

    // Link in the sticker at the end
    sticker_t *st = psStickers;
    if( psStickers ) {
        for(; st; st=st->psNext ) {
            if( st->psNext == NULL ) {
                st->psNext = decal;
                break;
            }
        }
    } else
        psStickers = decal;


    // Clear the decal_poly list
    dec_poly_t *nt = NULL;
    for(; dp; dp=nt ) {
        nt=dp->psNext;
        delete dp;
    }
}


///////////////////
// Clear the decals from the car
void CCar::clearDecals(void)
{
    sticker_t *d = psStickers;
    sticker_t *n = NULL;

    for(; d; d=n ) {
        n = d->psNext;

        if( d->psPolygons )
            delete[] d->psPolygons;
        delete d;
    }

    psStickers = NULL;
}


///////////////////
// Returns true if the running gear is complete
bool CCar::runningGearComplete(void)
{
    int i;

    // 4 Tires & 4 Shocks
    for(i=0; i<4; i++) {
        if(!cTires[i])
            return false;
        if(!cShocks[i])
            return false;
    }

    // 2 Mufflers
    for(i=0; i<4; i++) {
        if(!cMufflers[i])
            return false;
    }

    // Diff & Trans
    if(!cDiff || !cTrans)
        return false;


    // Running gear must be complete
    return true;
}


///////////////////
// Save the car to a savegame file
void CCar::save(FILE *fp)
{
    int i;

    // Model car directory
    writePascalString(szCarDir,fp);    

    // Position in world
    fwrite(&vPos,           sizeof(CVec),   1,  fp);
    cMatrix.Save(fp);
	
    // Resting in the garage & restaurant
    fwrite(&fRestHeight,    sizeof(float),  1,  fp);
    fwrite(&cRestAngle,     sizeof(CVec),   1,  fp);

    // Body paint
    fwrite(&tColour,        sizeof(colour_t),1, fp);

    // Decals
    int nNumDecals = 0;
    sticker_t *d = psStickers;
    for(; d; d=d->psNext)
        nNumDecals++;
    
    // Write out the decals
    d = psStickers;
    fwrite(&nNumDecals,     sizeof(int),    1,  fp);    
    for(; d; d=d->psNext)
        writeDecal(d, fp);

    // Game details    	
    writePascalString(sName, fp);
    fwrite(&iMinPrice,      sizeof(int),    1,  fp);
    fwrite(&iMaxPrice,      sizeof(int),    1,  fp);
    fwrite(&iHoodType,      sizeof(int),    1,  fp);
    fwrite(&iSellingPrice,  sizeof(int),    1,  fp);
    fwrite(&nRearBumper,    sizeof(int),    1,  fp);
    fwrite(&nFrontBumper,   sizeof(int),    1,  fp);

    //
    // Parts
    //    
    for(i=0; i<4; i++)
        writePart(cTires[i], fp);
    writePart(cBlock, fp);
    for(i=0; i<2; i++)
        writePart(cMufflers[i], fp);
    for(i=0; i<4; i++)
        writePart(cShocks[i], fp);
    writePart(cDiff, fp);
    writePart(cTrans, fp);
}


///////////////////
// Load the car from a savegame file
bool CCar::loadSaveGame(FILE *fp)
{
    char buf[256];

    // Model car directory
    readPascalString(buf,fp);
    
    // Load the model
    if(!LoadModel(buf)) {
        writeLog("Failed to load model '%s'\n",buf);
        return false;
    }

    // Position in world
    fread(&vPos,            sizeof(CVec),   1,  fp);
    cMatrix.Load(fp);
	
    // Resting in the garage & restaurant
    fread(&fRestHeight,     sizeof(float),  1,  fp);
    fread(&cRestAngle,      sizeof(CVec),   1,  fp);

    // Body paint
    fread(&tColour,         sizeof(colour_t),1, fp);
    PaintCar(CVec(tColour.R, tColour.G, tColour.B), true);

    
    // Read the decals    
    int decals;
    fread(&decals,         sizeof(int),    1,  fp);
    for(int i=0; i<decals; i++) {
        if(!readDecal(fp))
            return false;
    }
    
    // Game details    	
    readPascalString(sName, fp);
    fread(&iMinPrice,       sizeof(int),    1,  fp);
    fread(&iMaxPrice,       sizeof(int),    1,  fp);
    fread(&iHoodType,       sizeof(int),    1,  fp);
    fread(&iSellingPrice,   sizeof(int),    1,  fp);
    fread(&nRearBumper,     sizeof(int),    1,  fp);
    fread(&nFrontBumper,    sizeof(int),    1,  fp);


    //
    // Hide any 'game' meshes
    //
    	
    // Make sure the hood hole is still hidden if need be
    setHoodType(iHoodType);
    
    // Make sure the bumpers are still invisible if need be
    if( !getFrontBumper() )
        removeBumper(0);
    if( !getRearBumper() )
        removeBumper(1);

    //
    // Parts
    //
    bool ok;
    for(i=0; i<4; i++) {
        cTires[i] = readPart(fp, &ok);
        if(!ok) return false;
        if(cTires[i])
            cTires[i]->setPartID(i);
    }

    cBlock = readPart(fp, &ok);
    if(!ok) return false;

    for(i=0; i<2; i++) {
        cMufflers[i] = readPart(fp, &ok);
        if(!ok) return false;
        if(cMufflers[i])
            cMufflers[i]->setPartID(i);
    }

    for(i=0; i<4; i++) {
        cShocks[i] = readPart(fp, &ok);
        if(!ok) return false;
        if(cShocks[i])
            cShocks[i]->setPartID(i);
    }

    cDiff = readPart(fp, &ok);
    if(!ok) return false;
    
    cTrans = readPart(fp, &ok);
    if(!ok) return false;

    return true;
}


///////////////////
// Write a decal
void CCar::writeDecal(sticker_t *d, FILE *fp)
{
    writePascalString(d->szFilename,fp);
    fwrite(&d->cColour,     sizeof(CVec),   1,  fp);

    // Polygons
    fwrite(&d->nNumPolygons,sizeof(int),    1,  fp);
    
    polygon_t *p = d->psPolygons;
    for(int i=0; i<d->nNumPolygons; i++, p++) {
        fwrite(p->nVertex,          sizeof(uint),   3,  fp);
        fwrite(p->nVertexNormal,    sizeof(uint),   3,  fp);
        fwrite(p->sDecalCoords,     sizeof(texcoord_t), 3,  fp);
        fwrite(&p->psMesh->nID,     sizeof(int),    1,  fp);
    }
}


///////////////////
// Read a decal
bool CCar::readDecal(FILE *fp)
{
    sticker_t *d = new sticker_t;
    if(!d)
        return false;

    d->psNext = NULL;

    readPascalString(d->szFilename, fp);
    d->psTexture = Cache_LoadTexture(d->szFilename);
    if(d->psTexture) {		
        d->psTexture->nWrap_S = GL_CLAMP;
        d->psTexture->nWrap_T = GL_CLAMP;
		d->psTexture->bMipFiltering = false;
        Tex_Upload(d->psTexture, true);
    }

    fread(&d->cColour,      sizeof(CVec),   1,  fp);

    fread(&d->nNumPolygons, sizeof(int),    1,  fp);
    d->psPolygons = new polygon_t[d->nNumPolygons];
    if(!d->psPolygons) {
        delete d;
        return false;
    }

    polygon_t *p = d->psPolygons;
    for(int i=0; i<d->nNumPolygons; i++, p++) {
        fread(p->nVertex,          sizeof(uint),   3,  fp);
        fread(p->nVertexNormal,    sizeof(uint),   3,  fp);
        fread(p->sDecalCoords,     sizeof(texcoord_t), 3,  fp);

        int id;
        fread(&id,                 sizeof(int),    1,  fp);

        p->psMesh = cModel->getMeshes()+id;
    }


    // Link the decal at the end    
    sticker_t *st = psStickers;
    if( psStickers ) {
        for(; st; st=st->psNext ) {
            if( st->psNext == NULL ) {
                st->psNext = d;
                break;
            }
        }
    } else
        psStickers = d;

    return true;
}


///////////////////
// Write out a part
void CCar::writePart(CPart *pcPart, FILE *fp)
{
    byte exists = 0;
    if(!pcPart) {
        fwrite(&exists,     sizeof(byte),   1,  fp);
        return;
    }

    exists = 1;
    fwrite(&exists,     sizeof(byte),   1,  fp);
    pcPart->write(fp);
}


///////////////////
// Read a part
CPart *CCar::readPart(FILE *fp, bool *bSuccess)
{
    byte exists = 0;
    fread(&exists,          sizeof(byte),   1,  fp);
    if(exists == 0) {
        *bSuccess = true;
        return NULL;
    }
    
    CPart *p = newPart();
    if(!p) {
        *bSuccess = false;
        return NULL;
    }

    p->read(fp);
    *bSuccess = true;
    return p;
}



///////////////////
// Shutdown the car
void CCar::Shutdown(void)
{
	if(cModel) {
		cModel->shutdown();
		delete cModel;
		cModel = NULL;
	}

	if(cBlock) {
		cBlock->Shutdown();
		delete cBlock;
		cBlock = NULL;
	}

    clearDecals();

}