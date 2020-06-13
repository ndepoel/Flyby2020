#ifndef __INPUT_H
#define __INPUT_H

#include "SoundManager.h"

typedef int InputHandle;

typedef struct source_s source_t;

class Input
{
public:
	virtual InputHandle LoadFile( char *filename ) = 0;
	virtual void Free(InputHandle handle) = 0;
	virtual void FreeAll() = 0;
	virtual void Update() = 0;
	virtual void Play(InputHandle handle, bool loop, source_t *src) = 0;
	virtual void Stop(InputHandle handle) = 0;
};

#endif

