#ifndef __ANIMSEQUENCE_H
#define __ANIMSEQUENCE_H

typedef struct activeAnim_s activeAnim_t;

class AnimSequence
{
public:
	AnimSequence( int startFrame, int numFrames, int fps, char *name );
	
	void	StartSequence( activeAnim_t *animation, bool loop = false );
	void	StopSequence( activeAnim_t *animation );
	void	GetFrame( activeAnim_t *animation, int *lastframe, int *curframe, float *mu );
	bool	IsPlaying( activeAnim_t *animation );
private:
	int		startFrame;
	int		numFrames;
	int		mspf;	// milliseconds per frame
	char	name[32];
};

typedef struct activeAnim_s
{
	AnimSequence	*sequence;
	int				timeOffset;
	bool			loop;
} activeAnim_t;

#endif

