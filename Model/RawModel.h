#ifndef __RAWMODEL_H
#define __RAWMODEL_H

#include "Model.h"
#include "../Math3D/Math3D.h"

#define RAW_MAX_VERTICES	1024
#define	RAW_MAX_TRIANGLES	4096

typedef struct RawVertex_s
{
	Vector3D	coord;
	Vector3D	normal;
} RawVertex_t;

typedef struct RawTriangle_s
{
	int		indices[3];
} RawTriangle_t;

class RawModel : public Model
{
public:
	RawModel();

	bool	Load( char *filename );
	void	Free();

	int		GetFrameCount();
	void	Draw( int lastframe, int curframe, float mu );
	void	DrawNormals( int lastframe, int curframe, float mu );

	void	GetBoundingBox( int frame, Vector3D *mins, Vector3D *maxs );
	float	GetRadius( int frame );
private:
	int AddVertex( Vector3D v );
	void AddTriangle( int indices[3] );
	void GenerateNormals();

	RawVertex_t		vertices[RAW_MAX_VERTICES];
	RawTriangle_t	triangles[RAW_MAX_TRIANGLES];
	int				numVertices;
	int				numTriangles;
};

#endif
