#include "../Common.h"
#include "../OpenGL.h"
#include "RawModel.h"

RawModel::RawModel()
{
	Free();
}

bool RawModel::Load( char *filename )
{
	FILE		*f;
	char		line[256], *lineptr;
	char		num[64], *numptr;
	Vector3D	verts[3];
	float		*vertptr;
	int			indices[3];

	if ( !( f = fopen( filename, "r" ) ) )
		return false;

	while( !feof( f ) )
	{
		memset( line, 0, 256 );
		lineptr = line;
		while ( !feof( f ) && ( *lineptr++ = fgetc( f ) ) != '\n' );

		lineptr = line;

		vertptr = &verts[0].x;
		for ( int i = 0; i < 9; i++ )
		{
			memset( num, 0, 64 );
			numptr = num;

			while ( *lineptr == ' ' || *lineptr == '\t' )
				lineptr++;

			while ( 1 )
			{
				*numptr = *lineptr;
				if ( *numptr == '\0' || *numptr == ' ' || *numptr == '\t' || *numptr == '\n' )
					break;

				numptr++;
				lineptr++;
			}

			*vertptr++ = (float)atof( num );
		}

		// FIXME: Null vertices at end of file
		indices[0] = AddVertex( verts[0] );
		indices[1] = AddVertex( verts[1] );
		indices[2] = AddVertex( verts[2] );

		if ( indices[0] < 0 || indices[1] < 0 || indices[2] < 0 )
			break;

		AddTriangle( indices );
	}

	fclose( f );

	GenerateNormals();

	printf( "RawModel::Load() - Loaded %i vertices, %i triangles\n", 
		numVertices, numTriangles );

	return true;
}

void RawModel::Free()
{
	memset( vertices, 0, sizeof( vertices ) );
	memset( triangles, 0, sizeof( triangles ) );
	numVertices = 0;
	numTriangles = 0;
}

int RawModel::GetFrameCount()
{
	return 1;
}

void RawModel::Draw( int lastframe, int curframe, float mu )
{
	glVertexPointer( 3, GL_FLOAT, sizeof( RawVertex_t ), &vertices[0].coord.x );
	glEnableClientState( GL_VERTEX_ARRAY );

	glNormalPointer( GL_FLOAT, sizeof( RawVertex_t ), &vertices[0].normal.x );
	glEnableClientState( GL_NORMAL_ARRAY );

	glDrawElements( GL_TRIANGLES, numTriangles * 3, GL_UNSIGNED_INT, triangles[0].indices );
}

void RawModel::DrawNormals( int lastframe, int curframe, float mu )
{
	glDisable( GL_LIGHTING );
	glPushAttrib( GL_CURRENT_BIT );
	glColor3f( 1, 0, 0 );

	glBegin( GL_LINES );

		for ( int i = 0; i < numVertices; i++ )
		{
			Vector3D start = vertices[i].coord;
			Vector3D end = start + vertices[i].normal * 5.0f;

			glVertex3fv( &start.x );
			glVertex3fv( &end.x );
		}

	glEnd();

	glPopAttrib();
}

void RawModel::GetBoundingBox( int frame, Vector3D *mins, Vector3D *maxs )
{
	// TODO: implement
}

float RawModel::GetRadius( int frame )
{
	// TODO: implement
	return 0;
}

#define EPSILON		0.000001f

int RawModel::AddVertex( Vector3D v )
{
	for ( int i = 0; i < numVertices; i++ )
	{
		if ( v.Distance( vertices[i].coord ) <= EPSILON )
			return i;
	}

	if ( numVertices < RAW_MAX_VERTICES )
	{
		vertices[numVertices].coord = v;
		return numVertices++;
	}
	else
	{
		return -1;
	}
}

void RawModel::AddTriangle( int indices[3] )
{
	if ( numTriangles < RAW_MAX_TRIANGLES )
	{
		int *ind = triangles[numTriangles].indices;
		ind[0] = indices[0];
		ind[1] = indices[1];
		ind[2] = indices[2];

		numTriangles++;
	}
}

void RawModel::GenerateNormals()
{
	for ( int i = 0; i < numTriangles; i++ )
	{
		RawTriangle_t *tri = &triangles[i];
		RawVertex_t *verts[3];

		verts[0] = &vertices[ tri->indices[0] ];
		verts[1] = &vertices[ tri->indices[1] ];
		verts[2] = &vertices[ tri->indices[2] ];

		Vector3D a = verts[0]->coord - verts[1]->coord;
		Vector3D b = verts[2]->coord - verts[1]->coord;
		Vector3D norm = b.CrossProduct( a ).Normalize2();
		
		verts[0]->normal = verts[0]->normal + norm;
		verts[1]->normal = verts[1]->normal + norm;
		verts[2]->normal = verts[2]->normal + norm;
	}

	for ( int i = 0; i < numVertices; i++ )
		vertices[i].normal.Normalize();
}
