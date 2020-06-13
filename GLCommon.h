#ifndef __GLCOMMON_H
#define __GLCOMMON_H

#include "OpenGL.h"
#include "Renderer/Renderer.h"

extern void			(*glSwapBuffers)( void );
extern bool			(*glLoadExtensions)( const char *glExtensions );

void InitDrawing();
void FinalizeDrawing();

void ResizeGLScene( int width, int height );
bool InitGL();
void CloseGL();

#endif

