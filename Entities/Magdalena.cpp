#include "../Common.h"
#include "Magdalena.h"
#include "../Shared/StringParser.h"

static AnimSequence idle( 0, 320, 30, "Idle" );
static AnimSequence idlecr( 320, 110, 30, "Crouch Idle" );

static AnimSequence *magdalenaAnims[2] = { &idle, &idlecr };

Magdalena::Magdalena()
{
	model = ModelManager::Instance()->LoadModel( "models/magdalena/magdalena.md3" );
	strcpy( skin, "models/magdalena/skin" );

	sequence = 0;
	magdalenaAnims[0]->StartSequence( &animation, true );
}

void Magdalena::Update()
{
}

void Magdalena::Draw( Renderer &renderer )
{
	renderer.AddModel( model, texture, origin, angles, &animation );
}

void Magdalena::Trigger( Entity *other, Entity *activator )
{
}

void Magdalena::Init()
{
	texture = TextureManager::Instance()->LoadImage( skin );
	magdalenaAnims[ sequence ]->StartSequence( &animation, true );
}

Entity *Create_Magdalena( EntityMap &map )
{
	Magdalena *mag = new Magdalena();

	mag->angles.y = StringParser::ParseFloat( map.GetValue( "angle" ) );
	mag->sequence = StringParser::ParseInt( map.GetValue( "sequence" ) );

	char *skin = map.GetValue( "skin" );
	if ( skin )
		strcpy( mag->skin, skin );

	mag->Init();

	return mag;
}
