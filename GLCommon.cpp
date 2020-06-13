#include "Common.h"
#include "GLCommon.h"
#include "SharedVars.h"

void		(*glSwapBuffers)( void ) = NULL;
bool		(*glLoadExtensions)( const char *glExtensions ) = NULL;

#ifndef NO_GLEXT
PFNGLACTIVETEXTUREARBPROC		glActiveTexture = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTexture = NULL;
PFNGLMULTITEXCOORD2FVARBPROC	glMultiTexCoord2fv = NULL;
PFNGLMULTITEXCOORD3FVARBPROC	glMultiTexCoord3fv = NULL;
#endif

void InitDrawing()
{
}

void FinalizeDrawing()
{
	glSwapBuffers();
}

void ResizeGLScene( int width, int height )
{
	if ( height == 0 )
		height = 1;

	screenWidth = width;
	screenHeight = height;
}

bool InitGL()
{
	if ( !glSwapBuffers || !glLoadExtensions )
		return false;

	glExtensions = (const char *)glGetString( GL_EXTENSIONS );
	if ( !glLoadExtensions( glExtensions ) )
	{
		printf( "ERROR: Your videocard doesn't support all required extensions\n" );
		return false;
	}

	int mtu;
	glGetIntegerv( GL_MAX_TEXTURE_UNITS_ARB, &mtu );
	if ( mtu < 2 )
	{
		printf( "ERROR: Your videocard doesn't have 2 or more texture units\n" );
		return false;
	}

	glShadeModel( GL_SMOOTH );
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClearDepth( 1.0f );
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glPointSize( 4.0f );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glSwapBuffers();

	return true;
}

void CloseGL()
{
}
