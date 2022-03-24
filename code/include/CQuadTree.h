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


#ifndef __CQUADTREE_H__
#define __CQUADTREE_H__


#define         QUAD_VERSION        1


// Quad structure
typedef struct quad_s {

	bool        bLeaf;
	CVec		cPos;
	CVec		cSize;
    float       fMaxSize;
    float       fMinSize;
    float       fTestSize;

	// Polys
	int			nNumPolys;
	polygon_t	**psPolyList;

	// Linking
	struct quad_s	*psChild[4];

} quad_t;



// QuadTree class
class CQuadTree {
private:
    // Attributes

    int         m_nNumPolygons;
    polygon_t   **m_psPolyDrawList;
    quad_t      *m_psQuadTree;
    CModel      *m_pcScene;

    // Quadtree rules
    int         m_nMinPolygons;
    int         m_nMinSize;

    // Processing
    plane_t     m_sFrustum[4];

    // Quad write types
    enum {
        SQ_QUAD,
        SQ_LEAF,
        SQ_END
    };


public:
    // Methods

    CQuadTree();

    bool        load(char *szName, CModel *pcScene);
    bool        save(char *szName, CModel *pcScene);
    
    bool        create(CModel *pcScene);
    void        shutdown(void);

    void        process(CCamera *pcCam);


private:
    // Private Methods

    void        saveQuad(quad_t *psQuad, FILE *fp);
    quad_t      *loadQuad(FILE *fp, CModel *pcScene);
   
    void        shutdownQuad(quad_t *psQuad);

    void        checkQuad(quad_t *psQuad, CVec cCamPos);

    void        createQuadTree(CModel *pcScene);
    quad_t      *calculateQuad(CModel *pcScene, CVec cCenter, CVec cSize);
    void        calculateQuadLeaf(CModel *pcScene, quad_t *psQuad, int nNumPolys);

    bool        polyInQuad(CModel *pcScene, polygon_t *psPoly, quad_t *psQuad);




};







#endif  //  __CQUADTREE_H__