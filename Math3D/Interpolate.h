#ifndef __INTERPOLATE_H
#define __INTERPOLATE_H

#include "Math3D.h"

class Interpolate
{
public:
	static Vector3D Linear( Vector3D &v1, Vector3D &v2, float mu );
	static Vector3D Cosine( Vector3D &v1, Vector3D &v2, float mu );
	static Vector3D Cubic( Vector3D &v0, Vector3D &v1, Vector3D &v2, Vector3D &v3, float mu );
	static Vector3D Hermite( Vector3D &v1, Vector3D &v2, Vector3D &t1, Vector3D &t2, float mu );
	static Vector3D Cardinal( Vector3D &v0, Vector3D &v1, Vector3D &v2, Vector3D &v3, float mu, float tightness );
	static Vector3D CatmullRom( Vector3D &v0, Vector3D &v1, Vector3D &v2, Vector3D &v3, float mu );
};

#endif

