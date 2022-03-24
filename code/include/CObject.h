/////////////////////////////////////////
//
//   Auxiliary Software class library
//
/////////////////////////////////////////


// Object class
// Created 28/9/01
// By Jason Boettcher


#ifndef __COBJECT_H__
#define __COBJECT_H__


// Object flags
#define		OBJ_HIDDEN		0x0001
#define		OBJ_SELECTED	0x0002
#define		OBJ_SMOOTH		0x0004
#define		OBJ_SEETHROUGH	0x0008


class CObject {
public:
	// Constructor
	CObject() {
		NumVertices = 0;
		NumPolygons = 0;
		
		Vertices = NULL;
		Polygons = NULL;

		iFlags = 0;
	}


private:
	// Attributes

	char		Name[64];
	char		Group[64];
	CVec		Position;
	CVec		Angle;

	uint		NumVertices;
	uint		NumPolygons;

	vertex_t	*Vertices;
	polygon_t	*Polygons;

	CMaterial	*cMaterial;

	int			iFlags;


public:
	// Methods


	// General
	int			Initialize(int _verts, int _polys);
	void		Shutdown(void);

	void		Render(void);
	void		RenderRaw(int lightmap);
	void		DrawSelected(void);
	void		DrawSeeThrough(void);

	void		CalculateVertexNormals(void);


	// Position
	CVec		GetPosition(void)		{ return Position; }
	void		SetPosition(CVec _pos)	{ Position = _pos; }
	

	// Vertices
	int			GetNumVertices(void)	{ return NumVertices; }
	vertex_t	*GetVertices(void)		{ return Vertices; }
	vertex_t	*GetVertex(int i)		{ return &Vertices[i]; }
	

	// Polygons
	int			GetNumPolygons(void)	{ return NumPolygons; }
	polygon_t	*GetPolygons(void)		{ return Polygons; }

	
	// Other
	char		*GetName(void)			{ return Name; }
	char		*GetGroupName(void)		{ return Group; }

	void		SetMaterial(CMaterial *mat)	{ cMaterial = mat; }
	CMaterial	*getMaterial(void)			{ return cMaterial; }

	// Flags
	void		addFlag(int _f)			{ iFlags |= _f; }
	void		removeFlag(int _f)		{ iFlags &= ~_f; }
};







#endif  //  __COBJECT_H__
