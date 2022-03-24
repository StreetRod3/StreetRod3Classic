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


// Location class
// Created 13/5/02
// Jason Boettcher


#include "defs.h"
#include "game.h"
#include "System.h"
#include "font.h"


///////////////////
// Load the location
int CLocation::Load(char *sFilename)
{
	// Allocate the classes
	cScene = new CModel;
	if(cScene == NULL)
		return false;

	cCamera = new CCamera;
	if(cCamera == NULL)
		return false;


	// Load the scene
	if( !cScene->load(sFilename) ) {
		Shutdown();
		return false;
	}


	// Setup the camera
	iGrabbed = false;
	vLookAt = CVec(-0.05f, 2, 1.5f);
	fDistance = 5;
    bCameraMoving = false;

	// Setup the game items
	tItems = new gitem_t[1024];
	if(tItems == NULL)
		return false;

    fPartInfoAlpha = 0;
    bPartInfo = false;
    pcPartInfo = NULL;



	return true;
}


///////////////////
// Processes the location
void CLocation::Process(void)
{
	ProcessCamera();

    bPartInfo = false;

	if(iCheckItems)
		ProcessGameItems();
	else
		iGotStatusText = false;
}


///////////////////
// Start a camera movement
void CLocation::startCameraMove(CVec destlookat, CVec destpos)
{
	bCameraMoving = true;
	fCamTime = 0;

	vCamPos = cCamera->getPos();

	// Destination
	vCamDest = destpos;
	vCamLookDest = destlookat;

	// Pos
	vCamDest_dir = vCamDest - vCamPos;
	float length = VectorNormalize(&vCamDest_dir);
	//vCamDest_dir = vCamDest_dir * 50;

	// Look at
	vCamLookDest_dir = vCamLookDest - vLookAt;
	length = VectorNormalize(&vCamLookDest_dir);
	//vCamLookDest_dir = vCamLookDest_dir * 50;

	fDistance = GetDistance(destlookat, destpos);

    // Important
    dCamTime = (double)SDL_GetTicks() * 0.001;
}


///////////////////
// Process the camera movement
void CLocation::processCameraMove(float dt)
{
	fCamTime += dt;


	// TODO: Make sure the camera doesn't pass through the car (doesn't look good)



	// Camera Pos
	float camspeed = 40;
	float length = GetDistance( vCamPos, vCamDest);
	if(length < 5)
		camspeed = length*10;
	
	vCamPos = cCamera->getPos() + (vCamDest_dir*camspeed) * dt;

	
	// Camera Lookat pos
	camspeed = 30;
	float length2 = GetDistance( vLookAt, vCamLookDest);
	if(length2 < 5)
		camspeed = length2*10;

	vLookAt = vLookAt + (vCamLookDest_dir*camspeed) * dt;


    // If the camera is a really large distance away from the destination, just snap it
    if(length > 50 || length2 > 50) {
        vCamPos = vCamDest;
        vLookAt = vCamLookDest;
    }

	cCamera->setPos(vCamPos);


	if( length < 0.25f && length2 < 0.25f) {
		bCameraMoving = false;

		fDistance = GetDistance(vCamPos, vLookAt);	

		// Find yaw & pitch
		// Difference between the two positions
		CVec diff = vCamPos - vLookAt;

		// Yaw = x / y
		fcamYaw = (float)-atan2(diff.GetX(),diff.GetY()) * (180/PI)-90;

		// Pitch = dist / z
		float dist = (float)sqrt((float)(diff.GetX() * diff.GetX() + diff.GetY() * diff.GetY()));
		fcamPitch = (float)-atan2(dist,diff.GetZ()) * (180/PI)+90;
	}
}


///////////////////
// Process the camera
void CLocation::ProcessCamera(void)
{
	keyboard_t *kb = System_GetKeyboard();
	SDL_Surface *scr = SDL_GetVideoSurface();
	mouse_t *tMouse = System_GetMouse();
	
	CVec pos = cCamera->getPos();

	if(bCameraMoving) {

        double dFixed = 0.025f;
        double time = (double)SDL_GetTicks() * 0.001;
        double dt=0;

        // Fixed time-step simulation
        while(dCamTime < time) {
            dt = time - dCamTime;
            if(dt > dFixed)
                dt = dFixed;

            processCameraMove(dt);

            dCamTime += dt;
        }
        
        dCamTime = time;
		return;
	}


	// Right mouse button starts the move
	if(tMouse->Button & SDL_BUTTON(3) && !bCamLocked) {

		// First grab
		if(!iGrabbed) {
			iGrabbed = true;
			vGrabPos = tMouse->vPos;
			SDL_WarpMouse((int)vGrabPos.GetX(), (int)vGrabPos.GetY());
			return;
		}

        // Yaw
        float x = (vGrabPos - tMouse->vPos).GetX() / 10;
        x *= psOptions->fRotateSensitivity;
        if( psOptions->nInverseX )
            x = -x;

        // Pitch
        float y = (vGrabPos - tMouse->vPos).GetY() / 10;
        y *= psOptions->fRotateSensitivity;
        if( psOptions->nInverseY )
            y = -y;

		fcamYaw += x;
		fcamPitch += y;
		SDL_WarpMouse((int)vGrabPos.GetX(), (int)vGrabPos.GetY());
        
        // Change the mouse position so we don't get annoying jitters in the cursor
        tMouse->X = vGrabPos.GetX();
        tMouse->Y = vGrabPos.GetY();
	} else
		iGrabbed = false;


	// The keyboard arrows also can control the camera move accuratly
	if(!bCamLocked) {
		if(kb->keys[SDLK_RIGHT])	fcamYaw++;
		if(kb->keys[SDLK_LEFT])		fcamYaw--;
		if(kb->keys[SDLK_UP])		fcamPitch++;
		if(kb->keys[SDLK_DOWN])		fcamPitch--;
	}


	if(iAngleLimiting) {
		// Limit the yaw
		if(fMinYaw != -1)		fcamYaw = MAX(fcamYaw, fMinYaw);
		if(fMaxYaw != -1)		fcamYaw = MIN(fcamYaw, fMaxYaw);
		// Limit the pitch
		if(fMinPitch != -1)		fcamPitch = MAX(fcamPitch, fMinPitch);
		if(fMaxPitch != -1)		fcamPitch = MIN(fcamPitch, fMaxPitch);
	} else {
		// Free look is still limited in the pitch
		fcamPitch = MAX(fcamPitch,-89);
		fcamPitch = MIN(fcamPitch,89);
	}
	

	CVec forw;
	GetAngles(fcamPitch,0,fcamYaw,&forw,NULL,NULL);

	if(!bCamLocked)
		vCamPos = vLookAt - forw*fDistance;

	// Limit the camera so it doesn't poke through objects
	// TODO: Add as a limiting plane
	//vCamPos.SetY( MIN(vCamPos.GetY(), 21.0f) );
    //vCamPos.SetX( MAX(vCamPos.GetX(), -12.0f) );

	cCamera->LookAt(vLookAt);
	cCamera->setPos(vCamPos);
}


///////////////////
// Process the game items
void CLocation::ProcessGameItems(void)
{
	// Cast a line from camera to mouse point
	CVec point,dir;
	UnprojectMouse(&point);

	int carobjid = -1;

	gitem_t *item = NULL;
	float dist = 99999;
	float p[3];

    bPartInfo = false;

    // If rotating camera, don't do game items
    if(iGrabbed)
        return;

	dir = point - cCamera->getPos();
	VectorNormalize(&dir);
	iGotStatusText = false;

	gitem_t *g = tItems;
	for(int i=0;i<iNumItems;i++,g++) {

		if(!g->iUsed)
			continue;

		/// Screen rectangle
		if(g->iType == GMI_SCREENRECT) {
			mouse_t *m = System_GetMouse();
			if(m->X > g->ix && m->X < g->ix2) {
				if(m->Y > g->iy && m->Y < g->iy2) {
					item = g;
				}
			}
		}

		
		// Part intersect
		if(g->iType == GMI_PART) {
			if(g->cPart) {
				if(g->cPart->LineIntersect(cCamera->getPos(),dir)) {

                    for( int n=0; n<g->cPart->getCD()->getNumCollisions(); n++ ) {
                        g->cPart->getCD()->getCollisionPoint(n,p,false);

					    // Is this the closest part that has been intersected?
					    float d = GetDistance(cCamera->getPos(),CVec(p[0],p[1],p[2]));
					    if(d<dist) {
						    item = g;
						    dist = d;
					    }
                    }
				}
			}
		}

		// Car object intersect
		if(g->iType == GMI_CAROBJECT) {
			if(g->cCar) {
				if(g->cCar->LineIntersect(cCamera->getPos(),dir,true)) {

					int t1,t2;
					g->cCar->getModel()->getCDModel()->getCollidingTriangles(t1,t2);

					// Find the mesh based on triangle number
					int mesh = g->cCar->getModel()->findMesh(t1);
					if(mesh>=0) {
						if(mesh == g->iMeshNum) {
							g->cCar->getModel()->getCDModel()->getCollisionPoint(0,p,false);

							// Is this the closest part that has been intersected?
							float d = GetDistance(cCamera->getPos(),CVec(p[0],p[1],p[2]));
							if(d<dist) {
								item = g;
								dist = d;
							}
						}
					}
				}
			}
		}
	}


	// Set the text, and run the function
	if(item) {
		strcpy(sStatusText, item->sText);
		iGotStatusText = true;

        if(item->iType == GMI_PART) {
			item->cPart->setDrawSpecial(true);
            bPartInfo = true;
            pcPartInfo = item->cPart;
            fPartInfoAlpha = 1.0f;
        }
		if(item->iType == GMI_CAROBJECT)
			if(item->cCar)
				(item->cCar->getModel()->getMeshes() + item->iMeshNum)->nFlags |= MSH_SELECTED;

		// If the mouse was clicked, run the function
        if(item->Use && System_GetMouse()->Up & SDL_BUTTON(1)) {
            // Flush the mouse
            System_GetMouse()->Up = 0;
            System_GetMouse()->Down = 0;

			item->Use(item);
        }
	}
}


///////////////////
// Draws the location
void CLocation::Draw(bool drawModel, bool lighting, bool setupCam)
{
    if(setupCam) {
	    cCamera->LookAt(vLookAt);
	    cCamera->Show();
    }
	
	if(drawModel) {
        if( !lighting )
		    State_Disable(GL_LIGHTING);

		cScene->render();

        if( !lighting )
		    State_Enable(GL_LIGHTING);
	}
}


///////////////////
// Draw the final-draw stuff
void CLocation::FinalDraw(void)
{

    // Draw the part info
    if( bPartInfo || fPartInfoAlpha > 0 ) {        
        Font_SetAlpha(fPartInfoAlpha);

        drawPartInfo(pcPartInfo, fPartInfoAlpha);
        
        bPartInfo = false;
    }

    if( !bPartInfo ) {
        fPartInfoAlpha -= tMainSR3.fDeltaTime*2;
        fPartInfoAlpha = MAX(fPartInfoAlpha,0);
    }

    Font_SetAlpha(1);
	Font_SetSize(14);

	// Status bar
	State_Disable(GL_TEXTURE_2D);
    State_Disable(GL_LIGHTING);
	State_Enable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0,0,0,0.5f);

	// TODO: get size from viewport, or options

	glBegin(GL_QUADS);		// Note: Anti-clockwise winding so we don't get back-face culling
    int nHeight = 24;
	glVertex2i(800,nHeight);	glVertex2i(0,nHeight);
	glVertex2i(0,0);	        glVertex2i(800,0);
	glEnd();

	// Cash
	Font_Draw(6,4,CVec(0,0,0),"Bankroll: $%d",cMainGuy->getBankroll());
	Font_Draw(4,6,CVec(1,1,1),"Bankroll: $%d",cMainGuy->getBankroll());


	// Status text
	if(iGotStatusText) {
		int length = strlen(sStatusText)*10;
		length /= 2;

		int x = 400-length;

		Font_Draw(x+2,4,CVec(0,0,0),sStatusText);
		Font_Draw(x,6,CVec(1,1,1),sStatusText);
	}
}


///////////////////
// Draw the part info
void CLocation::drawPartInfo(CPart *pcPart, float fAlpha)
{
    if(!pcPart)
        return;
    
    Font_SetAlpha(fAlpha);
    Font_SetSize(14);
    DrawRect(200,580, 600,530, CVec(1,1,1), CVec(0,0,0), fAlpha/2, true, 0);
    DrawRect(200,580, 600,530, CVec(1,1,1), CVec(0,0,0), fAlpha/2, false, 1);
    Font_Draw(210,560, CVec(1,1,1), pcPart->getName());
    Font_Draw(210,540, CVec(1,1,1), "Damage: %d%%%%", pcPart->getDamage());        
    Font_SetAlpha(1);
}


///////////////////
// Clear the game item list
void CLocation::ClearGameItems(void)
{
	iNumItems = 0;
}


///////////////////
// Add a game item to the list
void CLocation::AddGameItem(gitem_t *g)
{
	tItems[iNumItems] = *g;
	tItems[iNumItems++].iUsed = true;
}

///////////////////
// Shutdown the location
void CLocation::Shutdown(void)
{
	if(cScene) {
		cScene->shutdown();

		delete cScene;
		cScene = NULL;
	}

	if(cCamera) {
		delete cCamera;
		cCamera = NULL;
	}

	if(tItems) {
		delete[] tItems;
		tItems = NULL;
	}
}
