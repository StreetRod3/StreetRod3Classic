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


// Diner
// Created 18/6/03
// Jason Boettcher


#include "defs.h"
#include "system.h"
#include "game.h"
#include "font.h"
#include "gui.h"
#include "entity.h"


// Gui buttons
enum {
    dng_dlg,
    // Level 1
    dng_drag,
    dng_road,
    dng_cancel,

    //
    // DRAG
    //
    // Level 2
    dng_dragtrk1,       // Maximum 9 tracks
    dng_dragtrk2,
    dng_dragtrk3,
    dng_dragtrk4,
    dng_dragtrk5,
    dng_dragtrk6,
    dng_dragtrk7,
    dng_dragtrk8,
    dng_dragtrk9,

    // Level 3
    dng_dragkicks,
    dng_drag10,
    dng_drag50,
    dng_dragback,

    //
    // ROAD
    //
    // Level 2
    dng_roadtrk1,
    dng_roadtrk2,
    dng_roadtrk3,
    dng_roadtrk4,
    dng_roadtrk5,
    dng_roadtrk6,
    dng_roadtrk7,
    dng_roadtrk8,
    dng_roadtrk9,

    // Level 3
    dng_road25,
    dng_road50,
    dng_road100,
    dng_road150,
    dng_roadpink,
    dng_roadback
};



diner_t     *psDiner = NULL;
CGuiLayout  cDinerLayout;
CGuiLayout  cDinerLayout2;


///////////////////
// Start the diner
bool Dnr_Start(char *szDir)
{
    keyboard_t *kb = System_GetKeyboard();
    GLfloat     one[] = {0.3f,0.3f,0.3f,1};
    GLfloat     blue[] = {0.3f, 0.3f, 1.0f, 1.0f};
    GLfloat     pos1[4] = {1,1,1,1};
    GLfloat     pos2[4] = {1,1,1,1};
    GLfloat     pos3[4] = {25,-100,50,1};
    int         i;

    if( !Dnr_Load(szDir) )
        return false;

    for( i=0; i<3; i++ ) {
        pos1[i] = psDiner->pcLocation->getScene()->getLight(0).Position.GetIndex(i);
        pos2[i] = psDiner->pcLocation->getScene()->getLight(1).Position.GetIndex(i);        
    }

    // Setup the diner for an opponent
    psDiner->nOppState = opp_waiting;
    psDiner->fOppTimer = tMainSR3.fCurTime + (float)GetRandomInt(2)+2.0f;

    sRaceInfo.pcOpponent = NULL;

    // HACK: Quickly get the opponent in the diner
    //Dnr_RollinOpponent();


    //psDiner->pcLocation->SetupCamera( CVec(25,11,-1.4f),100);
    //psDiner->pcLocation->setCamPos( CVec(0,0,0) );

    // Setup the fog
    State_Disable(GL_FOG);
    
    // Setup the lighting
    State_Enable(GL_LIGHTING);
    State_Enable(GL_LIGHT1);
    State_Enable(GL_LIGHT2);    
    glLightfv(GL_LIGHT0, GL_DIFFUSE, one);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, one);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, one);    
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.00f);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.00f);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0f);
    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 0.80f);
	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.01f);
    glMaterialf(GL_FRONT, GL_SHININESS, 25.0);
    glMaterialfv(GL_FRONT, GL_SPECULAR, one);


    System_ProcessInput();
	while(psDiner->bProcessDiner) {
		System_ProcessInput();

        //
        // Processing
        //
        psDiner->pcLocation->Process();
        psDiner->cSky.Simulate(tMainSR3.fDeltaTime);

        if( !psDiner->bSpeechBubble ) {
            Dnr_ProcessOpponent();
            Dnr_ProcessGUI();
        }

		cMainGuy->processFrame();



        //
        // 3D Drawing
        //
        glLightfv(GL_LIGHT0, GL_POSITION, pos1);
        glLightfv(GL_LIGHT1, GL_POSITION, pos2);
        glLightfv(GL_LIGHT2, GL_POSITION, pos3);

        SetupFrame();
        // Manually setup the camera
        psDiner->pcLocation->getCamera()->LookAt(psDiner->pcLocation->getCamLook());
        psDiner->pcLocation->getCamera()->Show();

        psDiner->cSky.Render( psDiner->pcLocation->getCamera() );
        State_Enable(GL_LIGHTING);
		psDiner->pcLocation->Draw(true, true, false);

        Dnr_DrawOpponent();
        

        
        //
        // 2D Drawing
        //
        Render_Begin2D();

            // Draw the sidedoor bit
            State_Enable(GL_TEXTURE_2D);
            State_Disable(GL_LIGHTING);
            State_Enable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(1,1,1,1);
            BlitTexture(psDiner->psSideDoor, 0,0, 0,472, 105,128, 1,1);
            for(i=0; i<10; i++ ) {
                BlitTexture(psDiner->psSideDoor, 105,0, i*95+105,472, 95,128, 1,1);
            }

            if( psDiner->bSpeechBubble )
                Dnr_SpeechProcess();

            psDiner->pcLocation->FinalDraw();
            
            if(psOptions->nShowFPS) {
                Font_SetAlpha(1);
                Font_SetSize(14);
                int fps = GetFPS();
                Font_Draw(5, 583, CVec(0,0,0), "FPS:  %d", fps); 
                Font_Draw(5, 585, CVec(1,1,1), "FPS:  %d", fps);
            }
            if(psOptions->nDeveloper) {
                Font_SetAlpha(1);
                Font_SetSize(14);
                Font_Draw(5, 563, CVec(0,0,0), "Tris: %d", tMainSR3.TriCount);
                Font_Draw(5, 565, CVec(1,1,1), "Tris: %d", tMainSR3.TriCount);
            }

            // Draw the gui
            if(!psDiner->bSpeechBubble && psDiner->bProcessDiner) {
                if( psDiner->bOppClicked )
                    cDinerLayout.Draw();
                cDinerLayout2.Draw();
            }
        
            gui_DrawMouse();
            
		Render_End2D();

		SDL_GL_SwapBuffers();
    }

    //
    // Show a loading sign on the screen 
    //
    Render_Begin2D();
    Font_SetSize(14);
    Font_SetAlpha(1);
    Font_DrawCentered(400, 300, CVec(1,1,1), "Loading...");
    Render_End2D();
    SDL_GL_SwapBuffers();


    // Are we going to race?
    if(psDiner->bRacing)
        // Off to the track
        Game_SetLocation(LOC_RACE);
    else
        // Back to the garage
        Game_SetLocation(LOC_GARAGE);

    Dnr_Shutdown();

    return true;
}


///////////////////
// Load the diner
bool Dnr_Load(char *szDir)
{
    char buf[256];

    // Allocate the structures
    psDiner = new diner_t;
    if( !psDiner )
        return false;

    psDiner->pcLocation = new CLocation;
    if( !psDiner ) {
        delete psDiner;
        psDiner = NULL;
        return false;
    }

    psDiner->bProcessDiner = true;
    psDiner->bSpeechBubble = false;
    psDiner->bGoodOffer = false;
    psDiner->bRacing = false;

    // Load the diner scene
    sprintf(buf,"%s/diner",szDir);
    Tex_AddDir(buf);
	
	sprintf(buf,"%s/diner/diner",szDir);
    if(!psDiner->pcLocation->Load(buf)) {
        delete psDiner->pcLocation;
        delete psDiner;
        psDiner = NULL;
		return false;
    }

    sprintf(buf,"%s/diner",szDir);
    Tex_RemoveDir(buf);

    psDiner->pcLocation->lockCamera();
    psDiner->pcLocation->SetupCamera(CVec(25,-60,5), 30);
    psDiner->pcLocation->setCamPos(CVec(25,-90,9));

    psDiner->psSideDoor = Cache_LoadTexture("data/textures/sidedoor.png");
    Tex_Upload(psDiner->psSideDoor);

    // Setup the gui layout
    cDinerLayout.Initialize();

    cDinerLayout2.Initialize();
    cDinerLayout2.Add( new CButton("Leave", -1),     0,  650, 530, 0,0);
    
    // Load the sky
    psDiner->cSky.Initialize();


    // Load the track details
    psDiner->nNumDragTracks = 0;
    psDiner->nNumRoadTracks = 0;

    sprintf(buf,"%s/city.sr3",szDir);
    ReadInteger(buf, "Tracks", "NumDragTracks", &psDiner->nNumDragTracks, 0);
    ReadInteger(buf, "Tracks", "NumRoadTracks", &psDiner->nNumRoadTracks, 0);

    psDiner->nNumDragTracks = MAX(MIN(psDiner->nNumDragTracks,MAX_TRACKS-1), 0);
    psDiner->nNumRoadTracks = MAX(MIN(psDiner->nNumRoadTracks,MAX_TRACKS-1), 0);

    char    trk[32];
    int     i;

    // Drag
    for(i=0; i<psDiner->nNumDragTracks; i++) {
        sprintf(trk, "DragTrack%d", i+1);
        ReadString(buf, trk, "Name", psDiner->sDragTracks[i].szName, "");
        ReadString(buf, trk, "Dir", psDiner->sDragTracks[i].szDir, "");
    }

    // Road
    for(i=0; i<psDiner->nNumRoadTracks; i++) {
        sprintf(trk, "RoadTrack%d", i+1);
        ReadString(buf, trk, "Name", psDiner->sRoadTracks[i].szName, "");
        ReadString(buf, trk, "Dir", psDiner->sRoadTracks[i].szDir, "");
    }

    return true;
}


///////////////////
// Process the gui
void Dnr_ProcessGUI(void)
{
    gui_event_t *ev;

    // Process the bottom gui
    ev = cDinerLayout2.Process();
    if(ev) {
        // Leave button
        if(ev->iControlID == 0 && ev->iEventMsg == BUT_CLICKED) {
            psDiner->bProcessDiner = false;
            psDiner->bRacing = false;
            return;
        }
    }


    // Process the racing gui
    if(psDiner->bOppClicked) {
        ev = cDinerLayout.Process();
        if(!ev)
            return;
        
        switch(ev->iControlID) {

            // Cancel
            case dng_cancel:
                // Chicken
                if(ev->iEventMsg == BUT_CLICKED) {
                    // Leave
                    psDiner->bOppClicked = false;
                    psDiner->nOppState = opp_rollingout;

                    Dnr_InitSpeechBubble("Chicken");
                }
                break;

            // Drag race
            case dng_drag:
                if(ev->iEventMsg == BUT_CLICKED) {
                    sRaceInfo.nType = rc_drag;

                    // Show tracks
                    Dnr_ChallengeOpponent(1, 0);
                    return;
                }
                break;

            // Drag : track selection
            case dng_dragtrk1:      // Fall through
            case dng_dragtrk2:
            case dng_dragtrk3:
            case dng_dragtrk4:
            case dng_dragtrk5:
            case dng_dragtrk6:
            case dng_dragtrk7:
            case dng_dragtrk8:
            case dng_dragtrk9:
                if(ev->iEventMsg == BUT_CLICKED) {
                    int l = ev->iControlID - dng_dragtrk1;
                    sr_strncpy(sRaceInfo.szTrackDir, psDiner->sDragTracks[l].szDir, 127);

                    // Show bets
                    Dnr_ChallengeOpponent(2, 0);
                    return;
                }
                break;
            
            // Drag : Back
            case dng_dragback:
                if(ev->iEventMsg == BUT_CLICKED) {
                    // Show tracks
                    Dnr_ChallengeOpponent(1, 0);
                    return;
                }
                break;


            //
            // Drag : Bet
            //
            case dng_dragkicks:
                if(ev->iEventMsg == BUT_CLICKED) {
                    // Make an offer
                    Dnr_MakeOffer(rcb_kicks, 0);
                }
                break;

            case dng_drag10:
                if(ev->iEventMsg == BUT_CLICKED) {
                    // Make an offer
                    Dnr_MakeOffer(rcb_money, 10);
                }
                break;

            case dng_drag50:
                if(ev->iEventMsg == BUT_CLICKED) {
                    // Make an offer
                    Dnr_MakeOffer(rcb_money, 50);
                }
                break;
        }
    }
}


///////////////////
// Process the opponent stuff
void Dnr_ProcessOpponent(void)
{
    char buf[256];
    int  i;
    CVec p;

    switch(psDiner->nOppState) {

        // Waiting
        case opp_waiting:
            if( tMainSR3.fCurTime > psDiner->fOppTimer ) {
                // Roll in a new opponent
                Dnr_RollinOpponent();
            }
            break;


        // Rolling in
        case opp_rollingin:
            psDiner->pcLocation->setStatusText(psDiner->pcCurOpp->getDescription(buf));
            psDiner->cOppPosition += CVec(20,0,0)*tMainSR3.fDeltaTime;

            // Spin the tyres
            for( i=0; i<4; i++ ) {                
                CPart *p = psDiner->pcCurOpp->getCar()->getTire(i);
                if(p)
                    p->SpinTyre(2);
            }
            if( psDiner->cOppPosition.GetX() > 25 ) {
                psDiner->cOppPosition.SetX(25);

                // Setup the opponent ready state
                psDiner->bOppClicked = false;
                psDiner->nOppState = opp_ready;
                psDiner->fOppTimer = tMainSR3.fCurTime + 5;
            }
            break;


        // Ready
        case opp_ready:
            // Show the car description
            if( !psDiner->bOppClicked )
                psDiner->pcLocation->setStatusText(psDiner->pcCurOpp->getDescription(buf));

            // Has the opponent waited long enough for interaction?
            if( tMainSR3.fCurTime > psDiner->fOppTimer && !psDiner->bOppClicked ) {
                
                // Roll out the opponent
                psDiner->nOppState = opp_rollingout;
            }

            // Is the mouse over the car?
            UnprojectMouse(&p);
            if( psDiner->pcCurOpp->intersect(psDiner->pcLocation->getCamPos(), p) && !psDiner->bOppClicked ) {
                psDiner->pcLocation->setStatusText("Challenge opponent");

                if( System_GetMouse()->Up & SDL_BUTTON(1) ) {
                    // Challenge the opponent

                    Dnr_ChallengeOpponent(0, 0);
                }
            }
            break;


        // Rolling out
        case opp_rollingout:
            psDiner->cOppPosition += CVec(30,0,0)*tMainSR3.fDeltaTime;

            // Spin the tyres
            for( i=0; i<4; i++ ) {                
                CPart *p = psDiner->pcCurOpp->getCar()->getTire(i);
                if(p)
                    p->SpinTyre(3);
            }
            if( psDiner->cOppPosition.GetX() > 70 ) {

                psDiner->nOppState = opp_waiting;
                psDiner->fOppTimer = tMainSR3.fCurTime + (float)GetRandomInt(2)+3.0f;
            }
            break;
    }
}


///////////////////
// Draw the opponent
void Dnr_DrawOpponent(void)
{
    if( psDiner->nOppState == opp_waiting )
        return;

    psDiner->pcCurOpp->draw(psDiner->cOppPosition);

}


///////////////////
// Roll in a new opponent
void Dnr_RollinOpponent(void)
{
    // Choose a random opponent
    int num = GetRandomInt( cMainGuy->getNumOpponents()-1 );
    psDiner->pcCurOpp = cMainGuy->getOpponentList()+num;

    psDiner->nOppState = opp_rollingin;
    psDiner->cOppPosition = CVec(-10,-60,3.5);
    psDiner->bGoodOffer = false;

    // Rotate the opponent
    CMatrix m;
    m.Rotate(CVec(0,0,1),90);
    psDiner->pcCurOpp->zeroRotation();
    psDiner->pcCurOpp->rotate(m);
}


///////////////////
// Challenge the opponent
void Dnr_ChallengeOpponent(int nLevel, int nType)
{
    psDiner->bOppClicked = true;
    psDiner->bGoodOffer = false;

    // Setup the racing gui
    cDinerLayout.Initialize();

    // Level 1
    // Top level choice
    if(nLevel == 0) {
        cDinerLayout.Add( new CButton("Drag race",-1),          dng_drag,   335,230,0,  0);
        cDinerLayout.Add( new CButton("Road race",-1),          dng_road,   335,280,0,  0);
        cDinerLayout.Add( new CButton("Forget it",-1),          dng_cancel, 335,330,0,  0);
        cDinerLayout.Add( new CDialog("Race", dlgs_small),      dng_dlg,    0,  0,  230,200);
    }

    // Level 2
    // Location
    if(nLevel == 1) {
        // Drag
        if(nType == 0) {
            for(int i=0; i<psDiner->nNumDragTracks; i++) {
                cDinerLayout.Add(new CButton(psDiner->sDragTracks[i].szName,-1),   dng_dragtrk1+i,   335,230+i*5,0,  0);

                CButton *b = ((CButton *)cDinerLayout.getWidget(dng_dragtrk1+i));
                if(b)
                    b->setSize(10);
            }

            cDinerLayout.Add( new CButton("Forget it",-1),          dng_cancel, 335,330,0,  0);
            cDinerLayout.Add( new CDialog("Race", dlgs_small),      dng_dlg,    0,  0,  230,200);
        }
    }

    // Level 3
    // Bet
    if(nLevel == 2) {
        // Drag
        if(nType == 0) {
            cDinerLayout.Add( new CButton("For kicks",-1),          dng_dragkicks,   335,220,0,  0);
            cDinerLayout.Add( new CButton("$10",-1),                dng_drag10,      335,260,0,  0);
            cDinerLayout.Add( new CButton("$50",-1),                dng_drag50,      335,300,0,  0);            
            
            cDinerLayout.Add( new CButton("Back",-1),               dng_dragback,    335,350,0,  0);
            cDinerLayout.Add( new CDialog("Race", dlgs_small),      dng_dlg,    0,  0,  230,200);
        }

    }

    cDinerLayout.moveWidgets(-270, 180);
}


///////////////////
// Make an offer to the opponent
void Dnr_MakeOffer(int nType, int nMoney)
{
    // HACK: For now we accept all offers
    // In the future - when we have performance values working - we compare car performance
    // to accept/reject an offer

    // Accept the offer
    char    *szAcceptance[] = {"OK", "Lets go!"};

	// Do we have enough money?
	if(nType == rcb_money) {
		if(cMainGuy->getBankroll() < nMoney) {
			Dnr_InitSpeechBubble("You ain't got enough dough!");
			return;
		}
	}

    int i = GetRandomInt(1);
    Dnr_InitSpeechBubble(szAcceptance[i]);

    psDiner->bGoodOffer = true;
    sRaceInfo.nBetType = nType;
    sRaceInfo.nBetMoney = nMoney;
    sRaceInfo.pcOpponent = psDiner->pcCurOpp;
}


///////////////////
// LETS GO!!!!!!!!
void Dnr_LetsGo(void)
{
    psDiner->bRacing = true;
    psDiner->bProcessDiner = false;     // To leave the diner
}


///////////////////
// Shutdown the diner
void Dnr_Shutdown(void)
{
    cDinerLayout.Shutdown();
    cDinerLayout2.Shutdown();

    if( psDiner ) {

        if( psDiner->pcLocation ) {
            psDiner->pcLocation->Shutdown();
            delete psDiner->pcLocation;
        }

        psDiner->cSky.Shutdown();

        delete psDiner;
    }

    psDiner = NULL;
}


/*
========================

	 Speech Bubble

========================
*/


char        szDnrSpeech[256];
texture_t   *psDnrSpeech = NULL;


///////////////////
// Initialize the speech bubble
void Dnr_InitSpeechBubble(char *fmt, ...)
{
    va_list arg;

    psDnrSpeech = Cache_LoadTexture( "data/textures/speech.png" );
    Tex_Upload( psDnrSpeech );
    psDiner->bSpeechBubble = true;

    va_start(arg, fmt);
	vsprintf(szDnrSpeech, fmt, arg);
	va_end(arg);
    
    // Clear any mouse up's
    System_GetMouse()->Up = 0;
}


///////////////////
// Process & Draw the speech bubble
void Dnr_SpeechProcess(void)
{
    glColor4f(1,1,1,1);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    DrawPic( psDnrSpeech, 144, 500 );

    int size = 16;
    Font_SetSize(size);
    int length = strlen( szDnrSpeech ) * size;

    Font_Draw( 400 - length/2, 445, CVec(0,0,0), szDnrSpeech );

    if( (System_GetMouse()->Up & SDL_BUTTON(1)) || System_GetKeyboard()->KeyUp[SDLK_ESCAPE] ) {
        psDiner->bSpeechBubble = false;

        // If we have an offer, lets go!
        if(psDiner->bGoodOffer) {
            Dnr_LetsGo();
            return;
        }
    }
}
