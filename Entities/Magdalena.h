#ifndef __MAGDALENA_H
#define __MAGDALENA_H

#include "../Entity/Entity.h"
#include "../Model/AnimSequence.h"

class Magdalena: public ModelEntity
{
public:
	Magdalena();

	void	Update();
	void	Draw( Renderer &renderer );

	void	Trigger( Entity *other, Entity *activator );
	void	Init();

	int		sequence;
	char	skin[32];
private:
	activeAnim_t	animation;
};

#endif

