#include "../Common.h"
#include "Speaker.h"
#include "../Shared/StringParser.h"
#include "../SharedVars.h"

Speaker::Speaker()
{
	sfx = -1;
	src = -1;
	looped = false;
	active = false;
	global = false;
	wait = 0;
	random = 0;
}

void Speaker::Init( char *filename, int wait, int random, int flags )
{
	if ( filename[0] == '*' )
		return;

	sfx = SoundManager::Instance()->LoadSound( filename );
	this->wait = wait;
	this->random = random;
	looped = (flags & (SPEAKER_LOOPING_ON | SPEAKER_LOOPING_OFF)) ? true : false;
	active = (flags & SPEAKER_LOOPING_ON) ? true : false;
	global = (flags & SPEAKER_GLOBAL) ? true : false;
}

void Speaker::PlaySound()
{
	float attenuation = global ? 0.0f : 1.0f;
	src = SoundManager::Instance()->PlaySound( sfx, origin, attenuation, s_volume, looped );
}

void Speaker::Update()
{
	if ( looped && active && src < 0 )
		PlaySound();
}

void Speaker::Draw( Renderer &renderer )
{
}

void Speaker::Trigger( Entity *other, Entity *activator )
{
	active = !active;

	if ( active )
	{
		PlaySound();
	}
	else
	{
		SoundManager::Instance()->StopSound( src );
	}
}

Entity *Create_Speaker( EntityMap &map )
{
	Speaker *speaker = new Speaker();
	
	char *waitstr = map.GetValue( "wait" );
	int wait = waitstr ? atoi( waitstr ) : 0;
	char *randomstr = map.GetValue( "random" );
	int random = randomstr ? atoi( randomstr ) : 0;
	char *flagsstr = map.GetValue( "spawnflags" );
	int flags = flagsstr ? atoi( flagsstr ) : 0;

	speaker->Init( map.GetValue( "noise" ), wait, random, flags );
	speaker->origin = StringParser::ParseVector3D( map.GetValue( "origin" ) );
	speaker->origin.QtoOGL();

	return speaker;
}

