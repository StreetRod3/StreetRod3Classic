/////////////////////////////////////////
//
//             Street Rod 3
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Script
// Created 7/5/02
// Jason Boettcher


#include "defs.h"
#include "System.h"



///////////////////
// Read a script from a file
int CScript::Read(char *sFilename)
{
	FILE	*fp;
	char	sLine[256];
	char	sBuf[256];
	int		iInItem = false;

	fp = fopen(sFilename,"rt");
	if(fp == NULL) {
		// TODO: Set error
		return false;
	}


	// Allocate room for the items
	tItems = new sc_item_t[MAX_SC_ITEMS];
	if(tItems == NULL) {
		return false;
	}


	// Go through each line
	while(!feof(fp)) {

		// Parse the lines
		fscanf(fp,"%[^\n]\n",sLine);
		TrimSpaces(sLine);


		// Check for a comment
		if(sLine[0] == '/' &&
		   sLine[1] == '/')
		   continue;

		
		// Start of a group
		if(sLine[strlen(sLine)-1] == '{') {
			strcpy(sBuf,sLine);
			sBuf[strlen(sLine)-1] = 0;
			TrimSpaces(sBuf);

			NewItem(sBuf);
			iInItem = true;
			continue;
		}

		// End of a group
		if(sLine[0] == '}') {

			if(!iInItem) {
				// Error
			}
			iNumItems++;
			iInItem = false;
			continue;
		}

		// Must be a variable
		if(!iInItem) {
			// Error
			continue;
		}

		NewVariable(sLine);
	}
		



	fclose(fp);

	return true;
}


///////////////////
// Add a new item to the script
void CScript::NewItem(char *sName)
{
	strcpy(tItems[iNumItems].sName,sName);
	tItems[iNumItems].iNumVars = 0;
}


///////////////////
// Add a new variable to the current item
void CScript::NewVariable(char *sLine)
{
	char		*chardest;
	int			iPosition;
	sc_item_t	*item = &tItems[iNumItems];
	sc_var_t	*var = &item->tVariables[item->iNumVars++];
	char		*tok;

	chardest = strchr(sLine,'=');
	if(chardest != NULL) {
		
		// Variable
		iPosition = chardest - sLine + 1;
		strcpy(var->sName, sLine);
		var->sName[iPosition-1] = '\0';
		TrimSpaces(var->sName);

		// Value(s)
		strcpy(var->sValue,sLine+iPosition);
		TrimSpaces(var->sValue);

		// First value
		var->tValues[0].fValue = (float)atof(var->sValue);
		var->tValues[0].iValue = atoi(var->sValue);

		// Multiple values seperated by comma's
		var->iNumValues = 0;
		tok = strtok(sLine+iPosition,",");
		while(tok) {
			var->tValues[var->iNumValues].fValue = (float)atof(tok);
			var->tValues[var->iNumValues++].iValue = atoi(tok);
			tok = strtok(NULL,",");
		}
	}
}


///////////////////
// Trim the spaces from the start & end of the string
void CScript::TrimSpaces(char *str)
{
	char	temp[256];
	unsigned int n = 0;
	unsigned int i = 0;
	strcpy(temp,"");


	// Leading spaces
	for(n=0,i=0;n<strlen(str);n++)
	{
		if(str[n] != ' ')
			break;
		i++;
	}
	strcpy(temp,str+i);


	// proceeding spaces
	for(n=strlen(temp)-1;n>0;n--)
		if(temp[n] != ' ')
		{
			temp[n+1] = '\0';
			break;
		}

	strcpy(str,temp);
}


///////////////////
// Find an item based on name
sc_item_t *CScript::FindItem(char *sName)
{
	int i;
	sc_item_t *it = tItems;

	for(i=0;i<iNumItems;i++,it++) {
		if(stricmp(it->sName,sName) == 0)
			return it;
	}

	return NULL;
}


///////////////////
// Find a variable based on item & variable name
sc_var_t *CScript::FindVariable(sc_item_t *it, char *sVar)
{
	sc_var_t *var = it->tVariables;
	int i;

	for(i=0;i<it->iNumVars;i++,var++) {
		if(stricmp(var->sName,sVar) == 0)
			return var;
	}

	return NULL;
}


///////////////////
// Get a string from an item & variable name
int CScript::GetString(char *sItem, char *sVar, char *val, char *def)
{
	strcpy(val,def);

	sc_item_t *it = FindItem(sItem);
	if(!it)
		return false;

	sc_var_t *var = FindVariable(it,sVar);
	if(!var)
		return false;

	strcpy(val,var->sValue);

	return true;
}


///////////////////
// Get a keyword from an item & variable name
int CScript::GetKeyword(char *sItem, char *sVar, int *val, int def)
{
	*val = def;

	sc_item_t *it = FindItem(sItem);
	if(!it)
		return false;

	sc_var_t *var = FindVariable(it,sVar);
	if(!var)
		return false;

	

	// TODO: Search for keywords

	// HACK
	if(stricmp(var->sValue,"true") == 0)
		return true;
	if(stricmp(var->sValue,"false") == 0)
		return false;

	if(stricmp(var->sValue,"BLD_ALPHACOL") == 0)
		return 1;
	if(stricmp(var->sValue,"BLD_NORMAL") == 0)
		return 2;
	if(stricmp(var->sValue,"BLD_ONEONE") == 0)
		return 3;
	if(stricmp(var->sValue,"BLD_ONEZERO") == 0)
		return 4;


	return -666;
}


///////////////////
// Get a triple float from an item & variable name
void CScript::GetTriFloat(char *sItem, char *sVar, float *tri)
{
	sc_item_t *it = FindItem(sItem);
	if(!it)
		return;

	sc_var_t *var = FindVariable(it,sVar);
	if(!var)
		return;

	for(int i=0;i<3;i++)
		tri[i] = var->tValues[i].fValue;
}