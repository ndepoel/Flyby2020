#include "../Common.h"
#include "ModelManager.h"

ModelManager::ModelManager()
{
	Init();
}

ModelManager::~ModelManager()
{
	FreeAll();
}

ModelManager *ModelManager::Instance()
{
	static ModelManager instance;

	return &instance;
}

void ModelManager::Init()
{
	memset( models, 0, sizeof( models ) );

	activeModels.next = &activeModels;
	activeModels.prev = &activeModels;

	freeModels = &models[0];

	for ( int i = 0; i < MAX_NUM_MODELS; i++ )
	{
		models[i].next = &models[i+1];
		models[i].handle = i;
	}
	models[MAX_NUM_MODELS-1].handle = MAX_NUM_MODELS-1;
}

mdlHandle_t ModelManager::AllocateModel()
{
	model_t *mdl;

	if ( !freeModels )
		return -1;

	mdl = freeModels;
	freeModels = freeModels->next;

	mdl->next = activeModels.next;
	mdl->prev = &activeModels;
	activeModels.next->prev = mdl;
	activeModels.next = mdl;

//	printf( "ModelManager::AllocateModel() - Allocated model nr. %i\n", mdl->handle );

	return mdl->handle;
}

mdlHandle_t ModelManager::LoadModel( char *filename, int flags )
{
	for ( model_t *mdl = activeModels.next; mdl != &activeModels; mdl = mdl->next )
	{
		if ( !strcmp( mdl->filename, filename ) )
		{
//			printf( "ModelManager::LoadModel() - Model '%s' has already been loaded, returning model nr. %i\n", filename, mdl->handle );
			return mdl->handle;
		}
	}

	mdlHandle_t mdlHandle = AllocateModel();
	model_t *model = &models[mdlHandle];

	if ( mdlHandle >= 0 && mdlHandle < MAX_NUM_MODELS )
	{
		if ( strstr( filename, ".md3" ) )
		{
			model->model = new MD3Model();
		}
		else if ( strstr( filename, ".md2" ) )
		{
			model->model = new MD2Model();
		}
		else if ( strstr( filename, ".raw" ) )
		{
			model->model = new RawModel();
		}

		if ( !model->model->Load( filename ) )
		{
			FreeModel( mdlHandle );
			return -1;
		}

		model->frameCount = model->model->GetFrameCount();
		strcpy( model->filename, filename );
	}

//	printf( "ModelManager::LoadModel() - Loaded '%s' into model nr. %i\n", filename, mdlHandle );

	return mdlHandle;
}

int ModelManager::GetFrameCount( mdlHandle_t mdlHandle )
{
	if ( mdlHandle < 0 || mdlHandle >= MAX_NUM_MODELS )
		return 0;

	return models[mdlHandle].frameCount;
}

void ModelManager::DrawModel( mdlHandle_t mdlHandle, int lastframe, int curframe, float mu )
{
	if ( mdlHandle < 0 || mdlHandle >= MAX_NUM_MODELS )
		return;

	models[mdlHandle].model->Draw( lastframe, curframe, mu );
}

void ModelManager::DrawNormals( mdlHandle_t mdlHandle, int lastframe, int curframe, float mu )
{
	if ( mdlHandle < 0 || mdlHandle >= MAX_NUM_MODELS )
		return;

	models[mdlHandle].model->DrawNormals( lastframe, curframe, mu );
}

void ModelManager::GetBoundingBox( mdlHandle_t mdlHandle, int frame, Vector3D *mins, Vector3D *maxs )
{
	if ( mdlHandle < 0 || mdlHandle >= MAX_NUM_MODELS )
		return;

	models[mdlHandle].model->GetBoundingBox( frame, mins, maxs );
}

float ModelManager::GetRadius( mdlHandle_t mdlHandle, int frame )
{
	if ( mdlHandle < 0 || mdlHandle >= MAX_NUM_MODELS )
		return 0;

	return models[mdlHandle].model->GetRadius( frame );
}

void ModelManager::FreeModel( mdlHandle_t mdlHandle )
{
	if ( mdlHandle < 0 || mdlHandle >= MAX_NUM_MODELS )
		return;

	model_t *mdl = &models[mdlHandle];

	if ( !mdl->prev )
		return;

	mdl->prev->next = mdl->next;
	mdl->next->prev = mdl->prev;

	mdl->next = freeModels;
	mdl->prev = NULL;
	freeModels = mdl;

	if ( mdl->model )
		delete mdl->model;

//	printf( "ModelManager::FreeModel() - Freed model nr. %i\n", mdlHandle );
}

void ModelManager::FreeAll()
{
	model_t *mdl, *prev;

	for ( mdl = activeModels.prev; mdl != &activeModels; mdl = prev )
	{
		prev = mdl->prev;

		FreeModel( mdl->handle );
	}

//	printf( "ModelManager::FreeAll() - Freed all models\n" );
}
