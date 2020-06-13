#include "Timeshift.h"
#include "../SharedVars.h"
#include "../Shared/StringParser.h"

Timeshift::Timeshift()
{
	factor = 0;
	fastforward = false;
	active = false;
}

void Timeshift::Update()
{
}

void Timeshift::Draw( Renderer &renderer )
{
}

void Timeshift::Trigger( Entity *other, Entity *activator )
{
	if ( !active )
	{
		active = true;
		hyperdrive = fastforward ? factor : -factor;
	}
	else
	{
		active = false;
		hyperdrive = 0;
	}
}

Entity *Create_Timeshift( EntityMap &map )
{
	Timeshift *ts = new Timeshift();

	ts->factor = StringParser::ParseInt( map.GetValue( "factor" ) );

	int spawnflags = StringParser::ParseInt( map.GetValue( "spawnflags" ) );
	ts->fastforward = (spawnflags & 1) != 0;

	return ts;
}

