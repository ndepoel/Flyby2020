#ifndef __PARTICLEEMITTER_H
#define __PARTICLEEMITTER_H

#include "../Entity/Entity.h"
#include "../TextureManager/TextureManager.h"

#define PE_LINEAR		0
#define PE_GRAVITY		1

typedef struct
{
	Vector3D		origin;
	Vector3D		velocity;
	float			alpha;
	float			size;
	unsigned int	fadetime;
	unsigned int	startTime;
	unsigned int	endTime;
} particle_t;

class ParticleEmitter: public Entity
{
public:
	ParticleEmitter();
	~ParticleEmitter();
		
	void		Update();
	void		Draw( Renderer &renderer );

	void		Init();
	
	int			maxParticles;
	texHandle_t	texture;
	float		alpha;
	float		size;
	float		fadetime;
	float		lifetime;
	Vector3D	velocity;
	int			movetype;
	float		variation;
	Vector3D	originVariation;
private:
	void		SpawnParticle( particle_t *p );

	particle_t	*particles;
};

#endif

