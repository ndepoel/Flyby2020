#ifndef __MD3MODEL_H
#define __MD3MODEL_H

#include "Model.h"
#include "../Math3D/Math3D.h"

typedef struct MD3Header_s
{
	char	magic[4];
	int		version;
	char	name[64];
	int		flags;
	int		numFrames;
	int		numTags;
	int		numMeshes;
	int		numSkins;
	int		offsetFrames;
	int		offsetTags;
	int		offsetMeshes;
	int		filesize;
} MD3Header_t;

typedef struct MD3Frame_s
{
	float	mins[3];
	float	maxs[3];
	float	origin[3];
	float	radius;
	char	name[16];
} MD3Frame_t;

typedef struct MD3Tag_s
{
	char	name[64];
	float	origin[3];
	float	axis[3][3];
} MD3Tag_t;

typedef struct MD3Shader_s
{
	char	name[64];
	int		index;
} MD3Shader_t;

typedef struct MD3Triangle_s
{
	int		indices[3];
} MD3Triangle_t;

typedef struct MD3TexCoord_s
{
	float	uv[2];
} MD3TexCoord_t;

typedef struct MD3Vertex_s
{
	Vector3D	coord;
	Vector3D	normal;
} MD3Vertex_t;

typedef struct MD3MeshHeader_s
{
	char	magic[4];
	char	name[64];
	int		flags;
	int		numFrames;
	int		numShaders;
	int		numVertices;
	int		numTriangles;
	int		offsetTriangles;
	int		offsetShaders;
	int		offsetTexCoords;
	int		offsetVertices;
	int		length;
} MD3MeshHeader_t;

typedef struct MD3Mesh_s
{
	MD3MeshHeader_t	header;
	MD3Shader_t		*shaders;
	MD3Triangle_t	*triangles;
	MD3TexCoord_t	*texCoords;
	MD3Vertex_t		*vertices;

	Vector3D		*lerpVerts;
	Vector3D		*lerpNorms;
} MD3Mesh_t;

class MD3Model: public Model
{
public:
	MD3Model();
	~MD3Model();

	bool Load( char *filename );
	void Free();
	
	int GetFrameCount();
	void Draw( int lastframe, int curframe, float mu );
	void DrawNormals( int lastframe, int curframe, float mu );

	void GetBoundingBox( int frame, Vector3D *mins, Vector3D *maxs );
	float GetRadius( int frame );
private:
	MD3Header_t		header;
	MD3Frame_t		*frames;
	MD3Tag_t		*tags;
	MD3Mesh_t		*meshes;
};

#endif

