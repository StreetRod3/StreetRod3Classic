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


// Part class
// Created 23/5/02
// Jason Boettcher


#ifndef __CPART_H__
#define __CPART_H__


typedef struct gitem_s gitem_t;
class CLocation;

#define		MAX_BLOCK_PARTS		16


// Part sizes (for manifold & block)
enum {
    SZ_SMALL,
    SZ_MEDIUM,
    SZ_LARGE
};


// Manufacturer
enum {
    MAN_ANY,
    MAN_GM,
    MAN_FORD,
    MAN_CHRYSLER
};



// Part types
enum {
	// Whole engine
	PRT_ENGINE=0,
	
	// Engine bay
	PRT_BLOCK,
	PRT_INTAKEMAN,
	PRT_CARBY,
	PRT_AIRFILTER,
	PRT_ALTER,
	PRT_DIST,
	PRT_STARTER,
	PRT_FAN,

	// Running gear
	PRT_TIRE,
	PRT_DIFF,
	PRT_TRANS,
	PRT_LMUFFLER,
	PRT_RMUFFLER,
    PRT_SHOCK,


    // Last part value
    PRT_LAST
};


// Muffler helper
enum {
	LEFT_MUFFLER,
	RIGHT_MUFFLER
};


/*
Notes:
  A block is an engine if it has parts attached to it.
  If it is just the block on it's own, it's a block.
*/

class CPart;


class CPart {
public:
	// Constructor
	CPart() {
		cModel = NULL;
        cNut = NULL;
		iPartID = 0;
        nUniqueID = -1;

		iminPrice = 0;
		imaxPrice = 20;

        iManufacturer = MAN_ANY;
		iType = PRT_BLOCK;
		iUsed = false;
		iDamage = 0;
		iTuning = 0;
		iEngineRunning = false;
		iDrawSpecial = false;
		iDrawNuts = false;
		iDrawHidden = false;
        iThroughHood = false;

		fSpin = 0;
		fShake = 0;
		iShakeDir = 0;

		iNumNuts = 0;
		cNuts = NULL;
		
		cParent = NULL;
		//cInManifold = NULL;
		//cFan = NULL;

		for(int i=0;i<MAX_BLOCK_PARTS;i++)
			cAttachedParts[i] = NULL;

	}


private:
	// Attributes

	// 3D stuff
    char        szModelDir[128];
	CModel		*cModel;
    CModel      *cNut;
	CVec		vPos;
    CVec        cDimensions;
	CMatrix		cMatrix;
	gobject_t	tObjDetails;
	int			iPartID;
    int         nUniqueID;    


	// Game properties
    int         iManufacturer;
	char		sName[32];
	char		sObjName[32];
	int			iType;
	int			iUsed;
    int         iSize;
    int         iCarbys;
    int         iBarrels;
    int         iThroughHood;

	int			iminPrice;
	int			imaxPrice;
	
	int			iDamage;
	int			iTuning;

    // Suspension Performance
    float       fSpring;
    float       fDampening;
    float       fRestLength;
    float       fCornering;

	// Transmission
	int			bAutomatic;
	int			iNumGears;
	float		fGearRatio[33]; // Allow for up to 32 gears plus neutral

	// Engine running
	int			iEngineRunning;
	float		fSpin;
	float		fShake, iShakeDir;
	
	int			iDrawSpecial;
	int			iDrawHidden;
	int			iDrawNuts;

	int			iNumNuts;
	CNut		*cNuts;


	CPart		*cParent;

	// Parts attached if this is an engine
	CPart		*cAttachedParts[MAX_BLOCK_PARTS];
	

public:
	// Methods

	void		Initialize();

	int			Load(char *sFName);
	int			LoadSetup(char *sFilename, char *sSection);
	int			LoadPartSetup(char *sFName);
	void		LoadNuts(void);
	void		Setup(CMatrix carmat, CVec carpos, gobject_t *g);
	void		setPosition( CMatrix carmat, CVec carpos );
	
	// Blocks have parts attached to them
	int			LoadBlockParts(char *sSetupFile);
	int			LoadAttached(char *sSetupFile, char *sName, gobject_t *g, int id);

	int			AllocateAttached(void);
	void		SetupAttached(void);
	void		removeAttached(CPart *part);

	void		Shutdown(void);

	void		Draw(void);
	void		DrawSpecial(void);
	void		DrawNuts(void);
	void		DrawAttached(void);

	
	// Game functions
	void		StartEngine(void);
	void		SimulateEngine(void);
	void		SwitchEngineOff(void);
	void		SpinFan(void);
	void		SpinTyre(float dir);
	int			CalculateSelling(float fRandom);
    int         repairCost(void);
    int         repair(void);
    bool        isComplete(void);


	// Useful functions
	int			LineIntersect(CVec p1, CVec dir);
	int			LineIntersectNut(CVec p1, CVec dir);

	int			AllNutsGone(void);
	int			AllNutsOn(void);
	void		ClearNuts(void);
    void        putNutsOn(void);

    bool        checkManufacturer(CPart *prt);

	void		AddAttached(gitem_t *item, CLocation *loc);
	int			AddPart(CPart *part, char *msg);
	int			FindAttached(int type);
	int			AddOnto(CPart *part, char *msg);
	CPart		*FindPart(int type, int id);
    gobject_t   *getAirFilterGObject(void);

    bool        requiresHoodHole(void);

    void        write(FILE *fp);
    void        read(FILE *fp);


	char		*getString(char *sBuf);



	// Variables
	void		setPartID(int _id)		{ iPartID = _id; }
	int			getPartID(void)			{ return iPartID; }

    void        setUniqueID(int _u)     { nUniqueID = _u; }
    int         getUniqueID(void)       { return nUniqueID; }

	void		setUsed(int _u)			{ iUsed = _u; }
	int			isUsed(void)			{ return iUsed; }
	void		setPos(CVec _p)			{ vPos = _p; }
	CVec		getPos(void)			{ return vPos; }
    CVec        getDimensions(void)     { return cDimensions; }
	void		setDrawSpecial(int _s)	{ iDrawSpecial = _s; }
	void		setDrawNuts(int _n)		{ iDrawNuts = _n; }
	int			getType(void)			{ return iType; }
	CMatrix		getMatrix(void)			{ return cMatrix; }

	CNut		*getNut(int n)			{ return &cNuts[n]; }
	int			getNumNuts(void)		{ return iNumNuts; }

	void		setHidden(int h)		{ iDrawHidden = h; }
	void		setSpin(float s)		{ fSpin = s; }

	char		*getName(void)			{ return sName; }
    

    // Suspension Performance
    float       getSpring(void)         { return fSpring; }
    float       getDampening(void)      { return fDampening; }
    float       getRestLength(void)     { return fRestLength; }
    float       getCornering(void)      { return fCornering; }
    //int         getThroughHood(void)    { return iThroughHood; }

	// Transmission
	int			getAutomatic(void)		{ return bAutomatic; }
	int			getNumGears(void)		{ return iNumGears; }
	float		getGearRatio(int gear)	{ return fGearRatio[gear]; }
	

	// Attached parts
	void		setParent(CPart *p)		{ cParent = p; }
	CPart		*getParent(void)		{ return cParent; }
	CPart		*getAttached(int p)		{ return cAttachedParts[p]; }
	CModel		*getModel(void)			{ return cModel; }

	gobject_t	*getGameObject(void)	{ return &tObjDetails; }

    // Part info
    int         getNumCarbys(void)      { return iCarbys; }
    int         getSize(void)           { return iSize; }
    int         getManufacturer(void)   { return iManufacturer; }
    int         getNumBarrels(void)     { return iBarrels; }
    int         getDamage(void)         { return iDamage; }


	CollisionModel3D *getCD(void)		{ return cModel->getCDModel(); }

};



#endif  //  __CPART_H__