#ifndef __DRAGON_H
#define __DRAGON_H

#include "../Entity/Entity.h"
#include "../Model/AnimSequence.h"
#include "../SoundManager/SoundManager.h"

class Dragon: public ModelEntity
{
public:
	Dragon();

	void	Update();
	void	Draw( Renderer &renderer );

	void	Trigger( Entity *other, Entity *activator );
	void	Init();

	int		sequence;
	char	skin[32];
private:
	activeAnim_t	animation;

	sndHandle_t		scream;
};

#endif

