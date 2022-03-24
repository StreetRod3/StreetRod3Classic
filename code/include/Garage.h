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


// Garage
// Created 13/5/02
// Jason Boettcher


#ifndef __GARAGE_H__
#define __GARAGE_H__


#define     SAVE_GAMES      10


// Garage states
enum {
	GAR_NORMAL=0,
	GAR_RUNGPART,
	GAR_ENGINE,
	GAR_ENGINEPART,
	GAR_DIALOG,
	GAR_TUNING,
	GAR_NEWSPAPER,
	GAR_RUNNINGG,
	GAR_CARROLLING,
    GAR_DECAL
};


// Part change - part location
enum {
    PTL_ENGINE,
    PTL_RUNNING
};

// Part change - method
enum {
    PTM_ADD,
    PTM_REMOVE
};


// New: Part change structure
typedef struct {

    bool    bChanging;

    int     nLocation;
    int     nMethod;
    CPart   *pcPart;

} new_partchange_t;


// Part change structure
typedef struct {

	int		iChange;
	int		iNutsGoingon;

	CCar	*cCar;
	CPart	*cPart;
	gitem_t	*tItem;
} partchange_t;


// Car rolling structure
typedef struct {

    int		CarRolling;
    int		Swap;
    CCar	*cNewCar;
    bool    bSell;

} carrolling_t;


// Garage structure
typedef struct {

	int		    iState;	
    CVec        mousePoint;
    bool        bDrawGarage;

    // Assembled sign
    texture_t   *psAssembled;
    bool        bShowAssembled;
    float       fAssembledAlpha;
    

    // Speech bubble
    bool        bSpeechBubble;

	// State values
	void	    (*DialogProcess) (void);

} garage_t;


// Newspaper entry types
enum {
	nws_car,
	nws_part
};

// Newspaper entry
typedef struct newsentry_s {

	int		iType;
    int     nPartType;
    int     nHeight;
    int     nPageNum;

	char	sDescription[256];
	char	sFilename[256];
	
	char	sName[128];
	int		iPrice;

	struct newsentry_s	*tNext;
    struct newsentry_s  *tPrev;

} newsentry_t;


// Save game entries
typedef struct {
    bool    bUsed;
    char    szName[32];
    int     nSlot;

} savegame_t;



extern garage_t *tGarage;


// Routines
int		Gar_Start(char *sDir);
int		Gar_Load(char *sDir);
void    Gar_HitStreet(void);
void	Gar_Shutdown(void);
void    Gar_InitSystemMenu(void);
void    Gar_ProcessSysDialog(void);
void    Gar_InitQuitMenu(void);
void    Gar_ProcessQuitDialog(void);
void    Gar_DrawLights(void);
void    Gar_LimitCamera(void);

void    Gar_ShowAssembled(void);
void    Gar_ToggleAssembled(bool bShowing);

void	Gar_NormalView(void);
void    Gar_CarView(void);
void	Gar_MoveCamera(CVec look, float dist);
void	Gar_CarRollin(void);
void    Gar_CarRollOut(bool bSellCar);
void	Gar_SwapCars(CCar *newcar);
void	Gar_ProcessCarRoll(void);
void	Gar_SetItems(void);
void    Gar_ProcessEngRun(void);

void    Gar_SetupPartChange(void);
void	Gar_ChangePart(void);
void	Gar_PartChange2Garage(void);

// Engine
void	Gar_InitEngineScreen(void);
void	Gar_Part2Engine(void);
void	Gar_Engine2Garage(void);
void	Gar_SetEngineItems(void);

void	Gar_InitTuning(void);
void	Gar_TuningProcess(void);

// gui stuff
void	Gar_InitPartCarDialog(void);
void	Gar_ProcessPartCarDialog(void);

void	Gar_DrawSidebar(void);
void	Gar_InitPaintDialog(void);
void	Gar_ProcessPaintDialog(void);

// Newspaper
void	Gar_NewspaperInit(void);
void	Gar_NewspaperShutdown(void);
void    Gar_NewspaperFreeEntries(void);
void	Gar_NewspaperProcess(void);
void	Gar_NewspaperDrawLine(int w, int y);
void	Gar_NewspaperFrontpage(void);
void    Gar_NewspaperEntriesProcess(int nFilter);
int		Gar_Newspaper_DrawGarage(void);
void    Gar_NewspaperCalculateHeight(newsentry_t *ent);
void    Gar_NewspaperSortList(void);
void    Gar_NewspaperSwapEntries(newsentry_t **arr, int j, int j1);


// Newspaper->Parts
void	Gar_NewspaperPartsInit(void);
void	Gar_NewspaperAddPart(char *filename);
void	Gar_NewspaperBuyPart(newsentry_t *ent);
void	Gar_NewspaperProcessBuyPart(void);

// Newspaper->Cars
void	Gar_NewspaperCarsInit(void);
void	Gar_NewspaperAddCar(char *filename);
void	Gar_NewspaperProcessBuyCar(void);
void	Gar_NewspaperBuyCar(newsentry_t *ent);
void	Gar_NewspaperSeeCar(char *setupfile);
void	Gar_NewspaperViewCar(void);

// Newspaper->Stories
void    Gar_NewspaperLoadStories(void);

// Spare parts
void	Gar_InitSpareparts(void);
void	Gar_SparepartsProcess(void);

// Car list
void	Gar_InitCarList(void);
void	Gar_CarListProcess(void);

// Running Gear
void	Gar_InitRunningGear(void);
void	Gar_SetRunningGItems(void);
void	Gar_Running2Garage(void);
void	Gar_Part2RunningG(void);
void	Gar_RunningGPart2Garage(void);

// Hood repairs
void    Gar_CustomizeHood(gitem_t *g);
void    Gar_CustomizeHoodProcess(void);

// Speech
void    Gar_InitSpeechBubble(char *fmt, ...);
void    Gar_SpeechProcess(void);
void    Gar_DrawSpeech(char *text);


// Bumper bar
void    Gar_CutRepairBumper(gitem_t *g);
void    Gar_CutRepairBumperProcess(void);



/*
====================

  Garage_Parts.cpp

====================
*/

void	Gar_ChangeEnginePartCallback(gitem_t *g);
void    Gar_ChangeEnginePart(CPart *pcPart, bool bRemoval, gitem_t *g);
void    Gar_ChangeRunningGPartCallback(gitem_t *g);
void    Gar_ChangeRunningGPart(CPart *pcPart, bool bRemoval, gitem_t *g);
void    Gar_RemovePart(void);
void    Gar_FinishAddPart(void);


/*
====================

  Garage_Dlg.cpp

====================
*/

// Car Info
void	Gar_InitCarInfo(void);
void	Gar_CarInfoProcess(void);

// Stickers
void    Gar_InitStickers(void);
void    Gar_LoadStickers(void);
void    Gar_StickerProcess(void);
void    Gar_DecalInitPlacementDialog(void);
void    Gar_DecalApplyProcess(void);
void    Gar_DecalApplyProcess2(void);

// Radio
void    Gar_InitRadioMenu(void);
void    Gar_ProcessRadioDialog(void);

// New game
void    Gar_InitNewGame(void);
void    Gar_ProcessNewGameDialog(void);

// Save game
void    Gar_InitSaveGame(void);
void    Gar_SaveLoadFillSlots(void);
void    Gar_ProcessSaveGame(void);

// Load game
void    Gar_InitLoadGame(void);
void    Gar_ProcessLoadGame(void);
void    InitializeLoadGame(void);
int     ProcessLoadGame(void);

// Sell car
void    Gar_InitSellCar(CCar *pcCar);
void    Gar_ProcessSellCar(void);
void    Gar_FinishCarSell(void);
void    Gar_SellCar_MakeOffer(void);

#endif  //  __CGARAGE_H__