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


#ifndef __RACINGENGINE_H__
#define __RACINGENGINE_H__


// Track Polygon
/*typedef struct {

    uint		nV[3];
	texcoord_t	sTexcoord[3];
	CVec		cNorm[4];
	plane_t		sPlane;
    CMaterial   *pcMat;

} tpoly_t;*/



// A Quad structure
/*typedef struct quad_s {

	int			nLeaf;
	CVec		cPos;
	CVec		cSize;
    float       fMaxSize;
    float       fMinSize;

	// Polys
	int			nNumPolys;
	int	        *pnPolyList;

    int         temp;

	// Linking
	struct quad_s	*psChild[4];

} quad_t;*/



// Racing engine track array
/*typedef struct {

    CModel      *pcModel;

    int         nNumVertices;
    int         nNumPolygons;
    CVec        *psVertexList;
    tpoly_t     *psPolyList;
    int         *pnPolyDrawList;

    quad_t      *psQuadTree;

    plane_t     sFrustum[4];

} re_track_t;



// Quadtree
bool    RE_LoadTrack(char *szFilename);
void    RE_DrawTrack(CCamera *pcCam);

void    RE_CalculateQuadTree(void);
quad_t *RE_CalculateQuad(CVec center, CVec size);
void    RE_CalculateQuadLeaf(quad_t *q, CVec center, CVec size, int nNumPolys);
void    RE_CheckQuad(quad_t *q, CVec camPos);
void    RE_DrawQuad(quad_t *q);*/

#endif  //  __RACINGENGINE_H__