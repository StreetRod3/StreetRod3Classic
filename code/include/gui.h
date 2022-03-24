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



// Main GUI header file
// Created 30/5/02
// Jason Boettcher


#ifndef __GUI_H__
#define __GUI_H__

#include "System.h"

#include "CWidget.h"
#include "CDialog.h"
#include "CButton.h"
#include "CScrollbar.h"
#include "CListview.h"
#include "CColourSlider.h"
#include "CColourPicker.h"
#include "CCheckbox.h"
#include "CTextbox.h"
#include "CSlider.h"
#include "CLabel.h"
#include "Font.h"


#include "CGuiLayout.h"


// Functions
bool    gui_Initialize(void);
void    gui_DrawMouse(void);




#endif  //  __GUI_H__