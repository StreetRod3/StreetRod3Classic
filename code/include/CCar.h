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


// Car class
// Created 23/5/02
// Jason Boettcher


#ifndef __CCAR_H__
#define __CCAR_H__


#include <vector>


// Sticker structure
typedef struct sticker_s {

    // Sticker info
    char        szFilename[256];
    texture_t   *psTexture;
    CVec        cColour;

    // Polygons
    int         nNumPolygons;
    polygon_t   *psPolygons;

    struct sticker_s    *psNext;

} sticker_t;


// Hood types
enum {
    HOOD_NORMAL,
    HOOD_PERFORMANCE
};



class CCar {
public:
	// Constructor
	CCar() {
		iID = 0;
		cModel = NULL;
		tShadow = NULL;
        psCorona = NULL;
		cNextCar = NULL;
        cPrevCar = NULL;
		iEngineRunning = false;
        iHoodType = HOOD_NORMAL;
        nRearBumper = true;
        nFrontBumper = true;
        psStickers = NULL;

		cBlock = NULL;
        for(int i=0;i<4;i++) {
			cTires[i] = NULL;
            cShocks[i] = NULL;
        }
		for(i=0;i<2;i++)
			cMufflers[i] = NULL;
        cDiff = NULL;
        cTrans = NULL;

		fgotoYaw = 0;
		fCurYaw = 0;

	}


private:
	// Attributes

	int			iID;
	
	// 3D Model
	CModel		*cModel;
    char        szCarDir[256];
	texture_t	*tShadow;
    texture_t   *psCorona;
    CollisionModel3D    *BoundBox;


	// Position in world
	CVec		vPos;
	CMatrix		cMatrix;

    // Resting in the garage & restaurant
    float       fRestHeight;
    CVec        cRestAngle;



	// Driving
	carsim_t	sCarSim;
	float		fgotoYaw;
	float		fCurYaw;
    //CVec        cFrontLights[2];
    //CVec        cRearLights[2];
    CVec        cInCarCamera;

	std::vector<CVec>	m_FrontLights;
	std::vector<CVec>	m_RearLights;
	
	// TEST
	CVec		cDim[2];
	CModel		*pcBox;
    

	// Game details
	char		sName[32];
	int			iEngineRunning;
	colour_t	tColour;
	int			iMinPrice;
	int			iMaxPrice;
	int			iSellingPrice;
    int         iEngineBaySize;
    int         iHoodType;
    int         nRearBumper;
    int         nFrontBumper;

    // Stickers
    sticker_t   *psStickers;


	// Garage details
	int			iRollingIn;
	int			iRollingOut;
	CVec		vRollStartPos;
	CVec		vRollEndPos;


	// Parts
	CPart		*cTires[4];
	CPart		*cBlock;
	CPart		*cMufflers[2];
    CPart       *cShocks[4];
    CPart		*cDiff;
    CPart       *cTrans;


	// Linking
	CCar		*cNextCar;
    CCar        *cPrevCar;


public:
	// Methods

	int			LoadModel(char *sCarDir);
	void		Shutdown(void);

	int			LoadSetup(char *sSetupfile);

	void		LoadLightPositions(void);

	int			getMeshi(char *sName);
	mesh_t		*getMeshs(char *sName);

	void		setPosition(CVec pos);
    void        applyMatrix(CMatrix m);
    void        calculateRest(CVec pos);

	void		createMatrix(float *m);

	int			LineIntersect(CVec p1, CVec dir, bool closest);

	CVec		TranslateVector(CVec v);

	// Types of drawing:
	// Car
	// Outside parts
	// Engine parts
	// Underneath parts
	// Driving

	void		Draw(void);
	void		DrawOutsideParts(void);
	void		DrawEngine(void);
	void		DrawDriving(CCamera *pcCamera);
	void		DrawLights(CCamera *pcCamera);


	// Game routines
	int			checkPartRemoval(int type);	

	int			AddEnginePart(CPart *part, char *msg);
	int			AddPart(CPart *part);
	int			AddRunningGPart(CPart *part, char *msg);
	void		PaintCar(CVec colour, bool bClearDecals);
    void        removeEnginePart(CPart *part);
	void		removeRunningGearPart(CPart *part);
    bool        runningGearComplete(void);

	void		StartEngine(void);
	void		SwitchEngineOff(void);
	void		SimulateEngine(void);

    void        setRunningGearVis(int nVis);    
    void        setEngineVis(int nVis);

    void        createEnginePartFilter(bool *pbFilter);
    void        createRunningGFilter(bool *pbFilter);

	int			calculatePrice(void);
    int         calculateSellingPrice(void);

    void        applyDecal(char *szFilename, CCamera *psCam, int Rx, int Ry, int Rw, int Rh, CVec cColour);
    void        clearDecals(void);

    void        setHoodType(int nType);    
    void        setupHood(void);
    bool        needPerformanceHood(CPart *p);

    void        removeBumper(int bump);
    void        repairBumper(int bump);

	void		startGarageRollIn(void);
	void		processGarageRollIn(void);
    void        finishGarageRollIn(void);
	void		startGarageRollOut(void);
	void		processGarageRollOut(void);
    void        finishGarageRollOut(void);


	// Car Driving
	void		SetupSimulation(void);
	void		SimulateDriving(float dt);

	void		Camera_FollowCar(CVec *campos, float dt);


    // Save game
    void        save(FILE *fp);
    bool        loadSaveGame(FILE *fp);
    void        writeDecal(sticker_t *d, FILE *fp);
    bool        readDecal(FILE *fp);

    void        writePart(CPart *pcPart, FILE *fp);
    CPart       *readPart(FILE *fp, bool *bSuccess);



	//
	// Variables
	//

	void		setID(int id)			{ iID = id; }
	int			getID(void)				{ return iID; }

	void		setPos(CVec _p)			{ vPos = _p; }
	CVec		getPos(void)			{ return vPos; }
	CModel		*getModel(void)			{ return cModel; }

	// Parts
	CPart		*getTire(int t)			{ return cTires[t]; }
    CPart       *getShock(int s)        { return cShocks[s]; }
	CPart		*getBlock(void)			{ return cBlock; }
	CPart		*getMuffler(int m)		{ return cMufflers[m]; }
    CPart       *getDiff(void)          { return cDiff; }
    CPart       *getTrans(void)         { return cTrans; }
	void		setMuffler(CPart *p, int i)		{ cMufflers[i] = p; }
	
	// Simulation
	carsim_t	*getCarSim(void)		{ return &sCarSim; }

    CollisionModel3D    *getColMod(void)    { return BoundBox; }

    CMatrix     *getMatrix(void)        { return &cMatrix; }

    CVec        getInCarCamera(void)    { return cInCarCamera; }

	// Game
	char		*getName(void)			{ return sName; }
	int			getRollingIn(void)		{ return iRollingIn; }
	int			getRollingOut(void)		{ return iRollingOut; }
    colour_t    getColour(void)         { return tColour; }
    int         getHoodType(void)       { return iHoodType; }
    int         getFrontBumper(void)    { return nFrontBumper; }
    int         getRearBumper(void)     { return nRearBumper; }

	// Linking
	CCar		*getNext(void)			{ return cNextCar; }
	void		setNext(CCar *c)		{ cNextCar = c; }
    CCar        *getPrev(void)          { return cPrevCar; }
    void        setPrev(CCar *c)        { cPrevCar = c; }

};



#endif  //  __CCAR_H__