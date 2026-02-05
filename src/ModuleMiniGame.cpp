/*
	STARFLIGHT - THE LOST COLONY
	ModuleMiniGame.cpp
	Author: J.Harbour
	Date: Jan, 2008
*/

#include "env.h"
#include "ModuleMiniGame.h"
#include "AudioSystem.h"
#include "ModeMgr.h"
#include "Game.h"
#include "Events.h"
#include "GameState.h"
#include "Util.h"


ModuleMiniGame::ModuleMiniGame(void)
{
}

ModuleMiniGame::~ModuleMiniGame(void)
{
}


void ModuleMiniGame::OnKeyPress(int keyCode)
{
	Module::OnKeyPress(keyCode);
}


void ModuleMiniGame::OnKeyPressed(int keyCode)
{
	Module::OnKeyPressed(keyCode);
}

void ModuleMiniGame::OnKeyReleased(int keyCode)
{
	Module::OnKeyReleased(keyCode);

	switch (keyCode) {
		case KEY_ESC:
			std::string escape = g_game->getGlobalString("ESCAPEMODULE");
			g_game->modeMgr->LoadModule(escape);
			return;
			break;
	}
}

void ModuleMiniGame::OnMouseMove(int x, int y)
{
	Module::OnMouseMove(x,y);
}

void ModuleMiniGame::OnMouseClick(int button, int x, int y)
{
	Module::OnMouseClick(button,x,y);
}

void ModuleMiniGame::OnMousePressed(int button, int x, int y)
{
	Module::OnMousePressed(button, x, y);
}

void ModuleMiniGame::OnMouseReleased(int button, int x, int y)
{
	Module::OnMouseReleased(button, x, y);
}

void ModuleMiniGame::OnMouseWheelUp(int x, int y)
{
	Module::OnMouseWheelUp(x, y);
}

void ModuleMiniGame::OnMouseWheelDown(int x, int y)
{
	Module::OnMouseWheelDown(x, y);
}

void ModuleMiniGame::OnEvent(Event *event)
{
	//switch(event->getEventType()) 
	//{
	//}
}

void ModuleMiniGame::Close()
{
	
	
}


bool ModuleMiniGame::Init()
{
	BITMAP *background = al_load_bitmap("data/minigame_background.bmp");
	al_set_target_bitmap(g_game->GetBackBuffer()); al_draw_bitmap_region(background, 0, 0, al_get_bitmap_width(background), al_get_bitmap_height(background), 0, 0, 0);
	al_destroy_bitmap(background);
	
	window = al_create_sub_bitmap(g_game->GetBackBuffer(), 192, 144, 640, 480);

	return true;
}

void ModuleMiniGame::Draw3D()
{
	
}

void ModuleMiniGame::Update()
{
	
}

void ModuleMiniGame::Draw()
{
	al_set_target_bitmap(window);
	al_draw_text((ALLEGRO_FONT*)g_game->font24, WHITE, 10, 10, 0, "HELLO WORLD");
	
	
}

