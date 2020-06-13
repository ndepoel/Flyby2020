#include "../Common.h"
#include "TextureManager.h"
#include <SDL/SDL_image.h>

static const char	*glExtensions = NULL;
static GLuint		boundTexture = 0;

#define NUM_IMAGETYPES	5
static const char *imageTypes[NUM_IMAGETYPES] =
{
	"",
	".tga",
	".png",
	".bmp",
	".jpg"
};

#define NUM_BGRTYPES	2
static const char *bgrTypes[NUM_BGRTYPES] =
{
	".bmp",
	".tga"
};

// Constructor
// Creates a texture manager with the default size
TextureManager::TextureManager()
{
	if ( !glExtensions )
		glExtensions = (const char *)glGetString( GL_EXTENSIONS );

	Init();
}

// Destructor
// Frees all active textures and deletes the texture array
TextureManager::~TextureManager()
{
	FreeAll();
}

// Returns the singleton instance of the texture manager
TextureManager *TextureManager::Instance()
{
	static TextureManager instance;

	return &instance;
}

// Initializes the texture manager
// Automatically invoked by the constructor
void TextureManager::Init()
{
	useAnisotropy = false;
	maxAnisotropy = 1;

	useCompression = false;

	// Create an empty texture array
	memset( textures, 0, sizeof( textures ) );

	// Active textures is a circular double linked list
	activeTextures.next = &activeTextures;
	activeTextures.prev = &activeTextures;

	// Free textures is a single linked list
	freeTextures = &textures[0];

	// Initialize the single linked list and let each texture remember its handle
	for ( int i = 0; i < MAX_NUM_TEXTURES; i++ )
	{
		textures[i].next = &textures[i+1];
		textures[i].handle = i;
	}
	textures[MAX_NUM_TEXTURES-1].handle = MAX_NUM_TEXTURES-1;
}

// Retrieves a texture from the list of free textures and returns its handle
// Will return -1 if there aren't any free textures
texHandle_t TextureManager::AllocateTexture()
{
	texture_t *tex;

	if ( !freeTextures )
		return -1;

	// Take one texture from the free texture list
	tex = freeTextures;
	freeTextures = freeTextures->next;

	// Add texture to the active texture list
	tex->next = activeTextures.next;
	tex->prev = &activeTextures;
	activeTextures.next->prev = tex;
	activeTextures.next = tex;

	glGenTextures( 1, &tex->texName );

//	printf( "TextureManager::AllocateTexture() - Allocated texture nr. %i\n", tex->handle );

	return tex->handle;
}

// Applies a single texture's settings
void TextureManager::ApplyTextureParameters( texture_t *texture )
{
	BindTexture( texture->handle );

	// Use trilinear filtering if mipmapping is used, otherwise use normal bilinear filtering
	if ( texture->flags & TEX_MIPMAP )
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		if ( useAnisotropy )
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy );
	}
	else
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	}

	if ( texture->flags & TEX_SKY )
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	}
}

texHandle_t TextureManager::GetEmptyTexture( int flags )
{
	texHandle_t texHandle = AllocateTexture();
	if ( texHandle < 0 || texHandle >= MAX_NUM_TEXTURES )
		return -1;

	texture_t *texture = &textures[texHandle];
	texture->flags = flags;
	memset( texture->filename, 0, sizeof( texture->filename ) );
	ApplyTextureParameters( texture );

	// Make the texture black by default
	char black[3] = { 0, 0, 0 };
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, black );

	return texHandle;
}

static void FlipRGB( unsigned *data, unsigned int stride, unsigned int pixelCount )
{
	unsigned char *p = (unsigned char *)data;
	unsigned char c;

	for ( unsigned int i = 0; i < pixelCount; i++, p += stride )
	{
		c = p[0];
		p[0] = p[2];
		p[2] = c;
	}
}

// Uploads an OpenGL texture from a pixel array
texHandle_t TextureManager::UploadTexture(	unsigned *data, int width, int height, 
											int bpp, int flags )
{
	texHandle_t		texHandle;
	GLint			internalformat;
	GLenum			format;

	// Decide on number of components and texture format to use
	if ( bpp == 24 )
	{
		internalformat = useCompression ? GL_COMPRESSED_RGB_ARB : GL_RGB;
		format = GL_RGB;
	}
	else if ( bpp == 32 )
	{
		internalformat = useCompression ? GL_COMPRESSED_RGBA_ARB : GL_RGBA;
		format = GL_RGBA;
	}
	else
		return -1;

	// Flip R and B components if required
	if ( flags & TEX_FLIPRGB )
		FlipRGB( data, bpp >> 3, width * height );

	// Allocate a new texture
	texHandle = AllocateTexture();
	if ( texHandle < 0 || texHandle >= MAX_NUM_TEXTURES )
		return -1;

	// Initialize the new texture
	texture_t *texture = &textures[texHandle];

	texture->width = width;
	texture->height = height;
	texture->bpp = bpp;
	texture->flags = flags;
	texture->transparent = false;
	memset( texture->filename, 0, sizeof( texture->filename ) );

	if ( texture->bpp == 32 )	// Check for alpha transparency
	{
		unsigned char *pixels = (unsigned char *)data;
		for ( int k = 0; k < texture->width * texture->height; k++ )
		{
			if ( pixels[k*4+3] ^ 0xFF )	// True if the pixel's alpha channel is not 255
			{
				texture->transparent = true;
				break;
			}
		}
	}

	// This will also bind the texture
	ApplyTextureParameters( texture );

	// Build mipmaps if required, otherwise upload the texture directly
	if ( texture->flags & TEX_MIPMAP )
		gluBuild2DMipmaps( GL_TEXTURE_2D, internalformat, width, height, format,
			GL_UNSIGNED_BYTE, (GLvoid *)data );
	else
		glTexImage2D( GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, 
			GL_UNSIGNED_BYTE, (GLvoid *)data );

//	printf( "TextureManager::UploadTexture() - Uploaded texture nr. %i\n", texHandle );

	return texHandle;
}

static bool FileExists( char *filename )
{
	FILE *f = fopen( filename, "r" );
	if ( !f )
		return false;
	else
	{
		fclose( f );
		return true;
	}
}

static const char *FindExtension( char *filename )
{
	char fullname[72];

	for ( int i = 0; i < NUM_IMAGETYPES; i++ )
	{
		const char *extension = imageTypes[i];
		sprintf( fullname, "%s%s", filename, extension );

		if ( FileExists( fullname ) )
			return extension;
	}
	
	return NULL;
}

// Loads an image from a file and uploads it to an OpenGL texture
// Uses the SDL_image library for image loading
texHandle_t TextureManager::LoadImage( char *filename, int flags )
{
	SDL_Surface		*surf = NULL;
	texHandle_t		texHandle;
	const char		*extension;
	char			fullname[72];

	// Check if this image file hasn't already been loaded before
	for ( texture_t *tex = activeTextures.next; tex != &activeTextures; tex = tex->next )
	{
		if ( !strcmp( tex->filename, filename ) )
		{
//			printf( "TextureManager::LoadImage() - Image '%s' has already been loaded, returning texture nr. %i\n", filename, tex->handle );
			return tex->handle;
		}
	}

	// Find the file extension belonging to this file
	extension = FindExtension( filename );
	if ( !extension )
	{
//		printf( "TextureManager::LoadImage() - Could not find file '%s'\n", filename );
		return -1;
	}
	sprintf( fullname, "%s%s", filename, extension );

	// Use SDL_image to load the image into an SDL surface
	if ( !( surf = IMG_Load( fullname ) ) )
	{
//		printf( "TextureManager::LoadImage() - Could not load image '%s'\n", filename );
		return -1;
	}

	// Certain image formats need their R and B components swapped...
	for ( int i = 0; i < NUM_BGRTYPES; i++ )
		if ( strstr( fullname, bgrTypes[i] ) )
			flags |= TEX_FLIPRGB;

	// Upload the texture to OpenGL
	texHandle = UploadTexture(	(unsigned *)surf->pixels, surf->w, surf->h, 
								surf->format->BitsPerPixel, flags );

	// Copy the filename to the texture's properties, so we won't load the same image twice
	if ( texHandle >= 0 && texHandle < MAX_NUM_TEXTURES )
		strcpy( textures[texHandle].filename, filename );

	// Free the SDL surface
	SDL_FreeSurface( surf );

//	printf( "TextureManager::LoadImage() - Loaded '%s' into texture nr. %i\n", filename, texHandle );

	return texHandle;
}

// Sets the max level of anisotropy and refreshes all textures
void TextureManager::SetAnisotropy( bool enabled, int maxLevel )
{
	if ( !strstr( glExtensions, "GL_EXT_texture_filter_anisotropic" ) )
		enabled = false;

	useAnisotropy = enabled;

	if ( useAnisotropy )
	{
		useAnisotropy = true;
		maxAnisotropy = 1;
		// Make sure max anisotropy is a power of two
		while ( maxAnisotropy < maxLevel && maxAnisotropy < 16 )
			maxAnisotropy <<= 1;
	}

	RefreshTextures();
}

// Sets texture compression on or off
// Note that this doesn't refresh the textures, because that would require reuploading of all textures
void TextureManager::SetCompression( bool enabled )
{
	if ( !strstr( glExtensions, "GL_ARB_texture_compression" ) )
		enabled = false;

	useCompression = enabled;
}

// Reapplies the texture settings for all active textures
void TextureManager::RefreshTextures()
{
	for ( texture_t *tex = activeTextures.next; tex != &activeTextures; tex = tex->next )
	{
		ApplyTextureParameters( tex );
	}
}

// Binds a texture
void TextureManager::BindTexture( texHandle_t texHandle )
{
	if ( texHandle < 0 || texHandle >= MAX_NUM_TEXTURES )
	{
		UnbindTexture();
		return;
	}

	GLuint texName = textures[texHandle].texName;
	if ( boundTexture != texName )
	{
		glBindTexture( GL_TEXTURE_2D, texName );
		boundTexture = texName;
	}
}

// Binds a non-existing texture
void TextureManager::UnbindTexture()
{
	glBindTexture( GL_TEXTURE_2D, 0 );
}

// Returns whether or not the texture has alpha transparency
bool TextureManager::HasTransparency( texHandle_t texHandle )
{
	if ( texHandle < 0 || texHandle >= MAX_NUM_TEXTURES )
		return false;
	else
		return textures[texHandle].transparent;
}

// Frees a single texture
void TextureManager::FreeTexture( texHandle_t texHandle )
{
	if ( texHandle < 0 || texHandle >= MAX_NUM_TEXTURES )
		return;

	texture_t *tex = &textures[texHandle];

	// Check if texture isn't already free
	if ( !tex->prev )
		return;

	// Remove from active textures list
	tex->prev->next = tex->next;
	tex->next->prev = tex->prev;

	// Add to free texture list
	tex->next = freeTextures;
	tex->prev = NULL;
	freeTextures = tex;

	glDeleteTextures( 1, &tex->texName );

//	printf( "TextureManager::FreeTexture() - Freed texture nr. %i\n", texHandle );
}

// Frees all textures
// Automatically invoked by the destructor
void TextureManager::FreeAll()
{
	texture_t *tex, *prev;

	// Go backwards through the active textures list and free all active textures
	for ( tex = activeTextures.prev; tex != &activeTextures; tex = prev )
	{
		prev = tex->prev;

		FreeTexture( tex->handle );
	}

//	printf( "TextureManager::FreeAll() - Freed all textures\n" );
}
