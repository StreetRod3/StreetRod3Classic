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


// Main game
// Created 13/5/02
// Jason Boettcher


#include "defs.h"
#include "game.h"
#include "gui.h"
#include "City.h"
#include "racing.h"
#include "Menu.h"
#include "Entity.h"


int			iGameLocation = LOC_GARAGE;
CPlayer		*cMainGuy = NULL;
CRadio      *pcRadio = NULL;
race_details_t  sRaceInfo;


///////////////////
// Start the main game
void Game_Start(void)
{
	char CityDir[] = {"data/cities/LA"};

	// Load
	if(!gui_Initialize())
	{
		// 17/06/05
		writeLog("Failed to Initialize Graphic Interface");
		//
        return;
	}

    Game_InitBook();
	InitializeRenderer();

    cMainGuy = new CPlayer;
	if(cMainGuy == NULL)
		return;
	cMainGuy->Initialize();

    // Do the menu
    if(!Menu_Initialize(true))
        return;

    // Initialize the controls
    cMainGuy->InitializeControls();

    // Initialize the entities
    InitializeEntities();

    // Radio
    pcRadio = new CRadio;
    if(pcRadio)
        pcRadio->initialize();
    pcRadio->loadSongs();

	// TESTING
    cMainGuy->buyCar("data/newspaper/tester.sr3", 100);


	// Main loop
	while(iGameLocation != LOC_EXIT) {

		switch(iGameLocation) {

			// Garage
			case LOC_GARAGE:
				if(!Gar_Start(CityDir))
					return;
				break;

            // Diner
            case LOC_DINER:
                if(!Dnr_Start(CityDir))
                    return;
                break;

			/*// City
			case LOC_CITYDRIVING:
				if(!City_Start(CityDir))
					return;
				break;*/

			// Race
			case LOC_RACE:
				if(!Race_Start("track1"))
					return;
				break;
		}
	}

    ShutdownEntities();

	cMainGuy->Shutdown();
	delete cMainGuy;

    if( pcRadio ) {
        pcRadio->shutdown();
        delete pcRadio;
        pcRadio = NULL;
    }

}


///////////////////
// Set the location
void Game_SetLocation(int l)
{
	iGameLocation = l;
}


///////////////////
// Start a new game
void Game_NewGame(void)
{
    // Get rid of the old guy
    if(cMainGuy) {
        cMainGuy->Shutdown();
        delete cMainGuy;
        cMainGuy = NULL;
    }

    // Create a new guy
    cMainGuy = new CPlayer;
	if(cMainGuy == NULL)
		return;
	cMainGuy->Initialize();

    iGameLocation = LOC_GARAGE;
}


///////////////////
// Quit back to the menu
void Game_QuittoMenu(void)
{
    // Do the menu
    if(!Menu_Initialize(false)) {
        iGameLocation = LOC_EXIT;
        return;
    }

    // Initialize the controls
    cMainGuy->InitializeControls();
}


///////////////////
// Start a new game from a save game file
void Game_LoadGame(int nSlot)
{
    // Create a new game first
    Game_NewGame();

    cMainGuy->loadGame(nSlot);
}





/*
    Book
*/

texture_t       *psBookTex = NULL;

///////////////////
// Initialize the book
void Game_InitBook(void)
{
    // Load the book
    psBookTex = Cache_LoadTexture("data/gui/book.png");
    Tex_Upload(psBookTex);
}


///////////////////
// Draw a book
void Game_DrawBook(void)
{
    State_Disable(GL_FOG);
    State_Disable(GL_LIGHTING);
    State_Disable(GL_BLEND);
    State_Enable(GL_CULL_FACE);

    // Background & Main page
    DrawRect( 0,0, 800,600, CVec(1,1,1), CVec(0,0,0), 1, true, 0 );
    DrawRect( 250,591, 730,50, CVec(1,1,1), CVec(1,1,1), 1, true, 0 );
    
    State_Disable(GL_BLEND);
    State_Enable(GL_TEXTURE_2D);
    glColor4f(1,1,1,1);

    // Top-right corner
    BlitTexture( psBookTex, 3,2, 717,9, 60,64, 1,1 );

    // Top-middle bit
    BlitTexture( psBookTex, 0,170, 0,7, 256,86, 1,1 );

    // Side bit
    BlitTexture( psBookTex, 10,71, 724,71, 53,54, 1,9 );

    // Spline
    BlitTexture( psBookTex, 0,230, 0,90, 250,26, 1,18.5f );

    // Bottom-middle bit
    BlitTexture( psBookTex, 95,5, 120,522, 57,53, 11,1 );
        
    // Bottom-right corner
    BlitTexture( psBookTex, 94,66, 716,515, 60,60, 1,1 );

    // Bottom-curve bit
    BlitTexture( psBookTex, 110,136, 0,552, 128,23, 1,1 );
}