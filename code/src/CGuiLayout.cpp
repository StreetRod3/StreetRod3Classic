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



// GUI Layout class
// Created 5/6/02
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "gui.h"


///////////////////
// Initialize the layout
void CGuiLayout::Initialize(void)
{
	cWidgets = NULL;
    pcCapturedWidget = NULL;
	tEvent = new gui_event_t;
    pcFocus = NULL;

    // Flush the mouse
    System_InitializeInput();    
}


///////////////////
// Add a widget to the gui layout
void CGuiLayout::Add(CWidget *widget, int id, int x, int y, int w, int h)
{
	if(widget == NULL) {
		writeLog("Out of memory in CGuiLayout::Add() \n");
		return;
	}
	
	widget->Setup(id, x, y, w, h);
	widget->Create();

	// Link the widget in
    if( cWidgets )
        cWidgets->setPrev(widget);
    widget->setPrev(NULL);
	widget->setNext(cWidgets);
	cWidgets = widget;

    widget->setParent(this);
}


///////////////////
// Shutdown the gui layout
void CGuiLayout::Shutdown(void)
{
	CWidget *w,*wid;

	for(w=cWidgets ; w ; w=wid) {		
		wid = w->getNext();

		w->Destroy();

		if(w)
			delete w;
	}
	cWidgets = NULL;

	if(tEvent) {
		delete tEvent;
		tEvent = NULL;
	}

    pcFocus = NULL;
    pcCapturedWidget = NULL;
}


///////////////////
// Draw the widgets
void CGuiLayout::Draw(void)
{
	CWidget *w;

	for(w=cWidgets ; w ; w=w->getNext())
		w->Draw();
}


///////////////////
// Process all the widgets
gui_event_t *CGuiLayout::Process(void)
{
	CWidget     *w;
	int         ev=-1;
    mouse_t     *tMouse = System_GetMouse();
    SDL_Event   *Event = System_GetEvent();
   	int         Y = 600-tMouse->Y;

    // Go to the last widget
    for(w=cWidgets ; w ; w=w->getNext()) {
        if( w->getNext() == NULL )
            break;
    }


    // If a widget has captured the mouse, only process that widget
    if(pcCapturedWidget) {
        tEvent->cWidget = pcCapturedWidget;
		tEvent->iControlID = pcCapturedWidget->getID();

        ev = processWidget(pcCapturedWidget);
        if(ev >= 0) {
            tEvent->iEventMsg = ev;
            return tEvent;
        }
        return NULL;
    }


    // Parse keyboard events to the focused widget
	if(pcFocus) {
		
		// Make sure a key event happened
		if(Event->type == SDL_KEYUP || Event->type == SDL_KEYDOWN) {

			// Check the characters
			if(Event->key.state == SDL_PRESSED || Event->key.state == SDL_RELEASED) {
				tEvent->cWidget = pcFocus;
				tEvent->iControlID = pcFocus->getID();

				char input = (char)(Event->key.keysym.unicode & 0x007F);

				if(Event->type == SDL_KEYUP || Event->key.state == SDL_RELEASED)
					ev = pcFocus->KeyUp(input);
				
				if(Event->type == SDL_KEYDOWN)
					ev = pcFocus->KeyDown(input);

				if(ev >= 0) {
					tEvent->iEventMsg = ev;
					return tEvent;
				}
			}
		}
	}


	// Go through all the widgets
	for(; w ; w=w->getPrev()) {
		tEvent->cWidget = w;
		tEvent->iControlID = w->getID();

        if(w->InBox(tMouse->X,Y)) {

            ev = processWidget(w);
            if(ev >= 0) {
                tEvent->iEventMsg = ev;
                return tEvent;
            }

            return NULL;
        }


		


		// TODO: Keyboard events
	}


	return NULL;
}


///////////////////
// Process a widget
int CGuiLayout::processWidget(CWidget *w)
{
    mouse_t *tMouse = System_GetMouse();
   	int Y = 600-tMouse->Y;

    // Mouse up events ALWAYS release the widget
    if(tMouse->Up)
        releaseMouse();

    if(w->InBox(tMouse->X,Y)) {
        
        // Mouse button event first
        if(tMouse->Down) {

            // Set focus
            if(pcFocus)
                pcFocus->setFocus(false);
            w->setFocus(true);
            pcFocus = w;

            return w->MouseDown(tMouse);            
        }
        
        // Mouse up event
        if(tMouse->Up) {

            // Set focus
            if(pcFocus)
                pcFocus->setFocus(false);
            w->setFocus(true);
            pcFocus = w;

            return w->MouseUp(tMouse);
        }
                
        // Mouse over
        return w->MouseOver(tMouse);
    }

    return -1;
}


///////////////////
// Move all the widgets
void CGuiLayout::moveWidgets(int dx, int dy)
{
    CWidget *w;
    for(w=cWidgets ; w ; w=w->getNext()) {
        w->move(dx,dy);
    }
}


///////////////////
// Return a widget based on id
CWidget *CGuiLayout::getWidget(int id)
{
	CWidget *w;

	for(w=cWidgets ; w ; w=w->getNext()) {
		if(w->getID() == id)
			return w;
	}

	return NULL;
}


///////////////////
// Set the captured widget
void CGuiLayout::captureMouse(CWidget *psWidget)
{
    pcCapturedWidget = psWidget;
}


///////////////////
// Release the captured widget
void CGuiLayout::releaseMouse(void)
{
    pcCapturedWidget = NULL;
}