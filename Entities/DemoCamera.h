#ifndef __DEMOCAMERA_H
#define __DEMOCAMERA_H

#include "../Entity/Entity.h"
#include "../Camera/AxisCamera.h"
#include "CameraTarget.h"

typedef struct camTarget_s
{
	struct camTarget_s	*prev, *next;

	CameraTarget *target;
} camTarget_t;

class DemoCamera: public Entity
{
public:
	DemoCamera();
	~DemoCamera();

	void		Update();
	void		Draw( Renderer &renderer );
	void		Trigger( Entity *other, Entity *activator );

	Camera		*GetCamera();

	bool		initial;
	bool		active;
private:
	bool		AddCameraTarget( CameraTarget *ct );
	void		ResetCamera();

	AxisCamera		camera;

	camTarget_t		*targets;
	bool			initialized;

	camTarget_t		*curTarget;
	float			mu;
};

#endif

