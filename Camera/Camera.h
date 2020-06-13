#ifndef __CAMERA_H
#define __CAMERA_H

#include "../Math3D/Math3D.h"

class Camera
{
public:
	Camera();
	Camera( Vector3D origin, Vector3D angles );

	virtual void	ApplyTranslation();
	virtual void	ApplyRotation();
	virtual void	CalculateAxes();

	Vector3D	origin;
	Vector3D	angles;

	Vector3D	forward, right, up;
};

#endif

