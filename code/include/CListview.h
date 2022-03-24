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



// gui list view class
// Created 30/5/02
// Jason Boettcher


#ifndef __CLISTVIEW_H__
#define __CLISTVIEW_H__


// Event msg's for this control
enum {
	LV_NONE=-1,
	LV_CHANGED=0
};


// Sub-item types
enum {
	LVS_IMAGE=0,
	LVS_TEXT
};


// Column structure
typedef struct lv_column_s {
	char		sText[32];
	int			iWidth;

	struct	lv_column_s *tNext;

} lv_column_t;


// Sub item structure
typedef struct lv_subitem_s {
	int			iType;
	char		sText[128];
	texture_t	*tTexture;

	struct	lv_subitem_s *tNext;

} lv_subitem_t;


// Item structure
typedef struct lv_item_s {
	char		sIndex[128];
	int			iIndex;
	int			iSelected;

	lv_subitem_t *tSubitems;

	struct	lv_item_s	*tNext;

} lv_item_t;


// Button control class
class CListview: public CWidget {
public:
	// Constructors
	CListview() {
		tColumns = NULL;
		tItems = NULL;
		tLastItem = NULL;
		tSelected = NULL;
		iGotScrollbar = false;
		iItemCount = false;
	}


private:
	// Attributes

	// Columns
	lv_column_t		*tColumns;

	// Items
	lv_item_t		*tItems;
	lv_item_t		*tLastItem;
	lv_item_t		*tSelected;

	int				iItemCount;
	int				iGotScrollbar;
	CScrollbar		cScrollbar;

	
public:
	// Methods
	
	void	Create(void);
	void	Destroy(void);

	//These events return an event id, otherwise they return -1
	int		MouseOver(mouse_t *tMouse);
	int		MouseUp(mouse_t *tMouse);
	int		MouseDown(mouse_t *tMouse);
	int		KeyDown(char c)			{ return LV_NONE; }
	int		KeyUp(char c)			{ return LV_NONE; }

	void	Draw(void);

	

	void	AddColumn(char *sText, int iWidth);
	void	AddItem(char *sIndex, int iIndex);
	void	AddSubitem(int iType, char *sText, char *sTexture);

	int		getCurIndex(void)		{ if(tSelected) return tSelected->iIndex; else return -1; }

};



#endif  //  __CLISTVIEW_H__