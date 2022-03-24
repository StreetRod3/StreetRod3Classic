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



// QuadTree Class
// Created 18/7/03
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "game.h"
#include "racing.h"


///////////////////
// CQuadTree Constructor
CQuadTree::CQuadTree()
{
    m_psPolyDrawList = NULL;
    m_pcScene = NULL;
    m_nNumPolygons = 0;
    m_psQuadTree = NULL;

    m_nMinPolygons = 50;
    m_nMinSize = 50;
}


///////////////////
// Create the quadtree from a model
bool CQuadTree::create(CModel *pcScene)
{
    mesh_t  *m = pcScene->getMeshes();
    int     i, poly;
    uint    p;

    assert(pcScene);

    m_pcScene = pcScene;

    // Count the number of polygons in the model
    m_nNumPolygons = 0;
    m = pcScene->getMeshes();
    for(i=0; i<pcScene->getNumMeshes(); i++, m++)
        m_nNumPolygons += m->nNumPolygons;

    // Allocate the polygon list
    m_psPolyDrawList = new polygon_t*[m_nNumPolygons];
    if( !m_psPolyDrawList )
        return false;


    // Link the polygons in the list
    poly = 0;
    m = pcScene->getMeshes();
    for(i=0; i<pcScene->getNumMeshes(); i++, m++) {
        for(p=0; p<m->nNumPolygons; p++)
            m_psPolyDrawList[poly++] = &m->psPolygons[p];
    }

    // Create the quads
    createQuadTree(pcScene);

    return true;
}


///////////////////
// Process the quad tree
void CQuadTree::process(CCamera *pcCam)
{
    int     i, p;
    CVec    f, r;
    float   farDistance = 1000;
    
    // Initialize the drawing list to all hidden
    for(i=0; i<m_nNumPolygons; i++)
        m_psPolyDrawList[i]->nFlags |= PLY_HIDDEN;


    // Calculate the frustum    
    GetAngles( pcCam->getPitch(), 0, pcCam->getYaw(), &f, &r, NULL);
 
    // Near
    m_sFrustum[0].vNormal = f;
    m_sFrustum[0].fDistance = DotProduct(-m_sFrustum[0].vNormal, pcCam->getPos());
    // Far
    m_sFrustum[1].vNormal = -f;
    m_sFrustum[1].fDistance = DotProduct(-m_sFrustum[1].vNormal, pcCam->getPos()+f*farDistance);
    // Left
    m_sFrustum[2].vNormal = f+r;
    m_sFrustum[2].fDistance = DotProduct(-m_sFrustum[2].vNormal, pcCam->getPos());
    // Right
    m_sFrustum[3].vNormal = f-r;
    m_sFrustum[3].fDistance = DotProduct(-m_sFrustum[3].vNormal, pcCam->getPos());


    // Go through all the quads
    checkQuad(m_psQuadTree, pcCam->getPos());



    //
    // Hide Meshes
    //

    // If a all the polys in a mesh are hidden, hide the mesh
    // This significantly reduces the number of state changes & the extra processing
    mesh_t *m = m_pcScene->getMeshes();
    for(i=0; i<m_pcScene->getNumMeshes(); i++, m++) {
        bool bHidden = true;
        polygon_t *poly = m->psPolygons;
        for(p=0; p<m->nNumPolygons; p++, poly++) {
            if(!(poly->nFlags & PLY_HIDDEN)) {
                bHidden = false;
                break;
            }
        }

        if(bHidden)
            m->nFlags |= MSH_HIDDEN;
        else
            m->nFlags &= ~MSH_HIDDEN;
    }
}


///////////////////
// Checks if a quad is in the frustum
void CQuadTree::checkQuad(quad_t *psQuad, CVec cCamPos)
{
    if(!psQuad)
        return;

    bool    inside = false;
    float   camSize = 25;       // Make a fat camera so nearby polys are drawn
    int     i, p;


    // If the camera position is in the quad, add the quad
    if(fabs((psQuad->cPos-cCamPos).GetX()) < psQuad->cSize.GetX()+camSize && 
       fabs((psQuad->cPos-cCamPos).GetY()) < psQuad->cSize.GetY()+camSize)
        inside = true;

    // Check the frustum
    if(!inside) {
        
        // Within left/right sides
        if(ProjectPoint(&m_sFrustum[2], psQuad->cPos) > -psQuad->fTestSize && 
           ProjectPoint(&m_sFrustum[3], psQuad->cPos) > -psQuad->fTestSize)
           inside = true;

        // Within far plane
        if(ProjectPoint(&m_sFrustum[1], psQuad->cPos) < -psQuad->fTestSize)
            inside = false;

        // Within near plane
        if(ProjectPoint(&m_sFrustum[0], psQuad->cPos) < -psQuad->fTestSize)
            inside = false;
    }


    // If this quad is inside go through the children, or add the polys if it's a leaf
    if(inside) {

        if(psQuad->bLeaf) {

            // Unset the hidden flag on this quads polygons
            for(p=0; p<psQuad->nNumPolys; p++)
                psQuad->psPolyList[p]->nFlags &= ~PLY_HIDDEN;
        } else {

            // Go down the children
            for(i=0; i<4; i++)
                checkQuad(psQuad->psChild[i], cCamPos);
        }
    }
}


///////////////////
// Create the quad tree
void CQuadTree::createQuadTree(CModel *pcScene)
{
    // Find the bounding rectangle around the level
	CVec	min = CVec(9999,9999,9999);
	CVec	max = CVec(-9999,-9999,-9999);

    uint    i;

    // Go through every vertex
    vector_t *v = pcScene->getVertexList();
    for(i=0; i<pcScene->getNumVertices(); i++, v++) {
        // Min
        if( v->X < min.GetX() )
            min.SetX(v->X);
        if( v->Y < min.GetY() )
            min.SetY(v->Y);
        
        // Max
        if( v->X > max.GetX() )
            max.SetX(v->X);
        if( v->Y > max.GetY() )
            max.SetY(v->Y);
    }

	// Find the center of the bounding box
	CVec center = min + (max-min)/2;

	// Start the quad tree
	m_psQuadTree = calculateQuad(pcScene, center, (max-min)/2);
}


///////////////////
// Calculate a single quad (recursively)
quad_t *CQuadTree::calculateQuad(CModel *pcScene, CVec cCenter, CVec cSize)
{
    int     i;
    int     nNumPolys = 0;

    quad_t *psQuad = new quad_t;
    if(!psQuad)
        return NULL;


    // Fill in the info
    psQuad->bLeaf = false;
    psQuad->cPos = cCenter;
    psQuad->cSize = cSize;
    psQuad->fMaxSize = MAX(cSize.GetX(), cSize.GetY());
    psQuad->fMinSize = MIN(cSize.GetX(), cSize.GetY());
    psQuad->fTestSize = (float)sqrt(psQuad->fMaxSize*psQuad->fMaxSize*2);
    psQuad->nNumPolys = 0;
    psQuad->psPolyList = NULL;
    for(i=0; i<4; i++)
        psQuad->psChild[i] = NULL;


    // Go through the scene checking how many polygons are in the quad
    for(i=0; i<m_nNumPolygons; i++ ) {
        polygon_t *p = m_psPolyDrawList[i];

        if( polyInQuad(pcScene, p, psQuad) )
            nNumPolys++;
    }


    // If this quad doesn't match the rules to continue, create a leaf out of the quad
    if(nNumPolys < m_nMinPolygons || psQuad->fMaxSize < m_nMinSize) {
        calculateQuadLeaf(pcScene, psQuad, nNumPolys);
        return psQuad;
    }


    // Go down to the 4 children
    CVec quart = cSize/2;

	psQuad->psChild[0] = calculateQuad(pcScene, CVec(cCenter.GetX()-quart.GetX(), cCenter.GetY()-quart.GetY(),0), quart);
	psQuad->psChild[1] = calculateQuad(pcScene, CVec(cCenter.GetX()+quart.GetX(), cCenter.GetY()-quart.GetY(),0), quart);
	psQuad->psChild[2] = calculateQuad(pcScene, CVec(cCenter.GetX()+quart.GetX(), cCenter.GetY()+quart.GetY(),0), quart);
	psQuad->psChild[3] = calculateQuad(pcScene, CVec(cCenter.GetX()-quart.GetX(), cCenter.GetY()+quart.GetY(),0), quart);


    return psQuad;
}


///////////////////
// Calculate a leaf from a quad
void CQuadTree::calculateQuadLeaf(CModel *pcScene, quad_t *psQuad, int nNumPolys)
{
    int i;
    int poly;

    psQuad->bLeaf = true;
    psQuad->nNumPolys = nNumPolys;

    // Allocate the poly list
    psQuad->psPolyList = new polygon_t*[psQuad->nNumPolys];
    if( !psQuad->psPolyList )
        return;

    // Add the polygons to the quad
    poly = 0;
    for(i=0; i<m_nNumPolygons; i++ ) {
        polygon_t *p = m_psPolyDrawList[i];

        if( polyInQuad(pcScene, p, psQuad) )
            psQuad->psPolyList[poly++] = p;
    }
}


///////////////////
// Checks if a polygon is in a quad
bool CQuadTree::polyInQuad(CModel *pcScene, polygon_t *psPoly, quad_t *psQuad)
{
    vector_t    *vlist = pcScene->getVertexList();
    int         i;

    // Check if any of the vertices are in the polygon
    for(i=0; i<3; i++) {
        vector_t *v = vlist + psPoly->nVertex[i];
        if( fabs(psQuad->cPos.GetX() - v->X) < psQuad->cSize.GetX() &&
            fabs(psQuad->cPos.GetY() - v->Y) < psQuad->cSize.GetY())
            return true;
    }


    // Check if the bounding box of the polygon intersects with the quad box
    CVec max = CVec(-999999, -999999, -999999);
    CVec min = CVec(999999, 999999, 999999);
    for(i=0; i<3; i++) {
        vector_t *v = vlist + psPoly->nVertex[i];
        if(v->X > min.GetX())   min.SetX(v->X);
        if(v->Y > min.GetY())   min.SetY(v->Y);
        if(v->X < max.GetX())   max.SetX(v->X);
        if(v->Y < max.GetY())   max.SetY(v->Y);
    }

    CVec center = min + (max-min)/2;
    CVec ext = (max-min)/2;

    // AABB Seperation test
    CVec bb = psQuad->cPos - center;
    CVec be = psQuad->cSize + ext;
    if( fabs(bb.GetX()) < be.GetX() &&
        fabs(bb.GetY()) < be.GetY())
        return true;

    // Not in quad
    return false;
}


///////////////////
// Load a quadtree
// Compares the tree to the model
// If the tree is out of date, updates the tree
// Returns false if loading failed
bool CQuadTree::load(char *szName, CModel *pcScene)
{
    char        sig[] = {'A','u','x',' ','Q','u','a','d','T','r','e','e',0x1A,0x08,0x04};
    char        id[15];
    int         nVersion;

    if(!pcScene)
        return false;

    FILE *fp = fopen(szName, "rb");
    if(!fp)
        return false;

    // Check the header
    fread(id,           sizeof(char),   15, fp);
    fread(&nVersion,    sizeof(int),    1,  fp);

    if(memcmp(sig, id, 15) != 0 || nVersion != QUAD_VERSION) {
        fclose(fp);
        return false;
    }

    // Do some checks against the model
    int     nNumMeshes;
    int     nNumPolys;
    int     nNumVerts;
    CVec    cDim;

    fread(&nNumMeshes,  sizeof(int),    1,  fp);
    fread(&nNumPolys,   sizeof(int),    1,  fp);
    fread(&nNumVerts,   sizeof(int),    1,  fp);
    fread(&cDim,        sizeof(CVec),   1,  fp);

    if(nNumMeshes != pcScene->getNumMeshes() ||
       nNumPolys != pcScene->getNumPolygons() ||
       nNumVerts != pcScene->getNumVertices()) {

        fclose(fp);
        return false;
    }

    // Load the quad tree
    m_psQuadTree = loadQuad(fp, pcScene);


    m_pcScene = pcScene;
    mesh_t  *m = pcScene->getMeshes();
    int     i, poly;
    uint    p;

    // Count the number of polygons in the model
    m_nNumPolygons = 0;
    m = pcScene->getMeshes();
    for(i=0; i<pcScene->getNumMeshes(); i++, m++)
        m_nNumPolygons += m->nNumPolygons;

    // Allocate the polygon list
    m_psPolyDrawList = new polygon_t*[m_nNumPolygons];
    if( !m_psPolyDrawList )
        return false;

    // Link the polygons in the list
    poly = 0;
    m = pcScene->getMeshes();
    for(i=0; i<pcScene->getNumMeshes(); i++, m++) {
        for(p=0; p<m->nNumPolygons; p++)
            m_psPolyDrawList[poly++] = &m->psPolygons[p];
    }

    fclose(fp);

    return true;
}


///////////////////
// Load a single quad
quad_t *CQuadTree::loadQuad(FILE *fp, CModel *pcScene)
{
    quad_t  *psQuad = NULL;
    int     nType;
    int     i;

    fread(&nType,   sizeof(int),    1,  fp);

    if(nType == SQ_END)
        return NULL;

    // Create a new quad
    psQuad = new quad_t;
    if(!psQuad)
        return NULL;

    psQuad->bLeaf = false;
    if(nType == SQ_LEAF)
        psQuad->bLeaf = true;
    psQuad->nNumPolys = 0;
    psQuad->psPolyList = NULL;
    
    // Load the details
    fread(&psQuad->cPos,        sizeof(CVec),   1,  fp);
    fread(&psQuad->cSize,       sizeof(CVec),   1,  fp);
    fread(&psQuad->fMaxSize,    sizeof(float),  1,  fp);
    fread(&psQuad->fMinSize,    sizeof(float),  1,  fp);
    fread(&psQuad->fTestSize,   sizeof(float),  1,  fp);

    for(i=0; i<4; i++)
        psQuad->psChild[i] = NULL;

    // A Leaf?
    if(psQuad->bLeaf) {

        // Read the polygons
        fread(&psQuad->nNumPolys,   sizeof(int),    1,  fp);
        psQuad->psPolyList = new polygon_t*[psQuad->nNumPolys];
        if(!psQuad)
            return NULL;
        
        for(i=0; i<psQuad->nNumPolys; i++) {
            // Read the mesh & polygon id's
            int     nMesh;
            ushort  nPoly;
            
            fread(&nMesh,       sizeof(int),    1,  fp);
            fread(&nPoly,       sizeof(ushort), 1,  fp);

            if(nMesh >= 0 && nMesh < pcScene->getNumMeshes()) {
                mesh_t *m = pcScene->getMeshes()+nMesh;

                if(nPoly >= 0 && nPoly < m->nNumPolygons)
                    psQuad->psPolyList[i] = m->psPolygons + nPoly;
                else
                    printf("Error: Bad poly in loadQuad(): %d\n",nPoly);
            } else
                printf("Error: Bad mesh in loadQuad(): %d\n",nMesh);
        }
    } else {
        // Go down the children

        for(i=0; i<4; i++)
            psQuad->psChild[i] = loadQuad(fp, pcScene);
    }

    return psQuad;
}


///////////////////
// Save the quadtree
bool CQuadTree::save(char *szName, CModel *pcScene)
{
    char        sig[] = {'A','u','x',' ','Q','u','a','d','T','r','e','e',0x1A,0x08,0x04};
    int         nVersion = QUAD_VERSION;

    FILE *fp = fopen(szName, "wb");
    if(!fp)
        return false;


    // Write the header
    fwrite(sig,         sizeof(char),   15, fp);
    fwrite(&nVersion,   sizeof(int),    1,  fp);

    // Write out some scene information for updating when loading
    int     nNumMeshes = pcScene->getNumMeshes();
    int     nNumPolys = pcScene->getNumPolygons();
    int     nNumVerts = pcScene->getNumVertices();
    CVec    cDim = pcScene->calculateDimensions();

    fwrite(&nNumMeshes, sizeof(int),    1,  fp);
    fwrite(&nNumPolys,  sizeof(int),    1,  fp);
    fwrite(&nNumVerts,  sizeof(int),    1,  fp);
    fwrite(&cDim,       sizeof(CVec),   1,  fp);

    // Write out the quads
    saveQuad(m_psQuadTree, fp);
    
    fclose(fp);

    return true;
}


///////////////////
// Recursively save the quads
void CQuadTree::saveQuad(quad_t *psQuad, FILE *fp)
{
    int     i;
    int     nType = SQ_QUAD;

    // Write the quad type
    if(!psQuad)
        nType = SQ_END;
    else {
        if(psQuad->bLeaf)
            nType = SQ_LEAF;
    }

    fwrite(&nType, sizeof(int), 1,  fp);

    if(!psQuad)
        return;

    // Quad Details
    fwrite(&psQuad->cPos,       sizeof(CVec),   1,  fp);
    fwrite(&psQuad->cSize,      sizeof(CVec),   1,  fp);
    fwrite(&psQuad->fMaxSize,   sizeof(float),  1,  fp);
    fwrite(&psQuad->fMinSize,   sizeof(float),  1,  fp);
    fwrite(&psQuad->fTestSize,  sizeof(float),  1,  fp);

    // A Leaf?
    if(psQuad->bLeaf) {

        // Write out the polygons
        fwrite(&psQuad->nNumPolys,  sizeof(int),    1,  fp);
        
        for(i=0; i<psQuad->nNumPolys; i++) {
            // Write the mesh & polygon id's
            
            int nMesh = -1;
            if(psQuad->psPolyList[i]->psMesh)
                nMesh = psQuad->psPolyList[i]->psMesh->nID;
            
            fwrite(&nMesh,                      sizeof(int),    1,  fp);
            fwrite(&psQuad->psPolyList[i]->nID, sizeof(ushort), 1,  fp);
        }
    } else {
        // Go down the children

        for(i=0; i<4; i++)
            saveQuad(psQuad->psChild[i], fp);
    }
}


///////////////////
// Recursively free the quad tree
void CQuadTree::shutdownQuad(quad_t *psQuad)
{
    if(!psQuad)
        return;

    // If this is a leaf, free the poly structure
    if(psQuad->bLeaf) {
        if(psQuad->psPolyList)
            delete[] psQuad->psPolyList;

        return;
    }

    // Go down the children
    for(int i=0; i<4; i++)
        shutdownQuad(psQuad->psChild[i]);

    delete psQuad;
}


///////////////////
// Shutdown the quad tree
void CQuadTree::shutdown(void)
{
    if( m_psPolyDrawList ) {
        delete[] m_psPolyDrawList;
        m_psPolyDrawList = NULL;
    }

    shutdownQuad(m_psQuadTree);
}