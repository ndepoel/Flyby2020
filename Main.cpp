#include "Common.h"
#include <SDL/SDL.h>
#include "Game.h"
#include "SharedVars.h"
#include "GLCommon.h"
#include "SoundManager/SoundManager.h"

static bool		done = false;
static bool		active = true;
static bool		keys[SDL_NUM_SCANCODES];
static int		screenwidth = 800;
static int		screenheight = 600;
static bool		fullscreen = false;
static char		*programtitle = "Flyby";
static float	sdlgamma = 1.0f;

static SDL_Window *window;
static SDL_GLContext glContext;

void KillGLFrame()
{
	CloseGL();

	SDL_GL_DeleteContext( glContext );

	SDL_Quit();
}

bool SDL_GL_LoadExtensions( const char *glExtensions )
{
	if ( !strstr( glExtensions, "GL_ARB_multitexture" ) )
		return false;

#ifndef NO_GLEXT
	// ARB_multitexture functions
	glActiveTexture = (PFNGLACTIVETEXTUREARBPROC)SDL_GL_GetProcAddress( "glActiveTextureARB" );
	glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREARBPROC)SDL_GL_GetProcAddress( "glClientActiveTextureARB" );
	glMultiTexCoord2fv = (PFNGLMULTITEXCOORD2FVARBPROC)SDL_GL_GetProcAddress( "glMultiTexCoord2fvARB" );
	glMultiTexCoord3fv = (PFNGLMULTITEXCOORD3FVARBPROC)SDL_GL_GetProcAddress( "glMultiTexCoord3fvARB" );
	if ( !glActiveTexture || !glClientActiveTexture || !glMultiTexCoord2fv || !glMultiTexCoord3fv )
		return false;
#endif

	return true;
}

void SDL_GL_SwapBuffers()
{
	SDL_GL_SwapWindow( window );
}

void SDL_SetGamma( float redGamma, float greenGamma, float blueGamma )
{
	Uint16 red[256];
	Uint16 green[256];
	Uint16 blue[256];

	SDL_CalculateGammaRamp(redGamma, red);
	SDL_CalculateGammaRamp(greenGamma, green);
	SDL_CalculateGammaRamp(blueGamma, blue);
	SDL_SetWindowGammaRamp(window, red, green, blue);
}

bool CreateGLFrame( char *title, int width, int height, int bpp, bool fullscreen )
{
	unsigned int flags;

	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "Could not initialize SDL\n" );
		return false;
	}

	flags = SDL_WINDOW_OPENGL;
	if ( fullscreen )
		flags |= SDL_WINDOW_FULLSCREEN;

	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
	if ( !window )
	{
		printf( "Could not set video mode\n" );
		return false;
	}

	glContext = SDL_GL_CreateContext( window );
	if (!glContext)
	{
		printf( "Could not create OpenGL context\n" );
		return false;
	}

	SDL_SetGamma( sdlgamma, sdlgamma, sdlgamma );
	SDL_ShowCursor( SDL_DISABLE );

	glSwapBuffers = SDL_GL_SwapBuffers;
	glLoadExtensions = SDL_GL_LoadExtensions;

	ResizeGLScene( width, height );

	if ( !InitGL() )
	{
		printf( "OpenGL initialization failed\n" );
		KillGLFrame();
		return false;
	}

	return true;
}

void HandleEvent( SDL_Event *event )
{
	switch( event->type )
	{
	case SDL_SYSWMEVENT:
		// syscommand (screensaver, monitor power saving mode, etc)
		break;
	case SDL_QUIT:
		done = 1;
		break;
	case SDL_MOUSEMOTION:
		break;
	case SDL_KEYDOWN:
		keys[ event->key.keysym.scancode ] = true;
		break;
	case SDL_KEYUP:
		keys[ event->key.keysym.scancode ] = false;
		break;
	case SDL_WINDOWEVENT:
		if (event->window.event == SDL_WINDOWEVENT_RESIZED)
			ResizeGLScene( event->window.data1, event->window.data2 );

		break;
	}
}

void ReadSettings( char *filename )
{
	FILE	*f;
	int		w = 0, h = 0, full = 0, subdiv = 0;
	float	gamm = 0, volume = 0, music = 0;
	
	if ( !( f = fopen( filename, "r" ) ) )
	{
		printf( "Could not open settings file %s\n", filename );
		return;
	}

	fscanf( f, "width %i\n", &w );
	fscanf( f, "height %i\n", &h );
	fscanf( f, "fullscreen %i\n", &full );
	fscanf( f, "gamma %f\n", &gamm );
	fscanf( f, "volume %f\n", &volume );
	fscanf( f, "musicvolume %f\n", &music );
	fscanf( f, "subdivisions %i\n", &subdiv );

	if ( w && h )
	{
		screenwidth = w;
		screenheight = h;
	}
	fullscreen = (full != 0);
	if ( gamm )
		sdlgamma = gamm;
	s_volume = volume;
	s_musicvolume = music;
	if ( subdiv )
		bsp_subdivisions = subdiv;

	fclose( f );
}

int main( int argc, char **argv )
{
	SDL_Event		event;
	unsigned int	prevtime;

	ReadSettings( "settings.cfg" );

	if ( !CreateGLFrame( programtitle, screenwidth, screenheight, 32, fullscreen ) )
	{
		printf( "Could not open OpenGL window\n" );
		return 1;
	}

	InitGame();

	prevtime = SDL_GetTicks();

	while ( !done )
	{
		// Update time
		realtime = SDL_GetTicks();
		int timedelta = realtime - prevtime;
		if ( hyperdrive > 0 )
			timedelta *= hyperdrive;
		else if ( hyperdrive < 0 )
			timedelta /= -hyperdrive;

		time = time + timedelta;
		frametime = (float)timedelta / 1000.0f;

		// Poll SDL events
		while ( SDL_PollEvent( &event ) )
		{
			HandleEvent( &event );
		}

		// Process input (TODO: Create a better system for input handling)
		if ( keys[SDL_SCANCODE_ESCAPE] )
		{
			done = true;
		}
	
		Vector3D forward, right, up;
		cam.angles.AnglesToAxis( forward, right, up );

		if ( keys[SDL_SCANCODE_F] )
		{
			keys[SDL_SCANCODE_F] = false;
			hyperdrive = (hyperdrive > 0) ? 0 : 4;
		}
		if ( keys[SDL_SCANCODE_H] )
		{
			keys[SDL_SCANCODE_H] = false;
			hyperdrive = (hyperdrive < 0) ? 0 : -4;
		}

		if ( keys[SDL_SCANCODE_UP] )
			cam.angles.x += 100 * frametime;
		if ( keys[SDL_SCANCODE_DOWN] )
			cam.angles.x -= 100 * frametime;
		if ( keys[SDL_SCANCODE_LEFT] )
			cam.angles.y += 100 * frametime;
		if ( keys[SDL_SCANCODE_RIGHT] )
			cam.angles.y -= 100 * frametime;
		if ( keys[SDL_SCANCODE_W] )
			cam.origin = cam.origin + forward * 360 * frametime;
		if ( keys[SDL_SCANCODE_S] )
			cam.origin = cam.origin - forward * 360 * frametime;
		if ( keys[SDL_SCANCODE_A] )
			cam.origin = cam.origin - right * 360 * frametime;
		if ( keys[SDL_SCANCODE_D] )
			cam.origin = cam.origin + right * 360 * frametime;
		if ( keys[SDL_SCANCODE_Q] )
			cam.origin.y += 360 * frametime;
		if ( keys[SDL_SCANCODE_Z] )
			cam.origin.y -= 360 * frametime;

		if ( keys[SDL_SCANCODE_EQUALS] )
		{
			keys[SDL_SCANCODE_EQUALS] = false;
			sdlgamma += 0.1f;
			SDL_SetGamma( sdlgamma, sdlgamma, sdlgamma );
		}
		if ( keys[SDL_SCANCODE_MINUS] )
		{
			keys[SDL_SCANCODE_MINUS] = false;
			sdlgamma -= 0.1f;
			SDL_SetGamma( sdlgamma, sdlgamma, sdlgamma );
		}

		SoundManager::Instance()->Update();

		// Update the game
		UpdateFrame();
		
		// Render the screen
		if ( active )
		{	
			InitDrawing();
			DrawFrame();
			FinalizeDrawing();
		}

		prevtime = realtime;
	}

	CloseGame();

	KillGLFrame();

	return 0;
}

