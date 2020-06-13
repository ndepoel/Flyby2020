#include "../Common.h"
#include "SoundManager.h"

#define FADE_MIN_DISTANCE	600
#define FADE_MAX_DISTANCE	1000

SoundManager::SoundManager()
{
	Init();
}

SoundManager::~SoundManager()
{
	FreeAll();

	alcDestroyContext( context );
	alcCloseDevice( device );
}

SoundManager *SoundManager::Instance()
{
	static SoundManager instance;

	return &instance;
}

void SoundManager::Init()
{
	int i;

	// Initialize Open AL
	device = alcOpenDevice(NULL); // open default device
	if (device != NULL) 
	{
		context = alcCreateContext(device,NULL); // create context
		if (context != NULL) 
		{
			alcMakeContextCurrent(context); // set active context
		}
	}

	alDistanceModel( AL_INVERSE_DISTANCE );
	alListenerf( AL_GAIN, 1 );

	memset( source, 0, sizeof( source ) );
	memset( allsounds, 0, sizeof( allsounds ) );
	activeSource.next = &activeSource;
	activeSource.prev = &activeSource;

	freeSource = &source[0];

	for ( i = 0; i < MAX_NUM_SOURCE-1; i++ )
	{
		source[i].next = &source[i+1];
		source[i].srchandle = i;
	}

	source[MAX_NUM_SOURCE-1].srchandle = MAX_NUM_SOURCE-1;

	SetListener( Vector3D( 0, 0, 0 ), Vector3D( 0, 0, 0 ) );
	for (i =0; i< MAX_NUM_SOUNDS; i++)
	{
		allsounds[i].handle = i;
	}
	sndPos = 0;
}

void SoundManager::Update()
{
	float dist, gain;
	source_t *src, *prev;

	for ( src = activeSource.prev; src != &activeSource; src = prev )
	{
		// Check if source is done playing, if so free
		prev = src->prev;
		if ( src->stopTime && src->stopTime < time )
			FreeSource( src->srchandle );

		// Silence sound if it is far away from the listener
		dist = listenerPosition.Distance( src->position );
		if ( src->attenuation <= 0.0f )
			gain = 1;
		else if ( dist < FADE_MIN_DISTANCE )
			gain = 1;
		else if ( dist > FADE_MAX_DISTANCE )
			gain = 0;
		else
			gain = ( FADE_MAX_DISTANCE - dist ) / ( FADE_MAX_DISTANCE - FADE_MIN_DISTANCE );
		alSourcef( src->source, AL_GAIN, gain * src->gain );
	}
	WavInput::Instance()->Update();
	OggInput::Instance()->Update();
}

void SoundManager::FreeSource(srcHandle_t handle)
{
	if (handle <0 || handle >= MAX_NUM_SOURCE)
		return;

	source_t *src = &source[handle];

	if (!src->prev)
		return;

	if (IsPlaying(handle))
		alSourceStop(src->source);

	src->prev->next = src->next;
	src->next->prev = src->prev;

	src->next = freeSource;
	src->prev = NULL;
	freeSource = src;

	if ( src->source )
		alDeleteSources(1, &src->source);

	printf( "SoundManager::FreeSource() - Freed source %i\n", handle );
}

void SoundManager::FreeAll()
{
	source_t *src, *prev;
	
	for ( src = activeSource.prev; src != &activeSource; src = prev )
	{
		prev = src->prev;

		FreeSource( src->srchandle );
	}

	WavInput::Instance()->FreeAll();
	OggInput::Instance()->FreeAll();
}

sndHandle_t SoundManager::LoadSound(char *fileName)
{
	int id;


	if (sndPos < MAX_NUM_SOUNDS)
	{
		id = sndPos;
		sndPos++;
	}else return -1;

	if (strstr(fileName, ".wav"))
	{
		allsounds[id].type = WAV;
		allsounds[id].typeHandle = WavInput::Instance()->LoadFile(fileName);
	}
	if (strstr(fileName, ".ogg"))
	{
		allsounds[id].type = OGG;
		allsounds[id].typeHandle = OggInput::Instance()->LoadFile(fileName);
	}

	if (allsounds[id].typeHandle < 0) 
	{	
		sndPos--;
		return -1;
	}

	printf( "SoundManager::LoadSound() - unique id %i with buffer %i\n", id, allsounds[id].typeHandle );
	return id;
}

srcHandle_t SoundManager::GetSource()
{
	source_t *src;

	if ( !freeSource )
		return -1;

	src = freeSource;
	freeSource = freeSource->next;

	src->next = activeSource.next;
	src->prev = &activeSource;
	activeSource.next->prev = src;
	activeSource.next = src;

	alGenSources(1, &src->source);

	return src->srchandle;
}

void SoundManager::SetListener(Vector3D origin, Vector3D angles)
{
	static Vector3D fw,rt,up;
	
	angles.AnglesToAxis(fw,rt,up);
	
	SetListener( origin, fw, up );
}

void SoundManager::SetListener( Vector3D origin, Vector3D forward, Vector3D up )
{
	static float orient[6];

	listenerPosition = origin;
	alListenerfv( AL_POSITION, &listenerPosition.x );

	orient[0] = forward.x;
	orient[1] = forward.y;
	orient[2] = forward.z;
	orient[3] = up.x;
	orient[4] = up.y;
	orient[5] = up.z;

	alListenerfv( AL_ORIENTATION, orient );
}

srcHandle_t SoundManager::PlaySound(sndHandle_t handle, Vector3D origin, float attenuation, float gain, bool loop)
{
	srcHandle_t i = GetSource();
	if (i < 0)
		return -1;

	source_t *src = &source[i];

	src->position = origin;
	src->attenuation = attenuation;
	src->gain = gain;

	alSourcefv( src->source, AL_POSITION, &src->position.x );
	alSourcef( src->source, AL_ROLLOFF_FACTOR, src->attenuation );
	if ( src->attenuation == 0.0f )
		alSourcei( src->source, AL_SOURCE_RELATIVE, AL_TRUE );	// Small hack to make global sounds work
	alSourcef( src->source, AL_REFERENCE_DISTANCE, 75 );
	alSourcef( src->source, AL_GAIN, 0 );

	switch(allsounds[handle].type)
	{
	case WAV: WavInput::Instance()->Play(allsounds[handle].typeHandle, loop, src);
		break;
	case OGG: OggInput::Instance()->Play(allsounds[handle].typeHandle, loop, src);
		break;
	}

	alSourcePlay( src->source );

	printf( "SoundManager::PlaySound() - Playing sound %i with source %i at %f %f %f\n", handle, i, origin.x, origin.y, origin.z );

	return i;
}

void SoundManager::StopSound(srcHandle_t handle)
{
	if ( handle < 0 || handle >= MAX_NUM_SOURCE )
		return;

	source_t *src = &source[handle];

	alSourceStop(src->source);

	switch(allsounds[src->sndhandle].type)
	{
	case WAV: WavInput::Instance()->Stop(allsounds[src->sndhandle].typeHandle);
		break;
	case OGG: OggInput::Instance()->Stop(allsounds[src->sndhandle].typeHandle);
		break;
	}


	FreeSource(handle);
}

bool SoundManager::IsPlaying(srcHandle_t handle)
{
	if ( handle < 0 || handle >= MAX_NUM_SOURCE )
		return false;

	ALenum state;
	alGetSourcei( source[handle].source, AL_SOURCE_STATE, &state );
	return (state == AL_PLAYING);
}

