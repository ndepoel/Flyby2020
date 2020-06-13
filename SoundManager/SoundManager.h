#ifndef __SOUNDMANAGER_H
#define __SOUNDMANAGER_H

#include <AL/al.h>
#include <AL/alc.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#pragma warning(disable: 4244)	// Disable warning about possible loss of data
#include <vorbis/vorbisfile.h>
#pragma warning(default: 4244)

#include "Input.h"
#include "WavInput.h"
#include "OggInput.h"
#include "../Math3D/Math3D.h"
#include "../SharedVars.h"

#define MAX_NUM_SOUNDS		1024
#define MAX_NUM_SOURCE		1024

typedef int sndHandle_t; // sound handle < 0 means invalid sound handle
typedef int srcHandle_t; // source handle < 0 means invalid source handle

#define WAV 1
#define OGG 2

typedef struct snd_s
{
	int type;
	InputHandle typeHandle;
	sndHandle_t handle;
}snd_t;

typedef struct source_s
{
	struct source_s	*prev, *next;
	ALuint  source;

	Vector3D	position;
	float		attenuation;
	float		gain;
	unsigned int	stopTime;
	sndHandle_t sndhandle;
	srcHandle_t srchandle;
} source_t;

class SoundManager
{
protected:
	SoundManager();

public:
	~SoundManager();

	static SoundManager *Instance();

	void		Update();

	void		FreeSource(srcHandle_t handle);
	void		FreeAll();
	sndHandle_t LoadSound(char *fileName);
	void		SetListener(Vector3D origin, Vector3D angles);
	void		SetListener( Vector3D origin, Vector3D forward, Vector3D up );

	srcHandle_t PlaySound(sndHandle_t handle, Vector3D origin, float attenuation, float gain, bool loop);
	void		StopSound(srcHandle_t handle);
	bool		IsPlaying(srcHandle_t handle);
private:
	void		Init();
	srcHandle_t GetSource();
	
	source_t	source[MAX_NUM_SOURCE];
	snd_s		allsounds[MAX_NUM_SOUNDS];
	source_t	*freeSource;
	source_t	activeSource;
	int			sndPos;
	Vector3D	listenerPosition;

	ALCdevice	*device;
	ALCcontext	*context;
};

#endif

