#ifndef __BSPTREE_H
#define __BSPTREE_H

#include "../Math3D/Math3D.h"
#include "../Frustum/Frustum.h"
#include "../TextureManager/TextureManager.h"

// Content types
#define	CONTENTS_SOLID			1
#define	CONTENTS_LAVA			8
#define	CONTENTS_SLIME			16
#define	CONTENTS_WATER			32
#define	CONTENTS_FOG			64
#define	CONTENTS_AREAPORTAL		0x8000

// Surface flags
#define	SURF_SKY				0x4

// Face types
#define FACE_POLYGON	1
#define FACE_PATCH		2
#define FACE_MESH		3
#define FACE_BILLBOARD	4

typedef enum
{
	LUMP_ENTITIES = 0,
	LUMP_TEXTURES,
	LUMP_PLANES,
	LUMP_NODES,
	LUMP_LEAFS,
	LUMP_LEAFFACES,
	LUMP_LEAFBRUSHES,
	LUMP_MODELS,
	LUMP_BRUSHES,
	LUMP_BRUSHSIDES,
	LUMP_VERTICES,
	LUMP_INDICES,
	LUMP_SHADERS,
	LUMP_FACES,
	LUMP_LIGHTMAPS,
	LUMP_LIGHTVOLUMES,
	LUMP_VISDATA,
	LUMP_MAX_LUMPS
} BSPLumps_t;

typedef struct BSPHeader_s
{
	char	magic[4];
	int		version;
} BSPHeader_t;

typedef struct BSPLump_s
{
	int		offset;
	int		length;
} BSPLump_t;

typedef struct BSPVertex_s
{
	Vector3D	position;
	float		textureCoord[2];
	float		lightmapCoord[2];
	Vector3D	normal;
	char		colors[4];
} BSPVertex_t;

typedef struct BSPFace_s
{
	int			texture;
	int			effect;
	int			type;
	int			startVertIndex;
	int			numVertices;
	int			startIndex;
	int			numIndices;
	int			lightmap;
	int			mapCorner[2];
	int			mapSize[2];
	Vector3D	mapPos;
	Vector3D	mapVecs[2];
	Vector3D	normal;
	int			size[2];
} BSPFace_t;

typedef struct BSPTexture_s
{
	char	name[64];
	int		surfaceFlags;
	int		textureType;
} BSPTexture_t;

typedef struct BSPLightmap_s
{
	unsigned char	image[128*128][3];
} BSPLightmap_t;

typedef struct BSPNode_s
{
	int			plane;
	int			front;
	int			back;
	Vector3D	mins;
	Vector3D	maxs;
} BSPNode_t;

typedef struct BSPLeaf_s
{
	int			cluster;
	int			area;
	Vector3D	mins;
	Vector3D	maxs;
	int			startLeafFace;
	int			numLeafFaces;
	int			startLeafBrush;
	int			numLeafBrushes;
} BSPLeaf_t;

typedef Plane BSPPlane_t;

typedef struct BSPVisData_s
{
	int		numVectors;
	int		vectorSize;
	char	*vecs;
} BSPVisData_t;

typedef struct BSPBrush_s
{
	int		startBrushSide;
	int		numBrushSides;
	int		texture;
} BSPBrush_t;

typedef struct BSPBrushSide_s
{
	int		plane;
	int		texture;
} BSPBrushSide_t;

typedef struct BSPModel_s
{
	Vector3D	mins;
	Vector3D	maxs;
	int			startFaceIndex;
	int			numFaces;
	int			startBrushIndex;
	int			numBrushes;
} BSPModel_t;

typedef struct BSPShader_s
{
	char	name[64];
	int		brushIndex;
	int		unknown;
} BSPShader_t;

/*typedef struct BSPLightVolume_s
{
	char	ambient[3];
	char	directional[3];
	char	direction[2];
} BSPLightVolume_t;*/

typedef struct BSPLightVolume_s
{
	float	ambient[4];
	float	diffuse[4];
	float	direction[4];
} BSPLightVolume_t;

class BezierFace;

class BSPTree
{
public:
	BSPTree();
	~BSPTree();

	bool	Load( char *filename );
	void	UploadLightmaps();
	void	LoadTextures();
	void	Free();

	void	Draw( Vector3D &pos, Frustum *viewFrustum );
	void	DrawTransparentFaces();
	void	DrawModel( int modelIndex );

	char	*GetEntities();
	bool	PositionVisible( Vector3D &cam, Vector3D &test );
private:
	void	CorrectVectors();
	void	ReadLump( FILE *f, BSPLumps_t lump, int *count, int stride, void **dest );
	void	LoadNodes( FILE *f );
	void	LoadLeafs( FILE *f );
	void	LoadVisData( FILE *f );
	void	LoadLightVolumes( FILE *f );

	void	CreateBezierFaces();

	int		LeafAtPosition( Vector3D &pos );
	bool	ClusterVisible( int in, int test );

	void	DrawNode( int nodeIndex, Vector3D &pos, int cluster );
	void	DrawLeaf( int leafIndex, int cluster );
	void	DrawFace( int faceIndex, bool transparent );

	void	EnableClientStates();
	void	DisableClientStates();

	BSPHeader_t			header;
	BSPLump_t			lumps[LUMP_MAX_LUMPS];

	BSPVertex_t			*vertices;
	BSPFace_t			*faces;
	BSPTexture_t		*textures;
	BSPLightmap_t		*lightmaps;
	BSPNode_t			*nodes;
	BSPLeaf_t			*leafs;
	int					*leafFaces;
	BSPPlane_t			*planes;
	BSPVisData_t		visData;
	char				*entities;
	BSPBrush_t			*brushes;
	int					*leafBrushes;
	BSPBrushSide_t		*brushSides;
	BSPModel_t			*models;
	int					*indices;
	BSPShader_t			*shaders;
	BSPLightVolume_t	*lightVolumes;

	bool				*drawnFaces;
	int					*transparentFaces;
	int					numTransparentFaces;

	Frustum				*viewFrustum;

	texHandle_t			*hLightmaps;
	texHandle_t			*hTextures;

	BezierFace			*bezierFaces;
	
	int		numEntities;
	int		numTextures;
	int		numPlanes;
	int		numNodes;
	int		numLeafs;
	int		numLeafFaces;
	int		numLeafBrushes;
	int		numModels;
	int		numBrushes;
	int		numBrushSides;
	int		numVertices;
	int		numIndices;
	int		numShaders;
	int		numFaces;
	int		numLightmaps;
	int		numLightVolumes;
	int		numVisData;

	int		lightVols[3];
};

#endif

