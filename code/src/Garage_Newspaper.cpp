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


// Garage class - Newspaper routines
// Created 8/7/02
// Jason Boettcher


#include "defs.h"
#include "system.h"
#include "game.h"
#include "font.h"
#include "gui.h"


// Newspaper modes
enum {
	news_frontpage,
	news_cars,
	news_autoparts
};


newsentry_t *tEntries = NULL;
int			iBuyingItem = false;
texture_t	*tTopbit;
texture_t	*tSidebit;
texture_t	*tsmGarage;
texture_t   *tNewsTitles[4];
texture_t   *tTitlePics[4];
texture_t   *tBlurredText = NULL;
texture_t   *tBorder = NULL;
int			iNewsmode;
extern CLocation	*cGarLocation;
CGuiLayout	BuyPart;
CGuiLayout  BuyCar;
int			iViewingCar = false;
int         iOldGarageState = 0;
int         nNumPages = 1;
int         nCurPage = 0;



///////////////////
// Initialize the newspaper
void Gar_NewspaperInit(void)
{
	// Load the textures
	tTopbit = Cache_LoadTexture("data/newspaper/topbit.png");
	tSidebit = Cache_LoadTexture("data/newspaper/sidebit.png");
	tsmGarage = Cache_LoadTexture("data/newspaper/garage.png");
    tNewsTitles[0] = Cache_LoadTexture("data/newspaper/dailgaz.png");
    tNewsTitles[1] = Cache_LoadTexture("data/newspaper/autoparts.png");
    tNewsTitles[2] = Cache_LoadTexture("data/newspaper/usedcars.png");
    tNewsTitles[3] = Cache_LoadTexture("data/newspaper/stripe.png");
    tTitlePics[0] = Cache_LoadTexture("data/newspaper/part2.png");
    tTitlePics[1] = Cache_LoadTexture("data/newspaper/part1.png");
    tTitlePics[2] = Cache_LoadTexture("data/newspaper/car2.png");
    tTitlePics[3] = Cache_LoadTexture("data/newspaper/car1.png");
    tBlurredText = Cache_LoadTexture("data/newspaper/blurrytext.png");
    tBorder = Cache_LoadTexture("data/gui/border.png");
	Tex_Upload(tTopbit);
	Tex_Upload(tSidebit);
	Tex_Upload(tsmGarage);
    Tex_Upload(tBorder);

    for( int i=0; i<4; i++ ) {
        tNewsTitles[i]->iMipmaps = true;
        tTitlePics[i]->iMipmaps = true;
        Tex_Upload(tNewsTitles[i]);
        Tex_Upload(tTitlePics[i]);
    }
    Tex_Upload(tBlurredText);
    
    iOldGarageState = tGarage->iState;
	tGarage->iState = GAR_NEWSPAPER;
	iNewsmode = news_frontpage;

	iBuyingItem = false;
	iViewingCar = false;
    tGarage->bDrawGarage = false;
}


///////////////////
// Shutdown the newspaper
void Gar_NewspaperShutdown(void)
{
	Gar_NewspaperFreeEntries();

    tGarage->iState = iOldGarageState;
    tGarage->bDrawGarage = true;
}


///////////////////
// Free the newspaper entries
void Gar_NewspaperFreeEntries(void)
{
    newsentry_t *n = tEntries;
	newsentry_t *next;

	for(;n;n=next) {
		next = n->tNext;

		delete n;
	}

	tEntries = NULL;
}


///////////////////
// Returns true if the newspaper is covering the whole screen & we don't need to draw the garage
/*int Gar_Newspaper_DrawGarage(void)
{
	//if(iNewsmode == news_autoparts || iNewsmode == news_cars)
		return false;

	return true;
}*/


///////////////////
// Process the newspaper
void Gar_NewspaperProcess(void)
{
	switch(iNewsmode) {

		// Front page
		case news_frontpage:
			Gar_NewspaperFrontpage();
			break;

		// Auto parts & Used cars
		case news_autoparts:
            Gar_NewspaperEntriesProcess(nws_part);
            break;
		case news_cars:
			Gar_NewspaperEntriesProcess(nws_car);
			break;
	}

}


///////////////////
// Process the front page
void Gar_NewspaperFrontpage(void)
{
	mouse_t *m = System_GetMouse();


	// Draw the newspaper	
	State_Disable(GL_LIGHTING);
	State_Enable(GL_TEXTURE_2D);
	State_Enable(GL_BLEND);
	State_Disable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1,1,1,1);

	Tex_Bind(tTopbit);
	
	glBegin(GL_QUADS);

		// The top bits
		Gar_NewspaperDrawLine(690,600);
		Gar_NewspaperDrawLine(730,520);
		Gar_NewspaperDrawLine(772,440);

	glEnd();

	// White bit
	DrawRect(0,370, 799, 24, CVec(0,0,0), CVec(1,1,1), 1, true, 0);
	
	glColor4f(1,1,1,1);		

   	// Draw the top bit
    DrawPic(tNewsTitles[0], 190,410);
    DrawPic(tNewsTitles[1], 230,575);
    DrawPic(tNewsTitles[2], 265,497);

    // Stripe
    BlitTexture( tNewsTitles[3], 0,0, 5,270, 32,32, 24.7f,1);
    BlitTexture( tNewsTitles[3], 0,0, 5,300, 32,32, 24.7f,1);
    Font_SetSize(14);
    Font_SetAlpha(1);
    Font_Draw(680,302, CVec(0,0,0), "25 Cents");

    // Draw the story bits
    
    Font_SetSize(22);
    Font_DrawCentered(200,250, CVec(0,0,0), "Youth Killed");
    Font_DrawCentered(200,220, CVec(0,0,0), "Drag Racing");
    Font_DrawCentered(600,250, CVec(0,0,0), "Cops Vow Speeding");
    Font_DrawCentered(600,220, CVec(0,0,0), "Crackdown");
    DrawRect(399,270,401,25,CVec(0,0,0),CVec(0.5,0.5,0.5),1,true,0);
    
    State_Enable(GL_TEXTURE_2D);
    glColor4f(1,1,1,1);
    BlitTexture(tBlurredText, 0,0, 20,390, 256,170, 1.25f,1);
    BlitTexture(tBlurredText, 0,0, 420,390, 256,170, 1.25f,1);
    
    glColor4f(1,1,1,1);

    BlitTexture( tTitlePics[0], 0,0, 70,27, 90,52, 1,1 );
    BlitTexture( tTitlePics[1], 0,0, 550,30, 100,52, 0.9f,0.9f );
    BlitTexture( tTitlePics[2], 0,0, 55,105, 200,62, 0.9f,0.9f );
    BlitTexture( tTitlePics[3], 0,0, 540,107, 200,60, 0.9f,0.9f );

	// Check if the mouse is over any section
	cGarLocation->setStatusText("");
	if(m->X > 0 && m->X < 690) {
		if(m->Y > 0 && m->Y < 80) {
			cGarLocation->setStatusText("Auto Parts");

			// Check for a mouse click
			if(m->Up & SDL_BUTTON(1))
				Gar_NewspaperPartsInit();
		}
	}
	if(m->X > 0 && m->X < 730) {
		if(m->Y > 80 && m->Y < 160) {
			cGarLocation->setStatusText("Used Cars");

			// Check for a mouse click
			if(m->Up & SDL_BUTTON(1))
				Gar_NewspaperCarsInit();
		}
	}


    // Was escape pressed?
    if( System_GetKeyboard()->KeyUp[SDLK_ESCAPE] ) {
        Gar_NewspaperShutdown();
		return;
    }
}


///////////////////
// Draw a newspaper line graphic
void Gar_NewspaperDrawLine(int w, int y)
{
	// Top Line
	glTexCoord2f(0,    0);		glVertex2i(0,y);
	glTexCoord2f(0.78f, 0);		glVertex2i(w,y);
	glTexCoord2f(0.78f, 0.68f);	glVertex2i(w,y-85);
	glTexCoord2f(0,	   0.68f);	glVertex2i(0,y-85);

	// Top right corner
	glTexCoord2f(0.78f, 0);		glVertex2i(w,y);
	glTexCoord2f(1,    0);		glVertex2i(w+28,y);
	glTexCoord2f(1,    0.68f);	glVertex2i(w+28,y-85);
	glTexCoord2f(0.78f, 0.68f);	glVertex2i(w,y-85);
}



/*
  ===================

       Auto parts

  ===================
*/


newsentry_t *tPart = NULL;


///////////////////
// Initialize the auto parts section
void Gar_NewspaperPartsInit(void)
{
	iNewsmode = news_autoparts;

	// Go through the parts directory
	char filename[256];
	
	int found = FindFirst("data\\parts","*.sr3", filename);
	while(found) {
		Gar_NewspaperAddPart(filename);

		found = FindNext(filename);
	}

	iBuyingItem = false;

    // Sort the list
    Gar_NewspaperSortList();
}


///////////////////
// Add a part to the newspaper
void Gar_NewspaperAddPart(char *filename)
{
    // Part sort order
    int partList[] = {PRT_BLOCK, PRT_TRANS, PRT_CARBY, PRT_INTAKEMAN,
                      PRT_AIRFILTER, PRT_ALTER, PRT_STARTER, PRT_FAN,
                      PRT_TIRE, PRT_SHOCK, PRT_DIFF, PRT_LMUFFLER, PRT_RMUFFLER};
    
	newsentry_t *news = NULL;

	// Create the new news entry
	news = new newsentry_t;
	if(news == NULL)
		return;

	// Load the info
	strcpy(news->sFilename, filename);
	news->iType = nws_part;

	char name[64];
	char description[128];
	int price;
    int type;

	ReadString(filename, "General", "Name", name, "");
	ReadString(filename, "General", "Description", description, "");
	ReadInteger(filename, "Price", "MaxPrice", &price, 0);
    ReadKeyword(filename, "General", "Type", &type, PRT_BLOCK);
	
	sprintf(news->sDescription, "%s %s\n$%d",name,description,price);
    ConvertNewlines(news->sDescription);
    Gar_NewspaperCalculateHeight(news);

    news->tPrev = NULL;
	news->iPrice = price;
    strcpy(news->sName, name);
    news->nPartType = type;

    // Find the order number
    for(int i=0; i<sizeof(partList); i++) {
        if(partList[i] == type) {
            news->nPartType = i;
            break;
        }
    }

	// link the entry in
    news->tNext = tEntries;
    if(tEntries)
        tEntries->tPrev = news;
    tEntries = news;
}


///////////////////
// Show a buying part dialog
void Gar_NewspaperBuyPart(newsentry_t *ent)
{
	char buf[256];
	iBuyingItem = true;

	tPart = ent;
	sprintf(buf,"Buy '%s' for $%d?", ent->sName, ent->iPrice);

	BuyPart.Initialize();
	BuyPart.Add( new CLabel(buf),						0, 400, 270, 0,   0);
	BuyPart.Add( new CButton("Yeah",btn_yeah),			1, 175, 325, 75,  20);
	BuyPart.Add( new CButton("Forget it",btn_forgit),	2, 495, 325, 125, 20);
	BuyPart.Add( new CDialog("Buy Part", dlgs_medium),  3, 125, 180, 550, 200 );

    ((CLabel *)BuyPart.getWidget(0))->setCenter(true);
}


///////////////////
// Process the buying part dialog
void Gar_NewspaperProcessBuyPart(void)
{
	gui_event_t *event;
	
	// Process & Draw the gui layout
	event = BuyPart.Process();
	BuyPart.Draw();

	if(event == NULL)
		return;

	switch(event->iControlID) {

		// Yeah
		case 1:
			if(event->iEventMsg == BUT_CLICKED) {
				// Free the gui
				BuyPart.Shutdown();

				// Not buying a part anymore
				iBuyingItem = false;

				// Buy this part
                if(tPart) {

                    if( !cMainGuy->canBuy(tPart->iPrice) )
                        Gar_InitSpeechBubble("You're $%d short", tPart->iPrice - cMainGuy->getBankroll());
                    else
					    cMainGuy->BuyPart(tPart->sFilename, tPart->iPrice);
                }

				tPart = NULL;

				return;
			}
			break;


		// Forget it
		case 2:
			if(event->iEventMsg == BUT_CLICKED) {

				// Free the gui
				BuyPart.Shutdown();
				
				// Not buying a part anymore
				iBuyingItem = false;
				tPart = NULL;

				return;
			}
			break;
	}
}



/*
  ===================

       Used Cars

  ===================
*/


newsentry_t *tCar = NULL;


///////////////////
// Initialize the auto parts section
void Gar_NewspaperCarsInit(void)
{
	iNewsmode = news_cars;

	// Go through the parts directory
	char filename[256];
	
	int found = FindFirst("data\\newspaper","*.sr3", filename);
	while(found) {
		Gar_NewspaperAddCar(filename);

		found = FindNext(filename);
	}

    // Sort the list
    Gar_NewspaperSortList();
}


///////////////////
// Add a car to the newspaper
void Gar_NewspaperAddCar(char *filename)
{
	newsentry_t *news = NULL;

	// Create the new news entry
	news = new newsentry_t;
	if(news == NULL)
		return;

	// Load the info
	strcpy(news->sFilename, filename);
	news->iType = nws_car;

	char name[64];
	char description[128];
	int price;

	ReadString(filename, "Car", "Name", name, "");
	ReadString(filename, "Car", "Description", description, "");
	ReadInteger(filename, "Price", "SellingPrice", &price, 0);
	
	sprintf(news->sDescription, "%s. %s\n$%d",name,description,price);
    ConvertNewlines(news->sDescription);
    Gar_NewspaperCalculateHeight(news);

    news->tPrev = NULL;
	news->iPrice = price;
    news->nPartType = 0;        // To keep the sorter happy
	strcpy(news->sName, name);

	// link the entry in
    news->tNext = tEntries;
    if(tEntries)
        tEntries->tPrev = news;
    tEntries = news;
}



///////////////////
// Show a buying car dialog
void Gar_NewspaperBuyCar(newsentry_t *ent)
{
	char buf[256];
	iBuyingItem = true;

	tCar = ent;
	sprintf(buf,"Buy '%s' for $%d?", ent->sName, ent->iPrice);

	BuyCar.Initialize();
	BuyCar.Add( new CLabel(buf),					0, 400, 280, 0,   0);
	BuyCar.Add( new CButton("Yeah",btn_yeah),		1, 335, 325, 75,  20);
	BuyCar.Add( new CButton("Forget it",btn_forgit),2, 495, 325, 125, 20);
	BuyCar.Add( new CButton("See it",btn_seeit),	3, 175, 325, 125, 20);
	BuyCar.Add( new CDialog("Buy Car", dlgs_medium),4, 125, 180, 550, 200 );

    ((CLabel *)BuyCar.getWidget(0))->setCenter(true);
}


///////////////////
// Process the buying part dialog
void Gar_NewspaperProcessBuyCar(void)
{
	gui_event_t *event;
	
	// Process & Draw the gui layout
	event = BuyCar.Process();
	BuyCar.Draw();

	if(event == NULL)
		return;

	switch(event->iControlID) {

		// See it
		case 3:
			if(event->iEventMsg == BUT_CLICKED) {

				// Load the car and show it
				Gar_NewspaperSeeCar(tCar->sFilename);
			}
			break;


		// Yeah
		case 1:
			if(event->iEventMsg == BUT_CLICKED) {
				// Free the gui
				BuyCar.Shutdown();

				// Not buying a part anymore
				iBuyingItem = false;
				
				if(tCar) {

                    // Can we afford the car?
                    if( !cMainGuy->canBuy(tCar->iPrice) ) {

                        Gar_InitSpeechBubble("You're $%d short", tCar->iPrice - cMainGuy->getBankroll());

                    } else {

                        // BUY IT!!
					    CCar *oldcar = cMainGuy->getCurCar();
					    if( cMainGuy->buyCar(tCar->sFilename, tCar->iPrice) ) {
						    // Set the position
						
						    CCar *newcar = cMainGuy->getCurCar();
						    cMainGuy->setCurCar(oldcar);

						    // Go back to the garage, and roll the car in
						    Gar_NewspaperShutdown();
						    tGarage->iState = GAR_NORMAL;
						    if(oldcar)
							    Gar_SwapCars(newcar);
						    else {
							    cMainGuy->setCurCar(newcar);
							    Gar_CarRollin();
						    }
    
	    					return;
                        }
                    }
				}

				tCar = NULL;

				return;
			}
			break;


		// Forget it
		case 2:
			if(event->iEventMsg == BUT_CLICKED) {

				// Free the gui
				BuyCar.Shutdown();
				
				// Not buying a part anymore
				iBuyingItem = false;
				tCar = NULL;

				return;
			}
			break;
	}
}



/*
	Generic page in the newspaper
	Can be used for both auto parts & used cars
*/


///////////////////
// Process the auto parts section
void Gar_NewspaperEntriesProcess(int nFilter)
{
	mouse_t *Mouse = System_GetMouse();

	// Draw the newspaper	
	State_Disable(GL_LIGHTING);
	State_Enable(GL_TEXTURE_2D);
	State_Enable(GL_BLEND);
	State_Disable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1,1,1,1);

	cGarLocation->setStatusText("");

	Tex_Bind(tSidebit);

	glBegin(GL_QUADS);

		// Left side
		glTexCoord2f(0,0);		glVertex2d(0,600);
		glTexCoord2f(1,0);		glVertex2d(128,600);
		glTexCoord2f(1,4.5);	glVertex2d(128,24);
		glTexCoord2f(0,4.5);	glVertex2d(0,24);
        if(MouseinRect(0,0, 128,576)) {
            if( nCurPage > 0 )
			    cGarLocation->setStatusText("Previous page");
            if( (Mouse->Up & SDL_BUTTON(1)) && !tGarage->bSpeechBubble ) {
                nCurPage--;
                nCurPage = MAX(nCurPage,0);
            }
        }

		// Right side
		glTexCoord2f(0,0);		glVertex2d(800,600);
		glTexCoord2f(1,0);		glVertex2d(672,600);
		glTexCoord2f(1,4.5);	glVertex2d(672,24);
		glTexCoord2f(0,4.5);	glVertex2d(800,24);
        if(MouseinRect(672,0, 800,576)) {
            if( nCurPage < nNumPages-1 )
			    cGarLocation->setStatusText("Next page");
            if( (Mouse->Up & SDL_BUTTON(1)) && !tGarage->bSpeechBubble ) {
                nCurPage++;
                nCurPage = MIN(nCurPage,nNumPages-1);
            }
        }
	glEnd();


	// Garage
	Tex_Bind(tsmGarage);
	glBegin(GL_QUADS);
		glTexCoord2f(0,0);		glVertex2d(790,110);
		glTexCoord2f(1,0);		glVertex2d(682,110);
		glTexCoord2f(1,1);		glVertex2d(682,34);
		glTexCoord2f(0,1);		glVertex2d(790,34);		
	glEnd();


	// White centre
	DrawRect(128,600, 672,24, CVec(0,0,0), CVec(1,1,1), 1, true, 0);



	// Draw the entries
	int y = 585;
	newsentry_t *n = tEntries;
	CVec col = CVec(0,0,0);
    int count=0;
	
	Font_SetSize(16);
	for(;n;n=n->tNext,count++) {

        // If this is the first entry, on the first page show a heading
        if(count == 0 && nCurPage == 0) {
            char text[32];
            strcpy(text, nFilter == nws_car ? "Used Cars" : "Auto Parts" );
            if( nFilter == nws_part )
                DrawPic(tNewsTitles[1], 275,550);
            else
                DrawPic(tNewsTitles[2], 285,550);

            DrawRect(130,y-1, 670,y+1, col,col, 1, true, 1);
            Font_SetSize(24);
            y-=70;
            int x = 400 - (strlen(text)*24)/2;
            //Font_Draw(x, y, col, text);
            y-=50;
            DrawRect(130,y-1, 670,y+1, col,col, 1, true, 1);
            Font_SetSize(16);
            y-=18;
        }

                
		if(n->iType != nFilter)
			continue;
        if(n->nPageNum != nCurPage)
            continue;

		Font_Draw(130, y, col, n->sDescription);
		y-= n->nHeight;

		DrawRect(130, y, 670, y+1, col, col, 1, true, 0);

		if(!iBuyingItem) {

			// Check for a click
			if( (Mouse->Up & SDL_BUTTON(1)) && !tGarage->bSpeechBubble ) {
				int i = 600-y;
				if(MouseinRect(130, i-(n->nHeight+18), 670, i)) {
                    Mouse->Up = 0;
                    Mouse->Down = 0;
					if(iNewsmode == news_autoparts)
						Gar_NewspaperBuyPart(n);
					else if(iNewsmode == news_cars)
						Gar_NewspaperBuyCar(n);
					break;
				}
			}
		}
	
		y -= 18;
	}

	// View a car
	if(iViewingCar) {
		cGarLocation->setStatusText("");
		Gar_NewspaperViewCar();
		return;
	}

	if(MouseinRect(682,490, 790,566)) {
		cGarLocation->setStatusText("Back to garage");

		// Check for a click
		if( (Mouse->Up & SDL_BUTTON(1)) && !tGarage->bSpeechBubble ) {
			// Close the newspaper and go back to the garage
			Gar_NewspaperShutdown();
			return;
		}
	}

	if(iBuyingItem) {
		if(iNewsmode == news_autoparts)			
			Gar_NewspaperProcessBuyPart();

		if(iNewsmode == news_cars)			
			Gar_NewspaperProcessBuyCar();
	}


    // Was escape pressed?
    if( System_GetKeyboard()->KeyUp[SDLK_ESCAPE] && !iBuyingItem ) {
        // Go back to the front page
        Gar_NewspaperFreeEntries();
        tGarage->iState = GAR_NEWSPAPER;
	    iNewsmode = news_frontpage;
	    iBuyingItem = false;
	    iViewingCar = false;
		return;
    }
}


///////////////////
// Calculate the height of an entry
void Gar_NewspaperCalculateHeight(newsentry_t *ent)
{
    // Find the number of newlines in the description
    int count = 1;  // Start with 1 row

    for( uint i=0; i<strlen(ent->sDescription); i++) {
        if( ent->sDescription[i] == '\n' )
            count++;
    }

    ent->nHeight = count*14;
}


///////////////////
// Sort the entries
void Gar_NewspaperSortList(void)
{
    int count = 0;
    newsentry_t *n = tEntries;
   
    for(; n; n=n->tNext)
        count++;

    // If there is less then 2 entries, don't sort
    if( count < 2 )
        return;

    newsentry_t **arr = new newsentry_t *[count];
    if( !arr ) {
        writeLog("Warning: Could not allocate room to store newspaper entries");
        return;
    }

    // Fill in the array
    int num = 0;
    n = tEntries;    
    for(; n; n=n->tNext,num++) {
        arr[num] = n;
    }

    
    // Sort the array
    int i,j;
    for(i=0; i<count; i++) {
		for(j=0; j<count-1-i; j++) {

            // Check part types
            // NOTE: Cars must all have the same 'part type' so this won't affect cars (only the price will)
            if( arr[j]->nPartType > arr[j+1]->nPartType ) {

                // Swap the pointers
                Gar_NewspaperSwapEntries(arr, j, j+1);

            } else if( arr[j]->nPartType == arr[j+1]->nPartType ) {

                // Sort by price
                if( arr[j]->iPrice > arr[j+1]->iPrice ) {
                
                    // Swap the pointers
                    Gar_NewspaperSwapEntries(arr, j, j+1);
                }
            }
        }
    }

    tEntries = arr[0];
    delete[] arr;


    // Go through the list and calculate the number of pages, and assign page numbers to each entry
    newsentry_t *psEntry = tEntries;
    nCurPage = 0;
    nNumPages = 1;
    int height = 140;   // Starting header
    int PageHeight = 580;
    for(; psEntry; psEntry=psEntry->tNext ) {

        // Add 18 for a spacer between entries
        if( height + psEntry->nHeight+18 > PageHeight ) {
            nNumPages++;
            height = 0;
        }

        psEntry->nPageNum = nNumPages-1;
        height += psEntry->nHeight+18;
    }
}


///////////////////
// Swap 2 newspaper entries around
void Gar_NewspaperSwapEntries(newsentry_t **arr, int j, int j1)
{
    newsentry_t *n;
    newsentry_t b;

    // Swap the pointers
    n = arr[j]->tPrev;
    arr[j]->tPrev = arr[j1]->tPrev;
    arr[j1]->tPrev = n;

    n = arr[j]->tNext;
    arr[j]->tNext = arr[j1]->tNext;
    arr[j1]->tNext = n;

    // Swap the array around to keep the sort happy
    b = *arr[j];
    *arr[j] = *arr[j1];
    *arr[j1] = b;
}



CCar	cSeeCar;
float   fCarSpin;

///////////////////
// Load up a car to see it
void Gar_NewspaperSeeCar(char *setupfile)
{	
	if( !cSeeCar.LoadSetup(setupfile) ) {
		return;
	}
    cSeeCar.calculateRest(CVec(10,10,-4.61f));

    fCarSpin = 0;
	iViewingCar = true;
}


///////////////////
// Draw the car
void Gar_NewspaperViewCar(void)
{
    int x = 200;
    int y = 150;
    int x2 = 600;
    int y2 = 450;

    State_Disable(GL_LIGHTING);

	CCamera cam;
	DrawRect(x,y,x2,y2, CVec(1,1,1), CVec(0.4f,0.4f,0.4f), 1, true, 0);
    
    glColor4f(1,1,1,1);
    State_Enable(GL_BLEND);
    State_Enable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Tex_Bind(tBorder);
    
    glBegin(GL_QUADS);
	glTexCoord2f(0,0);  glVertex2i(x,y);
	glTexCoord2f(0,1);  glVertex2i(x,y2);
	glTexCoord2f(1,1);  glVertex2i(x2,y2);
	glTexCoord2f(1,0);  glVertex2i(x2,y);
    glEnd();


    Render_End2D();
	
	glPushMatrix();
	glLoadIdentity();
	
	glViewport(200,150,400,300);
		
	glClear( GL_DEPTH_BUFFER_BIT );

	// Setup a camera	
	// Swap to the SEA(?) coordinate system
	glTranslatef(0.0,0.0,0.0);
	glRotatef (-90,  1, 0, 0);
	glRotatef (90,  0, 0, 1);

	cam.Setup(CVec(20,20,10), CVec(10,10,0));
	cam.LookAt(CVec(0,0,0));
	cam.Show();

    // Draw the car
    glPushMatrix();
    glTranslatef(0,0,0);
    glRotatef(fCarSpin, 0,0,1);
    cSeeCar.setPosition(CVec(0,0,0));
	cSeeCar.Draw();
    glPopMatrix();

    // Spin the car
    fCarSpin += 10*tMainSR3.fDeltaTime;

	// If the user clicked the mouse, go back to the menu
	if(System_GetMouse()->Up & SDL_BUTTON(1)) {
		iViewingCar = false;
		cSeeCar.Shutdown();
	}

    glViewport(0,0,800,600);

	glPopMatrix();
	Render_Begin2D();
}
