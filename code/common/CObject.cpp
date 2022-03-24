/////////////////////////////////////////
//
//   Auxiliary Software class library
//
/////////////////////////////////////////


// Object class
// Created 28/9/01
// By Jason Boettcher


#include "defs.h"
#include "3D.h"


///////////////////
// Initialize the object
int CObject::Initialize(int _verts, int _polys)
{
	NumVertices = _verts;
	NumPolygons = _polys;

	// Allocate the vertices
	Vertices = new vertex_t[NumVertices];
	if(Vertices == NULL)
		return false;

	// Allocate the polygons
	Polygons = new polygon_t[NumPolygons];
	if(Polygons == NULL)
		return false;

	return true;
}


///////////////////
// Calculate vertex normals
void CObject::CalculateVertexNormals(void)
{
	uint v;
	uint p;
	int i;
	
	CVec sum;
	int count;

	CVec Threshold = CVec(0.7f, 0.7f, 0.7f);


	// Loop through each vertex
	for(v=0;v<NumVertices;v++) {

		sum = CVec(0,0,0);
		count = 0;

		// Loop through each poly
		for(p=0;p<NumPolygons;p++) {


			// go through the poly's 3 vertices
			for(i=0;i<3;i++) {

				if(Polygons[p].iV[i] == v) {
					// Add the face normal for the average

					// Check if it's within the threshold
					//if(VecAbs(sum) + VecAbs(Polygons[p].GetPlane()->GetNormal()) < Threshold || count == 0) {
						sum = sum + Polygons[p].tPlane.vNormal;
						count++;
					//}
				}
			}
		}

		sum = sum / (float)count;
		//VectorNormalize(&sum);
		Vertices[v].vNormal = sum;
	}
}

//extern int PolyCount;

///////////////////
// Render the object
void CObject::Render()
{
	if(iFlags & OBJ_HIDDEN)
		return;

	if(iFlags & OBJ_SELECTED) {
		DrawSelected();
		return;
	}

	if(iFlags & OBJ_SEETHROUGH) {
		DrawSeeThrough();
		return;
	}

	if(!cMaterial)
		return;

	for(int p=0;p<cMaterial->getNumPasses();p++) {

		//if(cMaterial->hasLightmap(p))
		//	continue;

		cMaterial->setupPass(p);
		
		RenderRaw(cMaterial->hasLightmap(p));
	}
}


///////////////////
// SR3: Draw a selected version of the object
void CObject::DrawSelected(void)
{
	// Manually set the material properties
	State_Disable(GL_TEXTURE_2D);
	State_Enable(GL_BLEND);
	glBlendFunc(GL_SRC_COLOR,GL_ONE_MINUS_SRC_COLOR);
	
	glColor4f(1,1,1,0.75f);

	RenderRaw(false);

	removeFlag(OBJ_SELECTED);
}


///////////////////
// SR3: Draw the object transparently
void CObject::DrawSeeThrough(void)
{
	// Manually set the material properties
	State_Disable(GL_TEXTURE_2D);
	State_Enable(GL_BLEND);
	glBlendFunc(GL_SRC_COLOR,GL_ONE_MINUS_SRC_COLOR);
	
	glColor4f(1,1,1,0.5f);

	RenderRaw(false);
}


///////////////////
// Render the raw object
void CObject::RenderRaw(int lightmap)
{
	polygon_t	*poly = Polygons;
	uint		p;
	int			i;
	CVec		v,cam;

	glBegin(GL_TRIANGLES);

	// Render all the polygons
	for(p=0;p<NumPolygons;p++,poly++) {

		//glNormal3f(poly->vNorm[0].GetX(), poly->vNorm[0].GetY(), poly->vNorm[0].GetZ());

		// Triangle
		for(i=0;i<3;i++) {
			v = Vertices[poly->iV[i]].vPos;
			
			if(lightmap)
				glTexCoord2f(poly->tLightcoord[i].U, poly->tLightcoord[i].V);
            else {
                glNormal3f(poly->vNorm[i+1].GetX(), poly->vNorm[i+1].GetY(), poly->vNorm[i+1].GetZ());
				glTexCoord2f(poly->tTexcoord[i].U, -poly->tTexcoord[i].V);
            }
			glVertex3f(v.GetX(), v.GetY(), v.GetZ());
		}
	}

	glEnd();
}

///////////////////
// Shutdown the object
void CObject::Shutdown(void)
{
	// Free the vertices
	if(Vertices)
		delete[] Vertices;
	Vertices = NULL;


	// Free the polygons
	if(Polygons)
		delete[] Polygons;
	Polygons = NULL;
}