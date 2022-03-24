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


// gui Listview class
// Created 16/6/02
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "gui.h"


///////////////////
// Draw the list view
void CListview::Draw(void)
{
	State_Disable(GL_TEXTURE_2D);
	State_Disable(GL_LIGHTING);
	State_Enable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(1,1,1,1);

    DrawRect(iX-1,iY+1, iX+iWidth+1, iY-iHeight-1, CVec(0.5f,0.5f,0.5f), CVec(0,0,0), 1, false, 1);
    DrawRect(iX,  iY,   iX+iWidth,   iY-iHeight,   CVec(1,1,1),          CVec(0,0,0), 1, false, 1);
    DrawRect(iX+1,iY-1, iX+iWidth-1, iY-iHeight+1, CVec(0.5f,0.5f,0.5f), CVec(0,0,0), 1, false, 1);


	// Draw the columns
	lv_column_t *col = tColumns;
	int x=iX+2;
	
	Font_SetSize(14);
	for(;col;col = col->tNext) {
		Font_Draw(x,iY+2,CVec(1,1,1),col->sText);
		x += col->iWidth;
	}


	// Draw the items
	int y = iY-18;
	x = iX+2;
	lv_item_t *item = tItems;
	int count=0;
	for(;item;item = item->tNext) {
		if(count++ < cScrollbar.getValue())
			continue;

		// Selected?
		if(item->iSelected)
			DrawRect(iX+2,y+17,iX+iWidth-1,y,CVec(0,0,0),CVec(0,0,1),0.75f,true,0);

		// Draw the sub items
		if(item->tSubitems)
			Font_Draw(x,y,CVec(1,1,1),item->tSubitems->sText);

		y-=18;
		if(y<iY-iHeight)
			break;
	}

	/*Menu_DrawBox(bmpDest, iX, iY+15, iX+iWidth, iY+iHeight);

	// Draw the columns
	lv_column_t *col = tColumns;
	int x=iX+3;
		
	for(;col;col = col->tNext) {
		tLX->cFont.Draw(bmpDest, x, iY, 0xffff, col->sText);
		x += col->iWidth;
	}


	// Draw the items
	int y = iY+17;
	x = iX+4;
	lv_item_t *item = tItems;
	int count=0;
	
	int selectsize = x+iWidth-5;
	if(iGotScrollbar)
		selectsize = x+iWidth-20;

	for(;item;item = item->tNext) {
		if(count++ < cScrollbar.getValue())
			continue;
		
		// Selected?
		if(item->iSelected && iFocused)
			DrawRectFill(bmpDest,x-2,y,selectsize,y+14,MakeColour(0,102,153));	

		// Draw the sub items
		if(item->tSubitems) {

			// TODO: Draw all the sub items
			tLX->cFont.Draw(bmpDest,x,y,0xffff,item->tSubitems->sText);
		}
			
		y+=15;
		if(y>=iY+iHeight-15)
			break;
	}*/


	// Draw the scrollbar
    if(iGotScrollbar) {
        cScrollbar.setPosition(iX+iWidth-16, 600-iY);
		cScrollbar.Draw();
    }
}


///////////////////
// Add a column to the list view
void CListview::AddColumn(char *sText, int iWidth)
{
	lv_column_t *col;

	col = new lv_column_t;
	if(col == NULL) {
		// Out of memory
		return;
	}

	// Set the info
	strcpy(col->sText,sText);
	col->iWidth = iWidth;
	col->tNext = NULL;


	// Add it to the list
	if(tColumns) {
		lv_column_t *c = tColumns;
		for(;c;c = c->tNext) {
			if(c->tNext == NULL) {
				c->tNext = col;
				break;
			}
		}
	}
	else
		tColumns = col;
}


///////////////////
// Add an item to the list view
void CListview::AddItem(char *sIndex, int iIndex)
{
	lv_item_t *item = new lv_item_t;
	
	if(item == NULL) {
		// Out of memory
		return;
	}

	// Set the info
	strcpy(item->sIndex,sIndex);
	item->iIndex = iIndex;
	item->tNext = NULL;
	item->iSelected = false;
	item->tSubitems = NULL;

	// Add it to the list
	if(tItems) {
		lv_item_t *i = tItems;
		for(;i;i = i->tNext) {
			if(i->tNext == NULL) {
				i->tNext = item;
				break;
			}
		}
	}
	else {
		tItems = item;
		tSelected = item;
		tSelected->iSelected = true;
		//writeLog("Index: %d\n",tSelected->iIndex);
	}
	
	tLastItem = item;

	// Adjust the scrollbar
	cScrollbar.setMax( cScrollbar.getMax()+1 );
	iItemCount++;

	// Do we use a scrollbar?
	if(iItemCount > iHeight / 18)
		iGotScrollbar = true;
}


///////////////////
// Add a sub item to the last item
void CListview::AddSubitem(int iType, char *sText, char *sImage)
{
	lv_subitem_t *sub = new lv_subitem_t;

	if(sub == NULL) {
		// Out of memory
		return;
	}

	// Set the info
	sub->iType = iType;
	strcpy(sub->sText,sText);
	//sub->bmpImage = NULL;
	sub->tNext = NULL;
	//if(iType == LVS_IMAGE)
	//	sub->bmpImage = LoadImage(sImage,0);
	

	// Add this sub item to the current item
	if(tLastItem) {
		if(tLastItem->tSubitems) {
			lv_subitem_t *s = tLastItem->tSubitems;
			for(;s;s = s->tNext) {
				if(s->tNext == NULL) {
					s->tNext = sub;
					break;
				}
			}
		} else
			tLastItem->tSubitems = sub;
	}
}


///////////////////
// Remove an item from the list
/*void CListview::RemoveItem(int iIndex)
{
	lv_item_t *item = NULL;
	lv_item_t *prev = NULL;
	lv_item_t *i = tItems;
	lv_item_t *next = NULL;
	lv_subitem_t *s,*sub;
	int first = true;
	int found = false;

	// Find the item and it's previous item	
	prev = i;	
	for(;i;i=i->tNext) {

		if(i->iIndex == iIndex) {
			found=true;
			next = i->tNext;

			// If it's the first item, we do it differently
			if(first) {

				// Free the sub items
				for(s=i->tSubitems;s;s=sub) {
					sub = s->tNext;
					delete s;
				}
				delete i;

				// Set the first one to point to the next in the list
				tItems = next;
			} else {
				// Free the sub items
				for(s=i->tSubitems;s;s=sub) {
					sub = s->tNext;
					delete s;
				}
				delete i;

				// The previous item now points to the next one
				prev->tNext = next;
			}
			i = next;
			//break;			
		}

		if(!i)
			break;
		
		prev = i;
		first=false;
	}

	tLastItem = i;

	// Adjust the scrollbar
	iItemCount--;
	cScrollbar.setMax( iItemCount );

	// Do we use a scrollbar?
	if(iItemCount > iHeight / 16)
		iGotScrollbar = true;
	else
		iGotScrollbar = false;
}*/


///////////////////
// Create event
void CListview::Create(void)
{
	// Destroy any previous settings
	Destroy();
	iItemCount=0;
	iGotScrollbar=false;

	cScrollbar.Create();
	cScrollbar.Setup(0, iX+iWidth-16, iY, 14, iHeight);
	cScrollbar.setItemsperbox( iHeight / 18 + 1 );
	cScrollbar.setMin(0);
	cScrollbar.setValue(0);
}


///////////////////
// Destroy event
void CListview::Destroy(void)
{
	// Free the columns
	lv_column_t *c,*col;
	for(c=tColumns ; c ; c=col) {
		col = c->tNext;

		delete c;
	}
	tColumns = NULL;


	// Free the items
	lv_item_t *i,*item;
	lv_subitem_t *s,*sub;
	for(i=tItems;i;i=item) {
		item = i->tNext;

		// Free the sub items
		for(s=i->tSubitems;s;s=sub) {
			sub = s->tNext;

			delete s;
		}

		delete i;
	}

	tItems = NULL;
}


///////////////////
// Mouse over event	
int	CListview::MouseOver(mouse_t *tMouse)
{
	if(tMouse->X > iX+iWidth-20 && iGotScrollbar)
		cScrollbar.MouseOver(tMouse);

	return -1;
}


///////////////////
// Mouse down event
int	CListview::MouseDown(mouse_t *tMouse)
{
	if(tMouse->X > iX+iWidth-20 && iGotScrollbar) 
		cScrollbar.MouseDown(tMouse);

    captureMouse();

	return -1;
}


///////////////////
// Mouse up event
int	CListview::MouseUp(mouse_t *tMouse)
{
	if(tMouse->X > iX+iWidth-20 && iGotScrollbar) 
		cScrollbar.MouseUp(tMouse);

    releaseMouse();

	if(tMouse->X < iX || tMouse->X > iX+iWidth-20)
		return LV_NONE;

	int my = 600-tMouse->Y;

	// Go through the items
	int y = iY-18;
	lv_item_t *item = tItems;
	int count=0;
	for(;item;item = item->tNext) {
		if(count++ < cScrollbar.getValue())
			continue;

		if(my > y && my < y+18) {
			tSelected->iSelected = false;
			tSelected = item;
			tSelected->iSelected = true;
			return LV_CHANGED;
		}

		y-=18;
		if(y<iY-iHeight)
			break;
	}

	return LV_NONE;
}
