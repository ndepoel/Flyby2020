#ifndef __ENTITYMAP_H
#define __ENTITYMAP_H

typedef struct entityKey_s
{
	struct entityKey_s	*next;

	char	name[64];
	char	value[64];
} entityKey_t;

class EntityMap
{
public:
	EntityMap();
	~EntityMap();

	void SetKey( char *key, char *value );
	char *GetValue( char *key );
private:
	void AddKey( char *key, char *value );

	entityKey_t	*keys;
};

#endif

