#include "Common.h"
#include "Game.h"
#include "Renderer/Renderer.h"
#include "SharedVars.h"
#include "World/Skybox.h"
#include "World/BSPTree.h"
#include "Entity/EntityParser.h"
#include "Shared/StringParser.h"
#include "SoundManager/SoundManager.h"
#include "Entities/DemoCamera.h"

#define MAX_ENTITIES	1024

Font			defaultFont;
Skybox			*skybox;
Camera			cam;
Camera			*activeCam = &cam;
DemoCamera		*activeDemoCam = NULL;

Renderer		mainRenderer;
Renderer		screenRenderer;

BSPTree			*world;
char			mapname[64];

Entity			*entities[MAX_ENTITIES];
int				numEntities = 0;

texHandle_t		blurTexture;

void UpdateFrame()
{
	for ( int i = 0; i < numEntities; i++ )
		entities[i]->Update();

	activeCam->CalculateAxes();
	SoundManager::Instance()->SetListener( activeCam->origin, activeCam->forward, activeCam->up );
}

void DrawFPSCounter()
{
	static int counted = 0, lastupdate = 0;
	static int fps = 0;

    counted++;

	if ( counted == 10 )
	{
		fps = (counted * 1000) / (realtime - lastupdate);
		lastupdate = realtime;
		counted = 0;
	}

	defaultFont.Printf( 0, 0, 1, "%i fps", fps );
}

void DrawFrame()
{
	static float blurring = false;

	DrawFPSCounter();

	for ( int i = 0; i < numEntities; i++ )
		entities[i]->Draw( mainRenderer );
		
	defaultFont.Printf( 0, 462, 1, "%s", mapname );
//	defaultFont.Printf( 0, 16, 1, "%i entities", numEntities );

	mainRenderer.SetCamera( activeCam );
	mainRenderer.AddWorld( world );
	mainRenderer.AddSkybox( skybox );

	if ( !hyperdrive )
	{
		blurring = false;
		mainRenderer.AddFont( &defaultFont );
		mainRenderer.Execute();
	}
	else	// Enable motion blurring in hyperdrive mode
	{
		mainRenderer.SetViewport( 0, 0, 512, 512, (float)screenWidth / (float)screenHeight );
		mainRenderer.AddRenderTarget( blurTexture );
		if ( blurring )
			mainRenderer.AddPicture( -8, 482, 656, -484, blurTexture, 0.95f );
		else
			blurring = true;
		mainRenderer.Execute();

		screenRenderer.AddPicture( 0, 480, 640, -480, blurTexture );
		screenRenderer.AddFont( &defaultFont );
		screenRenderer.Execute();
	}
}

void InitGame()
{
	TextureManager::Instance()->SetCompression( true );
	TextureManager::Instance()->SetAnisotropy( true, 8 );
	blurTexture = TextureManager::Instance()->GetEmptyTexture();

	world = new BSPTree();
	world->Load( "maps/demo1.bsp" );
	world->UploadLightmaps();
	world->LoadTextures();

	defaultFont.Init( "textures/fonts/amped" );
	defaultFont.SetColor( 1, 0.5f, 0, 1 );

	skybox = new Skybox();

	EntityParser eParser;
	eParser.ParseEntities( world->GetEntities() );

	eParser.StartIteration();
	while ( eParser.HasNextEntity() )
	{
		if ( numEntities >= MAX_ENTITIES )
			break;

		Entity *ent = eParser.CreateNextEntity();
		if ( ent )
		{
			entities[numEntities] = ent;
			numEntities++;
		}
	}

	for ( int i = 0; i < numEntities; i++ )
	{
		entities[i]->Init();
		if ( !strcmp( entities[i]->classname, "camera_spline" ) )
		{
			DemoCamera *demoCam = (DemoCamera *)entities[i];
			if ( demoCam->initial )
			{
				demoCam->Trigger( NULL, NULL );		// Comment out this line to disable the demo camera
			}
		}
	}

	EntityMap *map = eParser.FindEntity( "worldspawn" );
	if ( map )
	{
		char *message = map->GetValue( "message" );
		if ( message )
			strcpy( mapname, message );

		char *sky = map->GetValue( "skybox" );
		if ( sky )
			skybox->Init( sky );

		char *music = map->GetValue( "music" );
		if ( music )
		{
			sndHandle_t bgm = SoundManager::Instance()->LoadSound( music );
			SoundManager::Instance()->PlaySound( bgm, Vector3D( 0, 0, 0 ), 0, s_musicvolume, true );
		}
	}

	map = eParser.FindEntity( "info_player_intermission" );
	if ( map )
	{
		cam.origin = StringParser::ParseVector3D( map->GetValue( "origin" ) );
		cam.origin.QtoOGL();
	}
}

Entity *FindEntityForTargetname( char *targetname )
{
	if ( !targetname || !targetname[0] )
		return NULL;

	for ( int i = 0; i < numEntities; i++ )
	{
		Entity *ent = entities[i];
		if ( ent->targetname[0] && !strcmp( ent->targetname, targetname ) )
			return ent;
	}

	return NULL;
}

void SetActiveDemoCamera( DemoCamera *democam )
{
	if ( activeDemoCam )
		activeDemoCam->active = false;

	activeDemoCam = democam;

	if ( activeDemoCam )
	{
		activeCam = activeDemoCam->GetCamera();
		activeDemoCam->active = true;
	}
	else
	{
		activeCam = &cam;
	}
}

void CloseGame()
{
	for ( int i = 0; i < numEntities; i++ )
		delete entities[i];

	delete skybox;

	delete world;

	TextureManager::Instance()->FreeAll();
	ModelManager::Instance()->FreeAll();
	SoundManager::Instance()->FreeAll();
}

