#include "../Common.h"
#include "ParticleEmitter.h"
#include "../Shared/StringParser.h"
#include "../SharedVars.h"

static float frand()
{
	return (float)rand() / (float)RAND_MAX;
}

static float sfrand()
{
	return (frand() - 0.5f) * 2;
}

ParticleEmitter::ParticleEmitter()
{
	maxParticles = 0;
	texture = -1;
	alpha = 1;
	size = 10;
	fadetime = 0;
	lifetime = 1.0f;
	movetype = PE_LINEAR;
	variation = 0;

	particles = NULL;
}

ParticleEmitter::~ParticleEmitter()
{
	if ( particles )
	{
		delete[] particles;
		particles = NULL;
	}
}

void ParticleEmitter::Init()
{
	particles = new particle_t[ maxParticles ];

	for ( int i = 0; i < maxParticles; i++ )
	{
		particle_t *p = &particles[i];
		SpawnParticle( p );

		int timeoffset = (int)(lifetime * frand());
		p->startTime += timeoffset;
		p->endTime += timeoffset;
	}
}

void ParticleEmitter::SpawnParticle( particle_t *p )
{
	p->origin = origin;
	p->origin.x += originVariation.x * sfrand();
	p->origin.y += originVariation.y * sfrand();
	p->origin.z += originVariation.z * sfrand();

	p->velocity = velocity;
	p->velocity.x += velocity.x * variation * sfrand();
	p->velocity.y += velocity.y * variation * sfrand();
	p->velocity.z += velocity.z * variation * sfrand();

	p->alpha = alpha + alpha * variation * sfrand();
	p->size = size + size * variation * sfrand();
	p->fadetime = (int)(fadetime + fadetime * variation * sfrand());
	p->startTime = time;
	p->endTime = time + (int)(lifetime + lifetime * variation * sfrand());
}

void ParticleEmitter::Update()
{
	for ( int i = 0; i < maxParticles; i++ )
	{
		particle_t *p = &particles[i];
		if ( p->startTime > time )
			continue;

		if ( p->endTime < time )
			SpawnParticle( p );

		switch( movetype )
		{
		case PE_LINEAR:
			p->origin = p->origin + p->velocity * frametime;
			break;
		case PE_GRAVITY:
			p->velocity.y -= 800 * frametime;
			p->origin = p->origin + p->velocity * frametime;
			break;
		}
	}
}

void ParticleEmitter::Draw( Renderer &renderer )
{
	for ( int i = 0; i < maxParticles; i++ )
	{
		particle_t *p = &particles[i];
		if ( p->startTime > time )
			continue;

		float fadeAlpha = 1;
		if ( p->endTime - p->fadetime < time )
			fadeAlpha = (float)(p->endTime - time) / (float)p->fadetime;
		else if ( p->startTime + p->fadetime > time )
			fadeAlpha = (float)(time - p->startTime) / (float)p->fadetime;

		renderer.AddBillboard( p->origin, p->size, texture, p->alpha * fadeAlpha );
	}
}

Entity *Create_ParticleEmitter( EntityMap &map )
{
	ParticleEmitter *pe = new ParticleEmitter();

	pe->maxParticles = StringParser::ParseInt( map.GetValue( "maxparticles" ) );
	pe->texture = TextureManager::Instance()->LoadImage( map.GetValue( "texture" ) );
	pe->alpha = StringParser::ParseFloat( map.GetValue( "alpha" ) );
	pe->size = StringParser::ParseFloat( map.GetValue( "size" ) );
	pe->fadetime = StringParser::ParseFloat( map.GetValue( "fadetime" ) ) * 1000.0f;
	pe->lifetime = StringParser::ParseFloat( map.GetValue( "lifetime" ) ) * 1000.0f;
	pe->velocity = StringParser::ParseVector3D( map.GetValue( "velocity" ) );
	pe->movetype = StringParser::ParseInt( map.GetValue( "movetype" ) );
	pe->variation = StringParser::ParseFloat( map.GetValue( "variation" ) ) / 100.0f;
	pe->originVariation = StringParser::ParseVector3D( map.GetValue( "originvariation" ) );

	pe->velocity.QtoOGL();
	pe->originVariation.QtoOGL();

	return pe;
}

