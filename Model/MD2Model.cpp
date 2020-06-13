#include "../Common.h"
#include "MD2Model.h"
#include "../Math3D/Interpolate.h"
#include "../OpenGL.h"

#define MD2MAGIC			0x32504449
#define NUMVERTEXNORMALS	162

const float vertexnormals[NUMVERTEXNORMALS][3] =
{
#include "anorms.h"
};

MD2Model::MD2Model()
{
}

MD2Model::MD2Model( char *filename )
{
	MD2Model();
	Load( filename );
}

MD2Model::~MD2Model()
{
	Free();	
}

bool MD2Model::Load( char *filename )
{
	FILE *f;

	if ( !( f = fopen( filename, "rb" ) ) )
	{
		return false;
	}

	fread( &header, sizeof( MD2Header_t ), 1, f );
	if ( header.magic != MD2MAGIC || header.version != 8 )
	{
		fclose( f );
		return false;
	}

	skins = new MD2Skin_t[ header.numSkins ];
	fseek( f, header.offsetSkins, SEEK_SET );
	fread( skins, sizeof( MD2Skin_t ), header.numSkins, f );

	texcoords = new MD2TexCoord_t[ header.numTexCoords ];
	fseek( f, header.offsetTexCoords, SEEK_SET );
	fread( texcoords, sizeof( MD2TexCoord_t ), header.numTexCoords, f );

	triangles = new MD2Triangle_t[ header.numTriangles ];
	fseek( f, header.offsetTriangles, SEEK_SET );
	fread( triangles, sizeof( MD2Triangle_t ), header.numTriangles, f );

	frames = new MD2Frame_t[ header.numFrames ];
	fseek( f, header.offsetFrames, SEEK_SET );
	for ( int i = 0; i < header.numFrames; i++ )
	{
		MD2Frame_t *fr = &frames[i];

		fread( fr->scale, sizeof(float), 3, f );
		fread( fr->translate, sizeof(float), 3, f );
		fread( fr->name, sizeof(char), 16, f );

		fr->md2verts = new MD2Vertex_t[ header.numVertices ];
		fr->vertices = new Vector3D[ header.numVertices ];
		fr->normals = new Vector3D[ header.numVertices ];

		fread( fr->md2verts, sizeof( MD2Vertex_t ), header.numVertices, f );
		for ( int j = 0; j < header.numVertices; j++ )
		{
			MD2Vertex_t *md2vert = &fr->md2verts[j];
			Vector3D *vert = &fr->vertices[j];
			Vector3D *norm = &fr->normals[j];

			vert->x = (float)md2vert->vertex[0] * fr->scale[0] + fr->translate[0];
			vert->y = (float)md2vert->vertex[1] * fr->scale[1] + fr->translate[1];
			vert->z = (float)md2vert->vertex[2] * fr->scale[2] + fr->translate[2];

			norm->x = vertexnormals[ md2vert->normalIndex ][0];
			norm->y = vertexnormals[ md2vert->normalIndex ][1];
			norm->z = vertexnormals[ md2vert->normalIndex ][2];

			vert->QtoOGL();
			norm->QtoOGL();
		}
	}

	glCommands = new int[ header.numGLCommands ];
	fseek( f, header.offsetGlCommands, SEEK_SET );
	fread( glCommands, sizeof( int ), header.numGLCommands, f );

	fclose( f );

	lerpVerts = new Vector3D[ header.numVertices ];
	lerpNorms = new Vector3D[ header.numVertices ];

	printf( "MD2Model::Load() - Loaded %i vertices, %i triangles, %i frames\n", 
		header.numVertices, header.numTriangles, header.numFrames );

	return true;
}

void MD2Model::Free()
{
	if ( skins )
	{
		delete[] skins;
		skins = NULL;
	}

	if ( texcoords )
	{
		delete[] texcoords;
		texcoords = NULL;
	}

	if ( triangles )
	{
		delete[] triangles;
		triangles = NULL;
	}

	if ( frames )
	{
		for ( int i = 0; i < header.numFrames; i++ )
		{
			delete[] frames[i].md2verts;
			delete[] frames[i].vertices;
			delete[] frames[i].normals;
		}
		delete[] frames;
		frames = NULL;
	}

	if ( lerpVerts )
	{
		delete[] lerpVerts;
		lerpVerts = NULL;
	}

	if ( lerpNorms )
	{
		delete[] lerpNorms;
		lerpNorms = NULL;
	}
}

int MD2Model::GetFrameCount()
{
	return header.numFrames;
}

void MD2Model::Draw( int lastframe, int curframe, float mu )
{
	if ( lastframe < 0 || curframe < 0 )
		lastframe = curframe = 0;
	if ( lastframe >= header.numFrames || curframe >= header.numFrames )
		lastframe = curframe = 0;

	MD2Frame_t *last = &frames[lastframe];
	MD2Frame_t *cur = &frames[curframe];

	for ( int k = 0; k < header.numVertices; k++ )
	{
		lerpVerts[k] = Interpolate::Linear( last->vertices[k], cur->vertices[k], mu );
		lerpNorms[k] = Interpolate::Linear( last->normals[k], cur->normals[k], mu );
//		lerpVerts[k] = last->vertices[k] * (1.0f - mu) + cur->vertices[k] * mu;
//		lerpNorms[k] = last->normals[k] * (1.0f - mu) + cur->normals[k] * mu;
	}

	int *glcmd = glCommands;
	while ( int i = *glcmd++ )
	{
		if ( i > 0 )
		{
			glBegin( GL_TRIANGLE_STRIP );
		}
		else
		{
			i = -i;
			glBegin( GL_TRIANGLE_FAN );
		}

		for ( ; i > 0; i--, glcmd += 3 )
		{
			glTexCoord2f( ((float *)glcmd)[0], ((float *)glcmd)[1] );
			glNormal3fv( &lerpNorms[ glcmd[2] ].x );
			glVertex3fv( &lerpVerts[ glcmd[2] ].x );
		}

		glEnd();
	}
}

void MD2Model::DrawNormals( int lastframe, int curframe, float mu )
{
	Vector3D lastnorm, curnorm, lerpnorm;
	Vector3D start, end;

	if ( lastframe >= header.numFrames || curframe >= header.numFrames )
		lastframe = curframe = 0;

	MD2Frame_t *last = &frames[lastframe];
	MD2Frame_t *cur = &frames[curframe];

	glPushAttrib( GL_CURRENT_BIT | GL_LIGHTING_BIT );
		glDisable( GL_LIGHTING );
		glColor3f( 1, 0, 0 );

		glBegin( GL_LINES );
			for ( int i = 0; i < header.numVertices; i++ )
			{
				lastnorm = last->normals[i];
				curnorm = cur->normals[i];

				lerpnorm = lastnorm * (1.0f - mu) + curnorm * mu;
				start = last->vertices[i] * (1.0f - mu) + cur->vertices[i] * mu;
				end = start + lerpnorm * 5.0f;

				glVertex3fv( &start.x );
				glVertex3fv( &end.x );
			}
		glEnd();

	glPopAttrib();
}

void MD2Model::GetBoundingBox( int frame, Vector3D *mins, Vector3D *maxs )
{
	if ( frame < 0 || frame >= header.numFrames )
		return;

	MD2Frame_t *f = &frames[frame];

	mins->x = f->translate[0];
	mins->y = f->translate[1];
	mins->z = f->translate[2];

	maxs->x = f->scale[0] * 255 + f->translate[0];
	maxs->y = f->scale[1] * 255 + f->translate[1];
	maxs->z = f->scale[2] * 255 + f->translate[2];
}

float MD2Model::GetRadius( int frame )
{
	// TODO: implement
	return 0;
}

