#ifndef __FRUSTUM_H
#define __FRUSTUM_H

#include "../Math3D/Math3D.h"

typedef enum
{
	PLANE_RIGHT = 0,
	PLANE_LEFT,
	PLANE_TOP,
	PLANE_BOTTOM,
	PLANE_BACK,
	PLANE_FRONT,
	PLANE_MAX_PLANE_TYPES
} planeType_s;

class Frustum
{
public:
	void ExtractViewFrustum();
	bool PointInFrustum( const Vector3D &p );
	bool SphereInFrustum( const Vector3D &mid, float radius );
	bool BoxInFrustum( const Vector3D &mins, const Vector3D &maxs );

private:
	Plane	planes[PLANE_MAX_PLANE_TYPES];
};

#endif

