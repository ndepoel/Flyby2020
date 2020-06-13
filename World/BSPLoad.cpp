#include "../Common.h"
#include "BSPTree.h"
#include "BezierPatch.h"
#include "../SharedVars.h"

static struct
{
	int		plane;
	int		front;
	int		back;
	int		mins[3];
	int		maxs[3];
} tempNode;

static struct
{
	int		cluster;
	int		area;
	int		mins[3];
	int		maxs[3];
	int		startLeafFace;
	int		numLeafFaces;
	int		startLeafBrush;
	int		numLeafBrushes;
} tempLeaf;

static struct
{
	char	ambient[3];
	char	directional[3];
	char	direction[2];
} tempLightVolume;

void BSPTree::UploadLightmaps()
{
	int shift = 2;

	for ( int i = 0; i < numLightmaps; i++ )
	{
		// Increase lightmap brightness by saturating colors that have so-called overbright bits
		for ( int j = 0; j < 128 * 128; j++ )
		{
			unsigned char *texel = lightmaps[i].image[j];

			unsigned int r, g, b;
			r = ((unsigned int)texel[0]) << shift;
			g = ((unsigned int)texel[1]) << shift;
			b = ((unsigned int)texel[2]) << shift;

			// Normalize by color channel instead of saturating to white
			if ((r | g | b) > 255)
			{
				unsigned int max = r > g ? r : g;
				max = max > b ? max : b;
				r = r * 255 / max;
				g = g * 255 / max;
				b = b * 255 / max;
			}

			texel[0] = (unsigned char)r;
			texel[1] = (unsigned char)g;
			texel[2] = (unsigned char)b;
		}
		hLightmaps[i] = TextureManager::Instance()->UploadTexture( (unsigned *)lightmaps[i].image, 128, 128, 24 );
	}
}

void BSPTree::LoadTextures()
{
	for ( int i = 0; i < numTextures; i++ )
		hTextures[i] = TextureManager::Instance()->LoadImage( textures[i].name );
}

void BSPTree::CorrectVectors()
{
	int		i;
	float	tmp;

	for ( i = 0; i < numVertices; i++ )
	{
		vertices[i].position.QtoOGL();
		vertices[i].normal.QtoOGL();
	}

	for ( i = 0; i < numFaces; i++ )
	{
		faces[i].normal.QtoOGL();
		faces[i].mapPos.QtoOGL();
		faces[i].mapVecs[0].QtoOGL();
		faces[i].mapVecs[1].QtoOGL();
	}

	for ( i = 0; i < numNodes; i++ )
	{
		BSPNode_t *node = &nodes[i];

		node->mins.QtoOGL();
		node->maxs.QtoOGL();

		tmp = node->maxs.z;
		node->maxs.z = node->mins.z;
		node->mins.z = tmp;
	}

	for ( i = 0; i < numLeafs; i++ )
	{
		BSPLeaf_t *leaf = &leafs[i];

		leaf->mins.QtoOGL();
		leaf->maxs.QtoOGL();

		tmp = leaf->maxs.z;
		leaf->maxs.z = leaf->mins.z;
		leaf->mins.z = tmp;
	}

	for ( i = 0; i < numPlanes; i++ )
	{
		planes[i].normal.QtoOGL();
	}

	for ( i = 0; i < numModels; i++ )
	{
		BSPModel_t *model = &models[i];

		model->mins.QtoOGL();
		model->maxs.QtoOGL();

		tmp = model->maxs.z;
		model->maxs.z = model->mins.z;
		model->mins.z = tmp;
	}
}

void BSPTree::ReadLump( FILE *f, BSPLumps_t lump, int *count, int stride, void **dest )
{
	BSPLump_t *lmp = &lumps[lump];
	*count = lmp->length / stride;
	*dest = new char[ *count * stride ];
	fseek( f, lmp->offset, SEEK_SET );
	fread( *dest, lmp->length, 1, f );
}

void BSPTree::LoadNodes( FILE *f )
{
	BSPLump_t *lump = &lumps[LUMP_NODES];
	numNodes = lump->length / sizeof( tempNode );
	nodes = new BSPNode_t[ numNodes ];

	fseek( f, lump->offset, SEEK_SET );
	for ( int i = 0; i < numNodes; i++ )
	{
		fread( &tempNode, sizeof( tempNode ), 1, f );

		BSPNode_t *node = &nodes[i];
		node->plane = tempNode.plane;
		node->front = tempNode.front;
		node->back = tempNode.back;
		node->mins = Vector3D( tempNode.mins );
		node->maxs = Vector3D( tempNode.maxs );
	}
}

void BSPTree::LoadLeafs( FILE *f )
{
	BSPLump_t *lump = &lumps[LUMP_LEAFS];
	numLeafs = lump->length / sizeof( tempLeaf );
	leafs = new BSPLeaf_t[ numLeafs ];

	fseek( f, lump->offset, SEEK_SET );
	for ( int i = 0; i < numLeafs; i++ )
	{
		fread( &tempLeaf, sizeof( tempLeaf ), 1, f );

		BSPLeaf_t *leaf = &leafs[i];
		leaf->cluster = tempLeaf.cluster;
		leaf->area = tempLeaf.area;
		leaf->mins = Vector3D( tempLeaf.mins );
		leaf->maxs = Vector3D( tempLeaf.maxs );
		leaf->startLeafFace = tempLeaf.startLeafFace;
		leaf->numLeafFaces = tempLeaf.numLeafFaces;
		leaf->startLeafBrush = tempLeaf.startLeafBrush;
		leaf->numLeafBrushes = tempLeaf.numLeafBrushes;
	}
}

void BSPTree::LoadVisData( FILE *f )
{
	fseek( f, lumps[LUMP_VISDATA].offset, SEEK_SET );
	fread( &visData.numVectors, sizeof( int ), 1, f );
	fread( &visData.vectorSize, sizeof( int ), 1, f );
	numVisData = visData.numVectors * visData.vectorSize * sizeof( char );
	visData.vecs = new char[ visData.numVectors * visData.vectorSize ];
	fread( visData.vecs, numVisData, 1, f );
}

void BSPTree::LoadLightVolumes( FILE *f )
{
	BSPLump_t *lump = &lumps[LUMP_LIGHTVOLUMES];
	numLightVolumes = lump->length / sizeof( tempLightVolume );
	lightVolumes = new BSPLightVolume_t[ numLightVolumes ];

	fseek( f, lump->offset, SEEK_SET );
	for ( int i = 0; i < numLightVolumes; i++ )
	{
		fread( &tempLightVolume, sizeof( tempLightVolume ), 1, f );

		BSPLightVolume_t *lv = &lightVolumes[i];
		lv->ambient[0] = (float)tempLightVolume.ambient[0] / 255.0f;
		lv->ambient[1] = (float)tempLightVolume.ambient[1] / 255.0f;
		lv->ambient[2] = (float)tempLightVolume.ambient[2] / 255.0f;
		lv->ambient[3] = 1.0f;

		lv->diffuse[0] = (float)tempLightVolume.directional[0] / 255.0f;
		lv->diffuse[1] = (float)tempLightVolume.directional[1] / 255.0f;
		lv->diffuse[2] = (float)tempLightVolume.directional[2] / 255.0f;
		lv->diffuse[3] = 1.0f;

		float lat = ( (float)( tempLightVolume.direction[0] ) * 6.2831853f ) / 255.0f;
		float lng = ( (float)( tempLightVolume.direction[1] ) * 6.2831853f ) / 255.0f;
		Vector3D::LatLngToVector( lat, lng ).GetFloatArray( lv->direction );
		lv->direction[3] = 0.0f;	// Directional light
	}
}

bool BSPTree::Load( char *filename )
{
	FILE		*f;

	if ( !( f = fopen( filename, "rb" ) ) )
	{
		printf( "BSPTree::Load() - Could not open file '%s'\n", filename );
		return false;
	}

	// Read header
	fread( &header, sizeof( BSPHeader_t ), 1, f );
	if (	header.magic[0] != 'I' || header.magic[1] != 'B' ||
			header.magic[2] != 'S' || header.magic[3] != 'P' ||
			header.version != 0x2E )
	{
		printf( "BSPTree::Load() - Invalid header\n" );
		fclose( f );
		return false;
	}

	// Read lump information
	fread( lumps, sizeof( BSPLump_t ), LUMP_MAX_LUMPS, f );

	// Read lumps
	ReadLump( f, LUMP_VERTICES, &numVertices, sizeof( BSPVertex_t ), (void **)&vertices );
	ReadLump( f, LUMP_FACES, &numFaces, sizeof( BSPFace_t ), (void **)&faces );
	ReadLump( f, LUMP_TEXTURES, &numTextures, sizeof( BSPTexture_t ), (void **)&textures );
	ReadLump( f, LUMP_LIGHTMAPS, &numLightmaps, sizeof( BSPLightmap_t ), (void **)&lightmaps );
	ReadLump( f, LUMP_LEAFFACES, &numLeafFaces, sizeof( int ), (void **)&leafFaces );
	ReadLump( f, LUMP_PLANES, &numPlanes, sizeof( BSPPlane_t ), (void **)&planes );
	ReadLump( f, LUMP_ENTITIES, &numEntities, sizeof( char ), (void **)&entities );
	ReadLump( f, LUMP_BRUSHES, &numBrushes, sizeof( BSPBrush_t ), (void **)&brushes );
	ReadLump( f, LUMP_LEAFBRUSHES, &numLeafBrushes, sizeof( int ), (void **)&leafBrushes );
	ReadLump( f, LUMP_BRUSHSIDES, &numBrushSides, sizeof( BSPBrushSide_t ), (void **)&brushSides );
	ReadLump( f, LUMP_MODELS, &numModels, sizeof( BSPModel_t ), (void **)&models );
	ReadLump( f, LUMP_INDICES, &numIndices, sizeof( int ), (void **)&indices );
	ReadLump( f, LUMP_SHADERS, &numShaders, sizeof( BSPShader_t ), (void **)&shaders );
	LoadNodes( f );
	LoadLeafs( f );
	LoadVisData( f );
	LoadLightVolumes( f );

	fclose( f );

	// Convert vectors from Quake's coordinate system to OpenGL's
	CorrectVectors();

	// Calculate number of light volumes in each direction
	lightVols[0] = (int)floor( models[0].maxs.x / 64.0f ) - (int)ceil( models[0].mins.x / 64.0f ) + 1;
	lightVols[1] = (int)floor( models[0].maxs.y / 128.0f ) - (int)ceil( models[0].mins.y / 128.0f ) + 1;
	lightVols[2] = (int)floor( models[0].maxs.z / 64.0f ) - (int)ceil( models[0].mins.z / 64.0f ) + 1;	

	// Create lists for drawn faces and transparent faces
	drawnFaces = new bool[ numFaces ];
	transparentFaces = new int[ numFaces ];
	numTransparentFaces = 0;

	// Create texture handles for lightmaps and texture maps
	hLightmaps = new texHandle_t[ numLightmaps ];
	hTextures = new texHandle_t[ numTextures ];

	// Generate curved surfaces
	CreateBezierFaces();

	// Write entities list to file
	/*f = fopen( "entities.txt", "w" );
	fwrite( entities, numEntities * sizeof( char ), 1, f );
	fclose( f );*/

	printf( "BSPTree::Load() - Succesfully loaded '%s'\n", filename );

	return true;
}

void BSPTree::Free()
{
	if ( vertices )
	{
		delete[] vertices;
		vertices = NULL;
	}
	if ( faces )
	{
		delete[] faces;
		faces = NULL;
	}
	if ( textures )
	{
		delete[] textures;
		textures = NULL;
	}
	if ( lightmaps )
	{
		delete[] lightmaps;
		lightmaps = NULL;
	}
	if ( nodes )
	{
		delete[] nodes;
		nodes = NULL;
	}
	if ( leafs )
	{
		delete[] leafs;
		leafs = NULL;
	}
	if ( leafFaces )
	{
		delete[] leafFaces;
		leafFaces = NULL;
	}
	if ( planes )
	{
		delete[] planes;
		planes = NULL;
	}
	if ( visData.vecs )
	{
		delete[] visData.vecs;
		visData.vecs = NULL;
	}
	if ( entities )
	{
		delete[] entities;
		entities = NULL;
	}
	if ( brushes )
	{
		delete[] brushes;
		brushes = NULL;
	}
	if ( leafBrushes )
	{
		delete[] leafBrushes;
		leafBrushes = NULL;
	}
	if ( brushSides )
	{
		delete[] brushSides;
		brushSides = NULL;
	}
	if ( models )
	{
		delete[] models;
		models = NULL;
	}
	if ( indices )
	{
		delete[] indices;
		indices = NULL;
	}
	if ( shaders )
	{
		delete[] shaders;
		shaders = NULL;
	}
	if ( lightVolumes )
	{
		delete[] lightVolumes;
		lightVolumes = NULL;
	}

	if ( drawnFaces )
	{
		delete[] drawnFaces;
		drawnFaces = NULL;
	}
	if ( transparentFaces )
	{
		delete[] transparentFaces;
		transparentFaces = NULL;
	}
	if ( hLightmaps )
	{
		delete[] hLightmaps;
		hLightmaps = NULL;
	}
	if ( hTextures )
	{
		delete[] hTextures;
		hTextures = NULL;
	}

	if ( bezierFaces )
	{
		delete[] bezierFaces;
		bezierFaces = NULL;
	}

	printf( "BSPTree::Free() - Freed BSP structure\n" );
}

void BSPTree::CreateBezierFaces()
{
	int i, count = 0;

	for ( i = 0; i < numFaces; i++ )
		if ( faces[i].type == FACE_PATCH )
			count++;

	bezierFaces = new BezierFace[count];
	int bface = 0;

	for ( i = 0; i < numFaces; i++ )
	{
		if ( faces[i].type == FACE_PATCH )
		{
			bezierFaces[bface].SetFace( &faces[i] );
			bezierFaces[bface].SetVertices( vertices );
			bezierFaces[bface].CreatePatches( bsp_subdivisions );
			faces[i].startIndex = bface;
			bface++;
		}
	}
}

