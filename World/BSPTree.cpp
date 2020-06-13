#include "../Common.h"
#include "BSPTree.h"

BSPTree::BSPTree()
{
	vertices = NULL;
	faces = NULL;
	textures = NULL;
	lightmaps = NULL;
	nodes = NULL;
	leafs = NULL;
	leafFaces = NULL;
	planes = NULL;
	visData.vecs = NULL;
	entities = NULL;
	brushes = NULL;
	leafBrushes = NULL;
	brushSides = NULL;
	models = NULL;
	indices = NULL;
	shaders = NULL;
	lightVolumes = NULL;

	drawnFaces = NULL;
	transparentFaces = NULL;
	hLightmaps = NULL;
	hTextures = NULL;

	bezierFaces = NULL;

	numTransparentFaces = 0;
}

BSPTree::~BSPTree()
{
	Free();
}

char *BSPTree::GetEntities()
{
	return entities;
}
