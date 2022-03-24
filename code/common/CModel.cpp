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


// Model class
// Created 28/9/01
// By Jason Boettcher


#include "defs.h"
#include "System.h"
#include "3D.h"





/*
========================================================================

                           Loading Routines

========================================================================
*/


///////////////////
// Load the model
bool CModel::load(char *szName)
{
	char buf[256];

    m_bLightmap = false;
	
    // Lightmaps
    if( System_MultiTextureEnabled() ) {
        sprintf(buf,"%s.lm",szName);
        if( !loadLightmaps(buf) )
            m_bLightmap = false;
    }
    

	// 3D Data
	sprintf(buf,"%s.aux",szName);
    if( !load3DData(buf) )        
		return false;


	// Materials
	sprintf(buf,"%s.mat",szName);
    if( !loadMaterials(buf) )
		return false;    

	return true;
}


///////////////////
// Load the 3D Data
bool CModel::load3DData(char *szFilename)
{
    FILE        *fp;
    char        sig[] = {'A','u','x','i','l','i','a','r','y',' ','3','D',' ','v','2',0x1A,0x08,0x04};
	char        sig2[] = {'A','u','x','i','l','i','a','r','y',' ','3','D',0x1A,0x08,0x04};
    char        ID[32];
//    int         Version;
//	uint        n,i;


	
	// Open the file
	fp = fopen(szFilename,"rb");
	if(fp == NULL)
		return false;

	// Check for the new file format
	fread(ID, sizeof(uchar), 15, fp);
	fseek(fp, 0, SEEK_SET);
	
	if(memcmp(ID, sig2, 15) == 0) {
		
		return loadnew3DData(fp);
	}

	// Must be the old 3D data file
    
	return true;
}


///////////////////
// Load the 3D data
bool CModel::loadnew3DData(FILE *fp)
{
	char        sig[] = {'A','u','x','i','l','i','a','r','y',' ','3','D',0x1A,0x08,0x04};
    char        ID[32];
    int         Version;
	uint        n,i;


    //
	// Read the header
    //

    fread( ID,  sizeof(uchar),  15, fp );
	if( memcmp(ID,sig,15) != 0 ) {
        printf("Bad ID\n");
		fclose(fp);
		return false;
	}
    fread( &Version,    sizeof(int),    1,  fp );
    if( Version != 1 ) {
        printf("Bad version\n");
        fclose(fp);
        return false;
    }

    fread( &m_nNumMaterials,    sizeof(uint),   1,  fp );
    fread( &m_nNumMeshes,       sizeof(uint),   1,  fp );
    fread( &m_nNumLights,       sizeof(uint),   1,  fp );
    fread( &m_nNumGameObjects,  sizeof(uint),   1,  fp );

    fread(&m_nNumVertices, sizeof(uint), 1, fp);
	fread(&m_nNumTexCoords, sizeof(uint), 1, fp);
	fread(&m_nNumVertexNormals, sizeof(uint), 1, fp);



    m_nNumPolygons = 0;

    //
    // Load the lists
    //

    m_psVertexList = new vector_t[m_nNumVertices];
    if( !m_psVertexList ) {
        fclose(fp);
        return false;
    }

    for( i=0; i<m_nNumVertices; i++ ) {
        fread(&m_psVertexList[i].X, sizeof(float), 1, fp);
		fread(&m_psVertexList[i].Y, sizeof(float), 1, fp);
		fread(&m_psVertexList[i].Z, sizeof(float), 1, fp);
	}
    
    m_psTexCoordList = new texcoord_t[m_nNumTexCoords];
    if( !m_psTexCoordList ) {
        fclose(fp);
        return false;
    }

    for( i=0; i<m_nNumTexCoords; i++ ) {
        fread(&m_psTexCoordList[i].U, sizeof(float), 1, fp);
		fread(&m_psTexCoordList[i].V, sizeof(float), 1, fp);
	}
    
    m_psVertexNormalList = new vector_t[m_nNumVertexNormals];
    if( !m_psVertexNormalList ) {
        fclose(fp);
        return false;
    }

    for( i=0; i<m_nNumVertexNormals; i++ ) {
        fread(&m_psVertexNormalList[i].X, sizeof(float), 1, fp);
		fread(&m_psVertexNormalList[i].Y, sizeof(float), 1, fp);
		fread(&m_psVertexNormalList[i].Z, sizeof(float), 1, fp);
	}

    //
    // Load the materials
    //

	m_pcMaterials = new CMaterial[m_nNumMaterials];
	if( m_pcMaterials == NULL ) {
		fclose(fp);
		return false;
	}

	// Load the materials
	for( n=0; n<m_nNumMaterials; n++ ) {
        readString( m_pcMaterials[n].getName(), fp );
        //printf("   Material %s\n",m_pcMaterials[n].getName());
	}


    //
    // Load the meshes
    //

  	m_pcCDModel = newCollisionModel3D(true);        // True for StaticMesh
    if( m_pcCDModel == NULL ) {
        fclose(fp);
		return false;
    }


    m_psMeshes = new mesh_t[m_nNumMeshes];
    if( !m_psMeshes ) {
        fclose(fp);
        return false;
    }

    uint pcount = 0;

    for( n=0; n<m_nNumMeshes; n++ ) {
        mesh_t *mesh = &m_psMeshes[n];

        mesh->nFlags = 0;
        mesh->nID = n;

        // Read mesh header
        readString( mesh->szName, fp );
        fread( &mesh->nNumPolygons,     sizeof(uint),   1,  fp );

        mesh->psPolygons = new polygon_t[mesh->nNumPolygons];
        if( !mesh->psPolygons ) {
            fclose(fp);
            return false;
        }

        // Update stats
        m_nNumPolygons += mesh->nNumPolygons;

        // Read the polygons
        for( i=0; i<mesh->nNumPolygons; i++ ) {
            polygon_t *p = &mesh->psPolygons[i];
            p->psMesh = mesh;
            p->nID = i;
            p->nFlags = 0;


            fread( &p->nNumChannels,    sizeof(uchar),  1,  fp );
            p->psChannels = new mapchan_t[p->nNumChannels];
            if( !p->psChannels ) {
                fclose(fp);
                return false;
            }
            
            for( int j=0; j<p->nNumChannels; j++ )
                fread( p->psChannels[j].nTexCoord,   sizeof(uint),   3,  fp );

			for( j=0; j<3; j++ ) {
				fread( &p->nVertexNormal[j],    sizeof(uint),   1,  fp );
                fread( &p->nVertex[j],          sizeof(uint),   1,  fp );
            }

			fread( &p->nLightMapID,     sizeof(ushort), 1,  fp );
			fread( &p->nTextureID,      sizeof(uint),   1,  fp );
            

            // Calculate the plane
            CVec a = CVec( m_psVertexList[p->nVertex[0]].X, m_psVertexList[p->nVertex[0]].Y, m_psVertexList[p->nVertex[0]].Z );
            CVec b = CVec( m_psVertexList[p->nVertex[1]].X, m_psVertexList[p->nVertex[1]].Y, m_psVertexList[p->nVertex[1]].Z );
            CVec c = CVec( m_psVertexList[p->nVertex[2]].X, m_psVertexList[p->nVertex[2]].Y, m_psVertexList[p->nVertex[2]].Z );
            p->sPlane = CalculatePlane(a, b, c);
            
            m_pcCDModel->addTriangle(m_psVertexList[p->nVertex[0]].X, m_psVertexList[p->nVertex[0]].Y, m_psVertexList[p->nVertex[0]].Z,
                                     m_psVertexList[p->nVertex[1]].X, m_psVertexList[p->nVertex[1]].Y, m_psVertexList[p->nVertex[1]].Z,
                                     m_psVertexList[p->nVertex[2]].X, m_psVertexList[p->nVertex[2]].Y, m_psVertexList[p->nVertex[2]].Z);

            pcount++;
        }

        // Sort the mesh
        sortMesh(mesh);


        // Generate a vertex buffer for the mesh
        generateVertexBuffer(mesh);
    }

    //printf("Polycount for '%s' is %d\n",szFilename,pcount);

    
	m_pcCDModel->finalize();



    //
    // Load the lights
    //

	// Allocate room for the lights
	m_psLights = new light_t[m_nNumLights];
    if( m_psLights == NULL ) {
        fclose(fp);
		return false;
    }

	// Load the lights
	for( n=0; n<m_nNumLights; n++ ) {
		fread( &m_psLights[n].Position,     sizeof(CVec),       1,  fp );
		fread( &m_psLights[n].Colour,       sizeof(colour_t),   1,  fp );
		fread( &m_psLights[n].fRadius,      sizeof(float),      1,  fp );
        fread( &m_psLights[n].fBrightness,  sizeof(float),      1,  fp );
	}


    //
    // Load the game objects
    //

    m_psGameObjects = new gobject_t[m_nNumGameObjects];
    if( !m_psGameObjects ) {
        fclose(fp);
        return false;
    }

    for( n=0; n<m_nNumGameObjects; n++ ) {
        readString( m_psGameObjects[n].sName, fp );
        fread( &m_psGameObjects[n].vPosition,   sizeof(CVec),   1,  fp );
        m_psGameObjects[n].cMatrix.Load(fp);
        m_psGameObjects[n].cMatrix.Transpose();
    }
    


	// Close the file
	fclose(fp);


	return true;
}


///////////////////
// Read a string
char *CModel::readString(char *szString, FILE *fp)
{
    fread( szString,   sizeof(char),   1,  fp );
    
    int length = szString[0];

    fread( szString,  sizeof(char),   length,    fp);
    szString[ length ] = '\0';

    return szString;
}



///////////////////
// Generate vertex buffer for a mesh
void CModel::generateVertexBuffer(mesh_t *psMesh)
{
    /*
       Note:
       This vertex buffer is organised:
       VERTNORMAL,     VERTEX
       3 floats,       3 floats
       Size: 6 floats = 24bytes
    */

    psMesh->nNumVerts = psMesh->nNumPolygons * 3;
    int size = sizeof(float) * 6;

    psMesh->psVertexBuffer[0] = psMesh->psVertexBuffer[1] = NULL;

    // Allocate the data
    psMesh->psVertexBuffer[0] = new float[psMesh->nNumVerts * size];
    if( m_bLightmap )
        psMesh->psVertexBuffer[1] = new float[psMesh->nNumVerts * size];

    if( !psMesh->psVertexBuffer[0] || (!psMesh->psVertexBuffer[1] && m_bLightmap) )
        return;

    uint n = 0, k = 0;
    float *Buffer1 = psMesh->psVertexBuffer[0];
    float *Buffer2 = psMesh->psVertexBuffer[1];

    for( uint i=0; i<psMesh->nNumPolygons; i++ ) {
        polygon_t *p = &psMesh->psPolygons[i];
        
        for( int c=0; c<3; c++ ) {

            // Texture coordinates
            /*if( p->nNumChannels >= 2 ) {
                Buffer1[n++] = m_psTexCoordList[p->psChannels[1].nTexCoord[c]].U;
                Buffer1[n++] = -m_psTexCoordList[p->psChannels[1].nTexCoord[c]].V;
            } else {
                Buffer1[n++] = 0.0f;
                Buffer1[n++] = 0.0f;
            }

            // Lightmap coordinates
            if( Buffer2 ) {
                Buffer2[k++] = m_psTexCoordList[p->psChannels[0].nTexCoord[c]].U;
                Buffer2[k++] = m_psTexCoordList[p->psChannels[0].nTexCoord[c]].V;
            }*/


            // Vertex normals
            Buffer1[n++] = m_psVertexNormalList[p->nVertexNormal[c]].X;
            Buffer1[n++] = m_psVertexNormalList[p->nVertexNormal[c]].Y;
            Buffer1[n++] = m_psVertexNormalList[p->nVertexNormal[c]].Z;

            if( Buffer2 ) {
                Buffer2[k++] = m_psVertexNormalList[p->nVertexNormal[c]].X;
                Buffer2[k++] = m_psVertexNormalList[p->nVertexNormal[c]].Y;
                Buffer2[k++] = m_psVertexNormalList[p->nVertexNormal[c]].Z;
            }


            // Vertices
            Buffer1[n++] = m_psVertexList[p->nVertex[c]].X;
            Buffer1[n++] = m_psVertexList[p->nVertex[c]].Y;
            Buffer1[n++] = m_psVertexList[p->nVertex[c]].Z;

            if( Buffer2 ) {
                Buffer2[k++] = m_psVertexList[p->nVertex[c]].X;
                Buffer2[k++] = m_psVertexList[p->nVertex[c]].Y;
                Buffer2[k++] = m_psVertexList[p->nVertex[c]].Z;
            }
        }
    }
}


///////////////////
// Sorts the mesh
void CModel::sortMesh(mesh_t *psMesh)
{
    // Sorts the mesh so polygons are in order of lightmap id's
    // This will reduce texture changes
    uint i,j;

    polygon_t p;
    for(i=0; i<psMesh->nNumPolygons; i++) {
	    for(j=0; j<psMesh->nNumPolygons-1-i; j++) {
            if( psMesh->psPolygons[j].nLightMapID < psMesh->psPolygons[j+1].nLightMapID ) {
                // Swap em
                p = psMesh->psPolygons[j];
                psMesh->psPolygons[j] = psMesh->psPolygons[j+1];
                psMesh->psPolygons[j+1] = p;
            }
        }
    }
}


///////////////////
// Load the material
bool CModel::loadMaterials(char *szFilename)
{
	uint    n,i,j;
	int     val = 0;
	char    sLine[256];
	char    sMat[64];
	int     iLength = 0;


	FILE *fp = fopen(szFilename,"rt");
	if(fp == NULL)
		return false;

	
	// Go through each line looking for a material name
	while(!feof(fp)) {

		// Parse the lines
		fscanf(fp,"%[^\n]\n",sLine);
		TrimSpaces(sLine);
		iLength = strlen(sLine);
        if( iLength == 0 )
            continue;


		if(sLine[iLength-1] == '{') {
			strcpy(sMat,sLine);
			sMat[iLength-1] = '\0';
			TrimSpaces(sMat);
		
			// Match up the mat names and load the info
			CMaterial *mat = m_pcMaterials;
			for( n=0; n<m_nNumMaterials; n++,mat++ ) {
                if( mat->isLoaded() )
                    continue;
				if(stricmp(mat->getName(), sMat) == 0) {
					mat->Load(n,fp);
                    break;
				}
			}
		}
	}

	fclose(fp);


    //
    // Sort the materials
    //

    // We want to sort the materials so that materials using blending come last, and textured materials come first
    // This decreases the number of state changes and hopefully increases performance
    // Material ID's are kept the same to maintain linking for polygons

    CMaterial m;

    for(i=0; i<m_nNumMaterials; i++) {
		for(j=0; j<m_nNumMaterials-1-i; j++) {

            // Blended materials last
            if( m_pcMaterials[j].getContainsBlending() != m_pcMaterials[j+1].getContainsBlending() ) {
                // Swap it
                if( m_pcMaterials[j].getContainsBlending() && !m_pcMaterials[j+1].getContainsBlending()) {
                    m = m_pcMaterials[j+1];
                    m_pcMaterials[j+1] = m_pcMaterials[j];
                    m_pcMaterials[j] = m;
                }
            } else {
                // Make textures first
                if( !m_pcMaterials[j].getPass(0)->iHasTexture && m_pcMaterials[j+1].getPass(0)->iHasTexture ) {
                    // Swap it
                    m = m_pcMaterials[j+1];
                    m_pcMaterials[j+1] = m_pcMaterials[j];
                    m_pcMaterials[j] = m;
                }
            }
        }
    }


	return true;
}


///////////////////
// Load the lightmap file
bool CModel::loadLightmaps(char *szFile)
{
    FILE *fp = fopen(szFile,"rb");
    if( !fp )
        return false;

    uchar   sig[] = {'L', 'i', 'g', 'h', 't', 'm', 'a', 'p', ' ', 'A', 'U', 'X', 0x1A, 0x08, 0x04};
    uchar   ID[32];
    uint    Version, i;

    fread( ID,  sizeof(uchar),  15, fp );
    if( memcmp(sig,ID,15) != 0 ) {
        printf("Warning: Wrong lightmap file signiture\n");
        fclose(fp);
        return false;
    }

    fread( &Version,    sizeof(uint),   1,  fp );
    if( Version != 1 ) {
        printf("Warning: Wrong lightmap version\n");
        fclose(fp);
        return false;
    }

    fread( &m_nNumLightmaps,  sizeof(uint),   1,  fp );

    // Allocate the textures
    m_psLightmaps = new texture_t[m_nNumLightmaps];
    if( !m_psLightmaps ) {
        printf("Warning: Out of memory loading lightmaps\n");
        fclose(fp);
        return false;
    }    

    for( i=0; i<m_nNumLightmaps; i++ ) {
        texture_t *tex = &m_psLightmaps[i];
        fread( &tex->iWidth,    sizeof(uint),   1,  fp );
        fread( &tex->iHeight,   sizeof(uint),   1,  fp );

        tex->bmpImage = CreateSurface(24, tex->iWidth, tex->iHeight);

        fread( tex->bmpImage->pixels,   sizeof(uchar),  tex->iWidth*tex->iHeight*3, fp );

        // Swap the RGB around
        uchar *p = (uchar *)tex->bmpImage->pixels;
        for( int n=0; n<tex->iWidth*tex->iHeight; n++ ) {
            uchar temp = p[n*3+2];
            p[n*3+2] = p[n*3];
            p[n*3] = temp;
        }
        
        // Setup the texture
        glGenTextures(1, &tex->iID);
        tex->iBytesPerPixel = tex->bmpImage->format->BytesPerPixel;
	    tex->iMipmaps = true;
	    tex->nWrap_S = GL_CLAMP;
        tex->nWrap_T = GL_CLAMP;
        tex->iFormat = GL_RGB;
        tex->iUploaded = false;

        Tex_Upload(tex,true);
    }

    m_bLightmap = true;

    fclose(fp);

    return true;
}



/*
========================================================================

                           Calculation Routines

========================================================================
*/


///////////////////
// Calculate the dimensions of the model
CVec CModel::calculateDimensions(void)
{
    CVec min = CVec(99999,99999,99999);
    CVec max = CVec(-99999,-99999,-99999);
    
    vector_t *v = m_psVertexList;
    for( uint i=0; i<m_nNumVertices; i++,v++ ) {

        // Min's
        if( v->X < min.GetX() )
            min.SetX(v->X);
        if( v->Y < min.GetY() )
            min.SetY(v->Y);
        if( v->Z < min.GetZ() )
            min.SetZ(v->Z);

        // Max's
        if( v->X > max.GetX() )
            max.SetX(v->X);
        if( v->Y > max.GetY() )
            max.SetY(v->Y);
        if( v->Z > max.GetZ() )
            max.SetZ(v->Z);
    }

    return max-min;
}



/*
========================================================================

                           Rendering Routines

========================================================================
*/

///////////////////
// Render the model
void CModel::render(void)
{
    uint n, i;
    bool bSelected = false;
    bool bMultiTexture = System_MultiTextureEnabled();

    glColor3f(1,1,1);

    if( m_bLightmap ) {
        Tex_EnableMultitexturing(true);

        // Setup texture1 for lightmaps
        Tex_SelectTexture(GL_TEXTURE1);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ZERO, GL_SRC_COLOR);
	    glColor4f(1,1,1,1);
    }

    int nLighting = glIsEnabled(GL_LIGHTING);

    
    // Go through every material
    CMaterial *mat = m_pcMaterials;
    for( UINT k=0; k<(int)m_nNumMaterials; k++,mat++ ) {
        int mat_id = mat->getID();

        Tex_SelectTexture(GL_TEXTURE0);

        // Go through each pass
        for( int pass=0; pass<mat->getNumPasses(); pass++ ) {
            mat->setupPass(pass, nLighting);

            // BAD hack
            if( m_bLightmap ) {
                State_Disable(GL_TEXTURE_GEN_S);
		        State_Disable(GL_TEXTURE_GEN_T);
            }
            

            // Go through each mesh
            mesh_t *mesh = m_psMeshes;
            for( n=0; n<m_nNumMeshes; n++,mesh++ ) {

                // Don't draw selected meshes, they get drawn below
                if( mesh->nFlags & MSH_SELECTED ) {
                    bSelected = true;
                    continue;
                }

                // Don't draw hidden meshes
                if( mesh->nFlags & MSH_HIDDEN )
                    continue;

                glInterleavedArrays( GL_N3F_V3F, 0, mesh->psVertexBuffer[0] );
            

                polygon_t *p = mesh->psPolygons;
                for( i=0; i<mesh->nNumPolygons; i++, p++ ) {

                    // Is the polygon hidden?
                    if( p->nFlags & PLY_HIDDEN )
                        continue;

                    if( p->nTextureID != mat_id )
                        continue;                

                    if( m_bLightmap ) {
                        Tex_SelectTexture(GL_TEXTURE1);
                        Tex_Bind( m_psLightmaps + p->nLightMapID );
                    }

                    glBegin(GL_TRIANGLES);

                    for( int v=0; v<3; v++ ) {

                        if(bMultiTexture) {
                            // Multitexture coordinates
                            if( p->nNumChannels > 1 )
                                glMultiTexCoord2fARB( GL_TEXTURE0, 
                                                      m_psTexCoordList[p->psChannels[1].nTexCoord[v]].U, 
                                                      -m_psTexCoordList[p->psChannels[1].nTexCoord[v]].V );
                            if( m_bLightmap )
                                glMultiTexCoord2fARB( GL_TEXTURE1,
                                                      m_psTexCoordList[p->psChannels[0].nTexCoord[v]].U, 
                                                      m_psTexCoordList[p->psChannels[0].nTexCoord[v]].V );
                        } else {
                            // Normal texture coordinates
                            if( p->nNumChannels > 1 )
                                glTexCoord2f(m_psTexCoordList[p->psChannels[1].nTexCoord[v]].U, 
                                             -m_psTexCoordList[p->psChannels[1].nTexCoord[v]].V );
                        }


                        glArrayElement( i*3+v );
                    }
                    glEnd();

                    tMainSR3.TriCount++;
                }          
            }
        }
    }
    
    Tex_EnableMultitexturing(false);
    State_Disable(GL_TEXTURE_GEN_S);
	State_Disable(GL_TEXTURE_GEN_T);


    // If we had any selected meshes, draw em here
    if( bSelected ) {
        State_Disable(GL_TEXTURE_GEN_S);
	    State_Disable(GL_TEXTURE_GEN_T);
	    State_Disable(GL_TEXTURE_2D);
	    State_Enable(GL_BLEND);
	    glBlendFunc(GL_SRC_COLOR,GL_ONE_MINUS_SRC_COLOR);
    	glColor4f(1,1,1,0.75f);

        mesh_t *mesh = m_psMeshes;
        for( n=0; n<m_nNumMeshes; n++,mesh++ ) {
            // Don't draw hidden meshes
            //if( mesh->nFlags & MSH_HIDDEN )
                //continue;

            // Only draw selected meshes
            if( !(mesh->nFlags & MSH_SELECTED) )
                continue;

            // Make it unselected now
            mesh->nFlags &= ~MSH_SELECTED;

            glInterleavedArrays( GL_N3F_V3F, 0, mesh->psVertexBuffer[0] );

            glBegin(GL_TRIANGLES);
            for( i=0; i<mesh->nNumPolygons; i++ ) {
                glArrayElement( i*3 );
                glArrayElement( i*3+1 );
                glArrayElement( i*3+2 );
                tMainSR3.TriCount++;
            }
            glEnd();
        }
    }



    // Lightmaps
    if( !m_bLightmap ) {
        //glDisableClientState(GL_VERTEX_ARRAY);
        //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        //glDisableClientState(GL_NORMAL_ARRAY);
        return;
    }

    return;

    State_Disable(GL_TEXTURE_GEN_S);
	State_Disable(GL_TEXTURE_GEN_T);
	State_Enable(GL_TEXTURE_2D);
	State_Enable(GL_BLEND);
	glBlendFunc(GL_ZERO, GL_SRC_COLOR);
	glColor4f(1,1,1,1);	


    for( UINT l=0; l<m_nNumLightmaps; l++ ) {
        Tex_Bind( m_psLightmaps+l );

        mesh_t *mesh = m_psMeshes;
        for( n=0; n<m_nNumMeshes; n++,mesh++ ) {

            glInterleavedArrays( GL_T2F_N3F_V3F, 0, mesh->psVertexBuffer[1] );

            glBegin(GL_TRIANGLES);

            for( i=0; i<mesh->nNumPolygons; i++ ) {

                if( mesh->psPolygons[i].nLightMapID != l )
                    continue;

                glArrayElement( i*3 );
                glArrayElement( i*3+1 );
                glArrayElement( i*3+2 );
                //tMainSR3.TriCount++;
            }

            glEnd();
        }
    }

    
    //glDisableClientState(GL_VERTEX_ARRAY);
    //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    //glDisableClientState(GL_NORMAL_ARRAY);
}


///////////////////
// Renders the raw meshes of the model
void CModel::renderRaw(void)
{
    uint i,n;

    mesh_t *mesh = m_psMeshes;
    for( n=0; n<m_nNumMeshes; n++,mesh++ ) {
        // Don't draw hidden meshes
        if( mesh->nFlags & MSH_HIDDEN )
            continue;

        glInterleavedArrays( GL_N3F_V3F, 0, mesh->psVertexBuffer[0] );

        glBegin(GL_TRIANGLES);
        for( i=0; i<mesh->nNumPolygons; i++ ) {
            glArrayElement( i*3 );
            glArrayElement( i*3+1 );
            glArrayElement( i*3+2 );
            tMainSR3.TriCount++;
        }
        glEnd();
    }

    // Disable the arrays
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}


///////////////////
// Renders the raw wireframe of the model
void CModel::renderRawWireframe(void)
{
    uint i,n;

    mesh_t *mesh = m_psMeshes;
    for( n=0; n<m_nNumMeshes; n++,mesh++ ) {
        // Don't draw hidden meshes
        if( mesh->nFlags & MSH_HIDDEN )
            continue;

        glInterleavedArrays( GL_N3F_V3F, 0, mesh->psVertexBuffer[0] );

        glBegin(GL_LINE_STRIP);
        for( i=0; i<mesh->nNumPolygons; i++ ) {
            glArrayElement( i*3 );
            glArrayElement( i*3+1 );
            glArrayElement( i*3+2 );
            glArrayElement( i*3 );
            tMainSR3.TriCount++;
        }
        glEnd();
    }

    // Disable the arrays
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}


///////////////////
// Renders the model using decal polygons
void CModel::renderDecal(polygon_t *psPolygons, int nNumPolys)
{
    assert(psPolygons);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    polygon_t *p = psPolygons;
    
    glBegin(GL_TRIANGLES);
    for( int n=0; n<nNumPolys; n++,p++ ) {

        // If the mesh this polygon was a part of is hidden, don't draw this polygon
        if( p->psMesh->nFlags & MSH_HIDDEN )
            continue;

        for( int i=0; i<3; i++ ) {
            glNormal3f( m_psVertexNormalList[p->nVertexNormal[i]].X, 
                        m_psVertexNormalList[p->nVertexNormal[i]].Y,
                        m_psVertexNormalList[p->nVertexNormal[i]].Z );
            
            glTexCoord2f( p->sDecalCoords[i].U, p->sDecalCoords[i].V );
            
            glVertex3f( m_psVertexList[p->nVertex[i]].X, 
                        m_psVertexList[p->nVertex[i]].Y,
                        m_psVertexList[p->nVertex[i]].Z );
        }
    }

    glEnd();
}


///////////////////
// Find and return a game object by name
gobject_t *CModel::findGameObject(char *sName)
{
	gobject_t *g = m_psGameObjects;

	for( UINT n=0; n<m_nNumGameObjects; n++,g++ ) {
		if(stricmp(sName,g->sName) == 0)
			return g;
	}


	return NULL;
}
///////////////////
// Overloaded function
// Find first of two objects and return a game object by name
gobject_t *CModel::findGameObject(char *szName1,char *szName2)
{
	gobject_t *g = m_psGameObjects;

	for( UINT n=0; n<m_nNumGameObjects; n++,g++ ) {
		if( (stricmp(szName1,g->sName) == 0) || (stricmp(szName2,g->sName) == 0) )
			return g;
	}

	return NULL;
}
///////////////////
// Find and return a game object by name (using n chars of name)
gobject_t *CModel::findGameObjectn(char *sName, int n)
{
	gobject_t *g = m_psGameObjects;

	for( UINT i=0; i<m_nNumGameObjects; i++,g++ ) {
		if(strnicmp(sName,g->sName,n) == 0)
			return g;
	}


	return NULL;
}


///////////////////
// Find a material based on material id
CMaterial *CModel::findMaterial(int id)
{
    CMaterial *m = m_pcMaterials;
    for( UINT n=0; n<m_nNumMaterials; n++, m++ ) {
        if( m->getID() == id )
            return m;
    }

    // Not found
    return NULL;
}


///////////////////
// Find a mesh based on triangle index
int CModel::findMesh(int tri)
{
	int count = 0;

    mesh_t *m = m_psMeshes;	
	for( uint i=0; i<m_nNumMeshes; i++,m++ ) {
		count += m->nNumPolygons;
		if(tri < count)
			return (int)i;
	}

	// Not found
	return -1;
}


///////////////////
// Find a mesh based on name (overloads the previous func)
mesh_t *CModel::findMesh(char *name)
{
	mesh_t *m = m_psMeshes;	
	for( uint i=0; i<m_nNumMeshes; i++,m++ ) {
        if(stricmp(name,m->szName) == 0)
			return m;
	}

	return NULL;
}


///////////////////
// Add a flag to all the meshes
void CModel::addMeshFlag(int flag)
{
    mesh_t *m = m_psMeshes;	
	for( uint i=0; i<m_nNumMeshes; i++,m++ )
        m->nFlags |= flag;
}


///////////////////
// Remove a flag from all the meshes
void CModel::removeMeshFlag(int flag)
{
    mesh_t *m = m_psMeshes;	
	for( uint i=0; i<m_nNumMeshes; i++,m++ )
        m->nFlags &= ~flag;
}


///////////////////
// Shutdown the model
void CModel::shutdown(void)
{
	uint    i,n;

	// Meshes
	if( m_psMeshes ) {

        for( i=0; i<m_nNumMeshes; i++ ) {
			mesh_t *m = &m_psMeshes[i];

            if( m->psPolygons ) {
                for( n=0; n<m->nNumPolygons; n++ ) {
                    if( m->psPolygons[n].psChannels )
                        delete[] m->psPolygons[n].psChannels;
                }

                delete[] m->psPolygons;
            }

            if( m->psVertexBuffer[0] )
                delete[] m->psVertexBuffer[0];
            if( m->psVertexBuffer[1] )
                delete[] m->psVertexBuffer[1];
        }

		delete[] m_psMeshes;
		m_psMeshes = NULL;
	}

	// Lights
	if( m_psLights ) {
		delete[] m_psLights;
		m_psLights = NULL;
	}

	// Game objects
	if( m_psGameObjects ) {
		delete[] m_psGameObjects;
		m_psGameObjects = NULL;
	}

    // Collision detection
	if( m_pcCDModel ) {
		delete m_pcCDModel;
		m_pcCDModel = NULL;
	}

    // Materials
    if( m_pcMaterials ) {
        delete[] m_pcMaterials;
        m_pcMaterials = NULL;
    }

    // Lightmaps
    if( m_psLightmaps ) {

        for( i=0; i<m_nNumLightmaps; i++ ) {
            if( m_psLightmaps[i].bmpImage )
                SDL_FreeSurface( m_psLightmaps[i].bmpImage );
        }

        delete[] m_psLightmaps;
        m_psLightmaps = NULL;
    }
}