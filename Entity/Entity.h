#ifndef __ENTITY_H
#define __ENTITY_H

#include "../Game.h"
#include "../Math3D/Math3D.h"
#include "../Renderer/Renderer.h"
#include "../ModelManager/ModelManager.h"
#include "../TextureManager/TextureManager.h"

#include "EntityMap.h"

class Entity
{
public:
	virtual void	Update() = 0;
	virtual void	Draw( Renderer &renderer ) = 0;

	virtual void	Init() { targetEntity = FindEntityForTargetname( target ); }
	
	virtual void	Trigger( Entity *other, Entity *activator ) {}

	Vector3D	origin;

	char		classname[64];
	char		target[32];
	char		targetname[32];

	Entity		*targetEntity;
};

class VisualEntity: public Entity
{
public:
	Vector3D	angles;
};

class ModelEntity: public VisualEntity
{
protected:
	mdlHandle_t		model;
	texHandle_t		texture;
};

#endif

