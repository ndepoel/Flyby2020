#ifndef __MODEL_H
#define __MODEL_H

#include "../Math3D/Math3D.h"

class Model
{
public:
	virtual bool Load( char *filename ) = 0;
	virtual void Free() = 0;
	
	virtual int GetFrameCount() = 0;
	virtual void Draw( int lastframe, int curframe, float mu ) = 0;
	virtual void DrawNormals( int lastframe, int curframe, float mu ) = 0;

	virtual void GetBoundingBox( int frame, Vector3D *mins, Vector3D *maxs ) = 0;
	virtual float GetRadius( int frame ) = 0;
};

#endif

