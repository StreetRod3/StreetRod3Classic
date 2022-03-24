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

// Garage - Dialogs
// Created 29/5/03
// Jason Boettcher


#include "defs.h"
#include "system.h"
#include "game.h"
#include "font.h"
#include "gui.h"


extern partchange_t	tPartChange;
extern CLocation	*cGarLocation;



/*
========================

	   Car Info

========================
*/


texture_t   *psCarInfoIcons[5];
int         nCurCarInfoPage = 0;
CGuiLayout  cCarInfo;


///////////////////
// Initialize the car-info dialog
void Gar_InitCarInfo(void)
{
    // Do we have a car?
    if( !cMainGuy->getCurCar() ) {
        Gar_InitSpeechBubble("You got no car!");
        return;
    }

    tPartChange.iChange = false;
	tGarage->iState = GAR_DIALOG;
	tGarage->DialogProcess = Gar_CarInfoProcess;
    nCurCarInfoPage = 0;
    tGarage->bDrawGarage = false;

    // Flush the mouse
    System_GetMouse()->Up = 0;
    System_GetMouse()->Down = 0;

    // Load the car icons
    psCarInfoIcons[0] = Cache_LoadTexture("data/gui/icon_car.png");
    psCarInfoIcons[1] = Cache_LoadTexture("data/gui/icon_engine.png");
    psCarInfoIcons[2] = Cache_LoadTexture("data/gui/icon_runningg.png");
    psCarInfoIcons[3] = Cache_LoadTexture("data/gui/arrow.png");
    psCarInfoIcons[4] = Cache_LoadTexture("data/gui/arrow2.png");

    for( int i=0; i<5; i++ )
        Tex_Upload(psCarInfoIcons[i]);

    // Setup the 'OK' button
    cCarInfo.Initialize();
    cCarInfo.Add( new CButton("Done", btn_done),    0,  350,540, 75,25 );
}


///////////////////
// Process the car-info dialog
void Gar_CarInfoProcess(void)
{
    int i;
    int y;
    int spacer = 25;
    CCar *car = cMainGuy->getCurCar();
    if( !car )
        return;

    tGarage->bDrawGarage = false;

    Game_DrawBook();

    // Draw car info
    Font_SetSize(28);
    Font_DrawCentered(430, 550, CVec(0,0,0), "Car Information");

    

    /*
       Car Info
    */
    if( nCurCarInfoPage == 0 ) {
        glBlendFunc(GL_ONE, GL_ZERO);
        glColor4f(1,1,1,1);
        DrawPic(psCarInfoIcons[0], 140,530);

        Font_SetSize(20);
        Font_Draw( 221, 494, CVec(0,0,0), "Body" );
        Font_Draw( 220, 495, CVec(0.3f,0.3f,1), "Body" );
        
        Font_SetSize(16);
        Font_Draw( 220, 435, CVec(0,0,0), car->getName() );
    }


    /*
       Engine Info
    */
    if( nCurCarInfoPage == 1 ) {
        glBlendFunc(GL_ONE, GL_ZERO);
        glColor4f(1,1,1,1);
        DrawPic(psCarInfoIcons[1], 140,530);

        Font_SetSize(20);
        Font_Draw( 221, 494, CVec(0,0,0), "Engine" );
        Font_Draw( 220, 495, CVec(0.3f,0.3f,1), "Engine" );

        Font_SetSize(16);
        State_Disable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        y = 435;
        CPart *block = car->getBlock();
        if(block) {
            Font_Draw( 220, y, CVec(0,0,0), "%s",block->getName());
            y -= spacer;
        
            // Intake
            CPart *man = block->FindPart(PRT_INTAKEMAN,-1);
            if( man ) {
                Font_Draw( 220, y, CVec(0,0,0), "  %s",man->getName());
                y -= spacer;

                // Carby's
                for( i=0; i<man->getNumCarbys(); i++ ) {
                    CPart *carb = block->FindPart(PRT_CARBY,i);
                    if( carb ) {
                        Font_Draw( 220, y, CVec(0,0,0), "    %s",carb->getName());
                        y -= spacer;
                    }
                }
            }


            // Other engine parts
            for( i=PRT_AIRFILTER; i<=PRT_FAN; i++ ) {
                CPart *p = block->FindPart(i,-1);
                if( p ) {
                    Font_Draw( 220, y, CVec(0,0,0), "  %s",p->getName());
                    y -= spacer;
                }
            } 
        }
    }



    /*
       Running Gear Info
    */
    if( nCurCarInfoPage == 2 ) {
        glBlendFunc(GL_ONE, GL_ZERO);
        glColor4f(1,1,1,1);
        DrawPic(psCarInfoIcons[2], 140,530);

        Font_SetSize(20);
        Font_Draw( 221, 494, CVec(0,0,0), "Running Gear" );
        Font_Draw( 220, 495, CVec(0.3f,0.3f,1), "Running Gear" );

        Font_SetSize(16);
        y = 435;

        // Transmission
        CPart *trans = car->getTrans();
        if( trans ) {
            Font_Draw( 220, y, CVec(0,0,0), "%s",trans->getName());
            y -= spacer;
        }

        // Differential
        CPart *diff = car->getDiff();
        if( diff ) {
            Font_Draw( 220, y, CVec(0,0,0), "%s",diff->getName());
            y -= spacer;
        }

        y -= 5;

        // 4 Tires
        for( i=0; i<4; i++ ) {
            CPart *tire = car->getTire(i);
            if( tire ) {
                Font_Draw( 220, y, CVec(0,0,0), "%s",tire->getName());
                y -= spacer;
            }
        }

        y -= 5;

        // 2 Mufflers
        for( i=0; i<2; i++) { 
            CPart *muff = car->getMuffler(i);
            if( muff ) {
                Font_Draw( 220, y, CVec(0,0,0), "%s",muff->getName());
                y -= spacer;
            }
        }

        y -= 5;

        // 4 Shocks
        for( i=0; i<4; i++ ) {
            CPart *shock = car->getShock(i);
            if( shock ) {
                Font_Draw( 220, y, CVec(0,0,0), "%s",shock->getName());
                y -= spacer;
            }
        }
    }


    /*
       User input
    */
    mouse_t *psMouse = System_GetMouse();
    
    State_Disable(GL_CULL_FACE);

    Font_SetSize(14);

    cGarLocation->setStatusText("");

    // LEFT arrow
    glColor4f(1,1,1,1);    
    if( MouseinRect(200,510, 264,542) ) {
        if( psMouse->Up )
            nCurCarInfoPage = MAX(nCurCarInfoPage-1,0);
        if( nCurCarInfoPage > 0 ) {
            glColor4f(0,1,0,1);
            cGarLocation->setStatusText("Previous page");
        }
    }
    DrawPic( psCarInfoIcons[3], 200, 90 );


    // RIGHT Arrow
    glColor4f(1,1,1,1);
    if( MouseinRect(570,510, 634,542) ) {
        if( psMouse->Up )
            nCurCarInfoPage = MIN(nCurCarInfoPage+1,2);
        if( nCurCarInfoPage < 2 ) {
            glColor4f(0,1,0,1);
            cGarLocation->setStatusText("Next page");
        }
    }
    DrawPic( psCarInfoIcons[4], 570, 90 );

    Font_SetSize(12);
    Font_DrawCentered(410, 67, CVec(0,0,0), "Page %d / 3",nCurCarInfoPage+1);

    // Restore the old states
    State_Enable(GL_DEPTH_TEST);
    State_Enable(GL_CULL_FACE);



    // Process the button
    cCarInfo.Draw();
    gui_event_t *ev = cCarInfo.Process();
    if( ev ) {
        if( ev->iControlID == 0 && ev->iEventMsg == BUT_CLICKED ) {
            cCarInfo.Shutdown();
            tGarage->bDrawGarage = true;
            tGarage->iState = GAR_NORMAL;
            Gar_NormalView();
        }
    }    
}


/*
========================

	   Stickers

========================
*/


CGuiLayout  cStickersGui;
int         nCurDecalPage = 0;
int         nNumDecalPages = 0;

// Decal structure
typedef struct decal_s {

    texture_t   *psTex;
    char        szFilename[256];
    int         nPage;
    int         nWidth, nHeight;

    struct decal_s  *psNext;
} decal_t;

decal_t     *psDecals = NULL;
decal_t     *psApplyDecal = NULL;
int         nDecalCost = 15;

///////////////////
// Initialize the stickers screen
void Gar_InitStickers(void)
{
    // Do we have a car?
    if( !cMainGuy->getCurCar() ) {
        Gar_InitSpeechBubble("You got no car!");
        return;
    }


    tPartChange.iChange = false;
	tGarage->iState = GAR_DIALOG;
	tGarage->DialogProcess = Gar_StickerProcess;
    tGarage->bDrawGarage = false;

    nCurDecalPage = 0;
    nNumDecalPages = 0;
    psApplyDecal = NULL;    
    
    // Load the stickers/decals    
    Gar_LoadStickers();

    // Flush the mouse
    System_GetMouse()->Up = 0;
    System_GetMouse()->Down = 0;

    // Load the arrows (borrowed from car info)
    psCarInfoIcons[3] = Cache_LoadTexture("data/gui/arrow.png");
    psCarInfoIcons[4] = Cache_LoadTexture("data/gui/arrow2.png");

    for( int i=3; i<5; i++ )
        Tex_Upload(psCarInfoIcons[i]);

    // Setup the 'OK' button
    cStickersGui.Initialize();
    cStickersGui.Add( new CButton("Done", btn_done),    0,  350,540, 75,25 );
}


///////////////////
// Load the stickers/decals
void Gar_LoadStickers(void)
{
    char szFilename[256];

    psDecals = NULL;
    if( !FindFirst("data/stickers","*.*", szFilename) )
        return;

    int page = 0;
    int count = 0;

    // Load all the stickers
    while(1) {

        decal_t *d = new decal_t;
        if( d ) {
            d->psTex = Cache_LoadTexture(szFilename);
			if(d->psTex)
				d->psTex->bMipFiltering = false; 

            Tex_Upload(d->psTex);
            // Did the texture load?
            if( d->psTex ) {
                strcpy(d->szFilename, szFilename);
                d->psNext = NULL;
                d->nWidth = d->psTex->iWidth;
                d->nHeight = d->psTex->iHeight;

                // Link it at the end
                decal_t *e = psDecals;
                if( e ) {
                    for(; e; e=e->psNext) {
                        if( e->psNext == NULL ) {
                            e->psNext = d;
                            break;
                        }
                    }
                } else
                    psDecals = d;
            } else {
                delete d;
                d = NULL;
            }
        }

        // Calculate the page the sticker goes on
        if( d ) {
            if( count++ >= 20 ) {
                page++;
                count=0;
            }
            d->nPage = page;
        }


        if( !FindNext(szFilename) )
            break;
    }

    nNumDecalPages = page;
}


///////////////////
// Process the stickers screen
void Gar_StickerProcess(void)
{
    mouse_t *psMouse = System_GetMouse();

    tGarage->bDrawGarage = false;

    Game_DrawBook();

    cGarLocation->setStatusText("");

    // Draw the decals
    Font_SetSize(28);
    Font_DrawCentered(430, 550, CVec(0,0,0), "Decals");

    State_Disable(GL_CULL_FACE);
    State_Enable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1,1,1,1);

    // Display the stickers
    decal_t *d = psDecals;
    int x = 140;
    int y = 530;
    int maxheight = 0;
    for(; d; d=d->psNext) {

        // Does this decal belong on the page?
        if( nCurDecalPage != d->nPage )
            continue;
        
        State_Disable(GL_TEXTURE_2D);
        DrawRect( x-1,y-1, x+MIN(d->psTex->iWidth,64)+2, y-MIN(d->psTex->iHeight,64)+2, CVec(0,0,0), CVec(0.5f,0.5f,0.5f), 1, true, 0 );
        DrawRect( x-1,y-1, x+MIN(d->psTex->iWidth,64)+2, y-MIN(d->psTex->iHeight,64)+2, CVec(0,0,0), CVec(0,0,0), 1, false, 0 );        

        State_Enable(GL_TEXTURE_2D);
        glColor4f(1,1,1,1);

        // Draw the picture with a correct aspect ratio and centred
        int w = MIN(d->psTex->iWidth,64);
        float h = (64.0f / (float)d->psTex->iWidth) * (float)d->psTex->iHeight;
        h = MIN(h,64.0f);
        int y2 = y-32+h/2;
        DrawPicEx( d->psTex, x, y2, x+w, y2-h );

        if( MouseinRect(x,600-y, x+MIN(d->psTex->iWidth,64), 600-(y-MIN(d->psTex->iHeight,64)))) {
            cGarLocation->setStatusText("Buy decal");
            if( psMouse->Up & SDL_BUTTON(1) && !tGarage->bSpeechBubble ) {

                // Buy the sticker
                if( cMainGuy->makePurchase(nDecalCost) ) {
                    psApplyDecal = d;
                    tGarage->iState = GAR_DECAL;
                    tGarage->bDrawGarage = true;
                    cStickersGui.Shutdown();
                    cGarLocation->setItemCheck(false);
                    cGarLocation->setAngleLimit(false);

                    // Create the sticker dialog for placement
                    Gar_DecalInitPlacementDialog();

                    return;
                } else {
                    Gar_InitSpeechBubble("You ain't got enough cash");
                }
            }
        }

        maxheight = MAX(maxheight, MIN(d->psTex->iHeight,64));

        x += 150;
        if( x > 670 ) {
            x=140;
            y -= maxheight+30;
        }
    }



    /*
       User input
    */    
    
    Font_SetSize(14);


    // LEFT arrow
    glColor4f(1,1,1,1);    
    if( MouseinRect(200,510, 264,542) ) {
        if( psMouse->Up )
            nCurDecalPage = MAX(nCurDecalPage-1,0);
        if( nCurDecalPage > 0 ) {
            glColor4f(0,1,0,1);
            cGarLocation->setStatusText("Previous page");
        }
    }
    DrawPic( psCarInfoIcons[3], 200, 90 );


    // RIGHT Arrow
    glColor4f(1,1,1,1);
    if( MouseinRect(570,510, 634,542) ) {
        if( psMouse->Up )
            nCurDecalPage = MIN(nCurDecalPage+1,nNumDecalPages);
        if( nCurDecalPage < nNumDecalPages ) {
            glColor4f(0,1,0,1);
            cGarLocation->setStatusText("Next page");
        }
    }
    DrawPic( psCarInfoIcons[4], 570, 90 );

    Font_SetSize(12);
    Font_DrawCentered(410, 67, CVec(0,0,0), "Page %d / %d",nCurDecalPage+1,nNumDecalPages+1);

    // Restore the old states
    State_Enable(GL_DEPTH_TEST);
    State_Enable(GL_CULL_FACE);


    // Process the button
    cStickersGui.Draw();
    gui_event_t *ev = cStickersGui.Process();
    if( ev ) {
        if( ev->iControlID == 0 && ev->iEventMsg == BUT_CLICKED ) {
            cStickersGui.Shutdown();
            tGarage->bDrawGarage = true;
            tGarage->iState = GAR_NORMAL;
            Gar_NormalView();
        }
    }    
}


///////////////////
// Initialize the sticker placement dialogbox
void Gar_DecalInitPlacementDialog(void)
{
    cStickersGui.Initialize();    
    cStickersGui.Add( new CColourPicker(),                  6,     270,235,125,125 );	
	cStickersGui.Add( new CButton("Forget it",btn_forgit),  5,     336,365,120,25  );

    cStickersGui.Add( new CButton("Bigger",0),              4,     405,240,120,25  );
    cStickersGui.Add( new CButton("Normal",0),              3,     405,280,120,25  );
    cStickersGui.Add( new CButton("Smaller",0),             2,     405,320,120,25  );
	cStickersGui.Add( new CDialog("Decal", dlgs_small),     1,     35, 25, 320,225 );

    cStickersGui.moveWidgets(230,-160);
}


///////////////////
// Process putting the decal on the car
void Gar_DecalApplyProcess(void)
{
    if( !psApplyDecal ) {
        tGarage->iState = GAR_NORMAL;
        Gar_NormalView();
        return;
    }

    mouse_t *psMouse = System_GetMouse();

    // Draw the sticker
    State_Disable(GL_LIGHTING);
    State_Enable(GL_TEXTURE_2D);
    State_Enable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    CVec col = ((CColourPicker *)cStickersGui.getWidget(6))->getCurrentColour();

    // If the mouse is over the dialogbox don't show the sticker
    if(!cStickersGui.getWidget(1)->InBox(psMouse->X,600-psMouse->Y)) {        

        glColor4f(col.GetX(),col.GetY(),col.GetZ(),0.5f);
        int x = psMouse->X - psApplyDecal->nWidth/2;
        int y = 600-(psMouse->Y - psApplyDecal->nHeight/2);

        DrawPicEx( psApplyDecal->psTex, x, y, x+psApplyDecal->nWidth, y-psApplyDecal->nHeight);
    }

    cStickersGui.Draw();
    gui_event_t *ev = cStickersGui.Process();
    if(!ev)
        return;

    switch(ev->iControlID) {
        // Forget it
        case 5:
            if(ev->iEventMsg == BUT_CLICKED) {
                // Leave & give back money
                tGarage->iState = GAR_NORMAL;
                Gar_NormalView();
                cMainGuy->addMoney(nDecalCost);
            }
            break;
            
        // Bigger
        case 4:
            if(ev->iEventMsg == BUT_CLICKED) {
                // Make the decal bigger
                psApplyDecal->nWidth = psApplyDecal->psTex->iWidth*2;
                psApplyDecal->nHeight = psApplyDecal->psTex->iHeight*2;
            }
            break;

        // Normal
        case 3:
            if(ev->iEventMsg == BUT_CLICKED) {
                // Make the decal normal size
                psApplyDecal->nWidth = psApplyDecal->psTex->iWidth;
                psApplyDecal->nHeight = psApplyDecal->psTex->iHeight;
            }
            break;

        // Smaller
        case 2:
            if(ev->iEventMsg == BUT_CLICKED) {
                // Make the decal smaller
                psApplyDecal->nWidth = psApplyDecal->psTex->iWidth/2;
                psApplyDecal->nHeight = psApplyDecal->psTex->iHeight/2;
            }
            break;
    }
}


///////////////////
// Process putting the decal on the car
void Gar_DecalApplyProcess2(void)
{
    mouse_t *psMouse = System_GetMouse();

    if( !psApplyDecal ) {
        tGarage->iState = GAR_NORMAL;
        Gar_NormalView();
        return;
    }
        
    // If the mouse is over the dialogbox don't place the sticker
    if(cStickersGui.getWidget(1)->InBox(psMouse->X,600-psMouse->Y))
        return;
    
    if( psMouse->Up & SDL_BUTTON(1) ) {
        // Apply the decal
        CCar *car = cMainGuy->getCurCar();
        CVec col = ((CColourPicker *)cStickersGui.getWidget(6))->getCurrentColour();

        if( car )
            car->applyDecal( psApplyDecal->szFilename, cGarLocation->getCamera(), psMouse->X - psApplyDecal->nWidth/2, psMouse->Y - psApplyDecal->nHeight/2, psApplyDecal->nWidth, psApplyDecal->nHeight, col );

        // Leave
        tGarage->iState = GAR_NORMAL;
        Gar_NormalView();
        return;
    }
}




/*
========================

	     Radio

========================
*/


CGuiLayout cGarRadio;


///////////////////
// Initialize the radio menu
void Gar_InitRadioMenu(void)
{
	tPartChange.iChange = false;
	tGarage->iState = GAR_DIALOG;
	tGarage->DialogProcess = Gar_ProcessRadioDialog;

	cGarRadio.Initialize();    
    
    cGarRadio.Add( new CButton("Previous",0),           6,  125,300,120,25  );
    cGarRadio.Add( new CButton("Play",0),               5,  265,300,120,25  );
    cGarRadio.Add( new CButton("Stop",0),               4,  405,300,120,25  );
    cGarRadio.Add( new CButton("Next",0),               3,  545,300,120,25  );
    cGarRadio.Add( new CButton("Forget it",0),          2,  120,340,120,25  );	
	cGarRadio.Add( new CDialog("Radio", dlgs_small),    1,  35, 25, 600,175 );
}


///////////////////
// Process the radio menu
void Gar_ProcessRadioDialog(void)
{
    char buf[512];
    buf[0] = 0;

   	gui_event_t *event;
	event = cGarRadio.Process();
    cGarRadio.Draw();

    // Draw the current MP3 file
    pcRadio->getCurrentSong(buf);

    Font_SetSize(10);
    Font_SetAlpha(1);
    Font_DrawCentered(400,320, CVec(1,1,1), "\"%s\"",buf);

    if( !event )
        return;

    switch( event->iControlID ) {

        // Previous song
        case 6:
            pcRadio->prevSong();
            break;

        // Play current song
        case 5:
            pcRadio->playCurrent();
            break;

        // Stop current song
        case 4:
            pcRadio->stopSong();
            break;

        // Next song
        case 3:
            pcRadio->nextSong();
            break;

        // Forget it
        case 2:
            Gar_InitSystemMenu();
            cGarRadio.Shutdown();
            break;
    }
}




/*
========================

	     New Game

========================
*/


CGuiLayout  cNewGameMenu;


///////////////////
// Ask the player if they are sure about starting a new game
void Gar_InitNewGame(void)
{
    tPartChange.iChange = false;
	tGarage->iState = GAR_DIALOG;
	tGarage->DialogProcess = Gar_ProcessNewGameDialog;

    cNewGameMenu.Initialize();
    cNewGameMenu.Add( new CLabel("Start a new game?"),      0, 400, 280, 0,   0);
	cNewGameMenu.Add( new CButton("Yeah",btn_yeah),		    1, 180, 325, 75,  20);
	cNewGameMenu.Add( new CButton("Forget it",btn_forgit),  2, 495, 325, 125, 20);
	cNewGameMenu.Add( new CDialog("New Game", dlgs_medium), 3, 125, 180, 550, 200 );

    ((CLabel *)cNewGameMenu.getWidget(0))->setCenter(true);
    ((CLabel *)cNewGameMenu.getWidget(0))->setSize(16);
}


///////////////////
// Process the new game dialog
void Gar_ProcessNewGameDialog(void)
{
    gui_event_t *event;
	event = cNewGameMenu.Process();
    cNewGameMenu.Draw();
    
    if( !event )
        return;

    switch( event->iControlID ) {

        // Yeah
        case 1:
            cNewGameMenu.Shutdown();
            tGarage->iState = GAR_NORMAL;

            // Clear the garage stuff
            cGarLocation->ClearGameItems();

            Game_NewGame();
            break;

        // Forget it
        case 2:
            cNewGameMenu.Shutdown();
            Gar_InitSystemMenu();
            break;
    }
}




/*
========================

	 Save/Load Game

========================
*/

CGuiLayout  cSaveLoadGameMenu;
savegame_t  sSaveGames[SAVE_GAMES];
texture_t   *psBookTitles = NULL;
texture_t   *psSaveBox = NULL;
int         nSaveSpot = 0;
CGuiLayout  cSaveOverwriteMsg;
bool        bOverwriteMessage = false;



///////////////////
// Initialize the save game book
void Gar_InitSaveGame(void)
{
    tPartChange.iChange = false;
	tGarage->iState = GAR_DIALOG;
    tGarage->bDrawGarage = false;
	tGarage->DialogProcess = Gar_ProcessSaveGame;

    cSaveLoadGameMenu.Initialize();
    cSaveLoadGameMenu.Add( new CButton("Forget it",0),  0, 600, 522, 125, 20);
    cSaveLoadGameMenu.Add( new CButton("Save",0),       1, 600, 482, 125, 20);
    cSaveLoadGameMenu.Add( new CTextbox(),              2, 165, 520, 400, 35);

    nSaveSpot = 0;
    bOverwriteMessage = false;

    // Fill in the save games
    Gar_SaveLoadFillSlots();

    psBookTitles = Cache_LoadTexture("data/gui/book_titles.png");
    Tex_Upload(psBookTitles);
    psSaveBox = Cache_LoadTexture("data/gui/savebox.png");
    Tex_Upload(psSaveBox);
}


///////////////////
// Fill in the save game entries
void Gar_SaveLoadFillSlots(void)
{
    char        buf[256];
    savegame_t  *s = sSaveGames;


    for(int i=0; i<SAVE_GAMES; i++, s++) {
        s->nSlot = i;

        sprintf(buf,"saves/save%d.sav",i);

        FILE *fp = fopen(buf,"rb");
        if(!fp)
            s->bUsed = false;
        else
            // Read the name
            s->bUsed = cMainGuy->readSaveGameName(fp, s->szName);

        if(s->bUsed)
            fclose(fp);
    }
}


///////////////////
// Process the save game book
void Gar_ProcessSaveGame(void)
{
    mouse_t     *ms = System_GetMouse();
    int         y = 500;
    int         i;
    savegame_t  *s = sSaveGames;
    gui_event_t *ev = NULL;

    Game_DrawBook();
    glColor4f(1,1,1,1);
    State_Disable(GL_BLEND);
    BlitTexture(psBookTitles, 0,30, 300,20, 256,30, 1,1);

    // Draw the save box
    BlitTexture(psSaveBox, 0,0,  150,510, 45,45, 1,1);
    BlitTexture(psSaveBox, 32,0, 195,510, 45,45, 7,1);
    BlitTexture(psSaveBox, 96,0, 510,510, 32,45, 1,1);
    
    State_Disable(GL_TEXTURE_2D);
    State_Disable(GL_BLEND);
    Font_SetAlpha(1);
    Font_SetSize(18);
    for(i=0; i<SAVE_GAMES; i++, y-=35, s++) {
        if(MouseinRect(150,600-y,600,600-(y-30)) && !bOverwriteMessage) {
            DrawRect(150,y,600,y-30, CVec(0,0,0), CVec(0.7f,0.7f,0.7f), 1,true,0);

            if(ms->Up & SDL_BUTTON(1)) {
                nSaveSpot = i;
                if(s->bUsed)
                    ((CTextbox *)cSaveLoadGameMenu.getWidget(2))->setText(s->szName);
                else
                    ((CTextbox *)cSaveLoadGameMenu.getWidget(2))->setText("");
                break;
            }
        } else {
            if(nSaveSpot == i) {
                DrawRect(150,y,600,y-30, CVec(0,0,0), CVec(0.9f,0.9f,0.9f), 1,true,0);
                DrawRect(150,y,600,y-30, CVec(0,0,0), CVec(0.9f,0.9f,0.9f), 1,false,1);
            }
        }

        if(!s->bUsed)
            Font_DrawShadow(150,y-23, CVec(0,0,0), CVec(0.7f, 0.7f, 0.7f), "-");            
        else
            Font_DrawShadow(150,y-23, CVec(0,0,0), CVec(0.7f, 0.7f, 0.7f), s->szName);
    }

    if(bOverwriteMessage) {
        cSaveLoadGameMenu.Draw();
        cSaveOverwriteMsg.Draw();
        ev = cSaveOverwriteMsg.Process();
        if(ev) {

            // Forget it
            if(ev->iControlID == 1 && ev->iEventMsg == BUT_CLICKED) {
                cSaveOverwriteMsg.Shutdown();
                bOverwriteMessage = false;
                return;
            }

            // Yeah
            if(ev->iControlID == 0 && ev->iEventMsg == BUT_CLICKED) {
                char *text = ((CTextbox *)cSaveLoadGameMenu.getWidget(2))->getText();

                // Save
                cMainGuy->saveGame(nSaveSpot, text);
                tGarage->iState = GAR_NORMAL;
                tGarage->bDrawGarage = true;
                cSaveLoadGameMenu.Shutdown();
                cSaveOverwriteMsg.Shutdown();
                bOverwriteMessage = false;
                return;
            }
        }
        return;
    }


    cSaveLoadGameMenu.Draw();
    ev = cSaveLoadGameMenu.Process();
    if(!ev)
        return;

    // Forget it
    switch(ev->iControlID) {
        // Forget it
        case 0:
            if(ev->iEventMsg == BUT_CLICKED) {
	            tGarage->iState = GAR_NORMAL;
                tGarage->bDrawGarage = true;
                cSaveLoadGameMenu.Shutdown();
                return;
            }
            break;

        // Save
        case 1:
            if(ev->iEventMsg == BUT_CLICKED) {
                char *text = ((CTextbox *)cSaveLoadGameMenu.getWidget(2))->getText();

                // Check for overwrite
                if(strlen(text) > 0 && sSaveGames[nSaveSpot].bUsed) {
                    bOverwriteMessage = true;

                    // Create the overwrite dialog
                    cSaveOverwriteMsg.Initialize();
                    cSaveOverwriteMsg.Add( new CLabel("Overwrite file?"),       -1,400, 280, 0,   0 );
                    cSaveOverwriteMsg.Add( new CButton("Yeah",btn_yeah),		0, 180, 320, 100, 20 );
                    cSaveOverwriteMsg.Add( new CButton("Forget it",btn_forgit), 1, 495, 320, 125, 20 );
                    cSaveOverwriteMsg.Add( new CDialog("Save Game",dlgs_medium),2, 25,  25,  250, 250 );
                    
                    ((CLabel *)cSaveOverwriteMsg.getWidget(-1))->setCenter(true);
                    break;
                }

                if(strlen(text) > 0) {
                    cMainGuy->saveGame(nSaveSpot, text);

                    tGarage->iState = GAR_NORMAL;
                    tGarage->bDrawGarage = true;
                    cSaveLoadGameMenu.Shutdown();
                }
                return;
            }
            break;
    }
}


///////////////////
// Initialize the load game book
void Gar_InitLoadGame(void)
{
    tPartChange.iChange = false;
	tGarage->iState = GAR_DIALOG;
    tGarage->bDrawGarage = false;
	tGarage->DialogProcess = Gar_ProcessLoadGame;

    InitializeLoadGame();
}


///////////////////
// Process the garage version of the load game book
void Gar_ProcessLoadGame(void)
{
    if(ProcessLoadGame() != 0) {
        // Go back to the garage
        tGarage->iState = GAR_NORMAL;
        tGarage->bDrawGarage = true;        
        
        // Reset the garage game items
        cGarLocation->ClearGameItems();
        Gar_SetItems();
    }
}


///////////////////
// Normal initialize the load game book
void InitializeLoadGame(void)
{
    cSaveLoadGameMenu.Initialize();
    cSaveLoadGameMenu.Add( new CButton("Forget it",0),  0, 600, 522, 125, 20);

    // Fill in the save games
    Gar_SaveLoadFillSlots();

    Game_InitBook();

    psBookTitles = Cache_LoadTexture("data/gui/book_titles.png");
    Tex_Upload(psBookTitles);
}


///////////////////
// Process the load game book
// Returns
// 1 if done
// 0 if processing
// -1 if cancelled
int ProcessLoadGame(void)
{
    mouse_t     *ms = System_GetMouse();
    int         y = 500;
    int         i;
    savegame_t  *s = sSaveGames;

    Game_DrawBook();
    glColor4f(1,1,1,1);
    State_Disable(GL_BLEND);
    BlitTexture(psBookTitles, 0,60, 300,20, 256,30, 1,1);
    
    State_Disable(GL_TEXTURE_2D);
    State_Disable(GL_BLEND);
    Font_SetAlpha(1);
    Font_SetSize(18);

    for(i=0; i<SAVE_GAMES; i++, y-=35, s++) {
        if(MouseinRect(150,600-y,600,600-(y-30))) {
            if(s->bUsed)
                DrawRect(150,y,600,y-30, CVec(0,0,0), CVec(0.7f,0.7f,0.7f), 1,true,0);

            if((ms->Up & SDL_BUTTON(1)) && s->bUsed) {
                cSaveLoadGameMenu.Shutdown();

                // Load a new game
                Game_LoadGame(i);

                return 1;
            }
        }

        if(!s->bUsed) {
            Font_Draw(152, y-25, CVec(0.7f,0.7f,0.7f), "Unused");
            Font_Draw(150, y-23, CVec(0,0,0), "Unused");
        } else {
            Font_Draw(152, y-25, CVec(0.7f,0.7f,0.7f), s->szName);
            Font_Draw(150, y-23, CVec(0,0,0), s->szName);
        }
    }
	

    cSaveLoadGameMenu.Draw();
    gui_event_t *ev = cSaveLoadGameMenu.Process();
    if(!ev)
        return 0;

    if(ev->iControlID == 0 && ev->iEventMsg == BUT_CLICKED) {        
	    cSaveLoadGameMenu.Shutdown();
        return -1;
    }

    return 0;
}




/*
========================

	   Sell a car

========================
*/


CGuiLayout      cSellCarGui;
texture_t       *psSellCarSpeech = NULL;
bool            bSellCarSpeech = false;
char            szSellCarText[32];
CCar            *pcSellCar = NULL;
int             nSellCarOffer = -1;
bool            bSellCar_Sold = false;
int             nSellCarNumOffers = 0;
bool            bSellCar_NoDeal = false;


///////////////////
// Initialize the sell car dialog
void Gar_InitSellCar(CCar *pcCar)
{
    if(!pcCar)
        return;

    tPartChange.iChange = false;
	tGarage->iState = GAR_DIALOG;
    tGarage->bDrawGarage = true;
	tGarage->DialogProcess = Gar_ProcessSellCar;

    cSellCarGui.Initialize();
    cSellCarGui.Add( new CLabel(pcCar->getName()),          0, 400, 260, 0,   0 );
    cSellCarGui.Add( new CLabel("I'll take $"),             1, 200, 300, 0,   0 );
    cSellCarGui.Add( new CTextbox(),                        2, 360, 283, 100, 20 );
    cSellCarGui.Add( new CButton("Offer",0),	            3, 180, 320, 100, 20 );
    cSellCarGui.Add( new CButton("Forget it",btn_forgit),   4, 495, 320, 125, 20 );
    cSellCarGui.Add( new CDialog("Save Game",dlgs_medium),  5, 25,  25,  250, 250 );

    // Setup the widgets
    ((CLabel *)cSellCarGui.getWidget(0))->setCenter(true);
    ((CLabel *)cSellCarGui.getWidget(0))->setSize(15);
    ((CLabel *)cSellCarGui.getWidget(1))->setSize(14);
    ((CTextbox *)cSellCarGui.getWidget(2))->setFGCol(CVec(1,1,1));
    ((CTextbox *)cSellCarGui.getWidget(2))->setBGCol(CVec(0,0,0));

    // Load the speech bubble
    psSellCarSpeech = Cache_LoadTexture("data/textures/speech.png");
    Tex_Upload(psSellCarSpeech);
    bSellCarSpeech = false;

    pcSellCar = pcCar;
    nSellCarOffer = -1;
    bSellCar_Sold = false;
    nSellCarNumOffers = 0;
    bSellCar_NoDeal = false;
}


///////////////////
// Process the sell car dialog
void Gar_ProcessSellCar(void)
{
    gui_event_t     *ev = NULL;
    mouse_t         *psMouse = System_GetMouse();

    // Show the offer speech bubble
    if(bSellCarSpeech) {
        glColor4f(1,1,1,1);
        State_Enable(GL_TEXTURE_2D);
        State_Enable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        DrawPic( psSellCarSpeech, 144, 500 );
        
        int size = 16;
        Font_SetSize(size);
        int length = strlen(szSellCarText) * size;
        Font_Draw( 400 - length/2, 450, CVec(0,0,0), szSellCarText );
    }


    //
    // Check for any mouse click
    //

    // No deal punk
    if(psMouse->Up && bSellCar_NoDeal) {
        cSellCarGui.Shutdown();
        tGarage->iState = GAR_NORMAL;
        // Go back to the car menu
        Gar_InitCarList();
        return;
    }

    // Deal!
    if(psMouse->Up && bSellCar_Sold) {
        cSellCarGui.Shutdown();
        tGarage->iState = GAR_NORMAL;
        
        //
        // Show the car leaving the garage (if it is the current car)
        //
        CCar *pcCur = cMainGuy->getCurCar();
        if(!pcCur) {
            cMainGuy->sellCar(pcSellCar, nSellCarOffer);
            pcSellCar = NULL;
            // Reset the car items
            Gar_SetItems();
            Gar_NormalView();
            return;
        }

        if(pcSellCar->getID() == pcCur->getID())
            Gar_CarRollOut(true);            
        else {
            cMainGuy->sellCar(pcSellCar, nSellCarOffer);
            pcSellCar = NULL;
        }
        
        // Reset the car items
        Gar_SetItems();
        Gar_NormalView();
        
        return;
    }


    // GUI Shit
    cSellCarGui.Draw();
    ev = cSellCarGui.Process();
    if(!ev)
        return;

    switch(ev->iControlID) {

        // Forget it/No thanks
        case 4:
            if(ev->iEventMsg == BUT_CLICKED) {

                // If an offer was made then disgard the offer
                if(nSellCarOffer != -1) {

                    // Change the 2 buttons to show 'Offer' and 'Forget it'
                    ((CButton *)cSellCarGui.getWidget(3))->setText("Offer");
                    ((CButton *)cSellCarGui.getWidget(4))->setText("Forget it");
                    
                    nSellCarOffer = -1;
                    bSellCar_Sold = false;                
                    bSellCar_NoDeal = false;
                    bSellCarSpeech = false;

                    break;
                }

                cSellCarGui.Shutdown();
                tGarage->iState = GAR_NORMAL;

                // Go back to the car menu
                Gar_InitCarList();
                return;
            }
            break;

        // Offer/OK
        case 3:
            if(ev->iEventMsg == BUT_CLICKED) {

                // Was an offer made?
                if(nSellCarOffer != -1) {
                    cSellCarGui.Shutdown();
                    tGarage->iState = GAR_NORMAL;

                    //
                    // Show the car leaving the garage (if it is the current car)
                    //
                    CCar *pcCur = cMainGuy->getCurCar();
                    if(!pcCur) {
                        cMainGuy->sellCar(pcSellCar, nSellCarOffer);
                        pcSellCar = NULL;
                        // Reset the car items
                        Gar_SetItems();
                        Gar_NormalView();
                        return;
                    }
                    
                    if(pcSellCar->getID() == pcCur->getID())
                        Gar_CarRollOut(true);            
                    else {
                        cMainGuy->sellCar(pcSellCar, nSellCarOffer);
                        pcSellCar = NULL;
                    }

                    // Reset the car items
                    Gar_SetItems();
                    Gar_NormalView();
                    
                    return;
                }

                Gar_SellCar_MakeOffer();
            }
            break;
    }
}


///////////////////
// Make an offer
void Gar_SellCar_MakeOffer(void)
{
    nSellCarOffer = -1;
    bSellCarSpeech = true;
    
    int     nCost = pcSellCar->calculateSellingPrice();
    int     nOffer;
    if(strlen(((CTextbox *)cSellCarGui.getWidget(2))->getText()) == 0)
        return;
    
    nOffer = atoi(((CTextbox *)cSellCarGui.getWidget(2))->getText());

    nSellCarNumOffers++;


    // Change the 2 buttons to show 'OK' and 'No Thanks'
    ((CButton *)cSellCarGui.getWidget(3))->setText("OK");
    ((CButton *)cSellCarGui.getWidget(4))->setText("No Thanks");


    // If we have made 3 offers, leave
    if(nSellCarNumOffers > 3) {
        strcpy(szSellCarText, "No Thanks");
        bSellCar_NoDeal = true;
        return;
    }


    bSellCar_Sold = false;
    bSellCar_NoDeal = false;
    
    // If the offer is $400 more than correct "You're dreaming!"
    if(nOffer > nCost+400) {
        strcpy(szSellCarText, "You're dreaming");
        bSellCar_NoDeal = true;
        return;
    }
    
    // If the offer is $400 less than correct "You must be kidding! I'll take it!"
    if(nOffer < nCost-400) {
        strcpy(szSellCarText, "You must be kidding! I'll take it");
        bSellCar_Sold = true;
        nSellCarOffer = nOffer;
        return;
    }
    
    // If the offer is 10% greater than correct "No thanks"
    if(nOffer > nCost + ((float)nCost*0.1f)) {
        strcpy(szSellCarText, "No thanks");
        bSellCar_NoDeal = true;
        return;
    }
    
    // If the offer is lower than 10% i'll take it
    if(nOffer < nCost - ((float)nCost*0.1f)) {
        strcpy(szSellCarText, "I'll take it");
        bSellCar_Sold = true;
        nSellCarOffer = nOffer;
        return;
    }
    
    
    // If the offer is within +- 10% make an offer around %5 lower than my offer
    float p = (float)nCost*0.1f;
    if(abs(nCost - nOffer) < p) {
        
        int myOffer = nCost - (float)nCost*(0.1f + fabs(GetRandomNum()*0.1f));
        // Round it off to the nearest $10
        myOffer = (myOffer/10) * 10;
        
        sprintf(szSellCarText, "I'll give you $%d", myOffer);
        
        nSellCarOffer = myOffer;
        return;
    }
    
    // Shouldn't get here
    strcpy(szSellCarText, "No thanks");
    bSellCar_NoDeal = true;
}


///////////////////
// Finish the sell car
void Gar_FinishCarSell(void)
{
    cMainGuy->sellCar(pcSellCar, nSellCarOffer);
    pcSellCar = NULL;

    // Reset the car items
    Gar_SetItems();
    Gar_NormalView();
}