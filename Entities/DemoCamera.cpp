#include "../Common.h"
#include "DemoCamera.h"
#include "CameraTarget.h"
#include "../Math3D/Interpolate.h"
#include "../Shared/StringParser.h"
#include "../SharedVars.h"

DemoCamera::DemoCamera()
{
	targets = NULL;
	initialized = false;
	initial = false;
	active = false;

	curTarget = NULL;
	mu = 0;
}

DemoCamera::~DemoCamera()
{
	camTarget_t *cur, *next;

	cur = targets;
	while ( cur )
	{
		next = cur->next;
		delete cur;
		cur = next;
	}
}

void DemoCamera::ResetCamera()
{
	if ( targets )
	{
		curTarget = targets->next;	// Skip the first target, that one's only for interpolation purposes
		if ( curTarget && curTarget->target )
		{
			camera.origin = curTarget->target->origin;
			camera.forward = curTarget->target->lookAt;
			camera.up = curTarget->target->up;
		}
	}
}

void DemoCamera::Update()
{
	// There's a good chance that most target_camera entities haven't been
	// initialized yet when this entity's Init() function is being called.
	// Therefore the camera will be initialized at the first update.
	if ( !initialized )
	{
		int count = 0;
		initialized = true;

		Entity *ent = targetEntity;
		while ( ent && !strcmp( ent->classname, "target_camera" ) )
		{
			CameraTarget *ct = (CameraTarget *)ent;
			if ( !AddCameraTarget( ct ) )
				break;

			ent = ct->targetEntity;

			count++;
		}

		ResetCamera();

		printf( "DemoCamera::Update() - Added %i camera targets to list\n", count );
	}

	// Non-active cameras don't move
	if ( !active )
		return;

	if ( curTarget )
	{
		CameraTarget *t0, *t1, *t2, *t3;
		t0 = t1 = t2 = t3 = NULL;

		if ( curTarget->prev )
			t0 = curTarget->prev->target;
		t1 = curTarget->target;
		if ( curTarget->next )
		{
			t2 = curTarget->next->target;
			if ( curTarget->next->next )
				t3 = curTarget->next->next->target;
		}

		if ( t0 && t1 && t2 && t3 )	// We need 4 points for interpolation
		{
			switch ( t1->type )
			{
			case CT_FIXED:
				camera.origin = t1->origin;
				camera.forward = t1->lookAt;
				camera.up = t1->up;
				break;
			case CT_LINEAR:
				camera.origin = Interpolate::Linear( t1->origin, t2->origin, mu );
				camera.forward = Interpolate::Linear( t1->lookAt, t2->lookAt, mu );
				camera.up = Interpolate::Linear( t1->up, t2->up, mu );
				break;
			case CT_COSINE:
				camera.origin = Interpolate::Cosine( t1->origin, t2->origin, mu );
				camera.forward = Interpolate::Cosine( t1->lookAt, t2->lookAt, mu );
				camera.up = Interpolate::Cosine( t1->up, t2->up, mu );
				break;
			case CT_CUBIC:
				camera.origin = Interpolate::Cubic( t0->origin, t1->origin, t2->origin, t3->origin, mu );
				camera.forward = Interpolate::Cubic( t0->lookAt, t1->lookAt, t2->lookAt, t3->lookAt, mu );
				camera.up = Interpolate::Cubic( t0->up, t1->up, t2->up, t3->up, mu );
				break;
			case CT_CARDINAL:
			default:
				camera.origin = Interpolate::CatmullRom( t0->origin, t1->origin, t2->origin, t3->origin, mu );
				camera.forward = Interpolate::CatmullRom( t0->lookAt, t1->lookAt, t2->lookAt, t3->lookAt, mu );
				camera.up = Interpolate::CatmullRom( t0->up, t1->up, t2->up, t3->up, mu );
				break;
			}

			// Interpolate time to create smooth speed changes
			float time = t1->time * (1.0f - mu) + t2->time * mu;

			if ( time > 0 )
				mu = mu + frametime / time;
			else
				mu = 1.0f;

			if ( mu >= 1.0f )
			{
				curTarget = curTarget->next;
				curTarget->target->Trigger( this, NULL );
				mu = 0;
			}
		}
		else	// We've reached the end of the path, so reset
		{
			ResetCamera();
		}
	}
}

void DemoCamera::Draw( Renderer &renderer )
{
}

void DemoCamera::Trigger( Entity *other, Entity *activator )
{
	SetActiveDemoCamera( this );
}

bool DemoCamera::AddCameraTarget( CameraTarget *ct )
{
	// First check if this camera target hasn't already been added.
	// If it has, we have a loop, so stop adding targets.
	camTarget_t *last = NULL, *cur = targets;
	while ( cur )
	{
		if ( !strcmp( cur->target->targetname, ct->targetname ) )
			return false;

		last = cur;
		cur = cur->next;
	}

	camTarget_t *result = new camTarget_t;
	result->target = ct;
	result->next = NULL;

	if ( last )
	{
		result->prev = last;
		last->next = result;
	}
	else
	{
		targets = result;
		result->prev = NULL;
	}

	return true;
}

Camera *DemoCamera::GetCamera()
{
	return &camera;
}

Entity *Create_DemoCamera( EntityMap &map )
{
	DemoCamera *cam = new DemoCamera();

	int spawnflags = StringParser::ParseInt( map.GetValue( "spawnflags" ) );
	cam->initial = (spawnflags & 1) != 0;

	return cam;
}

