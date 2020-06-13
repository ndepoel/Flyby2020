#include "../Common.h"
#include "CameraTarget.h"
#include "../Shared/StringParser.h"

CameraTarget::CameraTarget()
{
	up = Vector3D( 0, 1, 0 );
	lookAt = Vector3D( 0, 0, -1 );

	lookName[0] = 0;
	speed = 0;
	time = 0;
	type = 0;
}

void CameraTarget::Update()
{
}

void CameraTarget::Draw( Renderer &renderer )
{
}

void CameraTarget::Init()
{
	targetEntity = FindEntityForTargetname( target );
	if ( speed && targetEntity )
	{
		float dist = (targetEntity->origin - origin).Length();
		time = dist / speed;
	}
	if ( !time )
		time = 2.5f;

	Entity *lookEnt = FindEntityForTargetname( lookName );
	if ( lookEnt )
		lookAt = lookEnt->origin - origin;
	else if ( targetEntity )
		lookAt = targetEntity->origin - origin;
	else
		lookAt = Vector3D( 0, 0, -1 );
	
	lookAt.Normalize();

	triggerEnt = FindEntityForTargetname( triggerName );

	//
/*	if ( targetEntity )
	{
		lookAt = targetEntity->origin - origin;
		lookAt.Normalize();
	}
	else
	{
		lookAt = Vector3D( 0, 0, -1 );
	} */
	//
}

void CameraTarget::Trigger( Entity *other, Entity *activator )
{
	if ( triggerEnt )
		triggerEnt->Trigger( this, activator );
}

Entity *Create_CameraTarget( EntityMap &map )
{
	CameraTarget *result = new CameraTarget();

	result->up = StringParser::ParseVector3D( map.GetValue( "up" ) );
	result->up.QtoOGL();
	if ( result->up.LengthSqr() == 0 )
		result->up = Vector3D( 0, 1, 0 );
	result->up.Normalize();

	char *look = map.GetValue( "look" );
	if ( look )
		strcpy( result->lookName, look );
	else
		result->lookName[0] = 0;

	char *trigger = map.GetValue( "trigger" );
	if ( trigger )
		strcpy( result->triggerName, trigger );
	else
		result->triggerName[0] = 0;

	result->speed = StringParser::ParseFloat( map.GetValue( "speed" ) );
	result->time = StringParser::ParseFloat( map.GetValue( "time" ) );
	result->type = StringParser::ParseInt( map.GetValue( "type" ) );

	return result;
}

