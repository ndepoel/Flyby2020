#ifndef __GAME_H
#define __GAME_H

#include "Font/Font.h"
#include "Camera/Camera.h"

extern Font		defaultFont;
extern Camera	cam;

void UpdateFrame();
void DrawFrame();

class Entity;
Entity *FindEntityForTargetname( char *targetname );

class DemoCamera;
void SetActiveDemoCamera( DemoCamera *democam );

void InitGame();
void CloseGame();

#endif

