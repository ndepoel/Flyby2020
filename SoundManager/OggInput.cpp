#include "../Common.h"
#include "OggInput.h"

OggInput::OggInput()
{
	Init();
}

OggInput::~OggInput()
{
	FreeAll();
}

Input *OggInput::Instance()
{
	static OggInput instance;

	return &instance;
}

void OggInput::Init()
{

	memset( streams, 0, sizeof( streams ) );

	activeStreams.next = &activeStreams;
	activeStreams.prev = &activeStreams;

	freeStreams = &streams[0];

	for ( int i = 0; i < MAX_NUM_OGG_STREAMS-1; i++ )
	{
		streams[i].next = &streams[i+1];
		streams[i].handle = i;
	}
	streams[MAX_NUM_OGG_STREAMS-1].handle = MAX_NUM_OGG_STREAMS-1;

}

size_t OggReadFunc(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	return fread( ptr, size, nmemb, (FILE *)datasource );
}

int OggSeekFunc(void *datasource, ogg_int64_t offset, int whence)
{
	return fseek( (FILE *)datasource, (long)offset, whence );
}

int OggCloseFunc(void *datasource)
{
	return fclose( (FILE *)datasource );
}

long OggTellFunc(void *datasource)
{
	return ftell( (FILE *)datasource );
}

static ov_callbacks	oggCallbacks = { OggReadFunc, OggSeekFunc, OggCloseFunc, OggTellFunc };


Stream_t *OggInput::getStream()
{
	Stream_t *src;

	if ( !freeStreams )
		return NULL;

	src = freeStreams;
	freeStreams = freeStreams->next;

	src->next = activeStreams.next;
	src->prev = &activeStreams;
	activeStreams.next->prev = src;
	activeStreams.next = src;

	return src;
}

InputHandle OggInput::LoadFile( char *fileName)
{
	Stream_t *src = getStream();
	FILE *f;

	if (!src)
		return -1;
	
	
	if ( !( f = fopen( fileName, "rb" ) ) )
	{
		return -1;
	}

	if (ov_open_callbacks(f, &src->Stream.oggStream, NULL, 0, oggCallbacks ) < 0)
	{
		fclose(f);
		return -1;
	}
	
	src->Stream.oggInfo = ov_info(&src->Stream.oggStream, -1);
	if (src->Stream.oggInfo->channels == 1)
		src->Stream.oggFormat = AL_FORMAT_MONO16;
	else
		src->Stream.oggFormat = AL_FORMAT_STEREO16;

	alGenBuffers(NUM_BUF_PER_STREAM,	src->Stream.buffers);
	printf( "OGG %s Loaded\n", fileName );

	return src->handle;
}

void OggInput::FreeAll()
{
	Stream_t *src, *prev;
	
	for ( src = activeStreams.prev; src != &activeStreams; src = prev )
	{
		prev = src->prev;

		Free( src->handle );
	}	
}

void OggInput::Free(InputHandle handle)
{
	Stream_t *src = &streams[handle];

	ov_clear( &src->Stream.oggStream);
	src->Stream.oggInfo = NULL;
	alDeleteBuffers(NUM_BUF_PER_STREAM, src->Stream.buffers);

	src->prev->next = src->next;
	src->next->prev = src->prev;

	src->next = freeStreams;
	src->prev = NULL;
	freeStreams = src;
}


bool OggInput::StreamOgg( ALuint buffer, Stream_t *src)
{
	char	*data = new char[BUF_SIZE];
	int		size = 0;
	int		section;
	int		result;

	while ( size < BUF_SIZE )
	{
		result = ov_read( &src->Stream.oggStream, data + size, BUF_SIZE - size, 0, 2, 1, &section );
		if ( result > 0 )
			size += result;
		else if ( result < 0 )
		{
			printf( "Error while streaming OGG, error code = %i\n", result );
			delete[] data;
			return false;
		}
		else if (size != BUF_SIZE )
			// Apparantly we don't have enough sound data left to fill the buffer, 
			// so if we're looping rewind the ogg file and try again
			if ( src->Stream.loopOgg )
				ov_time_seek( &src->Stream.oggStream, 0 );
			else
				//einde van stream!! volgende nummer ??
				break;
	}

	alBufferData( buffer, src->Stream.oggFormat, data, size, src->Stream.oggInfo->rate );

	delete[] data;
	if ( !size )
		return false;
	else
		return true;
}
	
void OggInput::Update()
{
	Stream_t *src, *prev;
	int		processed = 0;
	
	for ( src = activeStreams.prev; src != &activeStreams; src = prev )
	{
		prev = src->prev;
		if ( src->src )
			alGetSourcei(src->src->source , AL_BUFFERS_PROCESSED, &processed );

		while ( processed-- )
		{
			ALuint buffer;

			alSourceUnqueueBuffers( src->src->source, 1, &buffer );
			if ( StreamOgg( buffer, src ) )
				alSourceQueueBuffers( src->src->source, 1, &buffer );
		}
	}
}

void OggInput::Play(InputHandle handle, bool loop, source_t *src)
{
	Stream_t *streamSrc = &streams[handle];
	streamSrc->src = src;
	streamSrc->Stream.loopOgg = loop;
	
	for (int i=0; i < NUM_BUF_PER_STREAM; i++)
	{
		StreamOgg(streamSrc->Stream.buffers[i], streamSrc);
	}
	alSourceQueueBuffers(src->source, NUM_BUF_PER_STREAM, streamSrc->Stream.buffers); 
}

void OggInput::Stop(InputHandle handle)
{
	Stream_t *streamSrc = &streams[handle];
	int queued;
	ALuint buffer;

	alGetSourcei(streamSrc->src->source , AL_BUFFERS_QUEUED, &queued );

	while (queued--)
	{
		alSourceUnqueueBuffers(streamSrc->src->source,1, &buffer);
	}

	Free(handle);
}

