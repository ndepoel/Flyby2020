#include "../Common.h"
#include "AnimSequence.h"
#include "../SharedVars.h"

AnimSequence::AnimSequence( int startFrame, int numFrames, int fps, char *name )
{
	this->startFrame = startFrame;
	if ( !numFrames )
		this->numFrames = 1;
	else
		this->numFrames = numFrames;
	if ( !fps )
		this->mspf = 100;	// 10 fps by default
	else
		this->mspf = 1000 / fps;
	strcpy( this->name, name );
}

void AnimSequence::StartSequence( activeAnim_t *animation, bool loop )
{
	animation->sequence = this;
	animation->loop = loop;
	animation->timeOffset = time;
}

void AnimSequence::StopSequence( activeAnim_t *animation )
{
	animation->loop = false;
	animation->timeOffset = -1;
}

void AnimSequence::GetFrame( activeAnim_t *animation, int *lastframe, int *curframe, float *mu )
{
	if ( !lastframe || !curframe || !mu )
		return;

	if ( !IsPlaying( animation ) )
	{
		*lastframe = *curframe = startFrame + numFrames - 1;
		*mu = 0;
		return;
	}

	int t = time - animation->timeOffset;
	int div = t / mspf;			// = frame number
	int mod = t % mspf;			// = time between last frame and current frame
	*mu = (float)mod / (float)mspf;

	*lastframe = div % numFrames;
	*curframe = (div + 1) % numFrames;

	*lastframe += startFrame;
	*curframe += startFrame;
}

bool AnimSequence::IsPlaying( activeAnim_t *animation )
{
	if ( animation->sequence != this )
		return false;

	if ( animation->loop )
		return true;

	if ( animation->timeOffset < 0 )
		return false;

	if ( (int)( time - animation->timeOffset ) > ( mspf * (numFrames-1) ) )
		return false;
	else
		return true;
}
