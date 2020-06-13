#ifndef __CAMERATARGET_H
#define __CAMERATARGET_H

#include "../Entity/Entity.h"

#define CT_FIXED		1
#define CT_LINEAR		2
#define CT_COSINE		3
#define CT_CUBIC		4
#define CT_CARDINAL		5

class CameraTarget: public Entity
{
public:
	CameraTarget();

	void	Update();
	void	Draw( Renderer &renderer );

	void	Init();

	void	Trigger( Entity *other, Entity *activator );

	char		lookName[32];
	char		triggerName[32];
	Entity		*triggerEnt;

	Vector3D	lookAt;
	Vector3D	up;

	float		speed;
	float		time;

	int			type;	// CT_ values
};

#endif

