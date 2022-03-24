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


// Part class
// Created 23/5/02
// Jason Boettcher


#include "defs.h"
#include "System.h"
#include "game.h"


///////////////////
// Load the part
int CPart::Load(char *sFName)
{
	char	buf[256];

    sr_strncpy(szModelDir, sFName, 127);
    
    // Add the part dir to the texture dir list
    sprintf(buf,"data/part_models/%s",sFName);
    Tex_AddDir(buf);
    
    sprintf(buf,"data/part_models/%s/part",sFName);

	cModel = Cache_LoadModel(buf);

    // Remove the part dir from the texture dir list
    sprintf(buf,"data/part_models/%s",sFName);
    Tex_RemoveDir(buf);

    if(cModel == NULL)        
		return false;

    // Load the nut
    cNut = Cache_LoadModel("data/misc/nut");

    // Get the model dimensions
    cDimensions = cModel->calculateDimensions();

	// Set nuts based from game objects in the model
	LoadNuts();

	iUsed = true;

	return true;
}


///////////////////
// Load a setup from a car file
int CPart::LoadSetup(char *sFilename, char *sSection)
{
	char	sFName[64];
	char	buf[256];

	// Part setup name
	if( !ReadString(sFilename, sSection, "Part",sFName,"") )
        return false;

	sprintf(buf, "data/parts/%s.sr3",sFName);

	// Load the part setup
	if(sName) {
		if(!LoadPartSetup(buf))
			return false;
	} else
		return false;

	// Other details
	ReadInteger(sFilename,sSection,"Damage",&iDamage,0);


	return true;
}


///////////////////
// Load the details from a part setup file
int CPart::LoadPartSetup(char *sFName)
{
	//char	buf[256];
	char	sModel[64];

	// Read the info
	ReadString(sFName,  "General",  "Model",        sModel,"");
	ReadString(sFName,  "General",  "Name",         sName,"bad_part");
	ReadKeyword(sFName, "General",  "Type",         &iType,PRT_BLOCK);
    ReadKeyword(sFName, "General",  "Manufacturer", &iManufacturer, MAN_ANY);

	ReadInteger(sFName, "Price",    "MinPrice",     &iminPrice, 0);
	ReadInteger(sFName, "Price",    "MaxPrice",     &imaxPrice, 0);

    /*
        Part specific stuff
    */

    // Shock absorber
    if( iType == PRT_SHOCK ) {        
        ReadFloat(sFName, "Performance", "Spring", &fSpring, 400);
        ReadFloat(sFName, "Performance", "Dampening", &fDampening, 15);
        ReadFloat(sFName, "Performance", "RestLength", &fRestLength, 2);
    }

    // Wheels
    if( iType == PRT_TIRE ) {        
        ReadFloat(sFName, "Performance", "Cornering", &fCornering, 1);
    }

    // Block
    if( iType == PRT_BLOCK ) {
        ReadKeyword(sFName, "General", "Size", &iSize, SZ_MEDIUM);
    }

    // Manifold
    if( iType == PRT_INTAKEMAN ) {
        ReadKeyword(sFName, "General", "Size", &iSize, SZ_MEDIUM);
        ReadInteger(sFName, "General", "Barrels", &iBarrels, 2);
        ReadInteger(sFName, "General", "Carbys", &iCarbys, 1);
    }

    // Carby
    if( iType == PRT_CARBY ) {
        ReadInteger(sFName, "General", "Barrels", &iBarrels, 2);
    }

	// Transmission
	if( iType == PRT_TRANS ) {
		ReadKeyword(sFName, "Performance", "Automatic", &bAutomatic, false);
		ReadInteger(sFName, "Performance", "NumGears", &iNumGears, 3);
		for( int i = 1; i <= iNumGears; i++ ) {
			char szRatioX[6];
			int n = sprintf(szRatioX, "Ratio%d", i);
			ReadFloat(sFName, "Performance", szRatioX, &fGearRatio[i], 1.0);
		}
	}


    // Air-Filter
    if( iType == PRT_AIRFILTER ) {
        ReadKeyword(sFName, "General", "ThroughHood", &iThroughHood, false);
        ReadInteger(sFName, "General", "Carbys", &iCarbys, 1);
    }

	
	if(!Load(sModel))
		return false;
	

	
	return true;
}


///////////////////
// Setup from a game object
void CPart::Setup(CMatrix carmat, CVec carpos, gobject_t *g)
{
	iEngineRunning = false;

	tObjDetails = *g;

	cMatrix = g->cMatrix;

	vPos = carmat.MultiplyVector(g->vPosition) + carpos;
	cMatrix = carmat * cMatrix;

	// Setup the nuts
	for(int n=0;n<iNumNuts;n++)
		cNuts[n].Setup(cNuts[n].getOrigPos(),cNuts[n].getOrigMat(),cMatrix,vPos);
}


///////////////////
// Set the position of the part
void CPart::setPosition(CMatrix carmat, CVec carpos)
{
	Setup(carmat, carpos, &tObjDetails);

	// Setup the attached parts
	for(int p=0;p<MAX_BLOCK_PARTS;p++) {
		if(cAttachedParts[p]) {			
			cAttachedParts[p]->Setup( carmat, vPos, cAttachedParts[p]->getGameObject() );
		}
	}

	// Special parts
	CPart *man = FindPart(PRT_INTAKEMAN,-1);
    if( man ) {

        // Setup all the carby's
        for( int i=0; i<man->getNumCarbys(); i++ ) {
	        CPart *carby = FindPart(PRT_CARBY,i);
	        if(carby)
		        carby->Setup( man->getMatrix(), man->getPos(), carby->getGameObject());
        }
    }
    
	CPart *filter = FindPart(PRT_AIRFILTER,-1);
    CPart *carb1 = FindPart(PRT_CARBY,0);
	if(filter && carb1)
		filter->Setup( carb1->getMatrix(), carb1->getPos(), filter->getGameObject());

}


///////////////////
// Load the block's attached parts
int CPart::LoadBlockParts(char *sSetupFile)
{
	CPart *p;
    char    *sCarbList[] = {"Carb1", "Carb2", "Carb3"};
    char    *sCarbObjList[] = {"$carb1", "$carb2", "$carb3"};

	LoadAttached(sSetupFile,"Intake",    cModel->findGameObject("$intake"), 0);
	LoadAttached(sSetupFile,"Fan",       cModel->findGameObject("$fan"), 0);
	LoadAttached(sSetupFile,"Alternator",cModel->findGameObject("$alt"), 0);
	LoadAttached(sSetupFile,"Starter",   cModel->findGameObject("$starter"), 0);
	
	// Carby's
	CPart *intake = FindPart(PRT_INTAKEMAN,-1);
    if(intake) {

        for( int i=0; i<intake->getNumCarbys(); i++ ) {
       		LoadAttached(sSetupFile, sCarbList[i], intake->getModel()->findGameObject(sCarbObjList[i]), i);
	        CPart *carby = FindPart(PRT_CARBY,i);
	        if(carby)
		        carby->Setup( intake->getMatrix(), intake->getPos(), intake->getModel()->findGameObject(sCarbObjList[i]));
        }
    }

	// Airfilter
	p = FindPart(PRT_CARBY,0);
    if(p) {
		LoadAttached(sSetupFile,"Airfilter", getAirFilterGObject(),0);

	    CPart *air = FindPart(PRT_AIRFILTER,-1);
	    if(air)
		    air->Setup( p->getMatrix(), p->getPos(), getAirFilterGObject());
    }
	
	return true;
}


///////////////////
// Load an attached part
int CPart::LoadAttached(char *sSetupFile, char *sName, gobject_t *g, int id)
{
	int	p =-1;
	
	// Find a free slot for the part
	for(int i=0;i<MAX_BLOCK_PARTS;i++) {
		if(cAttachedParts[i] == NULL) {
			p = i;
			break;
		}
	}

	if(p == -1) {
		// No free slots
		return false;
	}

	CPart *part;
	cAttachedParts[p] = part = newPart();
	if(part == NULL) {
		// Out of memory
		return false;
	}

	part->setParent(this);
    part->setPartID(id);

	if(!part->LoadSetup(sSetupFile,sName)) {
		
		// Get rid of the part properly
		part->Shutdown();
		deletePart(part);
		cAttachedParts[p] = NULL;
		return false;
	}
	
	if(g)
		part->Setup(cMatrix,vPos,g);	
	else {

		// Get rid of the part properly
		part->Shutdown();
		deletePart(part);
		cAttachedParts[p] = NULL;
		return false;
	}


	return true;
}


///////////////////
// Find an attached part via part type
CPart *CPart::FindPart(int type, int id)
{
	for(int p=0;p<MAX_BLOCK_PARTS;p++) {
		if(cAttachedParts[p]) {
            if(cAttachedParts[p]->getType() == type) {
                if( id != -1 ) {
                    if( cAttachedParts[p]->getPartID() == id )
				        return cAttachedParts[p];
                } else
                    return cAttachedParts[p];
            }
		}
	}

	return NULL;
}


///////////////////
// Allocate the attached parts
/*int CPart::AllocateAttached(void)
{
	// Intake
	cInManifold = new CPart;
	if(cInManifold == NULL)
		return false;

	cInManifold->setParent(this);

	// Fan
	cFan = new CPart;
	if(cFan == NULL)
		return false;

	cFan->setParent(this);
	

	return true;
}


///////////////////
// Setup the attached parts
void CPart::SetupAttached(void)
{
	// Intake
	gobject_t *g = cModel->FindGameObject("$intake");
	if(g) {
		if(cInManifold)
			cInManifold->Setup(cMatrix,vPos,g);
	}

	// Fan
	g = cModel->FindGameObject("$fan");
	if(g) {
		if(cFan)
			cFan->Setup(cMatrix,vPos,g);
	}
}*/


///////////////////
// Removed any attached parts
void CPart::removeAttached(CPart *part)
{
    assert(part);

	for(int p=0;p<MAX_BLOCK_PARTS;p++) {
		if(cAttachedParts[p]) {
            if(cAttachedParts[p]->getUniqueID() == part->getUniqueID()) {
				cAttachedParts[p] = NULL;
                return;
            }
		}
	}

    printf("Error: Part %s not removed from parent\n",part->getName());
}


///////////////////
// Load the nuts from the model
void CPart::LoadNuts(void)
{
	iNumNuts = 0;
	int n;
	gobject_t *g = cModel->getGameObject(0);
	
	// Get the nut count
	for(n=0;n<cModel->getNumGameObjects();n++,g++) {
		if(strnicmp(g->sName,"$bolt",5) == 0)
			iNumNuts++;
	}

	// Allocate the nuts
	cNuts = new CNut[iNumNuts];
	if(cNuts == NULL) {
		iNumNuts = 0;
		return;
	}

	// setup the nuts
	g = cModel->getGameObject(0);
	int i=0;
	for(n=0;n<cModel->getNumGameObjects();n++,g++) {
		if(strnicmp(g->sName,"$bolt",5) == 0)  {
			cNuts[i].setOrigPos(g->vPosition);
			cNuts[i++].setOrigMat(g->cMatrix);
		}
	}
}


///////////////////
// Draw the part
void CPart::Draw(void)
{
	float	mat[16];

	if(iDrawHidden)
		return;

	glPushMatrix();

	// Multiply our matrix by the model view matrix
	cMatrix.CreateGLMatrix(vPos,mat);
	glMultMatrixf(mat);

	if(fSpin != 0.0f)
		glRotatef(fSpin,0,1,0);	

    if(iDrawSpecial) {
		cModel->addMeshFlag(MSH_SELECTED);
        iDrawSpecial = false;
    }
        
	cModel->render();

	glPopMatrix();

	if(iDrawNuts)
		DrawNuts();
}


///////////////////
// Draw the attached parts (if this is a block)
void CPart::DrawAttached(void)
{
	if(iDrawHidden)
		return;

	for(int p=0;p<MAX_BLOCK_PARTS;p++) {
		if(cAttachedParts[p])
			cAttachedParts[p]->Draw();
	}
}


///////////////////
// Draw the nuts on the parts
void CPart::DrawNuts(void)
{
	for(int n=0;n<iNumNuts;n++)
		cNuts[n].Draw();
}


///////////////////
// Clear the nuts (set them to un-done)
void CPart::ClearNuts(void)
{
	for(int n=0;n<iNumNuts;n++)
		cNuts[n].setIn(false);
}


///////////////////
// Put all the nuts onto the part
void CPart::putNutsOn(void)
{
    for(int n=0;n<iNumNuts;n++)
		cNuts[n].setIn(true);
}


///////////////////
// Add all the attached parts to the engine screen item list
void CPart::AddAttached(gitem_t *item, CLocation *loc)
{
	for(int p=0;p<MAX_BLOCK_PARTS;p++) {
		if(cAttachedParts[p]) {
			item->cPart = cAttachedParts[p];
			loc->AddGameItem(item);
		}
	}
}


///////////////////
// Add a part onto this part (assuming this == block/engine)
// Msg should be set to an error string if this function fails
int CPart::AddPart(CPart *part, char *msg)
{
	char    *sCarbs[] = {"$carb1", "$carb2", "$carb3"};
    int i;

	msg[0] = '\0';

	// Safety check
	if(part == NULL)
		return false;


	// First check that the part type is ok
	switch(part->getType()) {
		case PRT_BLOCK:
		case PRT_ENGINE:
		case PRT_TIRE:
		case PRT_DIFF:
		case PRT_TRANS:
		case PRT_LMUFFLER:
		case PRT_RMUFFLER:
			// Can't do that
			return false;
	}

    // Special case
    if( part->getType() == PRT_CARBY ) {
        // Set the carby's id to -1 until we find a spot for it
        part->setPartID(-1);
    }

	// Make sure that we don't already have a part of this type attached
    if(FindAttached(part->getType())) {

        // If this is a carby, quit only if ALL carby spots are taken
        if( part->getType() == PRT_CARBY ) {

            // Get the intake manifold to find how many carby spots we have
            CPart *intake = FindPart(PRT_INTAKEMAN,-1);
            if( !intake )
                return false;

            int spots = 0;
            for( i=0; i<intake->getNumCarbys(); i++ ) {

                if( FindPart(PRT_CARBY,i) )
                    spots++;
            }

            // If all spots are used, don't add the part
            if( spots == intake->getNumCarbys()) {
                strcpy(msg,"No room for the part");
                return false;
            }

        } else {
            strcpy(msg,"No room for the part");
    		return false;
        }
    }

	// Add the part onto the engine, checking that dependencies are correct
	// (eg, carby needs a intake manifold)
	if(!AddOnto(part,msg))
		return false;
	
    
    // Setup the part	
	gobject_t *g = NULL;
	CPart *par = NULL;
	switch(part->getType()) {
				
		case PRT_INTAKEMAN:
            g = cModel->findGameObject( "$intake" );
            break;
		case PRT_FAN:
            g = cModel->findGameObject( "$fan" );
            break;
		case PRT_STARTER:
            g = cModel->findGameObject( "$starter" );
            break;
		case PRT_ALTER:
			g = cModel->findGameObject( "$alt" );
			break;


		// Carbys are done via intake->$carby
		case PRT_CARBY:
			par = FindPart(PRT_INTAKEMAN,-1);
            assert( par );

            // Check which carby position is free
            if(par) {

                for( i=0; i<par->getNumCarbys(); i++ ) {
                    if( !FindPart(PRT_CARBY, i) ) {
                        g = par->getModel()->findGameObject( sCarbs[i] );
                        part->setPartID(i);
                        break;
                    }
                }
            }
			break;

		// Air filters are done via carby->$airfilter
		case PRT_AIRFILTER:
            // Air filters are attached to the first carby
            // Check object name depending on air filter type (eg, dualcarbfilter, triplecarbfilter)
            par = FindPart(PRT_CARBY,0);
			g = getAirFilterGObject();
			break;
	}
	
	// The game object must exist to finish the process
	if(g) {
        part->setParent(this);
		part->Setup(cMatrix, vPos, g);

		if(par)
			part->Setup(par->getMatrix(), par->getPos(), g);

		// Set the nuts to un-done
		part->ClearNuts();
		return true;

	} else {
        writeLog("Modeller Warning: Could not find an '$' object in the engine section\n");
		// An error occured after putting on the part, so we gotta take of the part again
		// This is so this part, and the previous owner (player) don't both free the part
		
		// TODO: Use a part pool for allocations/freeing
	}


	return false;
}


///////////////////
// Check if a part of a certain type is attached to this part
int CPart::FindAttached(int type)
{
	int carbycount = 0;
	for(int p=0;p<MAX_BLOCK_PARTS;p++) {
        if(cAttachedParts[p]) {
			if(cAttachedParts[p]->getType() == type)
				return true;
		}
	}

	return false;
}


///////////////////
// Add a part onto this part
int CPart::AddOnto(CPart *part, char *msg)
{
    CPart *man = NULL;
    CPart *block = NULL;

    // Check that the manufacturers match up
    if( !checkManufacturer( part ) ) {
        strcpy(msg, "It doesn't fit");
        return false;
    }

	// Check if this part has the right dependent part
	switch(part->getType()) {

		// A carby needs an intake manifold
		case PRT_CARBY:
			if(!FindAttached(PRT_INTAKEMAN))
				return false;

            // The carby needs to be the same barrel number as the manifold
            man = FindPart(PRT_INTAKEMAN,-1);
            if( !man )
                return false;
            if( man->getNumBarrels() != part->getNumBarrels() ) {
                strcpy(msg, "It doesn't fit");
                return false;
            }
			break;

        // The intake intake manifold needs to be the same size as the engine
        case PRT_INTAKEMAN:
            if( getSize() != part->getSize() ) {
                strcpy(msg, "It doesn't fit");
                return false;
            }
            break;


		// Air filter needs all carby's on
		case PRT_AIRFILTER:
            // An air filter requires that all the carby spots on the intake manifold are used
            man = FindPart(PRT_INTAKEMAN,-1);
            if( !man )
                return false;
            for( int i=0; i<man->getNumCarbys(); i++ ) {
                if( !FindPart(PRT_CARBY,i) )
                    return false;
            }

            // The air filter must also have the right number of carby holes
            if( part->getNumCarbys() != man->getNumCarbys() ) {
                strcpy(msg, "Wrong air filter");
                return false;
            }
			break;
	}

    // All good
  
    // Find a free slot for the part
	for(int i=0;i<MAX_BLOCK_PARTS;i++) {
		if(cAttachedParts[i] == NULL) {

			// Set the part
			cAttachedParts[i] = part;
			return true;
		}
	}

	return false;
}


///////////////////
// Returns a gameobject structure for the airfilter
gobject_t *CPart::getAirFilterGObject(void)
{
    // Assumes that this part is a block/engine
    gobject_t *g = NULL;
    char *sObjList[] = {"$1carbfilt", "$2carbfilt", "$3carbfilt"};

    // We get the number of carbs on the intake, and use the correct objectname depending that value
    CPart *intake = FindPart(PRT_INTAKEMAN,-1);
    if( !intake )
        return NULL;

    int num = intake->getNumCarbys();

    if( num < 1 || num > 3 )
        return NULL;

    // We always use the first carby to get the object position
    CPart *carb = FindPart(PRT_CARBY,0);
    if( !carb )
        return NULL;


    return carb->getModel()->findGameObject( sObjList[num-1] );
}


///////////////////
// Returns whether a line intersected the part
int CPart::LineIntersect(CVec p1, CVec dir)
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


	return cd->rayCollision(p,d,true);
}


///////////////////
// Returns the index of the nut a line has intersected with
// Or -1 for no intersection
int CPart::LineIntersectNut(CVec p1, CVec dir)
{
    if(!cNut)
        return -1;

    float p[3];
	CollisionModel3D *cd = cNut->getCDModel();

	int nut=-1;
	float dist = 9999;
		

	for(int n=0;n<iNumNuts;n++) {

		if(cNuts[n].LineIntersect(p1,dir)) {
			cNut->getCDModel()->getCollisionPoint(0,p,false);

			// Is this the closest nut that has been intersected?
			float d = GetDistance(p1,CVec(p[0],p[1],p[2]));
			if(d<dist) {
				nut = n;
				dist = d;
			}
		}
	}

	return nut;
}


///////////////////
// Returns true if all the nuts on the part are not in
int CPart::AllNutsGone(void)
{
	for(int n=0;n<iNumNuts;n++)
		if(cNuts[n].getIn())
			return false;

	return true;
}


///////////////////
// Returns true if all the nuts on the part are on
int CPart::AllNutsOn(void)
{
	for(int n=0;n<iNumNuts;n++)
		if(!cNuts[n].getIn())
			return false;

	return true;
}


///////////////////
// Get a string description of the part
char *CPart::getString(char *sBuf)
{
    sprintf(sBuf,sName);

    if(iType != PRT_BLOCK)
        return sBuf;


	// If we are a block with attached parts, we become an engine
    // Check for any attached parts
    int i, count=0;
    for(i=0; i<MAX_BLOCK_PARTS;i++) {
        if(cAttachedParts[i])
            count++;
    }

    if(count > 0)
	    sprintf(sBuf,"%s w/parts", sName);
    

	return sBuf;
}


///////////////////
// Returns true if this and another parts matches in manufacturer
bool CPart::checkManufacturer(CPart *prt)
{
    assert(prt);

    if( prt->getManufacturer() == MAN_ANY || getManufacturer() == MAN_ANY )
        return true;

    return (prt->getManufacturer() == getManufacturer());
}


///////////////////
// If this is a block, start the engine
void CPart::StartEngine(void)
{
	fSpin = 0;
	fShake = 0;
	iShakeDir = 0;

	iEngineRunning = true;
}


///////////////////
// Switch of the engine
void CPart::SwitchEngineOff(void)
{
	iEngineRunning = false;
	
	CPart *fan = FindPart(PRT_FAN,-1);
	if(fan)
		fan->setSpin(0);
}


///////////////////
// Simulate the engine, by shaking it
void CPart::SimulateEngine(void)
{
	CPart *fan = FindPart(PRT_FAN,-1);

	// Shake the engine
	if(iShakeDir) {
		fShake += tMainSR3.fDeltaTime*10;
		if(fShake>1)
			iShakeDir = 0;
	} else {
		fShake -= tMainSR3.fDeltaTime*10;
		if(fShake<0)
			iShakeDir = 1;
	}

	// Spin the fan
	if(fan)
		fan->SpinFan();
}


///////////////////
// Spin the fan
void CPart::SpinFan(void)
{
	fSpin += tMainSR3.fDeltaTime*800.0f;
	if(fSpin>360)
		fSpin-=360;
}


///////////////////
// Spin the tyre
void CPart::SpinTyre(float dir)
{
	fSpin -= tMainSR3.fDeltaTime*200.0f * dir;
	if(fSpin<0)
		fSpin+=360;
	if(fSpin>360)
		fSpin-=360;
}


///////////////////
// Calculate the selling price of this part
int CPart::CalculateSelling(float fRandom)
{
	int price = iminPrice;
	float diff = (float)(imaxPrice - iminPrice);

	// Take 30% off because it's used
	float discount = 30.0f;

	// TODO: Add more discount depending on damage

	// Add a bit of randomness to the discount
	discount += GetRandomNum() * fRandom;

	discount /= 100.0f;

	// Calculate the final price
	price = imaxPrice - (int)(diff * discount);

	// Calculate attached parts, like on a whole engine
	if(iType == PRT_BLOCK) {
		for(int p=0;p<MAX_BLOCK_PARTS;p++) {
			if(cAttachedParts[p])
				price += cAttachedParts[p]->CalculateSelling(fRandom);
		}
	}

	return price;
}


///////////////////
// Calculate the cost of repairing the part
int CPart::repairCost(void)
{
    return imaxPrice * ((float)iDamage/100);
}


///////////////////
// Repair the part
// Returns the cost of repairing the part
int CPart::repair(void)
{
    int cost = repairCost();

    // Clear the damage
    iDamage = 0;

    return cost;
}


///////////////////
// Checks if this part is complete
// Returns true if is is complete
bool CPart::isComplete(void)
{
    CPart *p = NULL;

    if(iType != PRT_BLOCK)
        return false;
    
    // The block must have a starter, alternator, fan, airfilter, and a 'complete' intake manifold
    if(!FindAttached(PRT_STARTER))
        return false;
    if(!FindAttached(PRT_ALTER))
        return false;
    if(!FindAttached(PRT_FAN))
        return false;
    if(!FindAttached(PRT_AIRFILTER))
        return false;
    p = FindPart(PRT_INTAKEMAN,-1);
    if(!p)
        return false;
    
    // Check that we have all the carby's
    for(int i=0; i<p->getNumCarbys(); i++) {
        CPart *carby = FindPart(PRT_CARBY,i);
        if(!carby)
            return false;
    }

    // The block is complete
    return true;
}


///////////////////
// Returns true if the part or attached parts need a hole
bool CPart::requiresHoodHole(void)
{
    if(iThroughHood)
        return true;

    // Go through the attached parts
    if(iType != PRT_BLOCK)
        return false;

    for(int i=0; i<MAX_BLOCK_PARTS; i++) {
        if(cAttachedParts[i]) {
            if(cAttachedParts[i]->requiresHoodHole())
                return true;
        }
    }

    return false;
}


///////////////////
// Write out the part for a savegame
void CPart::write(FILE *fp)
{
    int i;

    // 3D Stuff
    writePascalString(szModelDir, fp);
    fwrite(&vPos,           sizeof(CVec),       1,  fp);    
    cMatrix.Save(fp);
    fwrite(&tObjDetails,    sizeof(gobject_t),  1,  fp);
    
    // Game stuff
    fwrite(&iPartID,        sizeof(int),        1,  fp);

    fwrite(&iManufacturer,  sizeof(int),        1,  fp);
    writePascalString(sName, fp);
    writePascalString(sObjName, fp);
    fwrite(&iType,          sizeof(int),        1,  fp);
    fwrite(&iUsed,          sizeof(int),        1,  fp);
    fwrite(&iSize,          sizeof(int),        1,  fp);
    fwrite(&iCarbys,        sizeof(int),        1,  fp);
    fwrite(&iBarrels,       sizeof(int),        1,  fp);
    fwrite(&iThroughHood,   sizeof(int),        1,  fp);

    fwrite(&iminPrice,      sizeof(int),        1,  fp);
    fwrite(&imaxPrice,      sizeof(int),        1,  fp);
    fwrite(&iDamage,        sizeof(int),        1,  fp);
    fwrite(&iTuning,        sizeof(int),        1,  fp);
    fwrite(&fSpring,        sizeof(float),      1,  fp);
    fwrite(&fDampening,     sizeof(float),      1,  fp);
    fwrite(&fRestLength,    sizeof(float),      1,  fp);
    fwrite(&fCornering,     sizeof(float),      1,  fp);


    // Does this part have any attached parts?
    int count = 0;
    for(i=0; i<MAX_BLOCK_PARTS; i++) {
        if(cAttachedParts[i])
            count++;
    }

    fwrite(&count,          sizeof(int),        1,  fp);
    for(i=0; i<MAX_BLOCK_PARTS; i++) {
        if(cAttachedParts[i])
            cAttachedParts[i]->write(fp);
    }
}


///////////////////
// Load a part
void CPart::read(FILE *fp)
{
    char buf[256];
    int i;

    // Load the model
    readPascalString(buf, fp);
    Load(buf);
    
    // 3D stuff
    fread(&vPos,            sizeof(CVec),       1,  fp);
    cMatrix.Load(fp);
    fread(&tObjDetails,     sizeof(gobject_t),  1,  fp);

    // Setup the nuts
	for(i=0; i<iNumNuts; i++)
		cNuts[i].Setup(cNuts[i].getOrigPos(),cNuts[i].getOrigMat(),cMatrix,vPos);
    
    // Game stuff
    fread(&iPartID,         sizeof(int),        1,  fp);

    fread(&iManufacturer,   sizeof(int),        1,  fp);
    readPascalString(sName, fp);
    readPascalString(sObjName, fp);
    fread(&iType,           sizeof(int),        1,  fp);
    fread(&iUsed,           sizeof(int),        1,  fp);
    fread(&iSize,           sizeof(int),        1,  fp);
    fread(&iCarbys,         sizeof(int),        1,  fp);
    fread(&iBarrels,        sizeof(int),        1,  fp);
    fread(&iThroughHood,    sizeof(int),        1,  fp);

    fread(&iminPrice,       sizeof(int),        1,  fp);
    fread(&imaxPrice,       sizeof(int),        1,  fp);
    fread(&iDamage,         sizeof(int),        1,  fp);
    fread(&iTuning,         sizeof(int),        1,  fp);
    fread(&fSpring,         sizeof(float),      1,  fp);
    fread(&fDampening,      sizeof(float),      1,  fp);
    fread(&fRestLength,     sizeof(float),      1,  fp);
    fread(&fCornering,      sizeof(float),      1,  fp);


    // Does this part have any attached parts?
    int count = 0;
    fread(&count,          sizeof(int),        1,  fp);

    for(i=0; i<count; i++) {
        cAttachedParts[i] = newPart();
        cAttachedParts[i]->setParent(this);
        cAttachedParts[i]->read(fp);
    }
}



///////////////////
// Shutdown the part
void CPart::Shutdown(void)
{
	if(cNuts) {
		delete[] cNuts;
		cNuts = NULL;
	}

	// Attached parts
	for(int p=0;p<MAX_BLOCK_PARTS;p++) {
		if(cAttachedParts[p]) {
			cAttachedParts[p]->Shutdown();
			delete cAttachedParts[p];
			cAttachedParts[p] = NULL;
		}
	}


}