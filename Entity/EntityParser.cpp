#include "../Common.h"
#include "EntityParser.h"
#include "../Shared/StringParser.h"
#include "../Entities/entitydefs.h"

EntityParser::EntityParser()
{
	entMaps = NULL;
	curMap = NULL;
}

EntityParser::~EntityParser()
{
	entMap_t *cur = entMaps, *next;

	while ( cur )
	{
		next = cur->next;
		delete cur;
		cur = next;
	}
}

entMap_t *EntityParser::AddEntityMap()
{
	entMap_t *newmap = new entMap_t;
	newmap->next = entMaps;
	entMaps = newmap;

	return newmap;
}

void EntityParser::ParseEntities( char *str )
{
	char	*token;
	char	key[64], value[64];

	if ( !str )
		return;

	while ( true )
	{
		do
		{
			token = StringParser::Parse( &str );
		} while ( *token && strcmp( token, "{" ) );

		if ( !*token )
			break;
		
		entMap_t *map = AddEntityMap();
		while ( true )
		{
			token = StringParser::Parse( &str );
			if ( !strcmp( token, "}" ) )
				break;
			strcpy( key, token );

			token = StringParser::Parse( &str );
			if ( !strcmp( token, "}" ) )
				break;
			strcpy( value, token );

			map->map.SetKey( key, value );
		}
	}
}

void EntityParser::StartIteration()
{
	curMap = entMaps;
}

bool EntityParser::HasNextEntity()
{
	if ( curMap )
		return true;
	else
		return false;
}

Entity *EntityParser::CreateNextEntity()
{
	if ( !curMap )
		return NULL;

	char *classname = curMap->map.GetValue( "classname" );
	Entity *result = NULL;

	for ( int i = 0; i < numEntityDefs; i++ )
	{
		if ( !strcmp( classname, entityDefs[i].classname ) )
		{
			result = entityDefs[i].create( curMap->map );
			if ( result )
				break;
		}
	}

	if ( result )
	{
		result->origin = StringParser::ParseVector3D( curMap->map.GetValue( "origin" ) );
		result->origin.QtoOGL();

		strcpy( result->classname, classname );

		char *target = curMap->map.GetValue( "target" );
		if ( target )
			strcpy( result->target, target );
		else
			result->target[0] = 0;

		char *targetname = curMap->map.GetValue( "targetname" );
		if ( targetname )
			strcpy( result->targetname, targetname );
		else
			result->targetname[0] = 0;
	}

	curMap = curMap->next;
	return result;
}

EntityMap *EntityParser::FindEntity( char *classname )
{
	entMap_t *cur = entMaps;

	while ( cur )
	{
		if ( !strcmp( cur->map.GetValue( "classname" ), classname ) )
			return &cur->map;
		cur = cur->next;
	}

	return NULL;
}

