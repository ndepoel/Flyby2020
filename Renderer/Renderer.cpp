#include "../Common.h"
#include "Renderer.h"
#include "../SharedVars.h"

Renderer::Renderer()
{
	Reset();
}

Renderer::~Renderer()
{
}

void Renderer::SetCamera( Camera *cam )
{
	this->camera = cam;
}

void Renderer::SetViewport( int x, int y, int width, int height, float ratio )
{
	viewport.x = x;
	viewport.y = y;
	viewport.width = width;
	viewport.height = height;
	if ( ratio == 0 )
		ratio = (float)width / (float)height;
	viewport.ratio = ratio;
}

void Renderer::SetFOV( float fov )
{
	this->fov = fov;
}

bool Renderer::AddWorld( BSPTree *world )
{
	this->world = world;
	return true;
}

bool Renderer::AddModel(	mdlHandle_t model, texHandle_t texture, 
							Vector3D origin, Vector3D angles, 
							activeAnim_t *animation, Vector3D scale )
{
	int lastframe = 0, curframe = 0;
	float mu = 0;

	if ( numModels >= MAX_MODELS )
		return false;

	if ( animation )
		if ( animation->sequence )
			animation->sequence->GetFrame( animation, &lastframe, &curframe, &mu );

	texturedModel_t *mdl = &models[numModels];

	mdl->mdlHandle = model;
	mdl->texHandle = texture;
	mdl->lastframe = lastframe;
	mdl->curframe = curframe;
	mdl->mu = mu;
	mdl->origin = origin;
	mdl->angles = angles;
	mdl->scale = scale;

	numModels++;
	return true;
}

bool Renderer::AddFont( Font *font )
{
	if ( numFonts >= MAX_FONTS )
		return false;

	fonts[numFonts] = font;

	numFonts++;
	return true;
}

bool Renderer::AddSkybox( Skybox *skybox )
{
	if ( numSkyboxes >= MAX_SKYBOXES )
		return false;

	skyboxes[numSkyboxes] = skybox;

	numSkyboxes++;
	return true;
}

bool Renderer::AddRenderTarget( texHandle_t target )
{
	if ( numRenderTargets >= MAX_RENDERTARGETS )
		return false;

	renderTargets[numRenderTargets] = target;

	numRenderTargets++;
	return true;
}

bool Renderer::AddPicture( int x, int y, int width, int height, texHandle_t texture, float alpha )
{
	if ( numPictures >= MAX_PICTURES )
		return false;

	picture_t *pict = &pictures[numPictures];

	pict->x = x;
	pict->y = y;
	pict->width = width;
	pict->height = height;
	pict->texture = texture;
	pict->alpha = alpha;

	numPictures++;
	return true;
}

bool Renderer::AddBillboard( Vector3D origin, float size, texHandle_t texture, float alpha )
{
	if ( numBillboards >= MAX_BILLBOARDS )
		return false;

	billboard_t *bb = &billboards[numBillboards];

	bb->origin = origin;
	bb->halfSize = size / 2;
	bb->texture = texture;
	bb->alpha = alpha;
	
	numBillboards++;
	return true;
}

void Renderer::DrawModel( texturedModel_t *mdl )
{
	Vector3D mins, maxs;
	ModelManager::Instance()->GetBoundingBox( mdl->mdlHandle, mdl->curframe, &mins, &maxs );

	if ( mdl->scale.LengthSqr() > 0 )
	{
		mins = mins * mdl->scale;
		maxs = maxs * mdl->scale;
	}

	if ( !viewFrustum.BoxInFrustum( mdl->origin + mins, mdl->origin + maxs ) )
		return;

	glPushMatrix();
		glTranslatef( mdl->origin.x, mdl->origin.y, mdl->origin.z );
		glRotatef( mdl->angles.y, 0, 1, 0 );
		glRotatef( mdl->angles.x, 1, 0, 0 );
		glRotatef( mdl->angles.z, 0, 0, 1 );
		if ( mdl->scale.LengthSqr() != 0 )
		{
			glEnable( GL_NORMALIZE );
			glScalef( mdl->scale.x, mdl->scale.y, mdl->scale.z );
		}
		TextureManager::Instance()->BindTexture( mdl->texHandle );
		ModelManager::Instance()->DrawModel( mdl->mdlHandle, mdl->lastframe, mdl->curframe, mdl->mu );
	glPopMatrix();

	glDisable( GL_NORMALIZE );
}

void Renderer::DrawPictures()
{
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	for ( int i = 0; i < numPictures; i++ )
	{
		picture_t *pict = &pictures[i];

		TextureManager::Instance()->BindTexture( pict->texture );
		glColor4f( 1, 1, 1, pict->alpha );

		glBegin( GL_QUADS );
			glTexCoord2f( 0, 0 ); 
			glVertex2i( pict->x, pict->y );
			glTexCoord2f( 0, 1 ); 
			glVertex2i( pict->x, pict->y + pict->height );
			glTexCoord2f( 1, 1 ); 
			glVertex2i( pict->x + pict->width, pict->y + pict->height );
			glTexCoord2f( 1, 0 ); 
			glVertex2i( pict->x + pict->width, pict->y );
		glEnd();
	}

	glColor4f( 1, 1, 1, 1 );
	glDisable( GL_BLEND );
}

void Renderer::DrawBillboards()
{
	Vector3D right( 0, 0, 1 ), up( 0, 1, 0 );
	if ( camera )
	{
		right = camera->right;
		up = camera->up;
	}

	glDepthMask( GL_FALSE );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	for ( int i = 0; i < numBillboards; i++ )
	{
		billboard_t *bb = &billboards[i];
		float halfSize = bb->halfSize;

		if ( !viewFrustum.SphereInFrustum( bb->origin, halfSize ) )		// Dubious...
			continue;

		TextureManager::Instance()->BindTexture( bb->texture );
		glColor4f( 1, 1, 1, bb->alpha );

		glBegin( GL_QUADS );
			glTexCoord2f( 0, 0 );
			glVertex3f( bb->origin.x - right.x * halfSize + up.x * halfSize,
						bb->origin.y - right.y * halfSize + up.y * halfSize,
						bb->origin.z - right.z * halfSize + up.z * halfSize );
			glTexCoord2f( 0, 1 );
			glVertex3f( bb->origin.x - right.x * halfSize - up.x * halfSize,
						bb->origin.y - right.y * halfSize - up.y * halfSize,
						bb->origin.z - right.z * halfSize - up.z * halfSize );
			glTexCoord2f( 1, 1 );
			glVertex3f( bb->origin.x + right.x * halfSize - up.x * halfSize,
						bb->origin.y + right.y * halfSize - up.y * halfSize,
						bb->origin.z + right.z * halfSize - up.z * halfSize );
			glTexCoord2f( 1, 0 );
			glVertex3f( bb->origin.x + right.x * halfSize + up.x * halfSize,
						bb->origin.y + right.y * halfSize + up.y * halfSize,
						bb->origin.z + right.z * halfSize + up.z * halfSize );
		glEnd();
	}

//	fonts[0]->Printf( 0, 16, 1, "%i billboards", numBillboards );

	glDepthMask( GL_TRUE );
	glDisable( GL_BLEND );
	glColor4f( 1, 1, 1, 1 );
}

void Renderer::CopyToTexture( texHandle_t texture )
{
	TextureManager::Instance()->BindTexture( texture );

	int width = 1, height = 1;
	while ( width < viewport.width )
		width <<= 1;
	while ( height < viewport.height )
		height <<= 1;

	glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, viewport.x, viewport.y, width, height, 0 );
}

void Renderer::SetDefaultState()
{
	glColor4f( 1, 1, 1, 1 );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glDisable( GL_BLEND );
	glDisable( GL_NORMALIZE );
	glDisable( GL_LIGHTING );
	glDisable( GL_CULL_FACE );

	// Configure texture unit 0
	glActiveTexture( GL_TEXTURE0_ARB );
	glEnable( GL_TEXTURE_2D );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE );
	glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE );
	glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE );

	// Configure texture unit 1
	glActiveTexture( GL_TEXTURE1_ARB );
	glDisable( GL_TEXTURE_2D );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE );
	glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE );
	glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS );
	glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE );

	// Set texture unit 0 as default texture unit
	glActiveTexture( GL_TEXTURE0_ARB );
	glClientActiveTexture( GL_TEXTURE0_ARB );

	// Set viewport
	glViewport( viewport.x, viewport.y, viewport.width, viewport.height );

	// Set projection matrix
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( fov, viewport.ratio, 10.0f, 10000.0f );

	// Set modelview matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

//	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
}

void Renderer::Set2DMode()
{
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, 640, 480, 0, -1, 1 );
	
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

void Renderer::Reset()
{
	world = NULL;
	camera = NULL;
	numModels = 0;
	numFonts = 0;
	numSkyboxes = 0;
	numRenderTargets = 0;
	numPictures = 0;
	numBillboards = 0;

	viewport.x = 0;
	viewport.y = 0;
	viewport.width = screenWidth;
	viewport.height = screenHeight;
	viewport.ratio = (float)screenWidth / (float)screenHeight;
	fov = 45.0f;
}

void Renderer::Execute()
{
	int i;

	SetDefaultState();

	if ( camera )
	{
		camera->ApplyRotation();
		camera->CalculateAxes();
	}

	for ( i = 0; i < numSkyboxes; i++ )
		skyboxes[i]->Draw();

	if ( camera )
		camera->ApplyTranslation();

	viewFrustum.ExtractViewFrustum();

	// Add just a bit of lighting for the models
	static float lpos[] = { 500, 100, -1000, 1 };
	static float lamb[] = { 0.75f, 0.75f, 0.75f, 1 };
	static float ldif[] = { 0.75f, 0.75f, 0.75f, 1 };
	glLightfv( GL_LIGHT0, GL_POSITION, lpos );
	glLightfv( GL_LIGHT0, GL_AMBIENT, lamb );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, ldif );
	glEnable( GL_LIGHT0 );

	if ( world )
		world->Draw( camera->origin, &viewFrustum );

	glEnable( GL_LIGHTING );

	for ( i = 0; i < numModels; i++ )
		DrawModel( &models[i] );

	glDisable( GL_LIGHTING );

	DrawBillboards();

	if ( world )
		world->DrawTransparentFaces();

	Set2DMode();

	DrawPictures();

	for ( i = 0; i < numFonts; i++ )
		fonts[i]->DrawTexts();

	for ( i = 0; i < numRenderTargets; i++ )
		CopyToTexture( renderTargets[i] );

	Reset();
}
