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


// Config file handler
// Created 30/9/01
// By Jason Boettcher


#include "defs.h"


///////////////////
// Read a keyword from a file
int ReadKeyword(char *filename, char *section, char *key, int *value, int defaultv)
{
	char string[MAX_MINOR_LENGTH];
	
	*value = defaultv;

	if(!GetString(filename,section,key,string))
		return false;

	// Try and find a keyword with matching keys
	int t = Key_FindValue(string);
	if(t == -666)
		return false;

	*value = t;
		

	return false;
}


///////////////////
// Read an interger from a file
int ReadInteger(char *filename, char *section, char *key, int *value, int defaultv)
{
	char string[MAX_MINOR_LENGTH];

	*value = defaultv;
	
	if(!GetString(filename,section,key,string))
		return false;

	*value = atoi(string);

	return true;
}


///////////////////
// Read a string from a file
int ReadString(char *filename, char *section, char *key, char *value, char *defaultv)
{
    if(!GetString(filename,section,key,value)) {
        strcpy(value,defaultv);
        return false;
    }

    return true;
}


///////////////////
// Read a float from a file
int ReadFloat(char *filename, char *section, char *key, float *value, float defaultv)
{
	char string[MAX_MINOR_LENGTH];

	*value = defaultv;
	
	if(!GetString(filename,section,key,string))
		return false;

	*value = (float)atof(string);

	return true;
}


///////////////////
// Read a vector from a file
int ReadVec(char *filename, char *section, char *key, CVec *value)
{
	char string[MAX_MINOR_LENGTH];
	
	if(!GetString(filename,section,key,string))
		return false;

	char *tok = strtok(string,",");
	if(tok == NULL)
		return false;

	value->SetX( (float)atof(tok) );
	tok = strtok(NULL,",");
	value->SetY( (float)atof(tok) );
	tok = strtok(NULL,",");
	value->SetZ( (float)atof(tok) );

	return true;
}








///////////////////
// Read a string
int GetString(char *filename, char *section, char *key, char *string)
{
	FILE	*config;
	char	Line[MAX_STRING_LENGTH];
	char	curSection[256];
	char	temp[MAX_STRING_LENGTH];
	char	curKey[MAX_STRING_LENGTH];
	char	*chardest = NULL;
	int		Position;
	int		found = false;
	
	if(strcmp(filename,"") == 0)
		return false;

	config = fopen(filename,"rt");
	if(!config)	
		return false;

	strcpy(string,"");
	strcpy(curSection,"");
	strcpy(temp,"");
	strcpy(curKey,"");


	while(!feof(config))
	{
		// Parse the lines
		fscanf(config,"%[^\n]\n",Line);
		TrimSpaces(Line);
		
		
		///////////////////
		// Comment, Ignore
		if(Line[0] == '#')				
			continue;

		////////////
		// Sections
		if(Line[0] == '[' && Line[strlen(Line)-1] == ']')
		{
			sr_strncpy(temp,Line+1,MAX_MINOR_LENGTH-1);
			temp[strlen(temp)-1] = '\0';
			sr_strncpy(curSection,temp,255);
			continue;
		}

		////////
		// Keys
		chardest = strchr(Line,'=');
		if(chardest != NULL)
		{
			// Key
			Position = chardest - Line + 1;
			sr_strncpy(curKey,Line,MAX_MINOR_LENGTH-1);
			curKey[Position-1] = '\0';
			TrimSpaces(curKey);

			// Check if this is the key were looking for under the section were looking for
			if(stricmp(curKey,key) == 0 && stricmp(curSection,section) == 0)
			{				
				// Get the value
				sr_strncpy(string,Line+Position,MAX_MINOR_LENGTH-1);
				TrimSpaces(string);
				found = true;
				break;
			}
			continue;
		}
	}

    ConvertNewlines(string);

	fclose(config);

	return found;
}