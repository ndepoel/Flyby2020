#include "../Common.h"
#include "Math3D.h"

/*
============
Vector3D
============
*/

Vector3D::Vector3D() 
{
	x = y = z = 0.0f;
}

Vector3D::Vector3D( float x, float y, float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3D::Vector3D( float v[3] )
{
	x = v[0];
	y = v[1];
	z = v[2];
}

Vector3D::Vector3D( int v[3] )
{
	x = (float)v[0];
	y = (float)v[1];
	z = (float)v[2];
}

Vector3D Vector3D::operator+( const Vector3D &other ) const
{
	return Vector3D( x + other.x, y + other.y, z + other.z );
}

Vector3D Vector3D::operator-( const Vector3D &other ) const
{
	return Vector3D( x - other.x, y - other.y, z - other.z );
}

Vector3D Vector3D::operator*( const Vector3D &other ) const
{
	return Vector3D( x * other.x, y * other.y, z * other.z );
}

Vector3D Vector3D::operator*( float scale ) const
{
	return Vector3D( x * scale, y * scale, z * scale );
}

Vector3D Vector3D::operator/( float scale ) const
{
	float invscale = 1.0f / scale;
	return Vector3D( x * invscale, y * invscale, z * invscale );
}

void Vector3D::Set( float x, float y, float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
}

float Vector3D::DotProduct( const Vector3D &other ) const
{
	return ( x * other.x + y * other.y + z * other.z );
}

Vector3D Vector3D::CrossProduct( const Vector3D &other ) const
{
	return Vector3D(	y * other.z - z * other.y, 
						z * other.x - x * other.z,
						x * other.y - y * other.x );
}

float Vector3D::Length() const
{
	float lengthSqr = x*x + y*y + z*z;
	if ( lengthSqr == 0 || lengthSqr == 1 )
		return lengthSqr;
	else
		return (float)sqrt( lengthSqr );
}

float Vector3D::LengthSqr() const
{
	return x*x + y*y + z*z;
}

void Vector3D::Normalize()
{
	float len = Length();
	if ( len != 0.0f )
	{
		float invlen = 1.0f / len;
		x = x * invlen;
		y = y * invlen;
		z = z * invlen;
	}
}

Vector3D Vector3D::Normalize2() const
{
	float len = Length();
	if ( len != 0.0f )
	{
		float invlen = 1.0f / len;
		return Vector3D( x * invlen, y * invlen, z * invlen );
	}
	else
		return *this;
}

float Vector3D::Distance( const Vector3D &other ) const
{
	return (*this - other).Length();
}

void Vector3D::AnglesToAxis( Vector3D &forward, Vector3D &right, Vector3D &up ) const
{
	float sx, sy, sz, cx, cy, cz;
	float xrad = x * (float)DEG2RAD;
	float yrad = y * (float)DEG2RAD;
	float zrad = z * (float)DEG2RAD;

	sx = (float)sin( xrad );
	cx = (float)cos( xrad );
	sy = (float)sin( yrad );
	cy = (float)cos( yrad );
	sz = (float)sin( zrad );
	cz = (float)cos( zrad );

	right.x = cy * cz;
	right.y = -1 * cy * sz;
	right.z = -sy;

	up.x = (sx * sy * cz) + (cx * sz);
	up.y = (-1 * sx * sy * sz) + (cx * cz);
	up.z = (sx * cy);

	forward.x = (-1 * cx * sy * cz) + (sx * sz);
	forward.y = (cx * sy * sz) + (sx * cz);
	forward.z = (-1 * cx * cy);
}

void Vector3D::GetFloatArray( float *array ) const
{
	array[0] = x;
	array[1] = y;
	array[2] = z;
}

void Vector3D::QtoOGL()
{
	float tmp = y;
	y = z;
	z = -tmp;
}

Vector3D Vector3D::LatLngToVector( float lat, float lng )
{
	float v[3];

	v[0] = (float)( cos( lat ) * sin( lng ) );
	v[1] = (float)( sin( lat ) * sin( lng ) );
	v[2] = (float)( cos( lng ) );

	return Vector3D( v );
}

/*
=============
Plane
=============
*/

Plane::Plane()
{
	this->normal = Vector3D( 0, 0, 0 );
	this->dist = 0;
}

Plane::Plane( float a, float b, float c, float d )
{
	this->normal = Vector3D( a, b, c );
	this->dist = d;
	Normalize();
}

Plane::Plane( const Vector3D &normal, float d )
{
	this->normal = normal;
	this->dist = d;
	Normalize();
}

float Plane::PointDistance( const Vector3D &point ) const
{
	return point.DotProduct( normal ) + dist;
}

float Plane::PointDistance2( const Vector3D &point ) const
{
	return point.DotProduct( normal ) - dist;
}

bool Plane::PointOnPlaneSide( const Vector3D &point ) const
{
	return ( PointDistance( point ) > 0.0f );
}

/*int Plane::BoxOnPlaneSide( Vector3D mins, Vector3D maxs )
{
	return 0;
}*/

void Plane::Normalize()
{
	float div;

	div = 1.0f / normal.Length();
	normal = normal * div;
	dist *= div;
}

/*
============
Matrix4x4
============
*/

Matrix4x4::Matrix4x4()
{
	LoadIdentity();
}

Matrix4x4::Matrix4x4( float m[16] )
{
	for ( int i = 0; i < 16; i++ )
		this->m[i] = m[i];
}

Matrix4x4 Matrix4x4::operator*( const Matrix4x4 &other ) const
{
	float mat[16];

	for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ )
			mat[j*4+i] =	m[0*4+i] * other.m[j*4+0] +
							m[1*4+i] * other.m[j*4+1] +
							m[2*4+i] * other.m[j*4+2] +
							m[3*4+i] * other.m[j*4+3];

	return Matrix4x4( mat );
}

Vector3D Matrix4x4::operator*( const Vector3D &v ) const
{
	float vec[3];

    vec[0] =	v.x * m[0] +
				v.y * m[4] +
				v.z * m[8] +
				m[12];
    vec[1] =	v.x * m[1] +
				v.y * m[5] +
				v.z * m[9] +
				m[13];
    vec[2] =	v.x * m[2] +
				v.y * m[6] +
				v.z * m[10] +
				m[14];

	return Vector3D( vec );
}

void Matrix4x4::LoadNull()
{
	for ( int i = 0; i < 15; i++ )
		m[i] = 0;
}

void Matrix4x4::LoadIdentity()
{
	LoadNull();
	m[0] = m[5] = m[10] = m[15] = 1;
}

