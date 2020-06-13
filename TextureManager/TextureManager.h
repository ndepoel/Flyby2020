#ifndef __TEXTUREMANAGER_H
#define __TEXTUREMANAGER_H

#define MAX_NUM_TEXTURES	1024

// Texture flags
#define TEX_MIPMAP			0x00000001
#define TEX_FLIPRGB			0x00000002
#define TEX_SKY				0x00000004	// Clamp to edge
#define	TEX_NOCOMPRESSION	0x00000008

typedef int	texHandle_t;	// < 0 means invalid texture handle

#include "../OpenGL.h"

typedef struct texture_s
{
	struct texture_s	*prev, *next;

	GLuint	texName;

	int		width, height;
	int		bpp;
	int		flags;
	bool	transparent;

	texHandle_t		handle;

	char	filename[64];
} texture_t;

class TextureManager
{
protected:
	TextureManager();

public:
	~TextureManager();

	static TextureManager *Instance();

	texHandle_t GetEmptyTexture( int flags = 0 );
	texHandle_t	UploadTexture(	unsigned *data, int width, int height, 
									int bpp, int flags = TEX_MIPMAP );
	texHandle_t	LoadImage( char *filename, int flags = TEX_MIPMAP );

	void		SetAnisotropy( bool enabled, int maxLevel );
	void		SetCompression( bool enabled );
	void		RefreshTextures();

	void		BindTexture( texHandle_t texHandle );
	void		UnbindTexture();
	bool		HasTransparency( texHandle_t texHandle );

	void		FreeTexture( texHandle_t texHandle );
	void		FreeAll();
private:
	void		Init();

	texHandle_t	AllocateTexture();

	void		ApplyTextureParameters( texture_t *texture );

	bool		useAnisotropy;
	int			maxAnisotropy;

	bool		useCompression;

	texture_t	textures[MAX_NUM_TEXTURES];
	texture_t	*freeTextures;
	texture_t	activeTextures;
};

#endif

