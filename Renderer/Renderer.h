#ifndef __RENDERER_H
#define __RENDERER_H

#include "../Common.h"
#include "../OpenGL.h"
#include "../Frustum/Frustum.h"
#include "../TextureManager/TextureManager.h"
#include "../ModelManager/ModelManager.h"
#include "../Model/AnimSequence.h"
#include "../Font/Font.h"
#include "../Camera/Camera.h"
#include "../World/Skybox.h"
#include "../World/BSPTree.h"

#define MAX_MODELS			256
#define MAX_FONTS			16
#define MAX_SKYBOXES		4
#define MAX_RENDERTARGETS	4
#define MAX_PICTURES		512
#define MAX_BILLBOARDS		2048

typedef struct
{
	Vector3D		origin;
	Vector3D		angles;
	Vector3D		scale;

	int				lastframe, curframe;
	float			mu;

	mdlHandle_t		mdlHandle;
	texHandle_t		texHandle;
} texturedModel_t;

typedef struct
{
	int		x, y;
	int		width, height;
	float	ratio;
} viewport_t;

typedef struct
{
	int			x, y;
	int			width, height;
	texHandle_t	texture;
	float		alpha;
} picture_t;

typedef struct
{
	Vector3D	origin;
	float		halfSize;

	texHandle_t	texture;
	float		alpha;
} billboard_t;

class Renderer
{
public:
	Renderer();
	~Renderer();

	void		SetCamera( Camera *cam );
	void		SetViewport( int x, int y, int width, int height, float ratio = 0 );
	void		SetFOV( float fov );

	bool		AddWorld( BSPTree *world );

	bool		AddModel(	mdlHandle_t model, texHandle_t texture, 
							Vector3D origin, Vector3D angles, 
							activeAnim_t *animation = NULL, 
							Vector3D scale = Vector3D( 0, 0, 0 ) );

	bool		AddFont( Font *font );

	bool		AddSkybox( Skybox *skybox );

	bool		AddRenderTarget( texHandle_t target );

	bool		AddPicture( int x, int y, int width, int height, texHandle_t texture, float alpha = 1.0f );

	bool		AddBillboard( Vector3D origin, float size, texHandle_t texture, float alpha = 1.0f );

	void		Execute();
private:
	void		DrawModel( texturedModel_t *mdl );
	void		DrawPictures();
	void		DrawBillboards();
	void		CopyToTexture( texHandle_t texture );

	void		SetDefaultState();
	void		Set2DMode();
	void		Reset();

	Frustum		viewFrustum;

	Camera		*camera;
	viewport_t	viewport;
	float		fov;

	BSPTree			*world;

	texturedModel_t	models[MAX_MODELS];
	int				numModels;

	Font			*fonts[MAX_FONTS];
	int				numFonts;

	Skybox			*skyboxes[MAX_SKYBOXES];
	int				numSkyboxes;

	texHandle_t		renderTargets[MAX_RENDERTARGETS];
	int				numRenderTargets;

	picture_t		pictures[MAX_PICTURES];
	int				numPictures;

	billboard_t		billboards[MAX_BILLBOARDS];
	int				numBillboards;
};

#endif

