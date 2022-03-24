/////////////////////////////////////////
//
//             Street Rod 3
//
//      Copyright Auxiliary Software 2000
//
//
/////////////////////////////////////////


// Texture class
// Created 15/7/01
// By Jason Boettcher


#include "defs.h"
#include "3D.h"


CTexture *TextureCache;


///////////////////
// Load a texture
int CTexture::Load(char *name)
{
	strcpy(Name,name);
	bmpTexture = IMG_Load(name);
	if(bmpTexture == NULL)
		return false;

	Used = true;

	//DebugPrintf("copy %s dist\\%s\n",name,name);

	// Copy the texture into a temp dir
	

	return true;
}


///////////////////
// Upload the texture using opengl
int CTexture::Upload(void)
{
	int Format = GL_BGR_EXT;

	// Check for the colour order in the pixel
	if(bmpTexture->format->Bshift == 16)
		Format = GL_RGB;
	if(bmpTexture->format->Ashift == 24)
		Format = GL_RGBA;

	// Bind the current id
	glBindTexture( GL_TEXTURE_2D, ID );

	// Build mipmaps from the texture, and upload them all
	gluBuild2DMipmaps( GL_TEXTURE_2D, bmpTexture->format->BytesPerPixel, bmpTexture->w,bmpTexture->h, Format, GL_UNSIGNED_BYTE, bmpTexture->pixels);

	// Linear Filtering
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	//glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	Uploaded = true;

	return true;
}





/////////////////////////////////////////////////////////
//						Texture Cache
/////////////////////////////////////////////////////////




///////////////////
// Initialize the texture cache
int InitializeTextureCache(void)
{
	TextureCache = new CTexture[MAX_TEXTURES];
	if(TextureCache == NULL)
		return false;

	return true;
}


///////////////////
// Shutdown the texture cache
int ShutdownTextureCache(void)
{
	if(TextureCache == NULL)
		return false;

	// Shutdown the textures
	for(int n=0;n<MAX_TEXTURES;n++)
		if(TextureCache[n].IsUsed())
			TextureCache[n].Shutdown();

	// Free the array
	if(TextureCache)
		delete[] TextureCache;
	TextureCache = NULL;

	return true;
}


///////////////////
// Load a texture
CTexture *LoadTexture(char *_name)
{
	// Search if we havn't already got a texture of the same name loaded
	int n;
	int tex = -1;

	for(n=0;n<MAX_TEXTURES;n++) {
		if(TextureCache[n].IsUsed()) {
			if(stricmp(TextureCache[n].GetName(),_name) == 0)
				return &TextureCache[n];
		} else
			if(tex == -1)
				tex=n;
	}
	
	if(tex == -1)
		return NULL;

	// We have to load the texture
	if(!TextureCache[tex].Load(_name))
		return NULL;
	TextureCache[tex].SetID(tex);
	
	return &TextureCache[tex];
}


///////////////////
// Upload all the textures
void UploadAllTextures(void)
{
	int n;

	for(n=0;n<MAX_TEXTURES;n++) {
		if(TextureCache[n].IsUsed())
			TextureCache[n].Upload();
	}
}


///////////////////
// Get a pointer for a non-texture material
CTexture *GetFlatTexture(char *_name)
{
	// Search if we havn't already got a texture of the same name loaded
	int n;
	int tex = -1;

	for(n=0;n<MAX_TEXTURES;n++) {
		if(!TextureCache[n].IsUsed()) {
			tex=n;
			break;
		}
	}
	
	if(tex == -1)
		return NULL;

	TextureCache[tex].SetUsed(true);
	strcpy(TextureCache[tex].GetName(),_name);

	return &TextureCache[tex];
}


///////////////////
// Bind a texture
void BindTexture(CTexture *Texture)
{
	static int oldtex = -1;

	if(Texture->GetID() != oldtex) {
		glBindTexture(GL_TEXTURE_2D,Texture->GetID());
		oldtex = Texture->GetID();
	}

	//if(!Texture->HasTexture())
	//	glColor3f(Texture->GetColour().GetR(),Texture->GetColour().GetG(),Texture->GetColour().GetB());
}