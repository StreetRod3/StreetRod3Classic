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


// Main racing file
// Created 22/8/02
// Jason Boettcher


#include "defs.h"
#include "System.h"
#include "game.h"
#include "racing.h"
#include "font.h"
#include "CSky.h"
#include "gui.h"
#include "Entity.h"


// Racing structure
race_t      *psRace = NULL;

texture_t	*tLight;
texture_t   *tTacho;
texture_t   *tSpeedo;
texture_t   *tNeedle;
texture_t   *tFlare;

CVec Tire = CVec(0,0,20);
CPart *Wheel;
float LastTime;


// Racing logic
int         nRaceMode = race_red;
float       fStartRace = 0;
int         nRaceWinner = -1;
bool        bProcessRace = true;
bool        bRacingMenu = false;



///////////////////
// Main racing routine
int Race_Start(char *track)
{
	keyboard_t	*kb = System_GetKeyboard();	

    // Load the race stuff
    if(!Race_Load(track))
        return false;


	// Fog
    State_Enable(GL_FOG);
	glFogf(GL_FOG_START, 800.0f);
	glFogf(GL_FOG_END, 1000.0f);

	State_Enable(GL_LIGHTING);
    State_Enable(GL_LIGHT0);
    State_Enable(GL_LIGHT1);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.00f);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.50f);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.10f);


  
    
	psRace->cCamera.Setup(CVec(-50,20, 10), CVec(0,0,0));

	// Load the car
    psRace->pcRaceCar = cMainGuy->getCurCar();
    assert( psRace->pcRaceCar );
    psRace->pcRaceCar->SetupSimulation();	// Find Bounding Box for Collision detection - User's Car

    // Setup the opponent
	//assert(sRaceInfo.pcOpponent);
	psRace->pcOppCar = cMainGuy->getOpponentList()->getCar();
    psRace->pcOppCar = sRaceInfo.pcOpponent->getCar();
	psRace->pcOppCar->SetupSimulation();	// Find Bounding Box for Collision detection - Opponent's Car


    // Car positions
    gobject_t *g = psRace->pcTrackModel->findGameObject("$maincar");
    if(g)
        psRace->pcRaceCar->getCarSim()->cPos = g->vPosition;
    g = psRace->pcTrackModel->findGameObject("$oppcar");
    if(g)
        psRace->pcOppCar->getCarSim()->cPos = g->vPosition;
    
    psRace->pcRaceCar->getCarSim()->bFinished = false;
    psRace->pcOppCar->getCarSim()->bFinished = false;


    gobject_t *RedLight = psRace->pcTrackModel->findGameObject("$redlight");
    gobject_t *GreenLight = psRace->pcTrackModel->findGameObject("$greenlight");


    // Racing logic
    fStartRace = 0;
    nRaceMode = race_red;
    bProcessRace = true;
    bRacingMenu = false;

    // Hack
    // nRaceMode = race_racing;


    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    
    float pos[4] = {0,-1000,1000,1};
    float amb[4] = {0.4,0.4,0.4,1};
    float diff[4] = {0.5,0.5,0.5,1};

	//check cInCarCamera variable here

	System_ProcessInput();
	while( bProcessRace ) {
		System_ProcessInput();

        if( !bRacingMenu )
            fStartRace += tMainSR3.fDeltaTime;

		// Global stuff
		cMainGuy->processFrame();

        // Racing logic
        if( fStartRace > 2.5f && nRaceMode == race_red ) {
            // Start the race
            nRaceMode = race_racing;
        }

        // Driving menu
        if( kb->KeyUp[SDLK_ESCAPE] ) {

            if( !psRace->pcRaceCar->getCarSim()->bFinished && !bRacingMenu ) {
                // In-race menu
                bRacingMenu = true;
                Race_InitMenu();
            } else if( psRace->pcRaceCar->getCarSim()->bFinished ) {
                // Go back to the diner
                break;
            }
        }

        if( kb->KeyUp[SDLK_SPACE] ) {
            if( psRace->pcRaceCar->getCarSim()->bFinished ) {
                // Go back to the diner
                break;
            }
        }

        tMainSR3.TriCount = 0;

        Race_ProcessDriving();

        psRace->cSky.Simulate( tMainSR3.fDeltaTime );

        psRace->cLightMan.calculateLights( psRace->pcRaceCar->getCarSim()->cPos );
        psRace->cLightMan.useLights();

        // Setup the ambient light
        glLightfv(GL_LIGHT0, GL_POSITION, pos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
        
        CCamera temp;
        temp.Setup( psRace->pcRaceCar->getCarSim()->cPos+CVec(0,0,800), psRace->pcRaceCar->getCarSim()->cPos);

		// Rendering
		SetupFrame();
		psRace->cCamera.Show();
        //temp.Show();

        psRace->cSky.Render(&psRace->cCamera);

        State_Enable(GL_LIGHTING);

        State_Disable(GL_LIGHT1);
        State_Enable(GL_FOG);

        // Process the quadtree
        if(psOptions->nQuadTree)
            psRace->cQuadTree.process(&psRace->cCamera);

		psRace->pcTrackModel->render();        

        psRace->cSceneScript.draw(&psRace->cCamera);

        // Entities
        SimulateEntities(tMainSR3.fDeltaTime);
        DrawEntities();

		State_Enable(GL_LIGHTING);
        glEnable(GL_LIGHT1);

        State_Enable(GL_FOG);
        psRace->pcRaceCar->DrawDriving(&psRace->cCamera);
        psRace->pcOppCar->DrawDriving(&psRace->cCamera);


        // Traffic Lights
        State_Disable(GL_LIGHTING);
        State_Enable(GL_TEXTURE_2D);
        State_Enable(GL_BLEND);
        glBlendFunc( GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
        State_Disable(GL_DEPTH_TEST);

        // Red Light
        if( RedLight && nRaceMode == race_red)  {
            glColor4f(1,0,0,1);
            RenderBillboardSprite(RedLight->vPosition, tFlare, &psRace->cCamera, 10, 0);
        }
        
        // Green Light
        if( GreenLight && nRaceMode == race_racing)  {
            glColor4f(0,1,0,1);
            RenderBillboardSprite(GreenLight->vPosition, tFlare, &psRace->cCamera, 10, 0);
        }

        State_Enable(GL_DEPTH_TEST);


		

		// 2D stuff
		Render_Begin2D();

		// Debug drawing
		Font_SetSize(10);
		//Font_Draw(0,570,CVec(1,1,1),"%f",cRaceCar->getCarSim()->fRaceTime);
		//Font_Draw(0,550,CVec(1,1,1),"%f",tMainSR3.f1);

        
        // End race stuff
        if( nRaceMode == race_finished ) {

            if( nRaceWinner == 0 ) {
                // I Won
                Font_SetSize(20);
                Font_Draw(330,300, CVec(1,1,1), "You Won!");
                
                // Draw the stats
                Race_DrawFinished();
            }

            if( nRaceWinner == 1 && psRace->pcRaceCar->getCarSim()->bFinished ) {
                // I Lost
                Font_SetSize(20);
                Font_Draw(320,300, CVec(1,1,1), "You Lost!");

                // Draw the stats
                Race_DrawFinished();
            }
        }


        // Draw the car HUD
        Race_DrawHUD( psRace->pcRaceCar );

        // Draw the racing menu
        if( bRacingMenu )
            Race_ProcessMenu();

        if(psOptions->nShowFPS) {
            Font_SetAlpha(1);
            Font_SetSize(14);
            int fps = GetFPS();
            Font_Draw(5, 583, CVec(0,0,0), "FPS:  %d", fps ); 
            Font_Draw(5, 585, CVec(1,1,1), "FPS:  %d", fps );
        }
        if(psOptions->nDeveloper) {
            Font_SetAlpha(1);
            Font_SetSize(14);
            Font_Draw(5, 563, CVec(0,0,0), "Tris: %d", tMainSR3.TriCount ); 
            Font_Draw(5, 565, CVec(1,1,1), "Tris: %d", tMainSR3.TriCount );
        }

        //Font_Draw(647, 553, CVec(0,0,0), "f1: %f", tMainSR3.f1 );
        //Font_Draw(645, 555, CVec(1,1,1), "f1: %f", tMainSR3.f1 );
        		
		Render_End2D();

        // Draw the track in the rearview mirror
        // Make sure this is the last thing we do because we fuck around with the perspective & camera
        // Note: We can't use this with the outside cam because we fuck with the cam pos which fucks
        //       up the camera spring
        //if( psRace->pcRaceCar->getCarSim()->nCameraType == CAM_INCAR )
            //Race_DrawRearView(psRace->pcRaceCar);


		SDL_GL_SwapBuffers();
	}


	// Back to diner
	Game_SetLocation(LOC_DINER);

	// HACK
	// Game_SetLocation(LOC_EXIT);

	// Shutdown the race
	//Race_Shutdown();




	return true;
}


///////////////////
// Load the racing stuff
int Race_Load(char *szTrack)
{
    char	buf[256];

    // Setup the race structure
    psRace = new race_t;
    if(!psRace)
        return false;

    // Load textures
	tLight = Cache_LoadTexture("data/textures/lightspot.png");
	Tex_Upload(tLight);
    tTacho = Cache_LoadTexture("data/textures/driving/tacho.png");
    Tex_Upload(tTacho);
    tSpeedo = Cache_LoadTexture("data/textures/driving/speedo.png");
    Tex_Upload(tSpeedo);
    tNeedle = Cache_LoadTexture("data/textures/driving/needle.png");
    Tex_Upload(tNeedle);
    //tFlare = Cache_LoadTexture("data/textures/driving/flare.png");
    tFlare = Cache_LoadTexture("data/textures/corona.png");
    Tex_Upload(tFlare);

    // Load the track
    sprintf(buf, "data/cities/la/%s/track", szTrack);
    psRace->pcTrackModel = Cache_LoadModel(buf);
    if(psRace->pcTrackModel == NULL) {
        delete psRace;
        return false;
    }

    // Load the quadtree
    sprintf(buf, "data/cities/la/%s/track.qd", szTrack);
    if(psOptions->nQuadTree) {
        if(!psRace->cQuadTree.load(buf, psRace->pcTrackModel)) {
            printf("Developer: Quad tree \"%s\" did not load. Updating...\n",buf);
            
            // Create the quad tree
            psRace->cQuadTree.create(psRace->pcTrackModel);
            
            // Save the quad tree
            psRace->cQuadTree.save(buf, psRace->pcTrackModel);
        }
    }
    

    // Load the extra stuff
    psRace->cSky.Initialize();


    // Setup the light manager
    psRace->cLightMan.Setup(1, psRace->pcTrackModel->getNumLights() );
    light_t *l = psRace->pcTrackModel->getLights();	
	for(int i=0; i<psRace->pcTrackModel->getNumLights(); i++, l++) {
        *(psRace->cLightMan.getLights()+i) = *l;
    }


    // Load the scene script
    sprintf(buf, "data/cities/la/%s/track.sr3", szTrack);
    psRace->cSceneScript.load(buf,psRace->pcTrackModel);


	gobject_t *g = psRace->pcTrackModel->findGameObject("$finishline1");
	if(g)
		psRace->fFinishLine = g->vPosition.GetY();
	else
		psRace->fFinishLine = 0;

    return true;
}


int Stepped = false;

///////////////////
// Process the race driving
void Race_ProcessDriving(void)
{
	float		fRxnTime = (rand()%501)/100; //generate a random Reaction Time between .000 and .500
	keyboard_t	*kb = System_GetKeyboard();
	carsim_t	*psCar = psRace->pcRaceCar->getCarSim();
    carsim_t	*psOpp = psRace->pcOppCar->getCarSim();
	float		dt = tMainSR3.fDeltaTime;
    CInput      *raceInputs = cMainGuy->getRaceInput(0);


    if( bRacingMenu ) {
        // We set the last arg to true (bPaused) so this won't process the car, only the camera
        Car_ProcessCar( psCar, psRace->pcTrackModel, psRace->pcRaceCar, &psRace->cCamera, dt, true );
        return;
    }

    if(!raceInputs) {
        writeLog("Error: Racing input is null!\n");
        return;
    }


	// Get the keyboard input
	psCar->bBraking = false;
	psCar->bHandBrake = false;

    // Opponent car
    psOpp->bBraking = false;
	psOpp->bHandBrake = false;


    // If either car has gone past the finish line, set it to finished
    if( psCar->cPos.GetY() > psRace->fFinishLine ) {
        if(!psOpp->bFinished) {
            nRaceWinner = 0;
            
			// Add money to the player
            if(nRaceMode != race_finished) {
				if(sRaceInfo.nBetType == rcb_money)
					cMainGuy->addMoney(sRaceInfo.nBetMoney);
			}
        }
        psCar->bFinished = true;
        nRaceMode = race_finished;        
    }
    if( psOpp->cPos.GetY() > psRace->fFinishLine ) {
        if(!psCar->bFinished) {
            nRaceWinner = 1;
		
			// Take money
            if(nRaceMode != race_finished) {
				if(sRaceInfo.nBetType == rcb_money)
					cMainGuy->makePurchase(sRaceInfo.nBetMoney);
			}
		}
        psOpp->bFinished = true;
        nRaceMode = race_finished;
    }

    // Process the ai
	if( psCar->bFinished || nRaceMode == race_red ) {
		psCar->nGear = 0;
		psCar->bBraking = true;
	}
	else if ( fStartRace > (2.5 + fRxnTime) )
		Race_ProcessAI( psOpp, psCar );


    // Gear
    if(raceInputs[INP_UPGEAR].isUp()) {
        psCar->nGear++;

        // Clutch zero's the acceleration
        psCar->fAcceleration = 0;
    }
    if(raceInputs[INP_DOWNGEAR].isUp()) {
        psCar->nGear--;
        
        // Clutch zero's the acceleration
        psCar->fAcceleration = 0;
    }

    psCar->nGear = MAX(psCar->nGear,-1);
    psCar->nGear = MIN(psCar->nGear,psCar->iNumGears);


    // Keep the car in neutral & braking until we start racing
    if( psOpp->bFinished || nRaceMode == race_red ) {
        psOpp->nGear = 0;
        psOpp->bBraking = true;
    }

    // Camera
    if(raceInputs[INP_CHANGECAMERA].isUp()) {
        psCar->nCameraType++;
        psCar->nCameraType %= 2;
    }


    // Acceleration
    if(raceInputs[INP_ACCELERATE].isDown())
        psCar->fAcceleration += 2.5f*dt;
    else
        psCar->fAcceleration -= 2.5f*dt;

    // Clamp acceleration
    psCar->fAcceleration = MIN(psCar->fAcceleration, 1);
    psCar->fAcceleration = MAX(psCar->fAcceleration, 0);

    tMainSR3.f1 = psCar->fAcceleration;

	if(raceInputs[INP_BRAKE].isDown())
		psCar->bBraking = true;

	// Steering wheels
	if(raceInputs[INP_STEERLEFT].isDown()) {
		psCar->fSteerAngle += 7.5f*dt;
		psCar->fSteerAngle = MIN(psCar->fSteerAngle, 1.0f);
	}
	else if(raceInputs[INP_STEERRIGHT].isDown()) {
		psCar->fSteerAngle -= 7.5f*dt;
		psCar->fSteerAngle = MAX(psCar->fSteerAngle, -1.0f);
	}
	else {
		if(psCar->fSteerAngle > 0)
			psCar->fSteerAngle -= 2*dt;
		else if(psCar->fSteerAngle < 0)
			psCar->fSteerAngle += 2*dt;
		if( fabs(psCar->fSteerAngle) < 0.025f)
			psCar->fSteerAngle = 0;
	}


	// Simulate the main car
	Car_ProcessCar( psCar, psRace->pcTrackModel, psRace->pcRaceCar, &psRace->cCamera, dt, false );

	// Simulate the opponent
    Car_ProcessCar( psOpp, psRace->pcTrackModel, psRace->pcOppCar, NULL, dt, false );

    Car_CheckCarCollisions( psRace->pcRaceCar, psRace->pcOppCar );

    
    // Update Stats
    if( !psCar->bFinished && nRaceMode != race_red ) {
        psCar->fRaceTime += dt;
        float vl = VectorLength( psCar->cVelocity );
        if( vl > psCar->fMaxSpeed )
            psCar->fMaxSpeed = vl;
    }
}


///////////////////
// Draw the car's HUD
void Race_DrawHUD(CCar *pcCar)
{
    State_Disable(GL_LIGHTING);
    State_Enable(GL_TEXTURE_2D);
    State_Enable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
	// Tachometer
	glColor4f(1,1,1,1);
	DrawPicEx(tTacho, 660, 140, 788, 12);

    // Gear
    Font_SetSize(20);
    char Gears[8] = {"N12345"};
    int nGear = pcCar->getCarSim()->nGear;
    if(nGear >= 0)
        Font_Draw( 715, 25, CVec(1,1,1), "%c",Gears[nGear]);
    else
        Font_Draw( 715, 25, CVec(1,1,1), "R");
	
	// Draw the RPM
	glPushMatrix();
	glColor4f(0.75f,0,0,1);
	glTranslatef(722, 76, 0);
	glRotatef((-(float)(pcCar->getCarSim()->nRPM-1000)/1000 * 27)+130, 0,0,1);
	DrawPicEx(tNeedle, 0,64, 32,0);
	glPopMatrix();



	// Speed-o-meter
	glColor4f(1,1,1,1);
	DrawPicEx(tSpeedo, 12, 140, 140, 12);

	// Draw the speed
	float speed = VectorLength(pcCar->getCarSim()->cVelocity) / SPEED_SCALE;
	glPushMatrix();
	glColor4f(0.75f,0,0,1);
	glTranslatef(74, 76, 0);
	glRotatef(-(speed/50 * 36)+130, 0,0,1);
	DrawPicEx(tNeedle, 0,64, 32,0);
	glPopMatrix();
}


///////////////////
// Draw the rearview mirror
void Race_DrawRearView(CCar *pcCar)
{
    carsim_t *psCar = pcCar->getCarSim();
    State_Disable(GL_LIGHTING);
    State_Disable(GL_BLEND);

    // Dimensions
    float x = 275;
    float y = 490;
    float w = 250;
    float h = 100;


    // Draw a black rectangle
    Render_Begin2D();
    DrawRect(x,y+h,x+w,y, CVec(1,1,1), CVec(0.0f,0.0f,0.0f), 1, true, 0);

    Render_End2D();

    // Setup the perspective matrix	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	// Setup the viewport
	gluPerspective(50.0f,w/h,1,256.0f);
	glViewport(x,y,w,h);

	// Setup the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glClear(GL_DEPTH_BUFFER_BIT);
	
	// Swap to the SEA(?) coordinate system
	glTranslatef(0.0,0.0,0.0);
	glRotatef (-90,  1, 0, 0);
	glRotatef (90,  0, 0, 1);


    // Set the camera to show backwards
    Car_SetRearviewCam( pcCar, &psRace->cCamera );
    psRace->cCamera.Show();

    State_Disable(GL_LIGHTING);
    psRace->pcTrackModel->render();

    State_Enable(GL_LIGHTING);
    psRace->pcOppCar->DrawDriving(&psRace->cCamera);
}


///////////////////
// Process the ai
void Race_ProcessAI(carsim_t *psCar, carsim_t *psOpp)
{
	// return;  // I guess they didn't want to process it?

    // psOpp is the _other_ car. Ie, human player
    psCar->bBraking = false;

    // We're accelerating always unless the race is over
    bool acc = false;
    if( !psCar->bFinished ) {
        acc = true;
        psCar->fAcceleration += 2.5f*tMainSR3.fDeltaTime;
    } else
        psCar->bBraking = true;

    // Clamp acceleration
    psCar->fAcceleration = MIN(psCar->fAcceleration, 1);
    psCar->fAcceleration = MAX(psCar->fAcceleration, 0);
    
    // Change gear
    if( psCar->nRPM > 4700 && psCar->nGear < psCar->iNumGears && psCar->nGear) {
        psCar->nGear++;
        psCar->fAcceleration = 0;
    }
	else if ( !psCar->nGear && nRaceMode == race_racing ) {
		psCar->nGear++;
		psCar->fAcceleration = 0;
	}
    else if( psCar->nRPM < 2000 && !acc && psCar->nGear > 0) {
        psCar->nGear--;
        psCar->fAcceleration = 0;
    }

    // Clamp the gear
    psCar->nGear = MAX(psCar->nGear,0);
    psCar->nGear = MIN(psCar->nGear,3);

    // If we're not racing, put the car in neutral
    if( psCar->bFinished || nRaceMode == race_red ) {
        psCar->nGear = 0;
		psCar->bBraking = true;
	}
}


CGuiLayout cRaceMenu;


///////////////////
// Initialize the racing menu
void Race_InitMenu(void)
{
    cRaceMenu.Initialize();
	cRaceMenu.Add( new CButton("Quit",btn_quit),		2,  350,280,75, 25  );
	cRaceMenu.Add( new CButton("Forget it",btn_forgit), 1,  350,330,120,25  );
	cRaceMenu.Add( new CDialog("System", dlgs_small),   0,  35, 25, 250,200 );
}


///////////////////
// Process the racing menu
void Race_ProcessMenu(void)
{
    gui_event_t *event;
	event = cRaceMenu.Process();

    cRaceMenu.Draw();

    // Draw a mouse
    gui_DrawMouse();

    if( !event )
        return;

    switch( event->iControlID ) {

        // Quit
        case 2:
            // Forfeit win (ie, you loose)
            if(nRaceMode != race_finished) {
				if(sRaceInfo.nBetType == rcb_money)
					cMainGuy->makePurchase(sRaceInfo.nBetMoney);
			}

            cRaceMenu.Shutdown();
            bRacingMenu = false;
            bProcessRace = false;
            break;


        // Forget it
        case 1:
            bRacingMenu = false;
            cRaceMenu.Shutdown();
            break;
    }
}


///////////////////
// Draw the finished stats
void Race_DrawFinished(void)
{
    DrawRect( 200,280, 600,150, CVec(0,0,0), CVec(0,0,0), 0.7f, true, 0);
    
    Font_SetSize(12);
    Font_Draw( 220,250, CVec(1,1,1), "Race time: %.1f seconds", psRace->pcRaceCar->getCarSim()->fRaceTime);
    Font_Draw( 220,230, CVec(1,1,1), "Max Speed: %.1f mph", psRace->pcRaceCar->getCarSim()->fMaxSpeed / SPEED_SCALE);
}