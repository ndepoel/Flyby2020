#include "../Common.h"
#include "Visor.h"
#include "../Shared/StringParser.h"

//static AnimSequence run( 110, 8, 18, "Run" );
static AnimSequence run( 157, 10, 15, "Idle" );

Visor::Visor()
{
	model = ModelManager::Instance()->LoadModel( "models/visor/visor.md3" );
	texture = TextureManager::Instance()->LoadImage( "models/visor/visor" );

//	model = ModelManager::Instance()->LoadModel( "models/magdalena/magdalena.md3" );
//	texture = TextureManager::Instance()->LoadImage( "models/magdalena/skin" );

	run.StartSequence( &animation, true );
}

void Visor::Update()
{
}

void Visor::Draw( Renderer &renderer )
{
	renderer.AddModel( model, texture, origin, angles, &animation );
}

Entity *Create_Visor( EntityMap &map )
{
	Visor *visor = new Visor();

	char *angle = map.GetValue( "angle" );
	if ( angle )
		visor->angles.y = (float)atof( angle );

	return visor;
}
