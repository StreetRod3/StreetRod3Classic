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


// Diner
// Created 18/6/03
// Jason Boettcher


#ifndef __DINER_H__
#define __DINER_H__


#define     MAX_TRACKS      9


// Opponent states
enum {
    opp_waiting,
    opp_rollingin,
    opp_ready,
    opp_rollingout
};


// Diner track structure
typedef struct {
    char            szDir[128];
    char            szName[64];

} dnr_track_t;


// Diner structure
typedef struct {

    bool            bProcessDiner;
    CLocation       *pcLocation;
    CSky            cSky;
    texture_t       *psSideDoor;
    bool            bSpeechBubble;

    // Opponent
    bool            bOppClicked;
    int             nOppState;
    float           fOppTimer;
    CVec            cOppPosition;
    COpponent       *pcCurOpp;

    // Tracks
    int             nNumDragTracks;
    dnr_track_t     sDragTracks[MAX_TRACKS];
    int             nNumRoadTracks;
    dnr_track_t     sRoadTracks[MAX_TRACKS];

    // Race information
    bool            bGoodOffer;
    bool            bRacing;
    
} diner_t;


// Functions
bool        Dnr_Load(char *szDir);
bool        Dnr_Start(char *szDir);
void        Dnr_Shutdown(void);
void        Dnr_ProcessGUI(void);

// Opponent
void        Dnr_ProcessOpponent(void);
void        Dnr_RollinOpponent(void);
void        Dnr_DrawOpponent(void);
void        Dnr_ChallengeOpponent(int nLevel, int nType);
void        Dnr_MakeOffer(int nType, int nMoney);
void        Dnr_LetsGo(void);

// Speech bubble
void        Dnr_InitSpeechBubble(char *fmt, ...);
void        Dnr_SpeechProcess(void);

#endif  //  __DINER_H__