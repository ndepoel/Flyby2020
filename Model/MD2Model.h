#ifndef __MD2MODEL_H
#define __MD2MODEL_H

#include "Model.h"
#include "../Math3D/Math3D.h"

typedef struct MD2Header_s
{
	int		magic;
	int		version;
	int		skinWidth;
	int		skinHeight;
	int		frameSize;
	int		numSkins;
	int		numVertices;
	int		numTexCoords;
	int		numTriangles;
	int		numGLCommands;
	int		numFrames;
	int		offsetSkins;
	int		offsetTexCoords;
	int		offsetTriangles;
	int		offsetFrames;
	int		offsetGlCommands;
	int		offsetEnd;
} MD2Header_t;

typedef struct MD2Skin_s
{
	unsigned char	name[64];
} MD2Skin_t;

typedef struct MD2Vertex_s
{
	unsigned char	vertex[3];
	unsigned char	normalIndex;
} MD2Vertex_t;

typedef struct MD2Frame_s
{
	float		scale[3];
	float		translate[3];
	char		name[16];
	MD2Vertex_t *md2verts;
	Vector3D	*vertices;
	Vector3D	*normals;
} MD2Frame_t;

typedef struct MD2Triangle_s
{
	short	vertexIndices[3];
	short	textureIndices[3];
} MD2Triangle_t;

typedef struct MD2TexCoord_s
{
	short	s, t;
} MD2TexCoord_t;

typedef struct MD2GLCommand_s
{
	float	s, t;
	int		vertexIndex;
} MD2GLCommand_t;

class MD2Model : public Model
{
public:
	MD2Model();
	MD2Model( char *filename );
	~MD2Model();

	bool	Load( char *filename );
	void	Free();
	
	int		GetFrameCount();
	void	Draw( int lastframe, int curframe, float mu );
	void	DrawNormals( int lastframe, int curframe, float mu );
	
	void	GetBoundingBox( int frame, Vector3D *mins, Vector3D *maxs );
	float	GetRadius( int frame );
private:
	MD2Header_t		header;
	MD2Skin_t		*skins;
	MD2Frame_t		*frames;
	MD2TexCoord_t	*texcoords;
	MD2Triangle_t	*triangles;
	int				*glCommands;

	Vector3D		*lerpVerts;
	Vector3D		*lerpNorms;
};

#endif
