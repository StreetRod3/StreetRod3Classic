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


// Garage class
// Created 13/5/02
// Jason Boettcher


#include "defs.h"
#include "system.h"
#include "game.h"
#include "font.h"
#include "CSky.h"
#include "gui.h"
#include "entity.h"

#include "CRigidBody.h"

garage_t		*tGarage = NULL;
CLocation		*cGarLocation = NULL;
partchange_t	tPartChange;
CTaskBar		cGarTaskb;
CTaskBar        cGarSection;
bool            bGarSystemMenu = false;
bool            bGarProcess = true;
CSceneScript    cGarScript;

HSTREAM			stmAmbient;
CSidebar		cSidebar;

CRigidBody      cTestBody;


///////////////////
// Start the garage scene
int Gar_Start(char *sDir)
{
	keyboard_t	*kb = System_GetKeyboard();
	CCar		*car = NULL;
    GLfloat     garLightPos[4] = {0, 0, 0,1};
    GLfloat     garLightPos2[4] = {0, 0, 0,1};
    GLfloat     spec[] = {1,1,1,1};
    GLfloat     one[] = {0.3f,0.3f,0.3f,1};
    GLfloat     amb[] = {0.2f,0.2f,0.2f,1};

	// Load the files
	if(!Gar_Load(sDir))
		return false;

    for( int i=0; i<3; i++ ) {
        garLightPos[i] = cGarLocation->getScene()->getLight(0).Position.GetIndex(i);
        garLightPos2[i] = cGarLocation->getScene()->getLight(1).Position.GetIndex(i);
    }

    bGarSystemMenu = false;
    bGarProcess = true;

	cGarLocation->SetupCamera(CVec(4,5,-1.4f),30);
    cGarLocation->setCamPos( CVec(25,5,5) );

	Gar_NormalView();


	//stmAmbient = BASS_StreamCreateFile(false,"data/sounds/a_garage.mp3",0,0,0);
	//BASS_StreamPlay(stmAmbient,true,BASS_SAMPLE_LOOP);

    // Setup the fog
    State_Disable(GL_FOG);
    glFogf(GL_FOG_DENSITY, 0.7f);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, 45.0f);
	glFogf(GL_FOG_END, 100.0f);

    // Setup the lighting
    State_Enable(GL_LIGHT1);
    //State_Enable(GL_LIGHT2);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, one);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, one);
    glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.00f);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.00f);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0f);
    glMaterialf(GL_FRONT, GL_SHININESS, 25.0);
    glMaterialfv(GL_FRONT, GL_SPECULAR, one);

    //
    // Rigid Body testing
    Tex_AddDir("data/part_models/tire2");
    CModel *tyre = Cache_LoadModel(/*"data/test"*/"data/part_models/tire1/part");
	cTestBody.reset();
    cTestBody.createFromModel(10000, 0.1f, tyre);

    cTestBody.setPosition( cGarLocation->getScene()->getLight(0).Position - CVec(0,0,3) );	



	// Setup the car position
	car = cMainGuy->getCurCar();
    if(car)
        car->calculateRest(CVec(4,5,-4.61f));

    Gar_NormalView();
	Gar_SetItems();

	System_ProcessInput();
	while(bGarProcess) {
		System_ProcessInput();

        if( kb->KeyUp[SDLK_ESCAPE] && !bGarSystemMenu && tGarage->iState == GAR_NORMAL) {
            Gar_InitSystemMenu();
            kb->KeyUp[SDLK_ESCAPE] = false;
        }

		// Process some global stuff
		cMainGuy->processFrame();

		// Processing
		switch(tGarage->iState) {

			// Normal
			case GAR_NORMAL:
			case GAR_ENGINE:
			case GAR_ENGINEPART:
			case GAR_RUNGPART:
			case GAR_RUNNINGG:
                if( !bGarSystemMenu && !tGarage->bSpeechBubble ) {
				    cGarLocation->Process();
				    cGarTaskb.Simulate();				    
                    cGarSection.Simulate();
                    cSidebar.Process();
                    Gar_ProcessEngRun();
                }
				break;


			// Dialog
			case GAR_DIALOG:
			//	Gar_ProcessPartCarDialog();
				break;

			// Tuning
			case GAR_TUNING:
				cGarLocation->Process();
				cGarSection.Simulate();
                cGarTaskb.Simulate();
				Gar_TuningProcess();
				break;

			// Car rolling
			case GAR_CARROLLING:
				// Process the car rolling
				Gar_ProcessCarRoll();
				break;

            // Decal applying
            case GAR_DECAL:
                cGarLocation->Process();
                break;
				
		}

        if(!bGarProcess)
            break;



        // Setup the light
        glLightfv(GL_LIGHT0, GL_POSITION, garLightPos);
        glLightfv(GL_LIGHT1, GL_POSITION, garLightPos2);
		SetupFrame();
		cGarLocation->Draw(tGarage->bDrawGarage, false,true);
		
		// Draw the car
		car = cMainGuy->getCurCar();
		if(car && tGarage->bDrawGarage) {
            car->DrawEngine();
			car->Draw();			
		}


        // Draw the lights
        if(tGarage->bDrawGarage) {
            cGarScript.draw(cGarLocation->getCamera());
        }


        //
        // Draw the body
        //
        //cTestBody.applyForce(CVec(0,0,-5800));
        //cTestBody.applyDampening(150,150);
        //cTestBody.applyPosTorque(cTestBody.getPosition()-CVec(0,0,100), CVec(0,10000,0));
		//cTestBody.applyTorque(CVec(0,10,0));
        //estBody.simulate(tMainSR3.fDeltaTime, cGarLocation->getScene());
        //cTestBody.checkCollisions(cGarLocation->getScene());
        //cTestBody.resolveCollisions();
/*
        float m[16];		
        //cTestBody.getOrientation().createGLMatrix( cTestBody.getPosition(), m );
		cTestBody.createDrawMatrix(m);
        glPushMatrix();
        //glTranslatef(cTestBody.getPosition().GetX(), cTestBody.getPosition().GetY(), cTestBody.getPosition().GetZ());
        glMultMatrixf(m);
        tyre->render();
        glPopMatrix();*/

        UnprojectMouse(&tGarage->mousePoint);

        // Decal application 2 (out here coz we need the proper view matrix setup)
        if(tGarage->iState == GAR_DECAL) {
            Gar_DecalApplyProcess2();
        }
			
              

		// 2D Stuff
		Render_Begin2D();

            // Part change
		    if(tPartChange.iChange)
			    Gar_ChangePart();

        	cGarLocation->FinalDraw();
            if(tGarage->iState != GAR_DIALOG && tGarage->iState != GAR_DECAL) {
			    cGarTaskb.Draw();
                cGarSection.Draw();
                cSidebar.Draw();
            }
			
            // Decal application
            if(tGarage->iState == GAR_DECAL) {
                Gar_DecalApplyProcess();
            }
				
			// Newspaper
			if(tGarage->iState == GAR_NEWSPAPER)
				Gar_NewspaperProcess();


			// Run the dialog process
			if(tGarage->iState == GAR_DIALOG)
				if(tGarage->DialogProcess)
					tGarage->DialogProcess();

            // Speech bubble
            if( tGarage->bSpeechBubble )
                Gar_SpeechProcess();

            // Running Gear/Engine screen
            if(tGarage->iState == GAR_ENGINE || tGarage->iState == GAR_ENGINEPART ||
               tGarage->iState == GAR_RUNNINGG || tGarage->iState == GAR_RUNGPART)
                Gar_ShowAssembled();

                    
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

            gui_DrawMouse();
            
		Render_End2D();

		SDL_GL_SwapBuffers();
	}

	Gar_Shutdown();

	return true;
}


///////////////////
// Load the garage files
int Gar_Load(char *sDir)
{
    char buf[256];

	// Load the location
	cGarLocation = new CLocation;
	if(cGarLocation == NULL)
		return false;

	// Initialize the garage
	tGarage = new garage_t;
	if(tGarage == NULL)
		return false;
	
	tGarage->iState = GAR_NORMAL;
	tGarage->DialogProcess = NULL;
    tGarage->bSpeechBubble = false;
    tGarage->bDrawGarage = true;

    // Assembled Sign
    tGarage->psAssembled = Cache_LoadTexture("data/textures/garage/assembled.png");
    Tex_Upload(tGarage->psAssembled);
    tGarage->bShowAssembled = true;
    tGarage->fAssembledAlpha = 1;

    sprintf(buf,"%s/garage",sDir);
    Tex_AddDir(buf);
	
	sprintf(buf,"%s/garage/garage",sDir);
	if(!cGarLocation->Load(buf))
		return false;

    sprintf(buf,"%s/garage",sDir);
    Tex_RemoveDir(buf);

	tPartChange.iChange = false;

	cSidebar.Setup(2,450,"data/gui/sidebuttons.png");

	cSidebar.addItem(0, Gar_InitPaintDialog);		// Paint
	cSidebar.addItem(1, Gar_InitCarList);			// Cars
	cSidebar.addItem(2, Gar_InitSpareparts);		// Spare parts
	cSidebar.addItem(3, Gar_InitCarInfo);			// Car info
	cSidebar.addItem(4, Gar_NewspaperInit);			// Newspaper
    cSidebar.addItem(5, Gar_InitStickers);			// Stickers
    cSidebar.addItem(6, Gar_HitStreet); 			// Hit the street

    cGarTaskb.Clear();
	cGarTaskb.setPos(800-70);
	cGarTaskb.setDirection(1);
	
    cGarSection.Clear();
    cGarSection.setPos(130);
    cGarSection.setDirection(1);
    cGarSection.Add("data/gui/but_runningg.png","runningg",Gar_InitRunningGear);
    cGarSection.Add("data/gui/but_engine.png","engine",Gar_InitEngineScreen);
    cGarSection.Add("data/gui/but_car.png","car",Gar_CarView);

    cGarSection.toggleItem("car");
    cGarSection.setEnabled(true);

    // Load the garage scene script
    sprintf(buf,"%s/garage/garage.sr3",sDir);
    cGarScript.load(buf, cGarLocation->getScene());

	return true;
}


///////////////////
// Limit the camera position for the garage
void Gar_LimitCamera(void)
{
    CCamera *cam = cGarLocation->getCamera();
    
    CVec pos = cam->getPos();
    





    cam->setPos(pos);
}


/*
    The System menu
*/

CGuiLayout cSysMenu;


///////////////////
// Initialize the system menu
void Gar_InitSystemMenu(void)
{
    bGarSystemMenu = true;
	tPartChange.iChange = false;
	tGarage->iState = GAR_DIALOG;
	tGarage->DialogProcess = Gar_ProcessSysDialog;

	cSysMenu.Initialize();
    cSysMenu.Add( new CButton("Load Game",0),          6,  336,230,120,25  );
    cSysMenu.Add( new CButton("Save Game",0),          5,  336,280,120,25  );
    cSysMenu.Add( new CButton("Radio",0),              4,  336,330,120,25  );
    cSysMenu.Add( new CButton("Forget it",btn_forgit), 3,  427,390,120,25  );
	cSysMenu.Add( new CButton("Quit",btn_quit),		   2,  245,390,75, 25  );	
	cSysMenu.Add( new CDialog("System", dlgs_small),   1,  35, 25, 350,280 );
}


///////////////////
// Process the system menu
void Gar_ProcessSysDialog(void)
{
   	gui_event_t *event;
	event = cSysMenu.Process();

    cSysMenu.Draw();

    if( !event )
        return;

    switch( event->iControlID ) {

        // Quit
        case 2:
            /*tGarage->iState = GAR_NORMAL;
            bGarSystemMenu = false;
			cSysMenu.Shutdown();
            bGarProcess = false;
	        // Exit
        	Game_SetLocation(LOC_EXIT);
            */
            Gar_InitQuitMenu();
            break;

        // Forget it
        case 3:
            tGarage->iState = GAR_NORMAL;
            bGarSystemMenu = false;
			cSysMenu.Shutdown();
            break;

        // Radio
        case 4:
            bGarSystemMenu = false;
            cSysMenu.Shutdown();

            Gar_InitRadioMenu();
            break;

        // Save game
        case 5:
            bGarSystemMenu = false;
            cSysMenu.Shutdown();

            Gar_InitSaveGame();
            break;

        // Load game
        case 6:
            bGarSystemMenu = false;
            cSysMenu.Shutdown();

            Gar_InitLoadGame();
            break;
    }
}


///////////////////
// Display the quit menu
void Gar_InitQuitMenu(void)
{
    bGarSystemMenu = true;
	tPartChange.iChange = false;
	tGarage->iState = GAR_DIALOG;
	tGarage->DialogProcess = Gar_ProcessQuitDialog;

	cSysMenu.Initialize();
    cSysMenu.Add( new CButton("To Menu",0),             4,  336,255,120,25  );
    cSysMenu.Add( new CButton("To System",0),           3,  336,300,120,25  );
    cSysMenu.Add( new CButton("Forget it",btn_forgit),  2,  336,345,120,25  );
	cSysMenu.Add( new CDialog("System", dlgs_small),    1,  35, 25, 270,200 );
}


///////////////////
// Process the quit menu
void Gar_ProcessQuitDialog(void)
{
   	gui_event_t *event;
	event = cSysMenu.Process();

    cSysMenu.Draw();

    if( !event )
        return;

    switch( event->iControlID ) {

        // Quit to menu
        case 4:
            tGarage->iState = GAR_NORMAL;
            bGarSystemMenu = false;
			cSysMenu.Shutdown();
            bGarProcess = false;
	        
            // Exit to menu
        	Game_QuittoMenu();
            break;

        // Quit to system
        case 3:
            tGarage->iState = GAR_NORMAL;
            bGarSystemMenu = false;
			cSysMenu.Shutdown();
            bGarProcess = false;
	        // Exit
        	Game_SetLocation(LOC_EXIT);
            break;

        // Forget it
        case 2:
            Gar_InitSystemMenu();
            break;
    }
}


/*
    Assembled
*/


///////////////////
// Show the assembled sign
void Gar_ShowAssembled(void)
{
    float dt = tMainSR3.fDeltaTime;

    // Engine section
    if(tGarage->iState == GAR_ENGINE || tGarage->iState == GAR_ENGINEPART) {
        CCar *car = cMainGuy->getCurCar();
        if(car) {
            if(car->getBlock())
                Gar_ToggleAssembled(car->getBlock()->isComplete());
            else
                Gar_ToggleAssembled(false);
        }
    }

    // Running gear section
    if(tGarage->iState == GAR_RUNNINGG || tGarage->iState == GAR_RUNGPART) {
        CCar *car = cMainGuy->getCurCar();
        if(car) {
            Gar_ToggleAssembled(car->runningGearComplete());
        }
    }
    
    // Showing
    if(tGarage->bShowAssembled) {
        tGarage->fAssembledAlpha += 2*dt;
        if(tGarage->fAssembledAlpha > 1.0f)
            tGarage->fAssembledAlpha = 1.0f;
    } else {
        // Hiding
        tGarage->fAssembledAlpha -= 4*dt;
        if(tGarage->fAssembledAlpha < 0.0f)
            tGarage->fAssembledAlpha = 0.0f;
    }


    // Show it
    State_Enable(GL_TEXTURE_2D);
    State_Disable(GL_LIGHTING);
    State_Enable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);

    glColor4f(0,tGarage->fAssembledAlpha, 0, 1.0f);
    DrawPic(tGarage->psAssembled, 336, 70);

    // Pulse when turned on
    if(tGarage->bShowAssembled && tGarage->fAssembledAlpha < 1.0f) {
        float f = tGarage->fAssembledAlpha;
        glColor4f(0,1-f, 0, 1.0f);
        DrawPicEx(tGarage->psAssembled, 336-f*10, 70+f*5, 464+f*10, 38-f*5);
    }

}


///////////////////
// Toggle the assembled sign
void Gar_ToggleAssembled(bool bShowing)
{
    if(bShowing) {
        if(!tGarage->bShowAssembled) {
            tGarage->bShowAssembled = true;
            tGarage->fAssembledAlpha = 0.0f;
        }
    } else {
        tGarage->bShowAssembled = false;
    }
}


///////////////////
// Hit the street
void Gar_HitStreet(void)
{
    // Check car
    CCar *pcCar = cMainGuy->getCurCar();
    if( !pcCar ) {
        // Let the user know they have no wheels to drive
        Gar_InitSpeechBubble( "You got no wheels to drive in" );
        return;
    }

    // Is this car drivable?
    if(!pcCar->runningGearComplete()) {
        Gar_InitSpeechBubble("Your car's not running, speedy!");
        return;
    }
    CPart *block = pcCar->getBlock();
    if(!block) {
        Gar_InitSpeechBubble("Your car's not running, speedy!");
        return;
    }
    if(!block->isComplete()) {
        Gar_InitSpeechBubble("Your car's not running, speedy!");
        return;
    }


    //
    // Show a loading sign
    //
    Render_Begin2D();
    Font_SetSize(14);
    Font_SetAlpha(1);
    Font_DrawCentered(400, 300, CVec(1,1,1), "Loading...");
    Render_End2D();
    SDL_GL_SwapBuffers();


    
    bGarProcess = false;
    Game_SetLocation(LOC_DINER);
}


///////////////////
// Shutdown the garage
void Gar_Shutdown(void)
{
	if(cGarLocation) {
		cGarLocation->Shutdown();
		delete cGarLocation;
		cGarLocation = NULL;
	}

	if(tGarage) {
		delete tGarage;
		tGarage = NULL;
	}

    cGarScript.shutdown();
}