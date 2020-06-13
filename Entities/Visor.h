#ifndef __VISOR_H
#define __VISOR_H

#include "../Entity/Entity.h"
#include "../Model/AnimSequence.h"

class Visor: public ModelEntity
{
public:
	Visor();

	void	Update();
	void	Draw( Renderer &renderer );
private:
	activeAnim_t	animation;
};

#endif

