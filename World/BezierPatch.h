#ifndef __BEZIERPATCH_H
#define __BEZIERPATCH_H

#include "BSPTree.h"

class BezierPatch
{
public:
	BezierPatch();
	~BezierPatch();

	void SetControls( BSPVertex_t *c[9] );
	void Tesselate( int polysize );

	void Draw();

	void Clear();
private:
	void Interpolate( BSPVertex_t *dest, float u, float v );

	BSPVertex_t		*controls[9];

	BSPVertex_t		*vertices;
	int				*indices;
	int				numIndices;

	int				subdivisions[2];
};

class BezierFace
{
public:
	BezierFace();
	~BezierFace();

	void SetFace( BSPFace_t *face );
	void SetVertices( BSPVertex_t *vertices );
	void CreatePatches( int polysize );

	void Draw();

	void Clear();
private:
	BSPFace_t		*face;
	BSPVertex_t		*vertices;

	BezierPatch		*patches;
	int				numPatches;
};

#endif
