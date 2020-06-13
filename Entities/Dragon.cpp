#include "../Common.h"
#include "Dragon.h"
#include "../Shared/StringParser.h"

static AnimSequence idle( 0, 990, 35, "Idle" );
static AnimSequence idlecr( 990, 495, 30, "Crouch Idle" );
static AnimSequence death3( 1485, 55, 30, "Death3" );

static AnimSequence *dragonAnims[4] = { &idle, &idlecr };

Dragon::Dragon()
{
	model = ModelManager::Instance()->LoadModel( "models/dragon/dragon.md3" );
	scream = SoundManager::Instance()->LoadSound( "sound/dragon/scream.wav" );
	strcpy( skin, "models/dragon/skin" );

	sequence = 0;
	dragonAnims[0]->StartSequence( &animation, true );
}

void Dragon::Update()
{
}

void Dragon::Draw( Renderer &renderer )
{
	renderer.AddModel( model, texture, origin, angles, &animation );
}

void Dragon::Trigger( Entity *other, Entity *activator )
{
	if ( animation.sequence != &death3 )
	{
		death3.StartSequence( &animation, false );
		SoundManager::Instance()->PlaySound( scream, origin, 1, s_volume, false );
	}
}

void Dragon::Init()
{
	texture = TextureManager::Instance()->LoadImage( skin );
	dragonAnims[ sequence ]->StartSequence( &animation, true );
}

Entity *Create_Dragon( EntityMap &map )
{
	Dragon *dragon = new Dragon();

	dragon->angles.y = StringParser::ParseFloat( map.GetValue( "angle" ) );
	dragon->sequence = StringParser::ParseInt( map.GetValue( "sequence" ) );

	char *skin = map.GetValue( "skin" );
	if ( skin )
		strcpy( dragon->skin, skin );

	dragon->Init();

	return dragon;
}
