#ifndef __OPENGL_H
#define __OPENGL_H

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

#ifndef NO_GLEXT
#include <GL/glext.h>

extern PFNGLACTIVETEXTUREARBPROC		glActiveTexture;
extern PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTexture;
extern PFNGLMULTITEXCOORD2FVARBPROC		glMultiTexCoord2fv;
extern PFNGLMULTITEXCOORD3FVARBPROC		glMultiTexCoord3fv;
#endif

#endif

