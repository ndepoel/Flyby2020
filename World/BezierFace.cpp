#include "../Common.h"
#include "BezierPatch.h"

BezierFace::BezierFace()
{
	face = NULL;
	vertices = NULL;

	patches = NULL;
	numPatches = 0;
}

BezierFace::~BezierFace()
{
	Clear();
}

void BezierFace::Clear()
{
	if ( patches )
	{
		delete[] patches;
		patches = NULL;
	}

	numPatches = 0;
}

void BezierFace::SetFace( BSPFace_t *face )
{
	this->face = face;
}

void BezierFace::SetVertices( BSPVertex_t *vertices )
{
	this->vertices = vertices;
}

void BezierFace::CreatePatches( int polysize )
{
	if ( !face || !vertices )
		return;

	int patchCount[2];
	patchCount[0] = (face->size[0] - 1) / 2;
	patchCount[1] = (face->size[1] - 1) / 2;
	numPatches = patchCount[0] * patchCount[1];

	patches = new BezierPatch[numPatches];
	int patchNum = 0;

	for ( int i = 0; i < patchCount[1]; i++ )
	{
		for ( int j = 0; j < patchCount[0]; j++ )
		{
			int startIndex = face->startVertIndex + 2 * i * face->size[0] + 2 * j;

			BSPVertex_t *controls[9];
			for ( int k = 0; k < 3; k++ )
			{
				for ( int l = 0; l < 3; l++ )
				{
					int index = k * face->size[0] + l;
					controls[k*3+l] = &vertices[startIndex + index];
				}
			}

			patches[patchNum].SetControls( controls );
			patches[patchNum].Tesselate( polysize );
			patchNum++;
		}
	}
}

void BezierFace::Draw()
{
	for ( int i = 0; i < numPatches; i++ )
		patches[i].Draw();
}

