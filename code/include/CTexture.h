/////////////////////////////////////////
//
//   Auxiliary Software class library
//
/////////////////////////////////////////


// Texture class
// Created 15/7/01
// By Jason Boettcher


#ifndef __CTEXTURE_H__
#define __CTEXTURE_H__


#define	MAX_TEXTURES		256


class CTexture {
public:
	// Constructor
	CTexture() {
		hasTexture = false;
		bmpTexture = NULL;
		ID=0;
		Used = false;			// For the cache

		Friction = 0.75;
		Opacity = 1;

		Uploaded = false;
	}


private:
	// Attributes

	int				hasTexture;
	SDL_Surface		*bmpTexture;
	int				ID;
	char			Name[128];
	char			OrigName[128];
	int				Used;

	int				Uploaded;

	// Material properties
	float			Friction;
	float			Opacity;
	//struct colour_s	Colour;
	int				Alpha;


public:
	// Methods

	int				HasTexture(void)			{ return hasTexture; }
	void			SethasTexture(int _h)		{ hasTexture = _h; }

	int				Load(char *name);
	int				LoadA(char *name);
	int				LoadEx(char *name);
	int				Upload(void);
	int				Shutdown(void)				{ if(bmpTexture) SDL_FreeSurface(bmpTexture); Used = false; return true; }

	SDL_Surface		*GetSurface(void)			{ return bmpTexture; }

	int				GetID(void)					{ return ID; }
	void			SetID(int _id)				{ ID = _id; }

	//void			SetColour(CColour _col)		{ Colour = _col; }
	//CColour			GetColour(void)				{ return Colour; }

	float			GetOpacity(void)			{ return Opacity; }
	void			SetOpacity(float _o)		{ Opacity = _o; }

	void			SetAlphaMask(int _a)		{ Alpha = _a; }
	int				isAlphaMask(void)			{ return Alpha; }

	char			*GetName(void)				{ return Name; }
	char			*GetOrigName(void)			{ return OrigName; }

	int				IsUsed(void)				{ return Used; }
	void			SetUsed(int _u)				{ Used = _u; }

	int				isUploaded(void)			{ return Uploaded; }
};



//////////////////////////////////////
//			Texture Cache
//////////////////////////////////////


// Routines
int			InitializeTextureCache(void);
int			ShutdownTextureCache(void);
CTexture	*GetFlatTexture(char *_name);
CTexture	*LoadTexture(char *_name);
CTexture	*GetFlatTexture(char *_name);
void		BindTexture(CTexture *tex);



#endif  //  __CTEXTURE_H__