#include "AxisCamera.h"
#include "../OpenGL.h"

AxisCamera::AxisCamera()
{
	right = Vector3D( 1, 0, 0 );
	up = Vector3D( 0, 1, 0 );
	forward = Vector3D( 0, 0, -1 );
}

AxisCamera::AxisCamera( Vector3D origin, Vector3D angles )
{
	this->origin = origin;
	angles.AnglesToAxis( forward, right, up );
}

AxisCamera::AxisCamera( Vector3D origin, Vector3D axis[3] )
{
	this->origin = origin;
	this->right = axis[0];
	this->up = axis[1];
	this->forward = axis[2] * -1;
}

void AxisCamera::ApplyRotation()
{
	Matrix m;

	Vector3D z = forward * -1;
	z.Normalize();
	Vector3D x = up.CrossProduct( z );
	x.Normalize();
	Vector3D y = z.CrossProduct( x );
	y.Normalize();

	right = x;
	up = y;

	m.m[0] = x.x;	m.m[1] = y.x;	m.m[2] = z.x;	m.m[3] = 0;
	m.m[4] = x.y;	m.m[5] = y.y;	m.m[6] = z.y;	m.m[7] = 0;
	m.m[8] = x.z;	m.m[9] = y.z;	m.m[10] = z.z;	m.m[11] = 0;
	m.m[12] = 0;	m.m[13] = 0;	m.m[14] = 0;	m.m[15] = 1;

	glMultMatrixf( m.m );
}

void AxisCamera::CalculateAxes()
{
}

