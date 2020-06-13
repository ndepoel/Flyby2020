#ifndef __ENTITYDEFS_H
#define __ENTITYDEFS_H

#include "../Entity/Entity.h"
#include "../Entity/EntityMap.h"

typedef struct entityDef_s
{
	char	*classname;
	Entity	*(*create)( EntityMap &map );
} entityDef_t;

Entity *Create_Visor( EntityMap &map );
Entity *Create_Speaker( EntityMap &map );
Entity *Create_Dragon( EntityMap &map );
Entity *Create_CameraTarget( EntityMap &map );
Entity *Create_DemoCamera( EntityMap &map );
Entity *Create_TargetPosition( EntityMap &map );
Entity *Create_ParticleEmitter( EntityMap &map );
Entity *Create_Timeshift( EntityMap &map );
Entity *Create_Magdalena( EntityMap &map );

entityDef_t entityDefs[] =
{
//	{ "info_player_deathmatch", Create_Visor },
	{ "target_speaker", Create_Speaker },
	{ "actor_dragon", Create_Dragon },
	{ "target_camera", Create_CameraTarget },
	{ "camera_spline", Create_DemoCamera },
	{ "target_position", Create_TargetPosition },
	{ "particle_emitter", Create_ParticleEmitter },
	{ "target_timeshift", Create_Timeshift },
	{ "actor_magdalena", Create_Magdalena }
};

int numEntityDefs = sizeof( entityDefs ) / sizeof( entityDef_t );

#endif

