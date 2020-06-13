#ifndef __ENTITYPARSER_H
#define __ENTITYPARSER_H

#include "EntityMap.h"
#include "Entity.h"

typedef struct entMap_s
{
	struct entMap_s *next;

	EntityMap map;
} entMap_t;

class EntityParser
{
public:
	EntityParser();
	~EntityParser();

	void		ParseEntities( char *str );

	void		StartIteration();
	bool		HasNextEntity();
	Entity		*CreateNextEntity();
	EntityMap	*FindEntity( char *classname );
private:
	entMap_t	*AddEntityMap();

	entMap_t	*entMaps;
	entMap_t	*curMap;
};

#endif

