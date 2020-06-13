#include "../Common.h"
#include "MD3Model.h"
#include "../Math3D/Interpolate.h"
#include "../OpenGL.h"

#define MD3_SCALE	(1.0f / 64.0f)

static struct
{
	short	coord[3];
	short	normal;
} tempVert;

static Vector3D LatLngToVec( short latlng )
{
	float lat = ( (float)( latlng & 0xFF ) * 6.2831853f ) / 255.0f;
	float lng = ( (float)( ( latlng >> 8 ) & 0xFF ) * 6.2831853f ) / 255.0f;

	return Vector3D::LatLngToVector( lat, lng );
}

MD3Model::MD3Model()
{
	frames = NULL;
	tags = NULL;
	meshes = NULL;
}

MD3Model::~MD3Model()
{
	Free();
}

bool MD3Model::Load( char *filename )
{
	FILE *f;

	if ( !( f = fopen( filename, "rb" ) ) )
	{
		return false;
	}

	fread( &header, sizeof( MD3Header_t ), 1, f );
	if (	header.magic[0] != 'I' || header.magic[1] != 'D' ||
			header.magic[2] != 'P' || header.magic[3] != '3' ||
			header.version != 15 )
	{
		fclose( f );
		return false;
	}

	frames = new MD3Frame_t[ header.numFrames ];
	tags = new MD3Tag_t[ header.numTags * header.numFrames ];
	meshes = new MD3Mesh_t[ header.numMeshes ];

	fseek( f, header.offsetFrames, SEEK_SET );
	fread( frames, sizeof( MD3Frame_t ), header.numFrames, f );

	fseek( f, header.offsetTags, SEEK_SET );
	fread( tags, sizeof( MD3Tag_t ), header.numTags * header.numFrames, f );

	int offset = header.offsetMeshes;
	for ( int i = 0; i < header.numMeshes; i++ )
	{
		MD3Mesh_t *mesh = &meshes[i];
		MD3MeshHeader_t *mheader = &mesh->header;

		fseek( f, offset, SEEK_SET );
		fread( mheader, sizeof( MD3MeshHeader_t ), 1, f );

		mesh->shaders = new MD3Shader_t[ mheader->numShaders ];
		mesh->triangles = new MD3Triangle_t[ mheader->numTriangles ];
		mesh->texCoords = new MD3TexCoord_t[ mheader->numVertices ];
		mesh->vertices = new MD3Vertex_t[ mheader->numFrames * mheader->numVertices ];

		fseek( f, offset + mheader->offsetShaders, SEEK_SET );
		fread( mesh->shaders, sizeof( MD3Shader_t ), mheader->numShaders, f );

		fseek( f, offset + mheader->offsetTriangles, SEEK_SET );
		fread( mesh->triangles, sizeof( MD3Triangle_t ), mheader->numTriangles, f );

		fseek( f, offset + mheader->offsetTexCoords, SEEK_SET );
		fread( mesh->texCoords, sizeof( MD3TexCoord_t ), mheader->numVertices, f );

		fseek( f, offset + mheader->offsetVertices, SEEK_SET );
		for ( int j = 0; j < mheader->numFrames * mheader->numVertices; j++ )
		{
			fread( &tempVert, sizeof( tempVert ), 1, f );
			mesh->vertices[j].coord.x = (float)tempVert.coord[0] * MD3_SCALE;
			mesh->vertices[j].coord.y = (float)tempVert.coord[1] * MD3_SCALE;
			mesh->vertices[j].coord.z = (float)tempVert.coord[2] * MD3_SCALE;
			mesh->vertices[j].normal = LatLngToVec( tempVert.normal );

			mesh->vertices[j].coord.QtoOGL();
			mesh->vertices[j].normal.QtoOGL();
		}

		mesh->lerpNorms = new Vector3D[ mheader->numVertices ];
		mesh->lerpVerts = new Vector3D[ mheader->numVertices ];

		offset += mheader->length;
	}

	printf( "MD3Model::Load() - Loaded %i frames, %i meshes\n", header.numFrames, header.numMeshes );

	fclose( f );
	return true;
}

void MD3Model::Free()
{
	if ( frames )
	{
		delete[] frames;
		frames = NULL;
	}

	if ( tags )
	{
		delete[] tags;
		tags = NULL;
	}

	if ( meshes )
	{
		for ( int i = 0; i < header.numMeshes; i++ )
		{
			delete[] meshes[i].shaders;
			delete[] meshes[i].triangles;
			delete[] meshes[i].texCoords;
			delete[] meshes[i].vertices;
			delete[] meshes[i].lerpNorms;
			delete[] meshes[i].lerpVerts;
		}
		delete[] meshes;
		meshes = NULL;
	}
}
	
int MD3Model::GetFrameCount()
{
	return header.numFrames;
}

void MD3Model::Draw( int lastframe, int curframe, float mu )
{
	if ( lastframe < 0 || curframe < 0 )
		lastframe = curframe = 0;
	if ( lastframe >= header.numFrames || curframe >= header.numFrames )
		lastframe = curframe = 0;

	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );
	glEnableClientState( GL_VERTEX_ARRAY );

	for ( int i = 0; i < header.numMeshes; i++ )
	{
		MD3Mesh_t *mesh = &meshes[i];
		int lastFrameStart = lastframe * mesh->header.numVertices;
		int curFrameStart = curframe * mesh->header.numVertices;

		for ( int j = 0; j < mesh->header.numVertices; j++ )
		{
			MD3Vertex_t *lastVert = &mesh->vertices[lastFrameStart + j];
			MD3Vertex_t *curVert = &mesh->vertices[curFrameStart + j];

			mesh->lerpVerts[j] = Interpolate::Linear( lastVert->coord, curVert->coord, mu );
			mesh->lerpNorms[j] = Interpolate::Linear( lastVert->normal, curVert->normal, mu );
		}

		glTexCoordPointer( 2, GL_FLOAT, 0, mesh->texCoords );
		glNormalPointer( GL_FLOAT, 0, &mesh->lerpNorms[0].x );
		glVertexPointer( 3, GL_FLOAT, 0, &mesh->lerpVerts[0].x );

		glDrawElements( GL_TRIANGLES, mesh->header.numTriangles * 3, GL_UNSIGNED_INT, &mesh->triangles[0].indices[0] );
	}

	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );
}

void MD3Model::DrawNormals( int lastframe, int curframe, float mu )
{
	glBegin( GL_LINES );

	for ( int i = 0; i < header.numMeshes; i++ )
	{
		MD3Mesh_t *mesh = &meshes[i];
		for ( int j = 0; j < mesh->header.numVertices; j++ )
		{
			glVertex3fv( &mesh->lerpVerts[j].x );

      Vector3D v = mesh->lerpVerts[j] + mesh->lerpNorms[j] * 5;
			glVertex3fv( &v.x );
		}
	}

	glEnd();
}

void MD3Model::GetBoundingBox( int frame, Vector3D *mins, Vector3D *maxs )
{
	if ( !frames || frame < 0 || frame >= header.numFrames )
		return;

	MD3Frame_t *f = &frames[frame];
	*mins = Vector3D( f->mins );
	*maxs = Vector3D( f->maxs );
}

float MD3Model::GetRadius( int frame )
{
	if ( !frames || frame < 0 || frame >= header.numFrames )
		return 0;

	return frames[frame].radius;
}
