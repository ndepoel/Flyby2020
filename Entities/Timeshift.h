#ifndef __TIMESHIFT_H
#define __TIMESHIFT_H

#include "../Entity/Entity.h"

class Timeshift: public Entity
{
public:
	Timeshift();

	void	Update();
	void	Draw( Renderer &renderer );

	void	Trigger( Entity *other, Entity *activator );

	int		factor;
	bool	fastforward;
	bool	active;
};

#endif

