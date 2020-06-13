#ifndef __OGGINPUT_H
#define __OGGINPUT_H

#include "SoundManager.h"
#include "Input.h"
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#define MAX_NUM_OGG_STREAMS 16
#define	NUM_BUF_PER_STREAM	4
#define BUF_SIZE			65536

typedef struct oggStream_s
{
	ALuint			buffers[NUM_BUF_PER_STREAM];
	OggVorbis_File	oggStream;
	ALsizei			oggFormat;
	vorbis_info		*oggInfo;
	bool			loopOgg;

} oggStream_t;


typedef struct Stream_s
{
	struct		Stream_s	*prev, *next;
	oggStream_t Stream;
	InputHandle	handle;
	source_t *src;

} Stream_t;

class OggInput : public Input
{

protected:
	OggInput();
public:
	static Input *Instance();
	~OggInput();

	InputHandle LoadFile( char *fileName);
	void Play(InputHandle handle, bool loop, source_t *src);
	void Stop(InputHandle handle);
	void IsPlaying(InputHandle handle);
	void FreeAll();
	void Free(InputHandle handle);
	void Update();
private:
	void		Init();
	bool StreamOgg( ALuint buffer, Stream_t *src);
	Stream_t *getStream();
	Stream_t streams[MAX_NUM_OGG_STREAMS];
	Stream_t *freeStreams;
	Stream_t activeStreams;
};
#endif

