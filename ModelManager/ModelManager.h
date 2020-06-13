#ifndef __MODELMANAGER_H
#define __MODELMANAGER_H

#include "../Model/Model.h"
#include "../Model/RawModel.h"
#include "../Model/MD2Model.h"
#include "../Model/MD3Model.h"

#define MAX_NUM_MODELS	128

typedef int	mdlHandle_t;	// < 0 means invalid texture handle

typedef struct model_s
{
	struct model_s	*prev, *next;

	Model	*model;

	mdlHandle_t	handle;
	int			frameCount;

	char	filename[64];
} model_t;

class ModelManager
{
protected:
	ModelManager();

public:
	~ModelManager();

	static ModelManager *Instance();

	mdlHandle_t	LoadModel( char *filename, int flags = 0 );

	int			GetFrameCount( mdlHandle_t mdlHandle );
	void		DrawModel( mdlHandle_t mdlHandle, int lastframe, int curframe, float mu );
	void		DrawNormals( mdlHandle_t mdlHandle, int lastframe, int curframe, float mu );

	void		GetBoundingBox( mdlHandle_t mdlHandle, int frame, Vector3D *mins, Vector3D *maxs );
	float		GetRadius( mdlHandle_t mdlHandle, int frame );

	void		FreeAll();
private:
	void		Init();

	mdlHandle_t	AllocateModel();
	void		FreeModel( mdlHandle_t mdlHandle );

	model_t		models[MAX_NUM_MODELS];
	model_t		*freeModels;
	model_t		activeModels;
};

#endif

