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


// File finding routines
// Created 30/9/01
// By Jason Boettcher


#include "defs.h"


char	_dir[256];
long	handle = 0;
struct _finddata_t fileinfo;


///////////////////
// Find the first file
int FindFirst(char *dir, char *ext, char *filename)
{
	char basepath[256];

	strcpy(_dir,dir);

	strcpy(basepath, dir);
	strcat(basepath, "/");
	strcat(basepath, ext);

	if((handle = _findfirst(basepath, &fileinfo)) < 0)
		return false;

	do
	{
		//If file is not self-directory or parent-directory
		if(strcmp(fileinfo.name, "."))
		 if(strcmp(fileinfo.name, ".."))
		 {
			//If found file is not a directory
			if(!(fileinfo.attrib & _A_SUBDIR))
			{
				sprintf(filename,"%s\\%s",_dir,fileinfo.name);

				return true;
			}
		}

	} while(!_findnext(handle, &fileinfo));		// Keep going until we found the first file


	return false;
}


///////////////////
// Find the next file
int FindNext(char *filename)
{
	if(_findnext(handle, &fileinfo))
		return false;

	do
	{
		//If file is not self-directory or parent-directory
		if(strcmp(fileinfo.name, "."))
		 if(strcmp(fileinfo.name, ".."))
		 {
			//If found file is not a directory
			if(!(fileinfo.attrib & _A_SUBDIR))
			{
				sprintf(filename,"%s\\%s",_dir,fileinfo.name);

				return true;
			}
		}
	} while(!_findnext(handle, &fileinfo));		// Keep going until we found the next file


	return false;
}