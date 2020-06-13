#include "../Common.h"
#include "Skybox.h"

static char *sky_suffixes[6] =
{
	"_bk",
	"_dn",
	"_ft",
	"_lf",
	"_rt",
	"_up"
};

static short sky_vertices[8][3] =
{
	{ 50, 50, 50 },
	{ 50, 50, -50 },
	{ 50, -50, 50 },
	{ 50, -50, -50 },
	{ -50, 50, 50 },
	{ -50, 50, -50 },
	{ -50, -50, 50 },
	{ -50, -50, -50 }
};

static float sky_texcoords[4][2] =
{
	{ 0, 0 },
	{ 0, 1 },
	{ 1, 1 },
	{ 1, 0 }
};

static char sky_indices[6][4] =
{
	{ 5, 7, 3, 1 },
	{ 3, 7, 6, 2 },
	{ 0, 2, 6, 4 },
	{ 4, 6, 7, 5 },
	{ 1, 3, 2, 0 },
	{ 5, 1, 0, 4 }
};

Skybox::Skybox()
{
	for ( int i = 0; i < 6; i++ )
		textures[i] = -1;
}

Skybox::Skybox( char *skyname )
{
	Init( skyname );
}

void Skybox::Init( char *skyname )
{
	char filename[64];

	for ( int i = 0; i < 6; i++ )
	{
		memset( filename, 0, sizeof( filename ) );
		sprintf( filename, "%s%s", skyname, sky_suffixes[i] );
		textures[i] = TextureManager::Instance()->LoadImage( filename, TEX_SKY );
	}
}

void Skybox::SetAttributes()
{
	glDisable( GL_DEPTH_TEST );
	glDepthMask( GL_FALSE );
	glDisable( GL_LIGHTING );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
}

void Skybox::Draw()
{
	glPushAttrib( GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT | GL_POLYGON_BIT );

		SetAttributes();

		for ( int i = 0; i < 6; i++ )
		{
			TextureManager::Instance()->BindTexture( textures[i] );
			glBegin( GL_QUADS );
				for ( int j = 0; j < 4; j++ )
				{
					glTexCoord2fv( sky_texcoords[j] );
					glVertex3sv( sky_vertices[ sky_indices[i][j] ] );
				}
			glEnd();
		}

	glPopAttrib();
}

unsigned int Skybox::CreateCubeMap()
{
	static int targets[6] =
	{
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB
	};

	static char pixels[512*512*4];

	GLuint cubemap;
	glGenTextures( 1, &cubemap );

	for ( int i = 0; i < 6; i++ )
	{
		TextureManager::Instance()->BindTexture( textures[i] );
		glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

		glBindTexture( GL_TEXTURE_CUBE_MAP_ARB, cubemap );
		glTexImage2D( targets[i], 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
	}

	glTexParameteri( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	return cubemap;
}

