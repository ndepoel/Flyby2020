#include "../Common.h"
#include "WavInput.h"
#include "SoundManager.h"

WavInput::WavInput()
{
	Init();
}

WavInput::~WavInput()
{
	FreeAll();
}

Input *WavInput::Instance()
{
	static WavInput instance;

	return &instance;
}

void WavInput::Init()
{

	memset( sounds, 0, sizeof( sounds ) );

	for (int i = 0; i < MAX_NUM_SOUND_BUF; i++ )
		sounds[i].handle = i;

	freeSoundId = 0;
}

struct WaveStructure
{
	struct
	{
		unsigned int id;
		unsigned int size;
		unsigned int format;
	} header;

	struct
	{
		unsigned int id;
		unsigned int size;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned int sampleRate;
		unsigned int byteRate;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
	} format;

	struct
	{
		unsigned int id;
		unsigned int size;
	} dataHeader;
};

InputHandle WavInput::LoadFile( char *fileName )
{
	for (int i=0; i < freeSoundId; i++)
	{
		if (!strcmp(fileName, sounds[i].filename))
		{
			printf( "Sound '%s' already loaded, returning id %i\n", fileName, i );
			return i;
		}
	}

	if ( freeSoundId >= MAX_NUM_SOUND_BUF )
		return -1;

	FILE *fp;
	if ( !( fp = fopen( fileName, "rb" ) ) )
		return -1;

	// Allocate a sound
	sound_t *snd = &sounds[freeSoundId];
	strcpy( snd->filename, fileName );
	alGenBuffers(1, &snd->buffer);

	WaveStructure wav;

	// Read WAVE header
	fread( &wav.header, sizeof(wav.header), 1, fp );
	if ( wav.header.id != 0x46464952 || wav.header.format != 0x45564157 )
	{
		fclose( fp );
		return -1;
	}

	// Read WAVE format
	fread( &wav.format, sizeof(wav.format), 1, fp );
	if ( wav.format.id != 0x20746d66 || wav.format.size != 16 || wav.format.audioFormat != 1 )
	{
		fclose( fp );
		return -1;
	}
	snd->freq = wav.format.sampleRate;

	// Determine sample format
	if ( wav.format.numChannels == 1 && wav.format.bitsPerSample == 8 )
		snd->format = AL_FORMAT_MONO8;
	else if ( wav.format.numChannels == 1 && wav.format.bitsPerSample == 16 )
		snd->format = AL_FORMAT_MONO16;
	else if ( wav.format.numChannels == 2 && wav.format.bitsPerSample == 8 )
		snd->format = AL_FORMAT_STEREO8;
	else if ( wav.format.numChannels == 2 && wav.format.bitsPerSample == 16 )
		snd->format = AL_FORMAT_STEREO16;
	else
	{
		fclose( fp );
		return -1;
	}

	// Read data header
	fread( &wav.dataHeader, sizeof(wav.dataHeader), 1, fp );
	if ( wav.dataHeader.id != 0x61746164 )
	{
		fclose( fp );
		return -1;
	}
	snd->size = wav.dataHeader.size;

	// Read WAVE data into buffer
	unsigned char *data = new unsigned char[wav.dataHeader.size];
	fread( data, wav.dataHeader.size, 1, fp );
	fclose( fp );

	alBufferData( snd->buffer, snd->format, data, snd->size, snd->freq );
	delete[] data;

	if ( alGetError() != AL_NO_ERROR )
	{
		return -1;
	}

	// Calculate the length of the sound
	int bps;	// bytes per sample
	switch ( snd->format )
	{
	case AL_FORMAT_MONO8:
		bps = 1; break;
	case AL_FORMAT_MONO16:
	case AL_FORMAT_STEREO8:
		bps = 2; break;
	case AL_FORMAT_STEREO16:
		bps = 4; break;
	}
	snd->length = (snd->size * 1000) / (snd->freq * bps) + 1;
	printf( "SoundManager::LoadSound() - Loaded sound '%s' (length = %i ms) into buffer %i\n", fileName, snd->length, freeSoundId );

	return freeSoundId++;
}

void WavInput::Free(InputHandle handle)
{
}

void WavInput::FreeAll()
{
	for ( int i = 0; i < freeSoundId; i++ )
	{
		if (sounds[i].buffer)
			alDeleteBuffers(1, &sounds[i].buffer);		
	}

	freeSoundId = 0;
}

void WavInput::Update()
{
}

void WavInput::Play(InputHandle handle, bool loop, source_t *src)
{
	if ( !loop )
		src->stopTime = time + sounds[handle].length;
	else
		src->stopTime = 0;

	alSourcei( src->source, AL_LOOPING, loop );
	alSourcei( src->source, AL_BUFFER, sounds[handle].buffer );
}

void WavInput::Stop(InputHandle handle)
{
}

