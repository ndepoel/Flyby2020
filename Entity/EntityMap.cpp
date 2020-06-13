#include "../Common.h"
#include "EntityMap.h"

EntityMap::EntityMap()
{
	keys = NULL;
}

EntityMap::~EntityMap()
{
	entityKey_t *cur = keys, *next;

	while ( cur )
	{
		next = cur->next;
		delete cur;
		cur = next;
	}
}

void EntityMap::AddKey( char *key, char *value )
{
	entityKey_t *newkey = new entityKey_t;
	strcpy( newkey->name, key );
	strcpy( newkey->value, value );
	newkey->next = keys;
	keys = newkey;
}

void EntityMap::SetKey( char *key, char *value )
{
	entityKey_t *cur = keys;

	while ( cur )
	{
		if ( !strcmp( cur->name, key ) )
		{
			strcpy( cur->value, value );
			return;
		}

		cur = cur->next;
	}

	AddKey( key, value );
}

char *EntityMap::GetValue( char *key )
{
	entityKey_t *cur = keys;

	while ( cur )
	{
		if ( !strcmp( cur->name, key ) )
			return cur->value;

		cur = cur->next;
	}

	return NULL;
}

