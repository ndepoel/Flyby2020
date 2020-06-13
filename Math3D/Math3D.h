#ifndef __MATH3D_H
#define __MATH3D_H

#define RAD2DEG		(57.2957795)
#define DEG2RAD		(0.01745329)

class Vector3D  
{
public:
	Vector3D();
	Vector3D( float x, float y, float z );
	Vector3D( float v[3] );
	Vector3D( int v[3] );

	Vector3D	operator+( const Vector3D &other ) const;
	Vector3D	operator-( const Vector3D &other ) const;
	Vector3D	operator*( const Vector3D &other ) const;
	Vector3D	operator*( float scale ) const;
	Vector3D	operator/( float scale ) const;

	void		Set( float x, float y, float z );
	float		DotProduct( const Vector3D &other ) const;
	Vector3D	CrossProduct( const Vector3D &other ) const;
	float		Length() const;
	float		LengthSqr() const;
	void		Normalize();
	Vector3D	Normalize2() const;
	float		Distance( const Vector3D &other ) const;

	void		AnglesToAxis( Vector3D &forward, Vector3D &right, Vector3D &up ) const;

	void		GetFloatArray( float *array ) const;
	void		QtoOGL();

	static Vector3D		LatLngToVector( float lat, float lng );

	float x, y, z;
};

class Plane  
{
public:
	Plane();
	Plane( float a, float b, float c, float d );
	Plane( const Vector3D &normal, float d );

	float	PointDistance( const Vector3D &point ) const;
	float	PointDistance2( const Vector3D &point ) const;
	bool	PointOnPlaneSide( const Vector3D &point ) const;
//	int		BoxOnPlaneSide( Vector3D &mins, Vector3D &maxs );
	void	Normalize();

	Vector3D	normal;
	float		dist;
};

class Matrix4x4
{
public:
	Matrix4x4();
	Matrix4x4( float m[16] );

	Matrix4x4	operator*( const Matrix4x4 &other ) const;
	Vector3D	operator*( const Vector3D &v ) const;

	void	LoadNull();
	void	LoadIdentity();

	float	m[16];
};

typedef Matrix4x4	Matrix;

#endif
