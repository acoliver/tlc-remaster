/*
	STARFLIGHT - THE LOST COLONY
	ModuleStarmap.cpp - The QuestLog Module
	Author: Justin Sargent
	Date: Nov-24-2007
*/

#include "env.h"
#include "env.h"
#include "ModuleQuestLog.h"
#include "GameState.h"
#include "Game.h"
#include "QuestMgr.h"
#include "Events.h"
#include "DataMgr.h"
#include "Label.h"
#include "ModuleControlPanel.h"
using namespace std;

#define VIEWER_MOVE_RATE 16

#define NAME_X 36
#define NAME_Y 36
#define NAME_H 48
#define NAME_W 290

#define DESC_X NAME_X
#define DESC_Y NAME_Y + NAME_H + 8
#define DESC_H 190
#define DESC_W NAME_W

//#define QUEST_VIEWER_BMP                 0        /* BMP  */

ModuleQuestLog::ModuleQuestLog() 
{
	log_active = false;
}

ModuleQuestLog::~ModuleQuestLog(){}

void ModuleQuestLog::OnKeyPressed(int keyCode){}
void ModuleQuestLog::OnKeyPress( int keyCode ){}
void ModuleQuestLog::OnKeyReleased(int keyCode){}
void ModuleQuestLog::OnMouseMove(int x, int y){}
void ModuleQuestLog::OnMouseClick(int button, int x, int y){}
void ModuleQuestLog::OnMousePressed(int button, int x, int y){}
void ModuleQuestLog::OnMouseReleased(int button, int x, int y){}
void ModuleQuestLog::OnMouseWheelUp(int x, int y){}
void ModuleQuestLog::OnMouseWheelDown(int x, int y){}
void ModuleQuestLog::OnEvent(Event *event)
{
	switch(event->getEventType()) 
	{
		case EVENT_CAPTAIN_QUESTLOG:
			if(!log_active){
				log_active = true;
			}else{
				log_active = false;
			}
			break;
	}
}

void ModuleQuestLog::Close()
{
	try {
        if (window!=NULL)
        {
            delete window;
            window=NULL;
        }
	}
	catch(std::exception e) {
		debug << e.what() << endl;
	}
	catch(...) {
		debug << "Unhandled exception in QuestLog::Close"  << endl;
	}
}

bool ModuleQuestLog::Init()
{
	viewer_offset_x = SCREEN_WIDTH;
	viewer_offset_y = 90;

	log_active = false;

	//create quest name label
	ALLEGRO_COLOR questTitleColor = al_map_rgb(255,84,0);
	questName = new Label(g_game->questMgr->getName(), 
		viewer_offset_x+NAME_X, viewer_offset_y+NAME_Y, NAME_W, NAME_H, questTitleColor, g_game->font22);
	questName->Refresh();

	//create quest description label
	ALLEGRO_COLOR questTextColor = al_map_rgb(255,255,255);
	questDesc = new Label(g_game->questMgr->getShort(), 
		viewer_offset_x+DESC_X, viewer_offset_y+DESC_Y, DESC_W, DESC_H, questTextColor, g_game->font22);
	questDesc->Refresh();


	//load window GUI
	//window = (BITMAP*)qldata[QUEST_VIEWER_BMP].dat;
    window=NULL;
	window=al_load_bitmap("data/questviewer/quest_viewer.bmp");
    if (!window) 
    {
		debug << "Error loading quest viewer image" << endl;
		return false;
	}

	return true;
}

void ModuleQuestLog::Update()
{
	if(log_active)
    {
		if(g_game->gameState->getCurrentSelectedOfficer() != OFFICER_CAPTAIN)
        {
			log_active = false;
		}
	}
}


void ModuleQuestLog::Draw()
{
	//is quest viewer visible?
	if(viewer_offset_x < SCREEN_WIDTH)
	{
		//draw background
		al_set_target_bitmap(g_game->GetBackBuffer()); al_draw_bitmap_region(window, 0, 0, al_get_bitmap_width(window), al_get_bitmap_height(window), viewer_offset_x, viewer_offset_y, 0);

		//draw quest title
		questName->SetX(NAME_X + viewer_offset_x);
		questName->Draw(g_game->GetBackBuffer());

		//draw quest description
		questDesc->SetX(DESC_X + viewer_offset_x);
		questDesc->Draw(g_game->GetBackBuffer());	


		//display quest completion status
		string metstr;
		ALLEGRO_COLOR metcolor;
		if (g_game->gameState->getQuestCompleted()) {
			metstr = "(COMPLETE)";
			metcolor = GREEN;
		}
		else {
			metstr = "(INCOMPLETE)";
			metcolor = RED;
		}
		g_game->Print20(g_game->GetBackBuffer(), viewer_offset_x + NAME_X, viewer_offset_y+DESC_Y+DESC_H, metstr, metcolor);


	}

	if(log_active)
	{
		if(viewer_offset_x > 680) {
			viewer_offset_x -= VIEWER_MOVE_RATE;
		}
	}else{
		if(viewer_offset_x < SCREEN_WIDTH) {
			viewer_offset_x += VIEWER_MOVE_RATE;
		}
	}
}

