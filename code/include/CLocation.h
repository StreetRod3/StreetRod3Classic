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


// Location class
// Created 9/5/02
// Jason Boettcher


#ifndef __CLOCATION_H__
#define __CLOCATION_H__


// Game item types
enum {
	GMI_GOBJECT=0,
	GMI_CAROBJECT,
	GMI_PART,
	GMI_SCREENRECT
};

// Game Item structure
typedef struct gitem_s {
	int			iUsed;
	char		sText[64];
	void		(*Use) (struct gitem_s *g);
	int			iType;

	int			ix, iy, ix2, iy2;

	int			iID;
	CCar		*cCar;
	int			iMeshNum;
	CPart		*cPart;	
} gitem_t;



class CLocation {
public:
	// Constructor
	CLocation() {

		cScene = NULL;
		cCamera = NULL;

		iNumItems = 0;
		tItems = NULL;
		iCheckItems = true;

		iGotStatusText = false;
		sStatusText[0] = 0;

		iGrabbed = false;
		fcamYaw = fcamPitch = 0;
		fMinYaw = fMaxYaw = -1;
		fMinPitch = fMaxPitch = -1;
		iAngleLimiting = true;

		bCameraMoving = false;
		bCamLocked = false;

        bPartInfo = false;
        fPartInfoAlpha = 0;
        pcPartInfo = NULL;
	}


private:
	// Attributes

	// Scene
	CModel		*cScene;
	CCamera		*cCamera;


	// Camera
	int			iGrabbed;
	CVec		vGrabPos;
	CVec		vLookAt;
	CVec		vCamPos;
    double      dCamTime;
	float		fDistance;
	float		fcamYaw, fcamPitch;
	float		fMinYaw, fMaxYaw;
	float		fMinPitch, fMaxPitch;
	int			iAngleLimiting;

	// Camera motions
	bool		bCameraMoving;
	float		fCamTime;
	CVec		vCamDest;
	CVec		vCamLookDest;
	CVec		vCamDest_dir;
	CVec		vCamLookDest_dir;
	bool		bCamLocked;


	// Gameplay
	int			iNumItems;
	gitem_t		*tItems;
	int			iCheckItems;

    // Part info
    bool        bPartInfo;
    float       fPartInfoAlpha;
    CPart       *pcPartInfo;

	int			iGotStatusText;
	char		sStatusText[64];


public:
	// Methods

	int			Load(char *sFilename);
	void		Shutdown(void);

	void		Process(void);
	void		ProcessGameItems(void);

	void		Draw(bool drawModel, bool lighting, bool setupCam);
	void		FinalDraw(void);
    void        drawPartInfo(CPart *pcPart, float fAlpha);


	// Camera
	void		ProcessCamera(void);
	void		startCameraMove(CVec destlookat, CVec destpos);
	void		processCameraMove(float dt);
	void		SetupCamera(CVec _look, float _dist)		{ vLookAt = _look; fDistance = _dist; }
	void		SetupYaw(float fmin, float fmax)			{ fMinYaw = fmin; fMaxYaw = fmax; }
	void		SetupPitch(float fmin, float fmax)			{ fMinPitch = fmin; fMaxPitch = fmax; }
	void		setAngleLimit(int _a)						{ iAngleLimiting = _a; }
	void		lockCamera(void)							{ bCamLocked = true; }
	void		unLockCamera(void)							{ bCamLocked = false; }


	// Game items
	void		ClearGameItems(void);
	void		AddGameItem(gitem_t *g);
	void		setItemCheck(int _c)				{ iCheckItems = _c; }	
	

	// Variables
	CVec		getCamPos(void)						{ return cCamera->getPos(); }
	void		setCamPos(CVec _p)					{ vCamPos = _p; cCamera->setPos(_p); }
	CVec		getCamLook(void)					{ return vLookAt; }
	void		setStatusText(char *sText)			{ strcpy(sStatusText,sText); iGotStatusText = true; }	
    CModel      *getScene(void)                     { return cScene; }
    CCamera     *getCamera(void)                    { return cCamera; }




};




#endif  //  __CLOCATION_H__