#ifndef __SPEAKER_H
#define __SPEAKER_H

#include "../Entity/Entity.h"
#include "../SoundManager/SoundManager.h"

#define SPEAKER_LOOPING_ON		1
#define SPEAKER_LOOPING_OFF		2
#define SPEAKER_GLOBAL			4

class Speaker: public Entity
{
public:
	Speaker();

	void	Init( char *filename, int wait, int random, int flags );

	void	Update();
	void	Draw( Renderer &renderer );

	void	Trigger( Entity *other, Entity *activator );
private:
	void	PlaySound();

	sndHandle_t	sfx;
	srcHandle_t src;

	bool	looped;
	bool	active;
	bool	global;
	int		wait;
	int		random;
};

#endif

