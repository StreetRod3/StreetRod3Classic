/////////////////////////////////////////
//
//             Street Rod 3
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Racing Engine
// Created 3/04/03
// Jason Boettcher


#include "defs.h"
#include "3D.h"
#include "system.h"
#include "RacingEngine.h"


re_track_t  *g_psTrack = NULL;
int depth = 0;


///////////////////
// Load the track
bool RE_LoadTrack(char *szFilename)
{
    g_psTrack = new re_track_t;
    if( !g_psTrack )
        return false;

    g_psTrack->pcModel = Cache_LoadModel(szFilename);
    if( !g_psTrack->pcModel )
        return false;

    // Create a quad tree out of the model
    RE_CalculateQuadTree();

    
    return true;
}


///////////////////
// Draw the track
void RE_DrawTrack(CCamera *pcCam)
{
    // Clear the drawing list
    memset( g_psTrack->pnPolyDrawList, 0, sizeof(int) * g_psTrack->nNumPolygons );

    // Calculate the frustum    
    CVec f, r;

    GetAngles( pcCam->getPitch(), 0, pcCam->getYaw(), &f, &r, NULL);

    float farDistance = 1500;
    
    // Near
    g_psTrack->sFrustum[0].vNormal = f;
    g_psTrack->sFrustum[0].fDistance = DotProduct(-g_psTrack->sFrustum[0].vNormal, pcCam->getPos());
    // Far
    g_psTrack->sFrustum[1].vNormal = -f;
    g_psTrack->sFrustum[1].fDistance = DotProduct(-g_psTrack->sFrustum[1].vNormal, pcCam->getPos()+f*farDistance);
    // Left
    g_psTrack->sFrustum[2].vNormal = f+r;
    g_psTrack->sFrustum[2].fDistance = DotProduct(-g_psTrack->sFrustum[2].vNormal, pcCam->getPos());
    // Right
    g_psTrack->sFrustum[3].vNormal = f-r;
    g_psTrack->sFrustum[3].fDistance = DotProduct(-g_psTrack->sFrustum[3].vNormal, pcCam->getPos());


    // Recurse through the quads checking which quads are in the frustum
    RE_CheckQuad( g_psTrack->psQuadTree, pcCam->getPos() );



    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, g_psTrack->psVertexList);

    // Draw the track
    int         n,i,p;

    // Go through the model's materials, and draw all polygons for that material
    CMaterial *m = g_psTrack->pcModel->getMaterial(0);
    int mcount = g_psTrack->pcModel->getNumMaterials();
    for( n=0; n<mcount; n++, m++) {

        if( m->getNumPasses() == 0)
            continue;

        m->setupPass(0);

        glColor4f(1,1,1,1);

        glBegin(GL_TRIANGLES);

        // Go through the poly list
        tpoly_t *poly = g_psTrack->psPolyList;
        for( p=0; p<g_psTrack->nNumPolygons; p++, poly++ ) {
            // Draw it?
            if( !g_psTrack->pnPolyDrawList[p] )
                continue;
            // This material?
            if( poly->pcMat->getID() != m->getID() )
                continue;
            
            for( i=0; i<3; i++) {
                //glNormal3f( poly->cNorm[i+1].GetX(), poly->cNorm[i+1].GetY(), poly->cNorm[i+1].GetZ() );
                glTexCoord2f( poly->sTexcoord[i].U, -poly->sTexcoord[i].V );
                glArrayElement( poly->nV[i] );
            }
        }

        glEnd();

    }

    glDisableClientState(GL_VERTEX_ARRAY);


    // Draw the quadtree
    depth=0;
    //RE_DrawQuad( g_psTrack->psQuadTree );
}







/*
==============

  Quad tree

==============
*/

// Rules to quit/keep going
float	minPolyCount = 50;
float	minSize = 50;


///////////////////
// Calculate the quad tree
// Also acts as loading the model into the track polygon array
void RE_CalculateQuadTree(void)
{
	// Find the bounding rectangle around the level
	CVec	min = CVec(9999,9999,9999);
	CVec	max = CVec(-9999,-9999,-9999);
	
	vertex_t	*v, *v2;

    g_psTrack->nNumPolygons = 0;
    g_psTrack->nNumVertices = 0;

	// Go through each object
	CObject *o = g_psTrack->pcModel->GetObjects();
	for(int i=0; i<g_psTrack->pcModel->GetNumObjects(); i++,o++) {
		v = o->GetVertices();

        g_psTrack->nNumVertices += o->GetNumVertices();

		// Go through each poly
		polygon_t *poly = o->GetPolygons();
		for(int p=0; p<o->GetNumPolygons(); p++,poly++) {

            g_psTrack->nNumPolygons++;

			// Go through each vertex
			for(int n=0; n<3; n++) {
				v2 = v+poly->iV[n];

				// Min
				if( v2->vPos.GetX() < min.GetX() )
					min.SetX(v2->vPos.GetX());
				if( v2->vPos.GetY() < min.GetY() )
					min.SetY(v2->vPos.GetY());

				// Max
				if( v2->vPos.GetX() > max.GetX() )
					max.SetX(v2->vPos.GetX());
				if( v2->vPos.GetY() > max.GetY() )
					max.SetY(v2->vPos.GetY());
			}
		}
	}

    

    // Allocate the vertex & polygon lists
    g_psTrack->psVertexList = new CVec[g_psTrack->nNumVertices];
    g_psTrack->psPolyList = new tpoly_t[g_psTrack->nNumPolygons];
    if( !g_psTrack->psVertexList || !g_psTrack->psPolyList )
        return;

    // Allocate room for the drawing list
    g_psTrack->pnPolyDrawList = new int[g_psTrack->nNumPolygons];
    if( !g_psTrack->pnPolyDrawList )
        return;

    //
    // Put the vertices & polygons in the list
    //
    
    // Go through each object
    int vcount=0;
    int pcount=0;
	o = g_psTrack->pcModel->GetObjects();
	for(i=0; i<g_psTrack->pcModel->GetNumObjects(); i++,o++) {
		v = o->GetVertices();

        // Get the vertices
        for(int n=0;n<o->GetNumVertices();n++)
            g_psTrack->psVertexList[vcount++] = v[n].vPos;

        // Get the polys        
		polygon_t *poly = o->GetPolygons();
		for(int p=0; p<o->GetNumPolygons(); p++,poly++) {
            
            // Normals
            for(int n=0; n<4; n++)
                g_psTrack->psPolyList[pcount].cNorm[n] = poly->vNorm[n];
            
            g_psTrack->psPolyList[pcount].sPlane = poly->tPlane;
            
            // Tex coords
            for(n=0; n<3; n++)
                g_psTrack->psPolyList[pcount].sTexcoord[n] = poly->tTexcoord[n];

            // Vertex indices
            for(n=0; n<3; n++)
                g_psTrack->psPolyList[pcount].nV[n] = poly->iV[n]+(vcount-o->GetNumVertices());

            
            g_psTrack->psPolyList[pcount].pcMat = o->getMaterial();
            pcount++;
        }
    }



	// Find the center of the bounding box
	CVec center = min + (max-min)/2;

	//writeLog("Quad tree = %f,%f   (%f,%f)\n",center.GetX(), center.GetY(), (max-min).GetX(), (max-min).GetY());

	// Start the quad tree
	g_psTrack->psQuadTree = RE_CalculateQuad(center, (max-min)/2);
}


///////////////////
// Calculate a quad
quad_t *RE_CalculateQuad(CVec center, CVec size)
{
	quad_t *q = new quad_t;
	if(!q)
		return NULL;

	CVec	*v;
	
	int nNumPolys = 0;

	// Fill in the info
	q->cPos = center;
	q->cSize = size;
	q->nNumPolys = 0;
	q->pnPolyList = NULL;
	q->nLeaf = false;
	for(int i=0; i<4; i++)
		q->psChild[i] = NULL;
    q->fMaxSize = MAX(size.GetX(), size.GetY());
    q->fMinSize = MIN(size.GetX(), size.GetY());


	// Go through checking if polys are in the quad
    tpoly_t *p = g_psTrack->psPolyList;
    for( i=0; i<g_psTrack->nNumPolygons; i++, p++) {
        
        for( int n=0; n<3; n++) {
            v = g_psTrack->psVertexList + p->nV[n];

            // Check if the vertex is inside the quad
			// Probably not the best checking method
			if( fabs((center-*v).GetX()) < size.GetX() &&
				fabs((center-*v).GetY()) < size.GetY()) {

				nNumPolys++;
				break;
			}
        }
    }


	// If this quad doesn't contain enough polygons, or is too small, make a leaf out of the quad
	// and don't go any further
	if( nNumPolys < minPolyCount || q->fMaxSize < minSize  ) {
		RE_CalculateQuadLeaf(q, center, size, nNumPolys);
		return q;
	}


	// Go down to the 4 children	
	CVec half = size/2;
    CVec quart = size/2;

	q->psChild[0] = RE_CalculateQuad(CVec(center.GetX()-quart.GetX(), center.GetY()-quart.GetY(),0), half);
	q->psChild[1] = RE_CalculateQuad(CVec(center.GetX()+quart.GetX(), center.GetY()-quart.GetY(),0), half);
	q->psChild[2] = RE_CalculateQuad(CVec(center.GetX()+quart.GetX(), center.GetY()+quart.GetY(),0), half);
	q->psChild[3] = RE_CalculateQuad(CVec(center.GetX()-quart.GetX(), center.GetY()+quart.GetY(),0), half);

	return q;
}


///////////////////
// Calculate a quad leaf
void RE_CalculateQuadLeaf(quad_t *q, CVec center, CVec size, int nNumPolys)
{
	q->nNumPolys = nNumPolys;
	q->nLeaf = true;

	// Allocate the poly list
	q->pnPolyList = new int[q->nNumPolys];
	if(!q->pnPolyList) {
		delete q;
		return;
	}

    writeLog("Leaf polys = %d\n",q->nNumPolys);

	CVec	*v;

	int num = 0;

	// Go through adding the polygons to the quad
    tpoly_t *p = g_psTrack->psPolyList;
    for( int i=0; i<g_psTrack->nNumPolygons; i++, p++) {
        
        for( int n=0; n<3; n++) {
            v = g_psTrack->psVertexList + p->nV[n];

            // Check if the vertex is inside the quad
			// Probably not the best checking method
			if( fabs((center-*v).GetX()) < size.GetX() &&
				fabs((center-*v).GetY()) < size.GetY()) {

				q->pnPolyList[num++] = i;
				break;
			}
        }
    }
}


///////////////////
// Check if a quad is in the frustum
void RE_CheckQuad(quad_t *q, CVec camPos)
{
    if( !q )
        return;

    int inside = false;
    float camSize = 25;     // Make a fat camera so nearby polys are drawn

    // If the camera position is in the quad, add the quad
    if( fabs( (q->cPos-camPos).GetX() ) < q->cSize.GetX()+camSize && 
        fabs( (q->cPos-camPos).GetY() ) < q->cSize.GetY()+camSize)
        inside = true;

    // Check the frustum
    if( !inside ) {
        
        // Within left/right sides
        if( ProjectPoint( &g_psTrack->sFrustum[2], q->cPos ) > -q->fMaxSize && 
            ProjectPoint( &g_psTrack->sFrustum[3], q->cPos ) > -q->fMaxSize)
            inside = true;

        // Within far plane
        if( ProjectPoint( &g_psTrack->sFrustum[1], q->cPos ) < -q->fMaxSize )
            inside = false;
    }


    // If this quad is inside go through the children, or add the polys if it's a leaf
    if( inside ) {

        if( q->nLeaf ) {
            q->temp = true;
            // Add the polygons to the list
            for( int p=0; p<q->nNumPolys; p++)
                g_psTrack->pnPolyDrawList[ q->pnPolyList[p] ] = true;
        } else {

            // Go down the children
            for( int i=0; i<4; i++ )
                RE_CheckQuad( q->psChild[i], camPos );
        }
    }
}


///////////////////
// Draw a quad (recursivly)
void RE_DrawQuad(quad_t *q)
{
    float Z = 5;

    if( !q )
        return;

    float c = 1;

    // Draw the quad
    if( q->temp ) {
        State_Disable(GL_TEXTURE_2D);
        State_Disable(GL_FOG);
        State_Disable(GL_BLEND);
        State_Disable(GL_LIGHTING);
        glColor4f(c,c,c,1);

        CVec min = q->cPos - q->cSize;
        CVec max = q->cPos + q->cSize;

        glBegin(GL_LINE_STRIP);
            glVertex3f( min.GetX(), min.GetY(), Z);
            glVertex3f( max.GetX(), min.GetY(), Z);
            glVertex3f( max.GetX(), max.GetY(), Z);
            glVertex3f( min.GetX(), max.GetY(), Z);
            glVertex3f( min.GetX(), min.GetY(), Z);
        glEnd();
    }
    depth++;

    q->temp = false;

    // Go down the children
    for( int i=0; i<4; i++ )
        RE_DrawQuad(q->psChild[i]);
}