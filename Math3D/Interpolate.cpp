#include "../Common.h"
#include "Interpolate.h"

Vector3D Interpolate::Linear( Vector3D &v1, Vector3D &v2, float mu )
{
	float invmu = 1.0f - mu;
	return Vector3D(	v1.x * invmu + v2.x * mu,
						v1.y * invmu + v2.y * mu,
						v1.z * invmu + v2.z * mu );
}

Vector3D Interpolate::Cosine( Vector3D &v1, Vector3D &v2, float mu )
{
	mu = mu * 3.1415927f;
	mu = (1.0f - (float)cos(mu)) * 0.5f;

	float invmu = 1.0f - mu;
	return Vector3D(	v1.x * invmu + v2.x * mu,
						v1.y * invmu + v2.y * mu,
						v1.z * invmu + v2.z * mu );
}

Vector3D Interpolate::Cubic( Vector3D &v0, Vector3D &v1, Vector3D &v2, Vector3D &v3, float mu )
{
	float mu2 = mu * mu;
	float mu3 = mu2 * mu;

	Vector3D p = (v3 - v2) - (v0 - v1);
	Vector3D q = (v0 - v1) - p;
	Vector3D r = v2 - v0;
	Vector3D s = v1;

	return Vector3D(	p.x * mu3 + q.x * mu2 + r.x * mu + s.x,
						p.y * mu3 + q.y * mu2 + r.y * mu + s.y,
						p.z * mu3 + q.z * mu2 + r.z * mu + s.z );
}

Vector3D Interpolate::Hermite( Vector3D &v1, Vector3D &v2, Vector3D &t1, Vector3D &t2, float mu )
{
	float mu2 = mu * mu;
	float mu3 = mu2 * mu;

	float h1 = 2 * mu3 - 3 * mu2 + 1;
	float h2 = -2 * mu3 + 3 * mu2;
	float h3 = mu3 - 2 * mu2 + mu;
	float h4 = mu3 - mu2;

	return Vector3D(	v1.x * h1 + v2.x * h2 + t1.x * h3 + t2.x * h4,
						v1.y * h1 + v2.y * h2 + t1.y * h3 + t2.y * h4,
						v1.z * h1 + v2.z * h2 + t1.z * h3 + t2.z * h4 );
}

Vector3D Interpolate::Cardinal( Vector3D &v0, Vector3D &v1, Vector3D &v2, Vector3D &v3, float mu, float tightness )
{
	Vector3D t1 = (v2 - v0) * tightness;
	Vector3D t2 = (v3 - v1) * tightness;
	return Hermite( v1, v2, t1, t2, mu );
}

Vector3D Interpolate::CatmullRom( Vector3D &v0, Vector3D &v1, Vector3D &v2, Vector3D &v3, float mu )
{
	return Cardinal( v0, v1, v2, v3, mu, 0.5f );
}
