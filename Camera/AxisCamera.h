#ifndef __AXISCAMERA_H
#define __AXISCAMERA_H

#include "Camera.h"

class AxisCamera: public Camera
{
public:
	AxisCamera();
	AxisCamera( Vector3D origin, Vector3D angles );
	AxisCamera( Vector3D origin, Vector3D axis[3] );

	void	ApplyRotation();
	void	CalculateAxes();
};

#endif

