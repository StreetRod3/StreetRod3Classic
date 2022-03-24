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


#ifndef __CMODEL_H__
#define __CMODEL_H__


// Simple light structure (because the tiles/ents use a special light structure)
typedef struct {
	CVec		Position;
	colour_t	Colour;
	int			Style;
	float		fRadius;
    float       fBrightness;
} light_t;


// Mesh flags
#define		MSH_HIDDEN		0x0001
#define		MSH_SELECTED	0x0002
#define		MSH_SMOOTH		0x0004


// Polygon flags
#define         PLY_HIDDEN      0x0001


// Mesh
typedef struct mesh_s {

    int         nFlags;
    int         nID;

    char        szName[64];
    uint        nNumPolygons;
    polygon_t   *psPolygons;

    uint        nNumVerts;
    float       *psVertexBuffer[2];

} mesh_t;


// A game object structure
typedef struct {
	char		sName[32];
	CVec		vPosition;
	CMatrix		cMatrix;
} gobject_t;


class CModel {
public:
	// Constructor
	CModel() {

	    m_nNumMaterials = 0;
		m_nNumMeshes = 0;
		m_nNumLights = 0;
		m_nNumGameObjects = 0;

        m_nNumVertices = 0;
        m_nNumTexCoords = 0;
        m_nNumVertexNormals = 0;

        m_psVertexList = NULL;
        m_psTexCoordList = NULL;
        m_psVertexNormalList = NULL;

        m_bLightmap = false;
        m_nNumLightmaps = 0;
        m_psLightmaps = NULL;
		
		m_pcMaterials = NULL;
        m_psMeshes = NULL;
		m_psLights = NULL;
		m_psGameObjects = NULL;
		m_pcCDModel = NULL;

        m_nNumPolygons = 0;

	}


private:
	// Attributes

    // Number of items
	uint		m_nNumMaterials;
    uint        m_nNumMeshes;	
	uint		m_nNumLights;
	uint		m_nNumGameObjects;

    // List counts
    uint        m_nNumVertices;
    uint        m_nNumTexCoords;
    uint        m_nNumVertexNormals;

    // Lists
    vector_t    *m_psVertexList;
    texcoord_t  *m_psTexCoordList;
    vector_t    *m_psVertexNormalList;

    // Lightmap
    bool        m_bLightmap;
    uint        m_nNumLightmaps;
    texture_t   *m_psLightmaps;

	CMaterial	*m_pcMaterials;
    mesh_t      *m_psMeshes;	
	light_t		*m_psLights;
	gobject_t	*m_psGameObjects;

	// Collision detection model
	CollisionModel3D	*m_pcCDModel;

    // Statistics
    uint        m_nNumPolygons;


public:
	// Methods


    //
    // Loading
    //
	bool        load(char *szName);
	bool        load3DData(char *szFilename);
	bool        loadMaterials(char *szFilename);
	bool        loadLightmaps(char *szFilename);
	bool		loadnew3DData(FILE *fp);

    char        *readString(char *szString, FILE *fp);
  

    
    //
    // Calculation
    //

    CVec        calculateDimensions(void);


    //
    // Closing
    //
    void		shutdown(void);



    //
    // Rendering
    //
	void		render(void);
	void		renderRaw(void);
    void        renderRawWireframe(void);
    void        renderDecal(polygon_t *psPolygons, int nNumPolys);



    //
    // Game Objects
    //
	gobject_t	*findGameObject(char *szName);
	gobject_t	*findGameObject(char *szName1,char *szName2);	// 17/06/05 New function to return first object found
	gobject_t	*findGameObjectn(char *szName, int n);
	int			getNumGameObjects(void)		    { return m_nNumGameObjects; }
	gobject_t	*getGameObject(int i)		    { return &m_psGameObjects[i]; }



    //
    // Meshes
    //
    void        generateVertexBuffer(mesh_t *psMesh);
    void        sortMesh(mesh_t *psMesh);
    int			findMesh(int tri);
	mesh_t		*findMesh(char *szName);
	int			getNumMeshes(void)				{ return m_nNumMeshes; }
	mesh_t		*getMeshes(void)				{ return m_psMeshes; }	
    void        addMeshFlag(int flag);
    void        removeMeshFlag(int flag);


    //
    // Materials
    //
    CMaterial   *findMaterial(int id);
	int			getNumMaterials(void)			{ return m_nNumMaterials; }
	CMaterial	*getMaterial(int _m)			{ return &m_pcMaterials[_m]; }    



    //
    // Lights
    //

	int			getNumLights(void)				{ return m_nNumLights; }
	light_t		getLight(int _l)				{ return m_psLights[_l]; }
	light_t		*getLights(void)				{ return m_psLights; }



    //
    // Lists
    //
    uint        getNumVertices(void)            { return m_nNumVertices; }
    vector_t    *getVertexList(void)            { return m_psVertexList; }

    texcoord_t  *getTexCoordList(void)          { return m_psTexCoordList; }



    //
    // Extra
    //
 	CollisionModel3D		*getCDModel(void)	{ return m_pcCDModel; }
    uint        getNumPolygons(void)            { return m_nNumPolygons; }
};









#endif  //  __CMODEL_H__