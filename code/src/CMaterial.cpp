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


// Material class
// Created 13/5/02
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "System.h"



///////////////////
// Load the material
void CMaterial::Load(int id, FILE *fp)
{
	char	sLine[256];
	int		iLength;
	int		iCurPass = 0;
	int		iInPass = false;

    nID = id;
	iContainsBlending = false;
    iNumPasses = 0;


	// Set the defaults for each pass
	pass_t *pass = tPasses;
	for(int p=0;p<MAX_PASSES;p++,pass++) {
		pass->fAlpha = 1;
		pass->iBackCull = true;
		pass->iBlending = BLEND_NONE;
		pass->iHasTexture = false;
		pass->iLightmap = false;
		pass->iTcGen = TC_NONE;
		pass->tColour.R = pass->tColour.G = pass->tColour.B = 1;
		pass->tTexture = NULL;
        pass->iUseColour = false;
        pass->iLighting = true;
	}

    bLoaded = true;

	while(!feof(fp)) {
		// Parse the lines
		fscanf(fp,"%[^\n]\n",sLine);
		TrimSpaces(sLine);
		iLength = strlen(sLine);

		// Blank lines
		if(iLength == 0)
			continue;


		// Check for a comment
		if(sLine[0] == '/' &&
		   sLine[1] == '/')
		   continue;


		// Check for a pass
		if(sLine[iLength-1] == '{') {
			
			// Read pass name
			if(strnicmp(sLine,"pass",4) == 0) {
				iInPass = true;
				iCurPass = sLine[4]-'1';

				if(iCurPass < 0 || iCurPass >= MAX_PASSES)
					return;
			} else
				return;

			continue;
		}

		// Check for the end of a pass (or material)
		if(sLine[0] == '}') {
			if(iInPass) {
				iNumPasses++;
                //printf(" New pass\n");
				iInPass = false;
				continue;
            } else
				return;
		}

		// Ignore stupid out-of-bounds functions
		if(!iInPass)
			continue;

		// Must be a function
		TranslateFunction(sLine,iCurPass);
	}    
}


///////////////////
// Translate a function
void CMaterial::TranslateFunction(char *sLine, int p)
{
	pass_t *pass = &tPasses[p];
	int val;
	char buf[256];

	char *tok = strtok(sLine," ");
	if(!tok)
		return;

	// Texture name
	if(stricmp(tok,"texture") == 0) {		
		tok = strtok(NULL," ");
		if(tok) {

            // Go through the texture dir list
            for( int i=MAX_TEXDIRLIST-1; i>=0; i-- ) {
                if( !Tex_DirOK(i) )
                    continue;                
                pass->tTexture = Cache_LoadTexture( Tex_GetDir(buf,tok,i) );
                if( pass->tTexture ) {
                    pass->iHasTexture = true;
                    
                    // Upload it
                    Tex_Upload(pass->tTexture);
                    break;
                }
            }

            if(!pass->iHasTexture) {
                writeLog("Error: Failed to load image '%s'\n",tok);
            }
		}
	} else

	// Blending type
	if(stricmp(tok,"blend") == 0) {
		tok = strtok(NULL," ");

		val = Key_FindValue(tok);
		if(val != -666) {
			pass->iBlending = val;
			iContainsBlending = true;
		}
	} else

	// Alpha value
	if(stricmp(tok,"alpha") == 0) {
		tok = strtok(NULL," ");

		pass->fAlpha = (float)atof(tok);
	} else

	// Texture generation type
	if(stricmp(tok,"tcGen") == 0) {
		tok = strtok(NULL," ");

		val = Key_FindValue(tok);
		if(val != -666)
			pass->iTcGen = val;
	} else

	// Lightmap
	/*if(stricmp(tok,"lightmap") == 0) {
		tok = strtok(NULL," ");

		val = Key_FindValue(tok);
		if(val != -666)
			pass->iLightmap = val;
	} else*/

	// Backface culling
	if(stricmp(tok,"backcull") == 0) {
		tok = strtok(NULL," ");

		val = Key_FindValue(tok);
		if(val != -666)
			pass->iBackCull = val;
	} else

	// Colour
	if(stricmp(tok,"rgb") == 0) {
		tok = strtok(NULL,",");
		pass->tColour.R = (float)atof(tok);
		tok = strtok(NULL,",");
		pass->tColour.G = (float)atof(tok);
		tok = strtok(NULL,",");
		pass->tColour.B = (float)atof(tok);

	} else

    // Lighting
    if( stricmp(tok,"lighting") == 0 ) {
        tok = strtok(NULL," ");
        val = Key_FindValue(tok);
		if(val != -666)
			pass->iLighting = val;
    } else {
		writeLog("Unknown function: %s",tok);
	}
}


///////////////////
// Setup the pass
void CMaterial::setupPass(int p, int nLighting)
{
	if(p<0 || p>=MAX_PASSES)
		return;

	pass_t *pass = &tPasses[p];

	// Lightmap
	if(pass->iLightmap) {
		State_Disable(GL_TEXTURE_GEN_S);
		State_Disable(GL_TEXTURE_GEN_T);
		State_Enable(GL_TEXTURE_2D);
		State_Enable(GL_BLEND);
		glBlendFunc(GL_ZERO, GL_SRC_COLOR);
		glColor4f(1,1,1,1);
		Tex_Bind(tLightmap);
		//Tex_Bind( Cache_LoadTexture("data/cities/LA/garage/garage_lm.bmp"));
		return;
	}

	// Texture
	if(pass->iHasTexture) {
		State_Enable(GL_TEXTURE_2D);	
		Tex_Bind(pass->tTexture);
	} else
		State_Disable(GL_TEXTURE_2D);


	// Blending
	if(pass->iBlending) {
		State_Enable(GL_BLEND);

		switch(pass->iBlending) {
			case BLEND_ONEONE:
				glBlendFunc(GL_ONE,GL_ONE);
				break;
			case BLEND_ONEZERO:
				glBlendFunc(GL_ONE,GL_ZERO);
				break;
			case BLEND_ALPHA:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;
			case BLEND_COLOUR:
				glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_COLOR);
				break;
            case BLEND_ALPHAONE:
                glBlendFunc(GL_SRC_ALPHA,GL_ONE);
                break;

			default:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;
		}

	} else
		State_Disable(GL_BLEND);


	// Texture coord generation
	if(pass->iTcGen) {
		State_Enable(GL_TEXTURE_GEN_S);
		State_Enable(GL_TEXTURE_GEN_T);

		if(pass->iTcGen == TC_SPHERE) {
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		}
		if(pass->iTcGen == TC_EYE_LINEAR) {
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		}


	} else {
		State_Disable(GL_TEXTURE_GEN_S);
		State_Disable(GL_TEXTURE_GEN_T);
	}


    // Lighting
    if( nLighting && pass->iLighting)
        State_Enable(GL_LIGHTING);
    else
        State_Disable(GL_LIGHTING);
    
    if( !pass->iLighting )
        State_Disable(GL_LIGHTING);

	// Backface culling
	if(pass->iBackCull)
		State_Enable(GL_CULL_FACE);
	else
		State_Disable(GL_CULL_FACE);


	// Colour
	if(pass->iHasTexture && !pass->iUseColour)
		glColor4f(1, 1, 1, pass->fAlpha);
	else
		glColor4f(pass->tColour.R, pass->tColour.G, pass->tColour.B, pass->fAlpha);
}





///////////////////
// Load the info from a script
/*void CMaterial::Load(CScript *script)
{
	val = script.GetKeyword(mat->getName(),"HasTexture");
	if(val != -666)
		mat->setHasTexture(val);


		if(val && val != -666) {
			char *texture = script.GetString(mat->getName(),"Texture");

			if(texture) {
				char buf[256];
				sprintf(buf,"data/textures/%s",texture);

				mat->LoadTexture(buf);
			}
		}


		// Colour
		colour_t c;
		script.GetTriFloat(mat->getName(),"Colour",(float *)&c);		
		mat->setColour(c);

		// Backface culling
		val = script.GetKeyword(mat->getName(),"BackFaceCull");
		if(val != -666)
			mat->setBackFaceCull(val);

		// Blending
		val = script.GetKeyword(mat->getName(),"Blending");
		if(val != -666)
			mat->setBlending(val);

		// Normal render
		val = script.GetKeyword(mat->getName(),"RenderNormal");
		if(val != -666)
			mat->setRenderNormal(val);

		// Environment map
		val = script.GetKeyword(mat->getName(),"HasEnviron");
		if(val != -666) {
			mat->setHasEnviron(val);

			// Load the environment map
			char *texture = script.GetString(mat->getName(),"Environment");

			if(texture) {
				char buf[256];
				sprintf(buf,"data/textures/%s",texture);

				mat->LoadEnviron(buf);
			}

		}


}


///////////////////
// Load a texture
int CMaterial::LoadTexture(char *sFilename) 
{
	tTexture = Cache_LoadTexture(sFilename);
	if(tTexture == NULL) {
		iHasTexture = false;
		return false;
	}

	Tex_Upload(tTexture);

	return true;
}


///////////////////
// Load the environment map
int CMaterial::LoadEnviron(char *sFilename)
{
	tEnvironment = Cache_LoadTexture(sFilename);
	if(tEnvironment == NULL) {
		iHasEnviron = false;
		return false;
	}

	Tex_Upload(tEnvironment);

	return true;
}


///////////////////
// Set this material as the current material
void CMaterial::SetCurrent(void)
{
	State_Disable(GL_TEXTURE_GEN_S);
	State_Disable(GL_TEXTURE_GEN_T);

	if(iHasTexture) {
		State_Enable(GL_TEXTURE_2D);
		glColor4f( 1,1,1,0.8f );

		Tex_Bind(tTexture);
	} else {

		State_Disable(GL_TEXTURE_2D);
		glColor4f( tColour.R, tColour.G, tColour.B, 0.7f );
	}

	// Backface culling
	if(iBackFaceCull)
		State_Enable(GL_CULL_FACE);
	else
		State_Disable(GL_CULL_FACE);

	// Blending
	if(iBlending) {
		State_Enable(GL_BLEND);
		
		switch(iBlending) {
			case BLD_ALPHACOL:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;
			case BLD_NORMAL:
				glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_COLOR);
				break;
			case BLD_ONEONE:
				glBlendFunc(GL_ONE,GL_ONE);
				break;
			case BLD_ONEZERO:
				glBlendFunc(GL_ONE,GL_ZERO);
				break;

		}
	} else
		State_Disable(GL_BLEND);
}


///////////////////
// Set our environment map as the current material
void CMaterial::SetEnvironment(void)
{
	if(!iHasEnviron)
		return;		// Double check

	// Backface culling
	if(iBackFaceCull)
		State_Enable(GL_CULL_FACE);
	else
		State_Disable(GL_CULL_FACE);


	State_Enable(GL_TEXTURE_2D);
	if(tEnvironment)
		Tex_Bind(tEnvironment);

	State_Enable(GL_TEXTURE_GEN_S);
	State_Enable(GL_TEXTURE_GEN_T);
	
	//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	//glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// Blend it a bit
	State_Disable(GL_BLEND);
}*/