#include "Camera.h"
#include "../OpenGL.h"

Camera::Camera()
{
	origin = Vector3D( 0, 0, 0 );
	angles = Vector3D( 0, 0, 0 );
}

Camera::Camera( Vector3D origin, Vector3D angles )
{
	this->origin = origin;
	this->angles = angles;
}

void Camera::ApplyTranslation()
{
	glTranslatef( -origin.x, -origin.y, -origin.z );
}

void Camera::ApplyRotation()
{
	angles.AnglesToAxis( forward, right, up );

	Matrix m;

	Vector3D z = forward * -1;
	Vector3D x = up.CrossProduct( z );
	Vector3D y = z.CrossProduct( x );

	m.m[0] = x.x;	m.m[1] = y.x;	m.m[2] = z.x;	m.m[3] = 0;
	m.m[4] = x.y;	m.m[5] = y.y;	m.m[6] = z.y;	m.m[7] = 0;
	m.m[8] = x.z;	m.m[9] = y.z;	m.m[10] = z.z;	m.m[11] = 0;
	m.m[12] = 0;	m.m[13] = 0;	m.m[14] = 0;	m.m[15] = 1;

	glMultMatrixf( m.m );

//	glRotatef( -angles.z, 0, 0, 1 );
//	glRotatef( -angles.x, 1, 0, 0 );
//	glRotatef( -angles.y, 0, 1, 0 );
}

void Camera::CalculateAxes()
{
	angles.AnglesToAxis( forward, right, up );
}

