#ifndef __FONT_H
#define __FONT_H

#include "../OpenGL.h"
#include "../TextureManager/TextureManager.h"

typedef struct textList_s
{
	struct textList_s	*next;

	char	text[128];
	float	x;
	float	y;
	float	scale;
} textList_t;

class Font
{
public:
	Font();
	~Font();

	bool Init( char *fontImage );
	void Free();

	void SetColor( float c[4] );
	void SetColor( float r, float g, float b, float a );

	void Printf( float x, float y, float scale, char *fmt, ... );
	void DrawTexts();
private:
	void ClearTexts();

	texHandle_t		fontTexture;
	GLuint			fontBase;
	textList_t		*textList;

	float			color[4];
};

#endif

