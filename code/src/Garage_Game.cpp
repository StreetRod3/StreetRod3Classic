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


// Garage - Game
// Created 30/5/02
// Jason Boettcher


#include "defs.h"
#include "system.h"
#include "game.h"
#include "font.h"
#include "gui.h"


extern CLocation	*cGarLocation;
extern partchange_t	tPartChange;
extern CTaskBar		cGarTaskb;
extern CTaskBar		cGarSection;
extern CSidebar     cSidebar;

///////////////////
// Set the normal view
void Gar_NormalView(void)
{
	//cGarLocation->SetupCamera(CVec(4,5,-1.4f),30);
	//cGarLocation->SetupYaw(45,315);
	cGarLocation->SetupPitch(-5,26);
	cGarLocation->setItemCheck(true);
	cGarLocation->setAngleLimit(true);

	CVec pos = CVec(4,5,-1.4f);
	if(cMainGuy->getCurCar())
		pos = cMainGuy->getCurCar()->getPos();

    cGarLocation->startCameraMove( pos, CVec(29,5,5) );
	cGarLocation->unLockCamera();

    cGarSection.toggleItem("car");
    cGarSection.setEnabled(true);
}


///////////////////
// Toggle the car view
void Gar_CarView(void)
{
    // Engine
    if( tGarage->iState == GAR_ENGINE ) {
        Gar_Engine2Garage();
        cGarSection.toggleItem("car");
        cGarSection.setEnabled(true);
        return;
    }

    // Running gear
    if( tGarage->iState == GAR_RUNNINGG ) {
        Gar_Running2Garage();
        cGarSection.toggleItem("car");
        cGarSection.setEnabled(true);
        return;
    }
}


///////////////////
// Move the camera to a distance, while keeping the same angle
void Gar_MoveCamera(CVec look, float dist)
{
	CVec diff = cGarLocation->getCamPos() - cGarLocation->getCamLook();
	VectorNormalize(&diff);

	cGarLocation->startCameraMove( look, look+diff*dist );
}


///////////////////
// Set the garage items
void Gar_SetItems(void)
{
	CCar *car = cMainGuy->getCurCar();
	cGarLocation->ClearGameItems();
	gitem_t g;

	if(car == NULL) {
		// Leave
		return;

	}
	
	g.iType = GMI_CAROBJECT;
    g.cCar = car;

    // Add the hood to the locations game item list
	strcpy(g.sText, "Hood Customization");
	g.Use = Gar_CustomizeHood;    
    g.iMeshNum = car->getMeshi("hood");
	cGarLocation->AddGameItem(&g);

    // Add the 2 bumpers to the game item list
    strcpy(g.sText, "Cut/Repair the front bumper");
	g.Use = Gar_CutRepairBumper;
	g.iMeshNum = car->getMeshi("fbumper");	
	cGarLocation->AddGameItem(&g);

    strcpy(g.sText, "Cut/Repair the rear bumper");
	g.Use = Gar_CutRepairBumper;
	g.iMeshNum = car->getMeshi("rbumper");
	cGarLocation->AddGameItem(&g);
}



///////////////////
// Process the engine/running gear section
void Gar_ProcessEngRun(void)
{
    keyboard_t *kb = System_GetKeyboard();

    // Engine
    if( tGarage->iState == GAR_ENGINE ) {
        // If escape is pressed, go back to the garage view
        if( kb->KeyUp[SDLK_ESCAPE] ) {
            Gar_Engine2Garage();
            cGarSection.toggleItem("car");
        }
        return;
    }

    // Running gear
    if( tGarage->iState == GAR_RUNNINGG ) {
        // If escape is pressed, go back to the garage view
        if( kb->KeyUp[SDLK_ESCAPE] ) {
            Gar_Running2Garage();
            cGarSection.toggleItem("car");
        }
        return;
    }

}


CGuiLayout partchange;
int		partchange_mode = GAR_NORMAL;

///////////////////
// Init the 'put-part-onto-car' dialog
void Gar_InitPartCarDialog(void)
{
	partchange_mode = tGarage->iState;

	tPartChange.iChange = false;
	tGarage->iState = GAR_DIALOG;
	tGarage->DialogProcess = Gar_ProcessPartCarDialog;

	int lvid = 1;
	int dlgid = 4;
	
	partchange.Initialize();
	partchange.Add( new CListview(),					lvid,  180,200,440,200 );
	partchange.Add( new CButton("Done",btn_done),		2,     170,415,100,25  );
	partchange.Add( new CButton("Forget it",btn_forgit),3,     500,415,100,25  );
	partchange.Add( new CDialog("Parts", dlgs_large),   dlgid, 25, 25, 250,250 );

    cGarTaskb.Clear();


    // Get an array of filters of what parts can be used
    bool PartFilter[PRT_LAST];

    // Engine
    if(partchange_mode == GAR_ENGINE) {
        CCar *car = cMainGuy->getCurCar();
        if( car )
            car->createEnginePartFilter( PartFilter );
    }

    // Running gear
    if(partchange_mode == GAR_RUNNINGG) {
        CCar *car = cMainGuy->getCurCar();
        if( car )
            car->createRunningGFilter( PartFilter );
    }



	// Add columns to the listview
	CListview *lv = (CListview *)partchange.getWidget(lvid);
	if(lv) {
		lv->AddColumn("Name",350);
		lv->AddColumn("Damage",30);
	}

	// Add the parts to the listview
	CPart *part;
	char buf[64];
	for(int p=0;p<MAX_PARTS;p++) {
		part = cMainGuy->getSparePart(p);
		if(part) {
            if( PartFilter[part->getType()] ) {
			    lv->AddItem("",p);
			    lv->AddSubitem(LVS_TEXT,part->getString(buf),"");
			    //lv->AddSubitem(LVS_TEXT,part->getDamage,"");
            }
		}
	}	
}


///////////////////
// Process the part dialog
void Gar_ProcessPartCarDialog(void)
{
	gui_event_t *event;
    char        msg[128];
	CCar *car = cMainGuy->getCurCar();

	// Safety check
	if(car == NULL)
		return;

	event = partchange.Process();

	// Draw the layout
	partchange.Draw();
	
	if(event == NULL)
		return;

	switch(event->iControlID) {

		// Forget it
		case 3:
			tGarage->iState = partchange_mode;
			partchange.Shutdown();

            if( partchange_mode == GAR_ENGINE )
                Gar_Part2Engine();
            else if( partchange_mode == GAR_RUNNINGG )
                Gar_Part2RunningG();
			break;

		// Done
		case 2:
			// Add the part onto the car
			CListview *lv = (CListview *)partchange.getWidget(1);
			
			int cur = lv->getCurIndex();
            if( cur == -1 )
                break;
			if(cur != -1) {

				CPart *part = cMainGuy->getSparePart(cur);

				// Engine part
				if(partchange_mode == GAR_ENGINE) {

                    // Do some checks before hand

                    // If this part goes through the hood, but the hood isn't a performance hood, show a message
                    if( part->requiresHoodHole() && car->getHoodType() == HOOD_NORMAL ) {

                        Gar_InitSpeechBubble("You need a performance hood");

                        Gar_Part2Engine();
                        partchange.Shutdown();
                        break;
                    }

					if(car->AddEnginePart(part, msg)) {

						// Remove the nuts from the part
						if(part) {
							part->ClearNuts();
							part->setDrawNuts(false);
						}

						cMainGuy->removeSparePart(cur);

						// Clear the location handler, and re-add the engine parts
						cGarLocation->ClearGameItems();
						Gar_SetEngineItems();
                        
                        // Zoom in on the part to put the nuts on if it has nuts
                        // Otherwise just go back to the engine screen

                        if( part->getNumNuts() > 0 )
                            Gar_ChangeEnginePart(part, false, NULL);
                        else
                            Gar_Part2Engine();

                        
                    } else {
                        // Show the error
                        if( msg[0] )
                            Gar_InitSpeechBubble(msg);

                        // Back to engine
                        Gar_Part2Engine();
                    }

				} 
				else
				// Running gear parts
				if(partchange_mode == GAR_RUNNINGG) {
					if(car->AddRunningGPart(part, msg)) {

						// Remove the nuts from the part
						if(part) {
							part->ClearNuts();
							part->setDrawNuts(false);
						}

						cMainGuy->removeSparePart(cur);

                        // Zoom in on the part to put the nuts on if it has nuts
                        // Otherwise just go back to the running gear

                        if( part->getNumNuts() > 0 )
                            Gar_ChangeRunningGPart(part, false, NULL);
                        else
                            Gar_Part2RunningG();

                    } else {
                        // Show the error
                        if( msg[0] )
                            Gar_InitSpeechBubble(msg);

                        // Go back to the running gear
					    Gar_Part2RunningG();
                    }
				}

                partchange.Shutdown();
            }			
			break;
	}
}



/*
========================

	Engine section

========================
*/


///////////////////
// Show the engine screen
void Gar_InitEngineScreen(void)
{
    CCar *pcCar = cMainGuy->getCurCar();

	// Safety check
	if(pcCar == NULL)
		return;

	tGarage->iState = GAR_ENGINE;


	// Make the car body hidden
	pcCar->getModel()->addMeshFlag(MSH_HIDDEN);

	// Setup the camera distance, turn off limiting
	gobject_t *obj = pcCar->getModel()->findGameObject("$block");
	if(obj==NULL) {
		// Something wrong
		return;
	}

	// Hide the running gear parts
    pcCar->setRunningGearVis(false);
    pcCar->setEngineVis(true);
	

	// Car coords -> world coords
	CVec v = pcCar->TranslateVector(obj->vPosition);

	//cGarLocation->SetupCamera(v,10);
	cGarLocation->startCameraMove(v, v + CVec(6,6,5));

	cGarLocation->setAngleLimit(false);
	cGarLocation->setItemCheck(true);

	// Add functions to the task bar
	cGarTaskb.Clear();
	cGarTaskb.Add("data/gui/but_parts.png","parts",Gar_InitPartCarDialog);
	cGarTaskb.Add("data/gui/but_tuning.png","tuning",Gar_InitTuning);

    // Toggle the engine button
    cGarSection.toggleItem("engine");

    // Disable some items on the sidebar
    cSidebar.setEnabled(false);
    
	
	
	// Clear the items, and add the engine parts to it
	cGarLocation->ClearGameItems();
	Gar_SetEngineItems();
}


///////////////////
// Set the engine items
void Gar_SetEngineItems(void)
{
	gitem_t item;
	CCar *car = cMainGuy->getCurCar();

	// Safety check
	if(car == NULL)
		return;
	
	item.iType = GMI_PART;
	strcpy(item.sText, "Remove part");
	item.Use = Gar_ChangeEnginePartCallback;
	item.cCar = car;
	

	// Add the engine parts	

	// Block
	item.cPart = car->getBlock();
	if(item.cPart) {
		cGarLocation->AddGameItem(&item);

		// Add the blocks attached parts
		cMainGuy->getCurCar()->getBlock()->AddAttached(&item,cGarLocation);
	}

}


///////////////////
// Go from the engine-part to the engine screen
void Gar_Part2Engine(void)
{
	CCar *car = cMainGuy->getCurCar();

	// Safety check
	if(car == NULL)
		return;

	// Setup the camera distance, turn off limiting
	gobject_t *obj = car->getModel()->findGameObject("$block");
	if(obj==NULL) {
		// Something wrong
		return;
	}

	// Car coords -> world coords
	CVec v = car->TranslateVector(obj->vPosition);
	car->SwitchEngineOff();

	//cGarLocation->SetupCamera(v,10);
	//cGarLocation->startCameraMove(v, v + CVec(6,6,5));
	Gar_MoveCamera(v, 10);

	cGarLocation->setAngleLimit(false);
	cGarLocation->setItemCheck(true);
	tGarage->iState = GAR_ENGINE;
	
    cGarTaskb.Clear();
	cGarTaskb.Add("data/gui/but_parts.png","parts",Gar_InitPartCarDialog);
	cGarTaskb.Add("data/gui/but_tuning.png","tuning",Gar_InitTuning);    

	tPartChange.iChange = false;
	if(tPartChange.cPart)
		tPartChange.cPart->setDrawNuts(false);

    cGarSection.setEnabled(true);
    cSidebar.setEnabled(false);

	// Show parts button

	
	// Clear the items, and add the engine parts to it
	cGarLocation->ClearGameItems();
	Gar_SetEngineItems();
}


///////////////////
// Go from the engine screen to the garage
void Gar_Engine2Garage(void)
{
	CCar *car = cMainGuy->getCurCar();

	// Safety check
	if(car == NULL)
		return;

	tGarage->iState = GAR_NORMAL;

	// Clear the taskbar
	cGarTaskb.Clear();	
	
	car->getModel()->removeMeshFlag(MSH_HIDDEN);

    // Make sure the hood hole is still hidden if need be
    car->setupHood();
    
    // Make sure the bumpers are still invisible if need be
    if( !car->getFrontBumper() )
        car->removeBumper(0);
    if( !car->getRearBumper() )
        car->removeBumper(1);


    // Show the running gear
    car->setEngineVis(true);
    car->setRunningGearVis(true);

    // Re-enable the sidebar
    cSidebar.enableAll(true);
    cSidebar.setEnabled(true);


	// Go back to normal view
	Gar_NormalView();

	// reset the normal items
	Gar_SetItems();
}



/*
========================

  Painting the car

========================
*/

CGuiLayout cPaint;
colour_t oldColour;

///////////////////
// Start the paint dialog box
void Gar_InitPaintDialog(void)
{
	CCar *car = cMainGuy->getCurCar();

	// Don't show the paint dialog if we ain't go no car
    if(car == NULL) {
	    Gar_InitSpeechBubble("You got no car!");
        return;
    }

    // If we can't afford it, display a message
    if( !cMainGuy->canBuy(15) ) {
        Gar_InitSpeechBubble("Paint ain't free");
        return;
    }


    // Save the old colour
    oldColour = car->getColour();

	tPartChange.iChange = false;
	tGarage->iState = GAR_DIALOG;
	tGarage->DialogProcess = Gar_ProcessPaintDialog;

	cPaint.Initialize();
    cPaint.Add( new CColourPicker(),                4,     338,235,125,125 );	
	cPaint.Add( new CButton("Done",btn_done),		3,     255,365,75, 25  );
	cPaint.Add( new CButton("Forget it",btn_forgit),2,     420,365,120,25  );
	cPaint.Add( new CDialog("Paint", dlgs_small),   1,     35, 25, 325,225 );

    cPaint.moveWidgets(0,-160);
}


///////////////////
// Process the paint dialog box
void Gar_ProcessPaintDialog(void)
{
	CCar *car = cMainGuy->getCurCar();

	gui_event_t *event;
	event = cPaint.Process();

	// Safety check
	if(car == NULL)
		return;

	// Draw the layout
	cPaint.Draw();

	// Draw the colour
	State_Disable(GL_TEXTURE_2D);
	State_Disable(GL_BLEND);

	//CColourSlider *c = (CColourSlider *)cPaint.getWidget(4);
	//CVec col = c->getColour();
    CVec col = ((CColourPicker *)cPaint.getWidget(4))->getCurrentColour();


    /*int x = cPaint.getWidget(1)->getX()+390;
    int y = cPaint.getWidget(1)->getY()-107;
    int w = 60;
    int h = 60;

	glColor4f(1,1,1,1);
	glBegin(GL_QUADS);
	glVertex2i(x-2,  y-2);
	glVertex2i(x-2,  y+h+2);
	glVertex2i(x+w+2,y+h+2);
	glVertex2i(x+w+2,y-2);
	glEnd();

	glColor4f(col.GetX(), col.GetY(), col.GetZ(), 1);
	glBegin(GL_QUADS);
	glVertex2i(x,  y);
	glVertex2i(x,  y+h);
	glVertex2i(x+w,y+h);
	glVertex2i(x+w,y);
	glEnd();*/

    car->PaintCar(col,false);
	
	if(event == NULL)
		return;

	switch(event->iControlID) {
     
		// Forget it
		case 2:
            car->PaintCar( CVec(oldColour.R, oldColour.G, oldColour.B), false );
			tGarage->iState = GAR_NORMAL;
			cPaint.Shutdown();
			break;

		// Done
		case 3:
            if( cMainGuy->makePurchase(15) )
			   car->PaintCar(col,true);            
			tGarage->iState = GAR_NORMAL;
			cPaint.Shutdown();
            
            BASS_SamplePlay(sfxParts.smpPaint);
			break;
	}
}



/*
========================

  Tuning the engine

========================
*/


///////////////////
// Initialize the engine tuning
void Gar_InitTuning(void)
{
	CVec look;
	CCar *car = cMainGuy->getCurCar();
    CPart *block = NULL;

	// Safety check
	if(car == NULL)
		return;

    block = car->getBlock();

	// Do we have an engine?
    if(!block) {
        Gar_InitSpeechBubble("You need an engine to tune!");
        return;
    }

    // Is the engine assembled?
    if(!block->isComplete()) {
        Gar_InitSpeechBubble("You can't tune that mess klutz!");
        return;
    }


	// Next, centre the camera on the tuning items
	look = block->getPos();

	cGarTaskb.Clear();
	
	//cGarLocation->SetupCamera(look,3);
	cGarLocation->startCameraMove( look, look+CVec(2,5,2));

	cGarLocation->setAngleLimit(false);
	cGarLocation->setItemCheck(false);
	System_GetMouse()->Up = 0;

	tGarage->iState = GAR_TUNING;

	car->StartEngine();
}


///////////////////
// Process the tuning
void Gar_TuningProcess(void)
{
	CCar *car = cMainGuy->getCurCar();

	// Safety check
	if(car == NULL)
		return;

    // Process an escape
    if( System_GetKeyboard()->KeyUp[SDLK_ESCAPE] ) {
        Gar_Part2Engine();
        return;
    }

	car->SimulateEngine();
}






/*
========================

  Spare parts bin

========================
*/


CGuiLayout cSpareParts;
CGuiLayout cSparePartsSell;
int sparepartState = 0;
int spare_sellingprice = 0;


///////////////////
// Initialize the spare parts bin
void Gar_InitSpareparts(void)
{
	tPartChange.iChange = false;
	tGarage->iState = GAR_DIALOG;
	tGarage->DialogProcess = Gar_SparepartsProcess;

	int lvid = 1;
	int dlgid = 4;

	sparepartState = 0;
	
	cSpareParts.Initialize();
	cSpareParts.Add( new CListview(),						0, 180,200,440,200 );
	cSpareParts.Add( new CButton("Sell it",btn_sellit),		1, 170,415,100,25  );
	cSpareParts.Add( new CButton("Forget it",btn_forgit),	2, 500,415,120,25  );
	cSpareParts.Add( new CDialog("Parts", dlgs_large),		3, 25, 25, 250,250 );

	// Add columns to the listview
	CListview *lv = (CListview *)cSpareParts.getWidget(0);
	if(lv) {
		lv->AddColumn("Name",350);
		lv->AddColumn("Damage",30);
	}

	// Add the parts to the listview
	CPart *part;
	char buf[64];
	for(int p=0;p<MAX_PARTS;p++) {
		part = cMainGuy->getSparePart(p);
		if(part) {
			lv->AddItem("",p);
			lv->AddSubitem(LVS_TEXT,part->getString(buf),"");
			//lv->AddSubitem(LVS_TEXT,part->getDamage,"");
		}
	}
}


///////////////////
// Process the spare parts dialog
void Gar_SparepartsProcess(void)
{
	gui_event_t *event;
	CPart		*part;
	CListview	*lv;
	char		buf[128];
	int			price=0;



	// Choose a part
	if(sparepartState == 0) {
		event = cSpareParts.Process();

		// Draw the layout
		cSpareParts.Draw();
	
		if(event == NULL)
			return;


		switch(event->iControlID) {

			// Sell it
			case 1:
				lv = (CListview *)cSpareParts.getWidget(0);
				part = cMainGuy->getSparePart( lv->getCurIndex() );
				if(part) {
					price = part->CalculateSelling(10.0f);
					spare_sellingprice = price;
					sparepartState = 1;
					sprintf(buf,"Sell '%s' for $%d?", part->getName(), price);
	
					cSparePartsSell.Initialize();
					cSparePartsSell.Add( new CLabel(buf),					0, 400, 290, 0, 0);
					cSparePartsSell.Add( new CButton("Yeah",btn_yeah),		1, 180, 320, 75,  20);
					cSparePartsSell.Add( new CButton("No thanks",btn_notha),2, 495, 320, 125, 20);
					cSparePartsSell.Add( new CDialog("", dlgs_medium),		3, 125, 180, 600, 200 );

                    ((CLabel *)cSparePartsSell.getWidget(0))->setCenter(true);
				}
				break;

			// Forget it
			case 2:
				cSpareParts.Shutdown();
				tGarage->iState = GAR_NORMAL;
				break;
		}
	} else

	// Sell a part
	if(sparepartState == 1) {
		event = cSparePartsSell.Process();

		cSparePartsSell.Draw();

		if(event == NULL)
			return;

		switch(event->iControlID) {

			// No thanks
			case 2:
				sparepartState = 0;
				cSparePartsSell.Shutdown();
				break;

			// Yeah!
			case 1:
				lv = (CListview *)cSpareParts.getWidget(0);
				cMainGuy->SellSparePart( lv->getCurIndex(), spare_sellingprice );

				sparepartState = 0;
				cSparePartsSell.Shutdown();

				// Shutdown the layout, and re-update it
				cSpareParts.Shutdown();
				Gar_InitSpareparts();
				break;
		}

	}
}




/*
========================

		Car list

========================
*/


CGuiLayout cCarList;


///////////////////
// Initialize the car list dialog
void Gar_InitCarList(void)
{
	tPartChange.iChange = false;
	tGarage->iState = GAR_DIALOG;
	tGarage->DialogProcess = Gar_CarListProcess;

	cCarList.Initialize();
	cCarList.Add( new CListview(),						0, 200,200,400,200 );
	cCarList.Add( new CButton("Sell it",btn_sellit),	1, 170,415,100, 25 );
	cCarList.Add( new CButton("Swap it",btn_swapit),	2, 335,415,100, 25 );
	cCarList.Add( new CButton("Forget it",btn_forgit),	3, 500,415,120, 25 );
	cCarList.Add( new CDialog("Cars",dlgs_large),		4, 25, 25, 250, 250 );

	// Add columns to the listview
	CListview *lv = (CListview *)cCarList.getWidget(0);
	if(lv) {
		lv->AddColumn("Name",155);
	}

	// Add my cars to the list
	CCar *car = cMainGuy->getCarList();
	int count = 0;
	for(; car; car=car->getNext(), count++ ) {

		lv->AddItem("",car->getID());
		lv->AddSubitem(LVS_TEXT,car->getName(),"");
	}
}


///////////////////
// Process the car list dialog
void Gar_CarListProcess(void)
{
	gui_event_t *event;
	CPart		*part;
	CListview	*lv;
	CCar		*car;
	char		buf[128];
	int			price=0, id;

	event = cCarList.Process();
	cCarList.Draw();

	lv = (CListview *)cCarList.getWidget(0);

	if(event == NULL)
		return;

	switch(event->iControlID) {

        // Sell the car
        case 1:
            id = lv->getCurIndex();
            car = cMainGuy->findCar(id);
			if(car) {
                tGarage->iState = GAR_NORMAL;
                cCarList.Shutdown();

                // Sell the car
                Gar_InitSellCar(car);
                return;
			}
            break;

		// Swap the cars
		case 2:
			// Make sure the car isn't the same as what i've currently got
			id = lv->getCurIndex();
			if(cMainGuy->getCurCar()) {
				// Same car, don't swap
				if(id == cMainGuy->getCurCar()->getID())
					break;
			}

			car = cMainGuy->findCar(id);
			if(car) {
				// Swap the car
				cCarList.Shutdown();
				tGarage->iState = GAR_NORMAL;
				Gar_SwapCars(car);
			}
			break;

		// Forget it
		case 3:
			cCarList.Shutdown();
			tGarage->iState = GAR_NORMAL;
			break;
	}
}




/*
========================

	  Running Gear

========================
*/


///////////////////
// Initialize the running gear section
void Gar_InitRunningGear(void)
{
	// No car, so leave
	if(!cMainGuy->getCurCar())
		return;

	tGarage->iState = GAR_RUNNINGG;

	// Zoom out on the whole car
	CVec pos = CVec(4,5,-1.4f);
	if(cMainGuy->getCurCar())
		pos = cMainGuy->getCurCar()->getPos();

	Gar_MoveCamera(pos, 20);
	
	//cGarLocation->setAngleLimit(true);


	// Hide the car & engine
	CCar *cCar = cMainGuy->getCurCar();

	cCar->getModel()->addMeshFlag(MSH_HIDDEN);
	
    cCar->setRunningGearVis(true);
    cCar->setEngineVis(false);
	
	cGarTaskb.Clear();
	cGarTaskb.Add("data/gui/but_parts.png","parts",Gar_InitPartCarDialog);
	
    // Disable some items on the sidebar
    cSidebar.setEnabled(false);
    /*cSidebar.enableAll(true);
    cSidebar.setItemEnabled(false,0);   // Paint
    cSidebar.setItemEnabled(false,1);   // Cars*/
	
	// Add only running gear objects to the game object list
	cGarLocation->ClearGameItems();
	Gar_SetRunningGItems();

    cGarSection.toggleItem("runningg");
}


///////////////////
// Set the running clear items
void Gar_SetRunningGItems(void)
{
    gitem_t g;
    int     i;

	// No car, so leave
	if(!cMainGuy->getCurCar())
		return;

	CCar *car = cMainGuy->getCurCar();

	// Add the 4 tires to the location's game item list
	//g;
	g.iType = GMI_PART;
	strcpy(g.sText, "Change tire");
	g.Use = Gar_ChangeRunningGPartCallback;
	g.cCar = car;

	for(i=0;i<4;i++) {
		g.iID = i;
		g.cPart = car->getTire(i);
		if(g.cPart)
			cGarLocation->AddGameItem(&g);
	}


    // Add the 4 shocks to the location's game item list
	g.iType = GMI_PART;
	strcpy(g.sText, "Change shock");
	g.Use = Gar_ChangeRunningGPartCallback;
	g.cCar = car;

	for(i=0;i<4;i++) {
		g.iID = i;
		g.cPart = car->getShock(i);
		if(g.cPart)
			cGarLocation->AddGameItem(&g);
	}	



	// Add the 2 mufflers
	g.iType = GMI_PART;
	strcpy(g.sText, "Change muffler");
	g.Use = Gar_ChangeRunningGPartCallback;
	g.cCar = car;

	for(i=0;i<2;i++) {
		g.iID = i;
		g.cPart = car->getMuffler(i);
		if(g.cPart)
			cGarLocation->AddGameItem(&g);
	}


    // Add the differential
    g.iType = GMI_PART;
	strcpy(g.sText, "Change differential");
	g.Use = Gar_ChangeRunningGPartCallback;
	g.cCar = car;
    g.cPart = car->getDiff();
    if(g.cPart)
	    cGarLocation->AddGameItem(&g);


    // Add the transmission
    g.iType = GMI_PART;
	strcpy(g.sText, "Change transmission");
	g.Use = Gar_ChangeRunningGPartCallback;
	g.cCar = car;
    g.cPart = car->getTrans();
    if(g.cPart)
	    cGarLocation->AddGameItem(&g);

}


///////////////////
// Move the camera from part back to the running gear
void Gar_Part2RunningG(void)
{
	// No car, so leave
	if(!cMainGuy->getCurCar())
		return;

    tPartChange.iChange = false;
	if(tPartChange.cPart)
		tPartChange.cPart->setDrawNuts(false);

	cGarLocation->setItemCheck(true);

	tGarage->iState = GAR_RUNNINGG;

	// Zoom out on the whole car
	CVec pos = CVec(4,5,-1.4f);
	if(cMainGuy->getCurCar())
		pos = cMainGuy->getCurCar()->getPos();

	Gar_MoveCamera(pos, 20);

	cGarTaskb.Clear();
	cGarTaskb.Add("data/gui/but_parts.png","parts",Gar_InitPartCarDialog);

    cGarSection.setEnabled(true);
    cSidebar.setEnabled(false);

	// Add only running gear objects to the game object list
	cGarLocation->ClearGameItems();
	Gar_SetRunningGItems();
}


///////////////////
// Move the camera from a part straight to the garage
void Gar_RunningGPart2Garage(void)
{
	// No car, so leave
	if(!cMainGuy->getCurCar())
		return;

	if(tPartChange.cPart)
		tPartChange.cPart->setDrawNuts(false);

	cGarLocation->setItemCheck(true);
	cGarSection.setEnabled(true);

	Gar_Running2Garage();	
}


///////////////////
// Switch from running gear to Garage mode
void Gar_Running2Garage(void)
{
	tGarage->iState = GAR_NORMAL;

	// Move back to normal
	Gar_NormalView();


	// Show the car & engine
	CCar *cCar = cMainGuy->getCurCar();
	if(cCar) {
		cCar->getModel()->removeMeshFlag(MSH_HIDDEN);
        cCar->setEngineVis(true);

        // Make sure the hood hole is still hidden if need be
        cCar->setupHood();

        // Make sure the bumpers are still invisible if need be
        if( !cCar->getFrontBumper() )
            cCar->removeBumper(0);
        if( !cCar->getRearBumper() )
            cCar->removeBumper(1);
	}

    cSidebar.setEnabled(true);
    cSidebar.enableAll(true);
    cGarSection.setEnabled(true);
	cGarTaskb.Clear();	

	cGarLocation->ClearGameItems();
	Gar_SetItems();
}



/*
========================

	Car rolling in/out

========================
*/


// Car rolling modes
enum {
	ROLL_ROLLINGOUT,
	ROLL_ROLLINGIN
};

carrolling_t    sCarRolling;

int		GarCarRolling = 0;
int		GarSwap = false;
CCar	*cNewCar = NULL;


///////////////////
// Setup the garage for a car rollin
void Gar_CarRollin(void)
{
	tGarage->iState = GAR_CARROLLING;
	tPartChange.iChange = false;
	
    sCarRolling.CarRolling = ROLL_ROLLINGIN;
    /*CVec(18,30,7)*/

	cGarLocation->setCamPos( CVec(29,5,5) );
	cGarLocation->lockCamera();
	cMainGuy->getCurCar()->startGarageRollIn();
}


///////////////////
// Setup the garage for a car swap
void Gar_SwapCars(CCar *newcar)
{
	// If we currently don't have a car, just roll the other car in
	if(!cMainGuy->getCurCar()) {
		cMainGuy->setCurCar(newcar);
		Gar_CarRollin();
		return;
	}

	tGarage->iState = GAR_CARROLLING;
	tPartChange.iChange = false;	

    sCarRolling.CarRolling = ROLL_ROLLINGOUT;
    sCarRolling.cNewCar = newcar;
    sCarRolling.Swap = true;
    sCarRolling.bSell = false;
	
	cGarLocation->setCamPos( CVec(29,5,5) );
	cGarLocation->lockCamera();
	cMainGuy->getCurCar()->startGarageRollOut();
}


///////////////////
// Setup the garage for a car rollout only
void Gar_CarRollOut(bool bSellCar)
{
    tGarage->iState = GAR_CARROLLING;
	tPartChange.iChange = false;

    sCarRolling.CarRolling = ROLL_ROLLINGOUT;
    sCarRolling.cNewCar = NULL;
    sCarRolling.Swap = false;
    sCarRolling.bSell = bSellCar;

	
	cGarLocation->setCamPos( CVec(29,5,5) );
	cGarLocation->lockCamera();
	cMainGuy->getCurCar()->startGarageRollOut();
}


///////////////////
// Process a car roll
void Gar_ProcessCarRoll(void)
{
	CCar *car = cMainGuy->getCurCar();
    keyboard_t  *kb = System_GetKeyboard();
	
	// Safety check
	if(!car)
		return;

	// Rolling In
	if(sCarRolling.CarRolling == ROLL_ROLLINGIN) {
		car->processGarageRollIn();	

        // Was escape pressed?
        if( kb->KeyUp[SDLK_ESCAPE] ) {
            car->finishGarageRollIn();
            cGarLocation->SetupCamera( car->getPos(), 20);
        }

		// If the car is no longer rolling, reset the camera
        if( !car->getRollingIn() ) {            
			Gar_NormalView();
			Gar_SetItems();
			tGarage->iState = GAR_NORMAL;
		}
	}
	if(sCarRolling.CarRolling == ROLL_ROLLINGOUT) {
		// Rolling out
		car->processGarageRollOut();

        // Was escape pressed?
        if( kb->KeyUp[SDLK_ESCAPE] ) {
            car->finishGarageRollOut();
            cGarLocation->SetupCamera( car->getPos(), 20);
        }

		// If the car is no longer rolling, and we're swapping the car, roll the new car in
		if( !car->getRollingOut() ) {            

			// Not swapping
			if(!sCarRolling.Swap || !sCarRolling.cNewCar) {
				Gar_NormalView();
				Gar_SetItems();
				tGarage->iState = GAR_NORMAL;

                // If we're selling, finish the sale
                if(sCarRolling.bSell)
                    Gar_FinishCarSell();

			} else {
				// Swapping, rolling in the new car				
				cMainGuy->setCurCar(sCarRolling.cNewCar);
				Gar_CarRollin();
			}
		}
	}


	// Look at the car
	cGarLocation->SetupCamera( /*car->getPos()*/CVec(4,5,-1.4f), 20);
}



/*
========================

   Hood Customizations

========================
*/


CGuiLayout cCutHood;
int HoodCost = 30;


///////////////////
// Customize the hood
void Gar_CustomizeHood(gitem_t *g)
{
   	CCar *car = cMainGuy->getCurCar();
    	
	// Safety check
	if(!car)
		return;

    // Can the player afford the customization?
    if( !cMainGuy->canBuy(HoodCost) ) {
        Gar_InitSpeechBubble("Customization ain't free");
        return;
    }


    // If the airfilter is sticking through the hood, we can't use a normal hood
    CPart *b = car->getBlock();
    if( b && car->getHoodType() != HOOD_NORMAL ) {
        if( b->requiresHoodHole() ) {
            Gar_InitSpeechBubble("The engine needs a performance hood");
            return;
        }
    }


    // Show a dialog verifying a cut or a repair
    tPartChange.iChange = false;
	tGarage->iState = GAR_DIALOG;
	tGarage->DialogProcess = Gar_CustomizeHoodProcess;
	
    char buf[128];
    if( car->getHoodType() == HOOD_PERFORMANCE )
        sprintf(buf,"Get regular hood for $%d?",HoodCost);
    else
        sprintf(buf,"Get performance hood for $%d?",HoodCost);

    cCutHood.Initialize();
    cCutHood.Add( new CLabel(buf),					     -1,400, 280, 0,   0 );
	cCutHood.Add( new CButton("Yeah",btn_yeah),			 0, 180, 320, 100, 20 );
    cCutHood.Add( new CButton("Forget it",btn_forgit),   1, 495, 320, 125, 20 );
	cCutHood.Add( new CDialog("Hood",dlgs_medium),	     2, 25,  25,  250, 250 );

    ((CLabel *)cCutHood.getWidget(-1))->setCenter(true);
}


///////////////////
// Process the hood customization
void Gar_CustomizeHoodProcess(void)
{
    CCar *car = cMainGuy->getCurCar();
	
	// Safety check
	if(!car)
		return;

	gui_event_t *event;
	
	event = cCutHood.Process();
	cCutHood.Draw();


	if(event == NULL)
		return;

	switch(event->iControlID) {

		// Yeah
		case 0:
			cCutHood.Shutdown();
			tGarage->iState = GAR_NORMAL;

            // Do the customization
            if( car->getHoodType() == HOOD_PERFORMANCE )
                car->setHoodType(HOOD_NORMAL);
            else
                car->setHoodType(HOOD_PERFORMANCE);

            // Do the financial transaction
            cMainGuy->makePurchase(HoodCost);
			break;

        
        // Forget it
        case 1:
            cCutHood.Shutdown();
			tGarage->iState = GAR_NORMAL;
            break;
	}
}




/*
========================

   Bumper Bar repairs

========================
*/


CGuiLayout cCutBumper;
int BumperCost = 30;
int BumperID = -1;


///////////////////
// Cut/Repair a bumper bar
void Gar_CutRepairBumper(gitem_t *g)
{
   	CCar *car = cMainGuy->getCurCar();
    	
	// Safety check
	if(!car)
		return;

    // Can the player afford the customization?
    if( !cMainGuy->canBuy(BumperCost) ) {
        Gar_InitSpeechBubble("Customization ain't free");
        return;
    }

     // Which bumper is it?
    if( g->iMeshNum == car->getMeshi("fbumper") )
        BumperID = 0;
    else if( g->iMeshNum == car->getMeshi("rbumper") )
        BumperID = 1;
    else {
        // BAD!
        writeLog("BAD: Bumper Bar ID doesn't match up!");
        return;
    }
	

    // Show a dialog verifying a cut or a repair
    tPartChange.iChange = false;
	tGarage->iState = GAR_DIALOG;
	tGarage->DialogProcess = Gar_CutRepairBumperProcess;

    char buf[128];
    if( (BumperID == 0 && !car->getFrontBumper()) || (BumperID == 1 && !car->getRearBumper()) )
        sprintf(buf,"Repair the bumper for $%d?",BumperCost);
    else
        sprintf(buf,"Remove the bumper for $%d?",BumperCost);

    cCutBumper.Initialize();
    cCutBumper.Add( new CLabel(buf),					        -1,400, 280, 0,   0 );
	cCutBumper.Add( new CButton("Yeah",btn_yeah),			    0, 180, 320, 100, 20 );
    cCutBumper.Add( new CButton("Forget it",btn_forgit),        1, 495, 320, 125, 20 );
	cCutBumper.Add( new CDialog("Bumper",dlgs_medium),	        2, 25,  25,  250, 250 );

    ((CLabel *)cCutBumper.getWidget(-1))->setCenter(true);
}


///////////////////
// Process the cut/repair dialog box
void Gar_CutRepairBumperProcess(void)
{
    CCar *car = cMainGuy->getCurCar();
	
	// Safety check
	if(!car)
		return;

	gui_event_t *event;
	
	event = cCutBumper.Process();
	cCutBumper.Draw();


	if(event == NULL)
		return;

	switch(event->iControlID) {

		// Yeah
		case 0:
			cCutBumper.Shutdown();
			tGarage->iState = GAR_NORMAL;

            // Do the customization
            if( BumperID == 0 ) {
                if( car->getFrontBumper() )
                    car->removeBumper(0);
                else
                    car->repairBumper(0);
            }

            if( BumperID == 1 ) {
                if( car->getRearBumper() )
                    car->removeBumper(1);
                else
                    car->repairBumper(1);
            }
        
            // Do the financial transaction
            cMainGuy->makePurchase(BumperCost);
			break;

        
        // Forget it
        case 1:
            cCutBumper.Shutdown();
			tGarage->iState = GAR_NORMAL;
            break;
	}
}




/*
========================

	 Speech Bubble

========================
*/


char        szSpeech[256];
texture_t   *psSpeech = NULL;
bool        oldEnables[3];
int         nSpeechLength = -1;
int         oldChange;


///////////////////
// Initialize the speech bubble
void Gar_InitSpeechBubble(char *fmt, ...)
{
    va_list arg;

    psSpeech = Cache_LoadTexture( "data/textures/speech.png" );
    Tex_Upload( psSpeech );

    va_start(arg, fmt);
	vsprintf(szSpeech, fmt, arg);
	va_end(arg);

    // Wrap the text
    wrapText(szSpeech, 500, 16);

    // Find the longest line
    int line = 0;
    nSpeechLength = -1;

    for(int i=0; i<strlen(szSpeech); i++) {
        line++;
        if(szSpeech[i] == '\n') {
            if(line > nSpeechLength)
                nSpeechLength = line;
            line = 0;
        }
    }
    if(nSpeechLength == -1)
        nSpeechLength = strlen(szSpeech);

    oldChange = tPartChange.iChange;
    tPartChange.iChange = false;
    tGarage->bSpeechBubble = true;

    // Disable all the side/task bars
    oldEnables[0] = cSidebar.setEnabled(false);
    oldEnables[1] = cGarSection.setEnabled(false);
    oldEnables[2] = cGarTaskb.setEnabled(false);
	
    // Clear any mouse up's
    System_GetMouse()->Up = 0;
}


///////////////////
// Process the speech bubble
void Gar_SpeechProcess(void)
{
    Gar_DrawSpeech(szSpeech);

    if( (System_GetMouse()->Up & SDL_BUTTON(1)) || System_GetKeyboard()->KeyUp[SDLK_ESCAPE] ) {
        tGarage->bSpeechBubble = false;
        tPartChange.iChange = oldChange;

        // Re-enabled everything
        cSidebar.setEnabled( oldEnables[0] );
        cGarSection.setEnabled( oldEnables[1] );
        cGarTaskb.setEnabled( oldEnables[2] );
    }
}


///////////////////
// Draw the speech
void Gar_DrawSpeech(char *text)
{
    glColor4f(1,1,1,1);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    DrawPic( psSpeech, 144, 500 );

    int size = 16;
    Font_SetSize(size);
    int length = nSpeechLength * size;

    Font_Draw( 400 - length/2, 450, CVec(0,0,0), text );
}