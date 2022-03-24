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

// Sky class
// Created 8/10/01
// Jason Boettcher


#ifndef __CSKY_H__
#define __CSKY_H__


#define		MAX_STARS	    128



// Star structure
typedef struct {
    CVec    pos;
    float   size;
    float   flicker;
    float   yaw, pitch;    
} star_t;



// Sky class
class CSky {
public:
	// Constructor
	CSky() {
		m_psStars = NULL;
		m_psMoonTexture = NULL;
		m_psStarTexture = NULL;
	}


private:
	// Attributes

	star_t		*m_psStars;

	texture_t	*m_psMoonTexture;
    texture_t	*m_psStarTexture;


public:
	// Methods

	bool		Initialize(void);
	void		Shutdown(void);

	void		Simulate(float dt);

	void		Render(CCamera *psCamera);
};






#endif  //  __CSKY_H__
