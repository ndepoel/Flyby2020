#include "../Common.h"
#include "Font.h"

Font::Font()
{
	fontTexture = -1;
	textList = NULL;
	for ( int i = 0; i < 4; i++ )
		color[i] = 1;
}

Font::~Font()
{
	Free();
}

#define INV_FONTSIZE	0.0625f	// = 1/16
bool Font::Init( char *fontImage )
{
	fontTexture = TextureManager::Instance()->LoadImage( fontImage, 0 );
	if ( fontTexture < 0 )
		return false;

	fontBase = glGenLists( 256 );
	for ( int i = 0; i < 256; i++ )
	{
		float x = (float)( i % 16 ) / 16.0f;
		float y = (float)( i / 16 ) / 16.0f;

		glNewList( fontBase + i, GL_COMPILE );
			glBegin( GL_QUADS );
				glTexCoord2f( x, y );
				glVertex2i( 0, 0 );
				glTexCoord2f( x, y + INV_FONTSIZE );
				glVertex2i( 0, 16 );
				glTexCoord2f( x + INV_FONTSIZE, y + INV_FONTSIZE );
				glVertex2i( 16, 16 );
				glTexCoord2f( x + INV_FONTSIZE, y );
				glVertex2i( 16, 0 );
			glEnd();
			glTranslatef( 14, 0, 0 );
		glEndList();
	}

	return true;
}

void Font::Free()
{
	glDeleteLists( fontBase, 256 );
}

void Font::SetColor( float c[4] )
{
	SetColor( c[0], c[1], c[2], c[3] );
}

void Font::SetColor( float r, float g, float b, float a )
{
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;
}

void Font::Printf( float x, float y, float scale, char *fmt, ... )
{
	textList_t	*newText = new textList_t;
	va_list		va;

	newText->next = textList;
	newText->x = x;
	newText->y = y;
	newText->scale = scale;
	textList = newText;

	memset( newText->text, 0, sizeof( newText->text ) );
	va_start( va, fmt );
		vsprintf( newText->text, fmt, va );
	va_end( va );
}

void Font::DrawTexts()
{
	textList_t	*cur;

	if ( fontTexture < 0 )
		return;

	TextureManager::Instance()->BindTexture( fontTexture );

	glPushAttrib( GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT );
		glColor4fv( color );
		if ( TextureManager::Instance()->HasTransparency( fontTexture ) )
		{
			glEnable( GL_BLEND );
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		}

		glPushMatrix();

			glListBase( fontBase );
			cur = textList;
			while ( cur )
			{
				glLoadIdentity();
				glTranslatef( cur->x, cur->y, 0 );
				glScalef( cur->scale, cur->scale, cur->scale );

				glCallLists( strlen( cur->text ), GL_UNSIGNED_BYTE, cur->text );

				cur = cur->next;
			}

		glPopMatrix();

	glPopAttrib();

	ClearTexts();
}

void Font::ClearTexts()
{
	textList_t	*item = textList, *next;

	while ( item )
	{
		next = item->next;
		delete item;
		item = next;
	}

	textList = NULL;
}
