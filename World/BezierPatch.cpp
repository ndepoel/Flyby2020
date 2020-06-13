#include "../Common.h"
#include "BezierPatch.h"
#include "../OpenGL.h"

BezierPatch::BezierPatch()
{
	for ( int i = 0; i < 9; i++ )
		controls[i] = NULL;

	vertices = NULL;
	indices = NULL;
	numIndices = 0;
	subdivisions[0] = subdivisions[1] = 0;
}

BezierPatch::~BezierPatch()
{
	Clear();
}

void BezierPatch::Clear()
{
	if ( vertices )
	{
		delete[] vertices;
		vertices = NULL;
	}

	if ( indices )
	{
		delete[] indices;
		indices = NULL;
	}

	numIndices = 0;
	subdivisions[0] = subdivisions[1] = 0;
}

void BezierPatch::SetControls( BSPVertex_t *c[9] )
{
	for ( int i = 0; i < 9; i++ )
		controls[i] = c[i];
}

void BezierPatch::Tesselate( int polysize )
{
	if ( !controls[0] )
		return;

	subdivisions[0] = subdivisions[1] = polysize;

	int numVerts = (subdivisions[0] + 1) * (subdivisions[1] + 1);
	numIndices = subdivisions[0] * (subdivisions[1] + 1) * 2;

	vertices = new BSPVertex_t[ numVerts ];
	indices = new int[ numIndices ];

	int n = 0, row, col;
	for ( row = 0; row < (subdivisions[0] + 1); row++ )
	{
		float u = (float)row / (float)subdivisions[0];
		for ( col = 0; col < (subdivisions[1] + 1); col++ )
		{
			float v = (float)col / (float)subdivisions[1];
			Interpolate( &vertices[n], u, v );
			n++;
		}
	}

	n = 0;
	for ( row = 0; row < subdivisions[0]; row++ )
	{
		for ( col = 0; col < (subdivisions[1] + 1); col++ )
		{
			indices[n] = row * (subdivisions[1] + 1) + col;
			indices[n+1] = (row + 1) * (subdivisions[1] + 1) + col;
			n += 2;
		}
	}
}

void BezierPatch::Interpolate( BSPVertex_t *dest, float u, float v )
{
	float	bu[3], bv[3];
	float	tmp;

	dest->position.Set( 0, 0, 0 );
	dest->normal.Set( 0, 0, 0 );
	dest->textureCoord[0] = dest->textureCoord[1] = 0;
	dest->lightmapCoord[0] = dest->lightmapCoord[1] = 0;
	float colors[4] = { 0, 0, 0, 0 };

	tmp = ( 1 - u );
	bu[0] = tmp * tmp;
	bu[1] = 2 * u * tmp;
	bu[2] = u * u;

	tmp = ( 1 - v );
	bv[0] = tmp * tmp;
	bv[1] = 2 * v * tmp;
	bv[2] = v * v;

	int n = 0;

	for ( int i = 0; i < 3; i++ )
	{
		for ( int j = 0; j < 3; j++ )
		{
			float b = bu[i] * bv[j];
			n = i * 3 + j;
			dest->position = dest->position + controls[n]->position * b;
			dest->normal = dest->normal + controls[n]->normal * b;
			dest->textureCoord[0] = dest->textureCoord[0] + controls[n]->textureCoord[0] * b;
			dest->textureCoord[1] = dest->textureCoord[1] + controls[n]->textureCoord[1] * b;
			dest->lightmapCoord[0] = dest->lightmapCoord[0] + controls[n]->lightmapCoord[0] * b;
			dest->lightmapCoord[1] = dest->lightmapCoord[1] + controls[n]->lightmapCoord[1] * b;
			colors[0] = colors[0] + (float)controls[n]->colors[0] * b;
			colors[1] = colors[1] + (float)controls[n]->colors[1] * b;
			colors[2] = colors[2] + (float)controls[n]->colors[2] * b;
			colors[3] = colors[3] + (float)controls[n]->colors[3] * b;
		}
	}

	for ( int k = 0; k < 4; k++ )
		dest->colors[k] = (char)colors[k];
}

void BezierPatch::Draw()
{
	static const int stride = sizeof( BSPVertex_t );

	if ( !vertices || !indices )
		return;

	glClientActiveTexture( GL_TEXTURE0_ARB );
	glTexCoordPointer( 2, GL_FLOAT, stride, vertices[0].textureCoord );

	glClientActiveTexture( GL_TEXTURE1_ARB );
	glTexCoordPointer( 2, GL_FLOAT, stride, vertices[0].lightmapCoord );

	glVertexPointer( 3, GL_FLOAT, stride, &vertices[0].position.x );
	glNormalPointer( GL_FLOAT, stride, &vertices[0].normal.x );

	for ( int i = 0; i < subdivisions[0]; i++ )
		glDrawElements(	GL_TRIANGLE_STRIP, (subdivisions[1] + 1) * 2, 
						GL_UNSIGNED_INT, &indices[ i * (subdivisions[1] + 1) * 2 ] );
}

