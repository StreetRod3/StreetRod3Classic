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


#ifndef __CPLAYER_H__
#define __CPLAYER_H__


#define		MAX_PARTS			256
#define		MAX_CARS			32
#define     MAX_OPPONENTS       32
#define     SAVE_VERSION        1


// Save game chunks
enum {
    svg_car,
    svg_part
};


class CPlayer {
public:
	// Constructor
	CPlayer() {
				
		iBankroll = 5000; // Will be 1500 in the final release

		iCarID = 0;
		iNumCars = 0;
		cCars = NULL;
		cCurrentCar = NULL;
        sName[0] = '\0';

	}


private:
	// Attributes

	// General
	char		sName[32];
	int			iBankroll;


	// Game
	CPart		*cSpareParts[MAX_PARTS];

	int			iCarID;
	int			iNumCars;
	CCar		*cCars;
	CCar		*cCurrentCar;

    // My opponents
    int         nNumOpponents;
    COpponent   cOpponents[MAX_OPPONENTS];

    // Racing input
    CInput      cRaceInputs[7];


public:
	// Methods

	//
	// System
	//
	int			Initialize(void);
	void		Shutdown(void);
    void        InitializeControls(void);


	//
	// Global stuff
	//
	void		processFrame(void);


	//
	// Game
	//
	int			makePurchase(int cost);
    void        addMoney(int money);
    bool        canBuy(int cost);
    CInput      *getRaceInput(int inp);
    void        setName(char *name);


    //
    // Save & Load
    //
    bool        saveGame(int nSaveSpot, char *szName);
    void        writeBankroll(FILE *fp);    
    bool        loadGame(int nLoadSpot);
    bool        readSaveGameName(FILE *fp, char *szName);
    void        readBankroll(FILE *fp);


    //
    // Opponents
    //
    void        loadOpponents(void);
    int         countOpponentNames(void);
    void        findOpponentName(char *szName, int num);
    COpponent   *getOpponentList(void)          { return cOpponents; }
    int         getNumOpponents(void)           { return nNumOpponents; }


	//
	// Parts
	//
	void		addPart(CPart *cPart);
	CPart		*getSparePart(int p);
	int			removeSparePart(int p);
	int			BuyPart(char *sFilename, int price);
	int			SellSparePart(int p, int money);


	//
	// Cars
	//
	int			buyCar(char *sSetupfile, int price);
	CCar		*getCurCar(void)				{ return cCurrentCar; }
	void		setCurCar(CCar *car)			{ cCurrentCar = car; }
	CCar		*getCarList(void)				{ return cCars; }
	CCar		*findCar(int iID);
    void        sellCar(CCar *pcCar, int nPrice);





	//
	// Variables
	//
	int			getBankroll(void)				{ return iBankroll; }    




};



#endif  //  __CPLAYER_H__