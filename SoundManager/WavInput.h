#ifndef __WAVINPUT_H
#define __WAVINPUT_H

#include "SoundManager.h"
#include "Input.h"

#define MAX_NUM_SOUND_BUF	1024
typedef int soundHandle_t;

typedef struct sound_s
{
	ALenum		format;
	ALsizei		size;
	ALsizei		freq;
	ALuint		buffer;
	char		filename[64];

	unsigned int	length;		// in msec

	soundHandle_t handle;
} sound_t;


class WavInput : public Input
{

protected:
	WavInput();

public:
	static Input *Instance();
	~WavInput();

	 InputHandle LoadFile( char *filename );
	 void Free(InputHandle handle);
	 void FreeAll();
	 void Update();
	 void Play(InputHandle handle, bool loop, source_t *src);
	 void Stop(InputHandle handle);
	 void IsPlaying(InputHandle handle);

private:
	void		Init();
	sound_t		sounds[MAX_NUM_SOUND_BUF];
	int			freeSoundId;

};
#endif

