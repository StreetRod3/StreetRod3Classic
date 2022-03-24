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


// Player class
// Created 30/5/02
// Jason Boettcher


#include "defs.h"
#include <direct.h>
#include "game.h"
#include "system.h"


///////////////////
// Initialize the player
int CPlayer::Initialize(void)
{
	for(int n=0;n<MAX_PARTS;n++)
		cSpareParts[n] = NULL;

	cCars = NULL;
	cCurrentCar = NULL;
	iNumCars = 0;
    iCarID = 0;

	iBankroll = 5000; // Will be 1500 for the final release

    // Load my opponents
    loadOpponents();

    InitializeControls();

	return true;
}


///////////////////
// Set the name
void CPlayer::setName(char *name)
{
    sr_strncpy(sName, name, 31);
}



///////////////////
// Initialize the controls
void CPlayer::InitializeControls(void)
{
    // Initialize the control inputs
    for(int n=0; n<7; n++)
        cRaceInputs[n].Setup(psOptions->szControls[n]);
}


///////////////////
// This makes a purchase
// Returns: True if we could afford it
int CPlayer::makePurchase(int cost)
{
	if(iBankroll - cost < 0)
		return false;

	iBankroll -= cost;
	return true;
}


///////////////////
// Tests if we can buy the item
bool CPlayer::canBuy(int cost)
{
    if(iBankroll - cost < 0)
		return false;

    return true;
}


///////////////////
// Add some money to the player
void CPlayer::addMoney(int money)
{
    iBankroll += money;
}


///////////////////
// Shutdown the player
void CPlayer::Shutdown(void)
{
	// It is up to the player to free his/her spare parts
	for(int n=0;n<MAX_PARTS;n++) {
		if(cSpareParts[n]) {
			cSpareParts[n]->Shutdown();
			delete cSpareParts[n];
			cSpareParts[n] = NULL;
		}
	}

	// Shutdown the cars
	CCar *car = cCars;
	CCar *next = NULL;
	for(; car; car=next) {
		next = car->getNext();

		car->Shutdown();
		delete car;
	}
}



///////////////////
// Process a frame
void CPlayer::processFrame(void)
{
	// Process the radio
	if(pcRadio)
		pcRadio->processPlayer();
}





/*
============================

			Parts

============================
*/


///////////////////
// Add a part to the player
void CPlayer::addPart(CPart *cPart)
{
	// Find a free slot
	for(int n=0;n<MAX_PARTS;n++) {
		if(cSpareParts[n] == NULL) {
			cSpareParts[n] = cPart;
			return;
		}
	}

	// Error: No free slots
}


///////////////////
// Remove a part from the spare parts list
int CPlayer::removeSparePart(int p)
{
    // Safety check
    if(p < 0 || p >= MAX_PARTS)    
		return false;

    // We dont' free the part because it gets transferred onto the car
	cSpareParts[p] = NULL;
	return true;
}


///////////////////
// Sell a spare part
int CPlayer::SellSparePart(int p, int money)
{
	// Safety check
	if(p < 0 || p >= MAX_PARTS)
		return false;

	// Increase my bankroll
	iBankroll += money;

	// Free the spare part
	cSpareParts[p]->Shutdown();
	deletePart( cSpareParts[p] );
	cSpareParts[p] = NULL;

	return true;
}


///////////////////
// Get a spare part from the list
CPart *CPlayer::getSparePart(int p)
{
	// Safety check
	if(p < 0 || p >= MAX_PARTS)
		return NULL;

	return cSpareParts[p];
}


///////////////////
// Buy a part and add it into the spare parts list
int CPlayer::BuyPart(char *sFilename, int price)
{
	if(!makePurchase(price))
		return false;

	CPart *part = newPart();
	if(part == NULL)
		return false;

    if(!part->LoadPartSetup(sFilename)) {
		part->Shutdown();
		deletePart(part);
		return false;
	}

	// Add the part to the spare parts list
	addPart(part);

	return true;
}



/*
============================

			Cars

============================
*/


///////////////////
// Buy a car
int CPlayer::buyCar(char *sSetupfile, int price)
{
	// Deduct the money
	if(!makePurchase( price )) {
		writeLog("Couldn't afford car :(\n");
		return false;
	}

	CCar *car = new CCar;
	if(car == NULL) {
		writeLog("Couldn't alloc the memory\n");
		return false;
	}

	if(!car->LoadSetup( sSetupfile )) {
		car->Shutdown();
		delete car;
		return false;
	}

	car->setID( iCarID++ );

	// Link the car into the list
	car->setNext( cCars );
    car->setPrev( NULL );
    if(cCars)
        cCars->setPrev(car);
	cCars = car;

	// This is now my current car
	cCurrentCar = car;

    //printf("Selling price for '%s' is %d\n",car->getName(),car->calculateSellingPrice());

	iNumCars++;

	return true;
}


///////////////////
// Return a car based on car id
CCar *CPlayer::findCar(int iID)
{
	CCar *car = cCars;

	for(; car; car=car->getNext() ) {
		if(car->getID() == iID)
			return car;
	}

	return NULL;
}


///////////////////
// Sell a car
void CPlayer::sellCar(CCar *pcCar, int nPrice)
{
    assert(pcCar);
    if(nPrice == -1)
        return;

    // If this is the current car, set the current car to nothing
    if(cCurrentCar) {
        if(pcCar->getID() == cCurrentCar->getID())
            cCurrentCar = NULL;
    }

    iNumCars--;
    iBankroll += nPrice;

    // Unlink the car
    if(pcCar->getPrev())
        pcCar->getPrev()->setNext(pcCar->getNext());
    if(pcCar->getNext())
        pcCar->getNext()->setPrev(pcCar->getPrev());

    // Set the car list
    if(cCars) {
        if(cCars->getID() == pcCar->getID())
            cCars = pcCar->getNext();
    }

    pcCar->Shutdown();
    delete pcCar;
}



/*
============================

	     Opponents

============================
*/


///////////////////
// Load the opponents
void CPlayer::loadOpponents(void)
{
    char szFilename[256];
    char szOppName[256];

    nNumOpponents = 0;

    // Get the number of names available
    int numNames = countOpponentNames();

    // Go through all the sr3 files in the opp dir
    if( !FindFirst("./data/opp","*.sr3", szFilename) )
        return;    

    while(1) {

        // Find an appropriate name
        findOpponentName(szOppName, GetRandomInt(numNames-1));


        // Load the opponent
        if( cOpponents[nNumOpponents].load(szOppName, szFilename) ) {
            nNumOpponents++;
            if( nNumOpponents >= MAX_OPPONENTS ) {
                writeLog("MAX_OPPONENTS reached\n");
                break;
            }
        }

        if( !FindNext(szFilename) )
            break;
    }
}


///////////////////
// Count the number of opponent names
int CPlayer::countOpponentNames(void)
{
    int     count = 0;
    FILE    *fp;

    fp = fopen("data/opp/namelist.txt","rt");
    if( !fp )
        return 0;

    char szLine[1024];
    while( fgets(szLine, 1023, fp) )
        count++;

    fclose(fp);
    return count;
}


///////////////////
// Find an opponent name
void CPlayer::findOpponentName(char *szName, int num)
{
    int     count = 0;
    FILE    *fp;
    
    strcpy(szName, "Jimmy");

    fp = fopen("data/opp/namelist.txt","rt");
    if( !fp )
        return;

    char szLine[1024];
    while( fgets(szLine, 1023, fp) ) {
        if( count == num ) {
            sr_strncpy(szName, szLine, 255);

            // Clean up the name
            if( szName[strlen(szName)-1] == '\n')
                szName[strlen(szName)-1] = '\0';
            TrimSpaces(szName);

            break;
        }
        count++;
    }

    fclose(fp);
}


///////////////////
// Return a race input
CInput *CPlayer::getRaceInput(int inp)
{
    if(inp < 0 || inp >= 7)
        return NULL;

    return &cRaceInputs[inp];
}



///////////////////
// Save the game
bool CPlayer::saveGame(int nSaveSpot, char *szName)
{
    char    szFilename[256];
    char    szSig[] = {'S','t','r','e','e','t',' ','R','o','d',' ','3',' ','S','A','V','E',0x1A,0x08,0x04};
    int     nVersion = SAVE_VERSION;
    int     type = 0;
    int     cur = -1;

    // Create the saves directory
    _mkdir("saves");
    sprintf(szFilename, "saves/save%d.sav",nSaveSpot);

    FILE *fp = fopen(szFilename,"wb");
    if(!fp) {
        writeLog("Error: Could not open '%s' for game save\n",szFilename);
        return false;
    }


    //
    // Save the game
    //

    // Header
    fwrite(szSig,       sizeof(char),   20,     fp);
    fwrite(&nVersion,   sizeof(int),    1,      fp);
    writePascalString(szName,fp);

    // Player info
    writePascalString(sName,fp);
    writeBankroll(fp);

    


    // Cars
    CCar *car = cCars;
    type = svg_car;
    for(; car; car=car->getNext()) {
        fwrite(&type,   sizeof(int),    1,  fp);
        if(cCurrentCar->getID() == car->getID())
            cur = 1;
        else
            cur = 0;
        fwrite(&cur,    sizeof(int),    1,  fp);

        car->save(fp);
    }


    // Spare Parts
    type = svg_part;
    for(int i=0; i<MAX_PARTS; i++) {
        if(!cSpareParts[i])
            continue;

        fwrite(&type,   sizeof(int),    1,  fp);
        cSpareParts[i]->write(fp);
    }


    fclose(fp);

    return true;
}


///////////////////
// Load the game
bool CPlayer::loadGame(int nLoadSpot)
{
    char    szFilename[256];
    char    szSig[] = {'S','t','r','e','e','t',' ','R','o','d',' ','3',' ','S','A','V','E',0x1A,0x08,0x04};
    char    szID[30];
    char    szName[128];
    int     nVersion = -1;
    int     type = 0;
    int     cur = 0;
    CCar    *car = NULL;
    CPart   *part = NULL;


    sprintf(szFilename, "saves/save%d.sav",nLoadSpot);

    FILE *fp = fopen(szFilename,"rb");
    if(!fp) {
        writeLog("Error: Could not open '%s' for load game save\n",szFilename);
        return false;
    }


    //
    // Load the game
    //

    // Header
    fread(szID,        sizeof(char),   20,     fp);
    if(memcmp(szID, szSig,20) != 0) {
        writeLog("Error: '%s' not a valid save game\n",szFilename);
        fclose(fp);
        return false;
    }
    fread(&nVersion,   sizeof(int),    1,      fp);
    if(nVersion != SAVE_VERSION) {
        writeLog("Error: '%s' wrong save game version\n",szFilename);
        fclose(fp);
        return false;
    }

    readPascalString(szName,fp);

    // Player info
    readPascalString(sName,fp);
    readBankroll(fp);

    int nCurPart = 0;
    
    while(1) {
        fread(&type,    sizeof(int),    1,  fp);
        if(feof(fp))
            break;

        switch(type) {

            // Car
            case svg_car:
                fread(&cur, sizeof(int),    1,  fp);

                car = new CCar;
                if(!car) {
                    writeLog("Error: Out of memory\n");
                    fclose(fp);
                    return false;
                }
                if(!car->loadSaveGame(fp)) {
                    delete car;
                    fclose(fp);
                    return false;
                }

                car->setID(iCarID++);
                iNumCars++;

                // Link the car in
                car->setNext( cCars );
	            cCars = car;

                if(cur)
                    cCurrentCar = car;
                break;

            // Part
            case svg_part:
                part = new CPart;
                if(!part) {
                    writeLog("Error: Out of memory\n");
                    fclose(fp);
                    return false;
                }
                part->read(fp);

                // Add it in
                cSpareParts[nCurPart++] = part;
                break;

            default:
                writeLog("Unknown item type\n");
        }
    }

    fclose(fp);

    return true;
}


///////////////////
// Get the save game name
// Returns true if the savegame is valid
bool CPlayer::readSaveGameName(FILE *fp, char *szName)
{
    char    szSig[] = {'S','t','r','e','e','t',' ','R','o','d',' ','3',' ','S','A','V','E',0x1A,0x08,0x04};
    char    szID[30];
    int     nVersion = -1;

    // Header
    fread(szID,        sizeof(char),   20,     fp);
    if(memcmp(szID, szSig, 20) != 0) {
        fclose(fp);
        return false;
    }
    fread(&nVersion,   sizeof(int),    1,      fp);
    if(nVersion != SAVE_VERSION) {
        fclose(fp);
        return false;
    }

    // Save game name
    readPascalString(szName,fp);

    return true;
}


///////////////////
// Write the bankroll in some obscure format
void CPlayer::writeBankroll(FILE *fp)
{
    byte    silly[] = {"Hello mr hacker, go fuck yourself!\n\n\n"};
    typedef union {
        int shityeah;
        byte blah[4];
    } jason;

    jason rocks;
    rocks.shityeah = iBankroll;

    fwrite(silly,           sizeof(byte),   38, fp);
    fwrite(&rocks.blah[2],  sizeof(byte),   1,  fp);
    fwrite(&rocks.blah[1],  sizeof(byte),   1,  fp);
    fwrite(&rocks.blah[3],  sizeof(byte),   1,  fp);
    fwrite(&rocks.blah[0],  sizeof(byte),   1,  fp);
}


///////////////////
// Read the bankroll from some obscure format
void CPlayer::readBankroll(FILE *fp)
{
    byte    silly[38];

    typedef union {
        int shityeah;
        byte blah[4];
    } jason;

    jason rocks;    

    fread(silly,           sizeof(byte),   38, fp);
    fread(&rocks.blah[2],  sizeof(byte),   1,  fp);
    fread(&rocks.blah[1],  sizeof(byte),   1,  fp);
    fread(&rocks.blah[3],  sizeof(byte),   1,  fp);
    fread(&rocks.blah[0],  sizeof(byte),   1,  fp);

    iBankroll = rocks.shityeah;
}