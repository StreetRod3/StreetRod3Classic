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


// Garage - Parts
// Created 8/5/03
// Jason Boettcher


#include "defs.h"
#include "system.h"
#include "game.h"
#include "font.h"
#include "gui.h"


extern partchange_t	tPartChange;
extern CLocation	*cGarLocation;
extern CTaskBar	    cGarTaskb;
extern CTaskBar     cGarSection;
extern CSidebar     cSidebar;


// Part change dialog box
CGuiLayout cPartChange;


///////////////////
// Exits the change part state and goes to the garage
void Gar_PartChange2Garage(void)
{
	Gar_NormalView();
	tGarage->iState = GAR_NORMAL;
	tPartChange.iChange = false;
	
	cGarLocation->setItemCheck(true);
	
	if(tPartChange.cPart)
		tPartChange.cPart->setDrawNuts(false);
	
	cGarTaskb.Clear();
}


///////////////////
// Change a part on the running gear
// Callback function
void Gar_ChangeRunningGPartCallback(gitem_t *g)
{
    Gar_ChangeRunningGPart(g->cPart, true, g);

}


///////////////////
// Change a part on the running gear
void Gar_ChangeRunningGPart(CPart *pcPart, bool bRemoval, gitem_t *g)
{
	// Safety check
	if(cMainGuy->getCurCar() == NULL)
		return;


    // If the part has no nuts, remove the part
	/*if(pcPart->getNumNuts() == 0) {
		
		// Add to the players spare parts
		cMainGuy->addPart(pcPart);
		
		// Remove the part from the car
		CCar *cCar = cMainGuy->getCurCar();
		cCar->removeRunningGearPart(pcPart);
		
        if( g )
		    g->iUsed = false;

		Gar_Part2RunningG();
		return;
	}*/


	// Zoom the camera on the part
	
	// Find the direction the tire is facing
	if(pcPart->getType() == PRT_TIRE) {
		CVec right = CVec(-0.5f,1,0);
		right = pcPart->getMatrix().MultiplyVector(right);

		cGarLocation->startCameraMove(pcPart->getPos(), pcPart->getPos() + right*5 + CVec(0,0,2));
	} else {
		// Normal part
		CVec p = pcPart->getPos();

		Gar_MoveCamera(p, 10);
	}


	cGarTaskb.Clear();
    cGarSection.setEnabled(false);
    cSidebar.setEnabled(false);

	cGarLocation->setItemCheck(false);
	cGarLocation->setAngleLimit(false);

	tGarage->iState = GAR_RUNGPART;

	// Setup the part change
	tPartChange.iNutsGoingon = false;
	tPartChange.iChange = true;
	tPartChange.cCar = cMainGuy->getCurCar();
	tPartChange.cPart = pcPart;
	tPartChange.tItem = g;
	tPartChange.iNutsGoingon = !bRemoval;

    // Initialize the part change
    Gar_SetupPartChange();

	pcPart->setDrawNuts(true);
}


///////////////////
// Change the engine part
// callback function
void Gar_ChangeEnginePartCallback(gitem_t *g)
{
    Gar_ChangeEnginePart(g->cPart, true, g);
}


///////////////////
// Change an engine part
void Gar_ChangeEnginePart(CPart *pcPart, bool bRemoval, gitem_t *g)
{
	CCar *car = cMainGuy->getCurCar();

	// Safety check
	if(car == NULL)
		return;

	// Check if this part can be unbolted
	if(!car->checkPartRemoval(pcPart->getType())) {
		Gar_InitSpeechBubble("You cannot take that part off");
		return;
	}

	// Move the camera
	
	if( pcPart->getType() == PRT_BLOCK ) {
		// Block
		cGarLocation->startCameraMove(pcPart->getPos(), pcPart->getPos() + CVec(3,3,2));
	}
	else {
		// Normal part
		Gar_MoveCamera(pcPart->getPos(), 5);
	}

    cGarSection.setEnabled(false);
    cSidebar.setEnabled(false);

	cGarLocation->setAngleLimit(false);
	cGarLocation->setItemCheck(false);
	System_GetMouse()->Up = 0;

	// Add an engine pic to the taskbar
	cGarTaskb.Remove("parts");
	cGarTaskb.Remove("tuning");

	tGarage->iState = GAR_ENGINEPART;

	// Setup the part change
	tPartChange.iNutsGoingon = false;
	tPartChange.iChange = true;
	tPartChange.cCar = car;
	tPartChange.cPart = pcPart;
	tPartChange.tItem = g;
	tPartChange.iNutsGoingon = !bRemoval;

    // Initialize the part change
    Gar_SetupPartChange();

	pcPart->setDrawNuts(true);
}


CGuiLayout  cPartDlg;


///////////////////
// Setup a part change
void Gar_SetupPartChange(void)
{
    // Initialize the dialog box for the part change
    cPartDlg.Initialize();

    if( tPartChange.iNutsGoingon )
        cPartDlg.Add( new CButton("Add All",-1),		    1,  340,255,75, 25  );
    else
        cPartDlg.Add( new CButton("Remove",-1),		        1,  340,255,75, 25  );            

    cPartDlg.Add( new CButton("Repair",-1),		            2,  340,300,75, 25  );
	cPartDlg.Add( new CButton("Forget it",btn_forgit),      3,  340,345,120,25  );
    cPartDlg.Add( new CDialog("Part Change",dlgs_small),    4,  35, 25, 300,200 );

    cPartDlg.moveWidgets(240,-170);
}


///////////////////
// Change a part
void Gar_ChangePart(void)
{
	mouse_t *mouse = System_GetMouse();

    
    // Process an escape
    /*if( System_GetKeyboard()->KeyUp[SDLK_ESCAPE] ) {
        // Go up a level
		if(tGarage->iState == GAR_ENGINEPART) {
			Gar_Part2Engine();
		}
		if(tGarage->iState == GAR_RUNGPART) {
			tGarage->iState = GAR_RUNNINGG;
			Gar_Part2RunningG();
		}
        return;
    }*/

    // Draw the part information
    cGarLocation->drawPartInfo(tPartChange.cPart, 1);


    // Process the change part dialog box
    cPartDlg.Draw();
    gui_event_t *ev = cPartDlg.Process();
    if( ev ) {
        switch(ev->iControlID) {

            // Add all / Remove all
            case 1:
                if( ev->iEventMsg == BUT_CLICKED ) {

                    if( tPartChange.iNutsGoingon ) {
                        // Put all the nuts on
                        tPartChange.cPart->putNutsOn();
                        Gar_FinishAddPart();
                    } else {

                        // Remove all the nuts
                        Gar_RemovePart();
                    }

                    cPartDlg.Shutdown();
                    return;

                }
                break;

            // Repair
            case 2:
                if( ev->iEventMsg == BUT_CLICKED ) {

                    // Parts with more then 50% damage cannot be repaired
                    if( tPartChange.cPart->getDamage() >= 50 ) {
                        Gar_InitSpeechBubble("The part is too damaged to be repaired");
                        return;
                    }

                    // Can we afford to repair the part?                    
                    if( !cMainGuy->canBuy( tPartChange.cPart->repairCost() ) ) {
                        Gar_InitSpeechBubble("You ain't got enough cash buddy");
                        return;
                    }
                    
                    // Repair the part
                    int cost = tPartChange.cPart->repair();
                    cMainGuy->makePurchase(cost);
                }
                break;

            // Forget it
            case 3:
                if( ev->iEventMsg == BUT_CLICKED ) {

                    // If this part is coming off but was cancelled; put all the nuts back on
                    if( !tPartChange.iNutsGoingon )
                        tPartChange.cPart->putNutsOn();
                    else {
                        // Remove the part
                        Gar_RemovePart();
                    }


                    // Go up a level
		            if(tGarage->iState == GAR_ENGINEPART) {
			            Gar_Part2Engine();
		            }
		            if(tGarage->iState == GAR_RUNGPART) {
			            tGarage->iState = GAR_RUNNINGG;
			            Gar_Part2RunningG();
		            }

                    cPartDlg.Shutdown();
                    return;
                }
                break;
        }

    }


	CVec dir = tGarage->mousePoint - cGarLocation->getCamPos();
	VectorNormalize(&dir);

	// Check if any of the nuts are highlighted
	int n = tPartChange.cPart->LineIntersectNut(cGarLocation->getCamPos(),dir);

	if(n == -1)
		return;
	CNut *nut = tPartChange.cPart->getNut(n);

	// Left clicking does the work
	if(mouse->Up & SDL_BUTTON(1)) {

        // If the nuts are going on, left clicking on a nut puts the nut in
        if( tPartChange.iNutsGoingon ) {

            // Do ratchet noise
            if( !nut->getIn() )
		        BASS_SamplePlay(sfxParts.smpRatchet);

            nut->setIn(true);

            // If the nuts are all on the part, then we finish adding the part
            if( tPartChange.cPart->AllNutsOn() )
                Gar_FinishAddPart();
        } else {

            // If the nuts are coming off, left clicking on a nut removes it
            
            // Do ratchet noise
            if( nut->getIn() )
		        BASS_SamplePlay(sfxParts.smpRatchet);

            nut->setIn(false);

            // If all the nuts are gone, remove the part from the car
            // and put it into the player's spare parts
            if( tPartChange.cPart->AllNutsGone() )
                Gar_RemovePart();
        }
    }

    // Highlight the nut
	nut->setHighlight(true);
	cGarLocation->setStatusText(nut->getIn() ? "Undo nut" : "Put nut");
}


///////////////////
// Remove the part and put it into the spare parts bin
void Gar_RemovePart(void)
{
    // Add to the players spare parts
	cMainGuy->addPart(tPartChange.cPart);
		
    // Engine part
    if(tGarage->iState == GAR_ENGINEPART) {
        // If this part is attached to a parent part (block), let the parent part handle it
        if(tPartChange.cPart->getParent())
		    tPartChange.cPart->getParent()->removeAttached(tPartChange.cPart);
        else
		    tPartChange.cCar->removeEnginePart(tPartChange.cPart);
    } else {

        // Running gear part
        if(tGarage->iState == GAR_RUNGPART)
	        cMainGuy->getCurCar()->removeRunningGearPart(tPartChange.cPart);
    }

    if( tPartChange.tItem )
	    tPartChange.tItem->iUsed = false;

    // Go up a level
	if(tGarage->iState == GAR_ENGINEPART) {
		Gar_Part2Engine();
	}
	if(tGarage->iState == GAR_RUNGPART) {
		Gar_Part2RunningG();
	}

    BASS_SamplePlay(sfxParts.smpRemove);

    // Shutdown the dialog box
    cPartDlg.Shutdown();
}


///////////////////
// Finish adding the part
void Gar_FinishAddPart(void)
{
    // Go up a level
	if(tGarage->iState == GAR_ENGINEPART) {
		Gar_Part2Engine();
		//cGarTaskb.Remove("engine");
	}
	if(tGarage->iState == GAR_RUNGPART) {
		Gar_Part2RunningG();
	}

	tPartChange.cPart->setDrawNuts(false);

    BASS_SamplePlay(sfxParts.smpRemove);

    // Shutdown the dialog box
    cPartDlg.Shutdown();
}