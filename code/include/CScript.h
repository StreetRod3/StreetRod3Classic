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


#ifndef __CSCRIPT_H__
#define __CSCRIPT_H__



#define		MAX_SC_VARS		64
#define		MAX_SC_ITEMS	64
#define		MAX_SC_VALUES	8



// Script value
typedef struct {
	int			iValue;
	float		fValue;
} sc_value_t;



// Script variable
typedef struct {
	char		sName[32];
	int			iNumValues;
	sc_value_t	tValues[MAX_SC_VALUES];

	// Can only have 1 text value
	char		sValue[64];
} sc_var_t;



// Script item structure
typedef struct {
	char		sName[32];
	int			iNumVars;
	sc_var_t	tVariables[MAX_SC_VARS];
} sc_item_t;



// Script class
class CScript {
public:
	// Constructor
	CScript() {
		iNumItems = 0;
		tItems = NULL;
	}

	// Destructor
	~CScript() {
		if(tItems)
			delete[] tItems;
		tItems = NULL;
	}


private:
	// Attributes

	int			iNumItems;
	sc_item_t	*tItems;



public:
	// Methods

	int			Read(char *sFilename);
	void		TrimSpaces(char *str);


	void		NewItem(char *sName);
	void		NewVariable(char *sLine);


	sc_item_t	*FindItem(char *sName);
	sc_var_t	*FindVariable(sc_item_t *it, char *sVar);
	int			GetString(char *sItem, char *sVar, char *val, char *def);
	int			GetKeyword(char *sItem, char *sVar, int *val, int def);
	int			GetTriFloat(char *sItem, char *sVar, float *tri);

};





#endif  //  __CSCRIPT_H__