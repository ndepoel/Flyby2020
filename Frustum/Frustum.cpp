#include "../OpenGL.h"
#include "Frustum.h"

void Frustum::ExtractViewFrustum()
{
	GLfloat	m[16], clip[16];
	Plane	*plane;

	glGetFloatv( GL_MODELVIEW_MATRIX, m );
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
		glMultMatrixf( m );
		glGetFloatv( GL_PROJECTION_MATRIX, clip );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );

	plane = &planes[PLANE_RIGHT];
	plane->normal.x = clip[3] - clip[0];
	plane->normal.y = clip[7] - clip[4];
	plane->normal.z = clip[11] - clip[8];
	plane->dist = clip[15] - clip[12];
	plane->Normalize();

	plane = &planes[PLANE_LEFT];
	plane->normal.x = clip[3] + clip[0];
	plane->normal.y = clip[7] + clip[4];
	plane->normal.z = clip[11] + clip[8];
	plane->dist = clip[15] + clip[12];
	plane->Normalize();

	plane = &planes[PLANE_TOP];
	plane->normal.x = clip[3] - clip[1];
	plane->normal.y = clip[7] - clip[5];
	plane->normal.z = clip[11] - clip[9];
	plane->dist = clip[15] - clip[13];
	plane->Normalize();

	plane = &planes[PLANE_BOTTOM];
	plane->normal.x = clip[3] + clip[1];
	plane->normal.y = clip[7] + clip[5];
	plane->normal.z = clip[11] + clip[9];
	plane->dist = clip[15] + clip[13];
	plane->Normalize();

	plane = &planes[PLANE_BACK];
	plane->normal.x = clip[3] - clip[2];
	plane->normal.y = clip[7] - clip[6];
	plane->normal.z = clip[11] - clip[10];
	plane->dist = clip[15] - clip[14];
	plane->Normalize();

	plane = &planes[PLANE_FRONT];
	plane->normal.x = clip[3] + clip[2];
	plane->normal.y = clip[7] + clip[6];
	plane->normal.z = clip[11] + clip[10];
	plane->dist = clip[15] + clip[14];
	plane->Normalize();
}

bool Frustum::PointInFrustum( const Vector3D &p )
{
	return SphereInFrustum( p, 0 );
}

bool Frustum::SphereInFrustum( const Vector3D &mid, float radius )
{
	for ( int i = 0; i < PLANE_MAX_PLANE_TYPES; i++ )
		if ( planes[i].PointDistance( mid ) <= -radius )
			return false;

	return true;
}

bool Frustum::BoxInFrustum( const Vector3D &mins, const Vector3D &maxs )
{
	for ( int i = 0; i < PLANE_MAX_PLANE_TYPES; i++ )
	{
		Plane *plane = &planes[i];
		if ( plane->PointDistance( Vector3D( mins.x, mins.y, mins.z ) ) > 0 ) continue;
		if ( plane->PointDistance( Vector3D( mins.x, mins.y, maxs.z ) ) > 0 ) continue;
		if ( plane->PointDistance( Vector3D( mins.x, maxs.y, mins.z ) ) > 0 ) continue;
		if ( plane->PointDistance( Vector3D( mins.x, maxs.y, maxs.z ) ) > 0 ) continue;
		if ( plane->PointDistance( Vector3D( maxs.x, mins.y, mins.z ) ) > 0 ) continue;
		if ( plane->PointDistance( Vector3D( maxs.x, mins.y, maxs.z ) ) > 0 ) continue;
		if ( plane->PointDistance( Vector3D( maxs.x, maxs.y, mins.z ) ) > 0 ) continue;
		if ( plane->PointDistance( Vector3D( maxs.x, maxs.y, maxs.z ) ) > 0 ) continue;

		return false;
	}

	return true;
}

