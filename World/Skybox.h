#ifndef __SKYBOX_H
#define __SKYBOX_H

#include "../TextureManager/TextureManager.h"

class Skybox
{
public:
	Skybox();
	Skybox( char *skyname );

	void Init( char *skyname );
	void Draw();

	unsigned int CreateCubeMap();
protected:
	virtual void SetAttributes();
private:
	texHandle_t		textures[6];
};

#endif

