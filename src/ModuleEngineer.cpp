/*
	STARFLIGHT - THE LOST COLONY
	ModuleEngineer.cpp - The Engineering module.
	Author: Keith "Daikaze" Patch
	Date: 5-27-2008
*/

#include "env.h"
#include "env.h"
#include "Util.h"
#include "ModuleEngineer.h"
#include "GameState.h"
#include "Game.h"
#include "AudioSystem.h"
#include "Events.h"
#include "DataMgr.h"
#include "Script.h"
#include "ModeMgr.h"
using namespace std;

//bar 1 = Lasers
//bar 2 = Missiles
//bar 3 = Hull
//bar 4 = Armor
//bar 5 = Shields (4)
//bar 6 = Engines (5)
#define X_OFFSET 120
#define EVENT_REPAIR_LASERS		-9301
#define EVENT_REPAIR_MISSILES	-9302
#define EVENT_REPAIR_HULL		-9303
#define EVENT_REPAIR_SHIELDS	-9304
#define EVENT_REPAIR_ENGINES	-9305





ModuleEngineer::ModuleEngineer():
	img_window(NULL),
	img_bar_base(NULL),
	text(NULL),
	img_bar_laser(NULL),
	img_bar_missile(NULL),
	img_bar_hull(NULL),
	img_bar_armor(NULL),
	img_bar_shield(NULL),
	img_ship(NULL),
	img_button_repair(NULL),
	img_button_repair_over(NULL)
{}

ModuleEngineer::~ModuleEngineer()
{
}

bool ModuleEngineer::Init()
{
	debug << "  ModuleEngineer Initialize" << endl;

	module_active = false;

	VIEWER_WIDTH = 800;//(int)lua->getGlobalNumber("VIEWER_WIDTH");
	VIEWER_HEIGHT = 500;//(int)lua->getGlobalNumber("VIEWER_HEIGHT");
	VIEWER_TARGET_OFFSET = VIEWER_HEIGHT;//(int)lua->getGlobalNumber("VIEWER_TARGET_OFFSET");
	VIEWER_MOVE_RATE = 12;//(int)lua->getGlobalNumber("VIEWER_MOVE_RATE");
	viewer_offset_y = -VIEWER_TARGET_OFFSET;

	g_game->audioSystem->Load("data/engineer/buttonclick.ogg", "click");

	img_window = al_load_bitmap("data/engineer/gui.bmp");
	if (!img_window) {
		g_game->message("Engineer: Error loading gui");
		return false;
	}

	img_bar_base = al_load_bitmap("data/engineer/Element_Gauge_Gray.bmp");
	img_bar_laser = al_load_bitmap("data/engineer/Element_Gauge_Magenta.bmp");
	img_bar_missile = al_load_bitmap("data/engineer/Element_Gauge_Purple.bmp");
	img_bar_hull = al_load_bitmap("data/engineer/Element_Gauge_Green.bmp");
	img_bar_armor = al_load_bitmap("data/engineer/Element_Gauge_Red.bmp");
	img_bar_shield = al_load_bitmap("data/engineer/Element_Gauge_Blue.bmp");
	img_bar_engine = al_load_bitmap("data/engineer/Element_Gauge_Orange.bmp");

	if (!img_bar_base || !img_bar_laser || !img_bar_missile || !img_bar_hull || 
		!img_bar_armor || !img_bar_shield || !img_bar_engine) {
		g_game->message("Engineer: Error loading gauge images");
		return false;
	}

	switch(g_game->gameState->getProfession())
	{
		case PROFESSION_FREELANCE:
			img_ship = al_load_bitmap("data/engineer/high_res_ship_freelance.tga");
			break;
		
		case PROFESSION_MILITARY:
			img_ship = al_load_bitmap("data/engineer/high_res_ship_military.tga");
			break;
		
		case PROFESSION_SCIENTIFIC:
		default:
			img_ship = al_load_bitmap("data/engineer/high_res_ship_science.tga");
			break;
	}

	if (!img_ship) {
		g_game->message("Engineer: Error loading ship image");
		return false;
	}
	
	text = create_bitmap(VIEWER_WIDTH, VIEWER_HEIGHT);
	tlc_clear_to_transparent(text);

	//load button images
	img_button_repair = al_load_bitmap("data/engineer/aux_repair.bmp");
	img_button_repair_over = al_load_bitmap("data/engineer/aux_repair_hover.bmp");
	if (!img_button_repair || !img_button_repair_over) {
		g_game->message("Engineer: Error loading button images");
		return false;
	}

	//Create and initialize the crew buttons
	button[0] = new Button(img_button_repair,img_button_repair_over,img_button_repair,
							700+X_OFFSET, 135 , 0, EVENT_REPAIR_LASERS, g_game->font22,"", al_map_rgb(255,255,255), "click");
	button[1] = new Button(img_button_repair,img_button_repair_over,img_button_repair,
							150+X_OFFSET, 180 , 0, EVENT_REPAIR_MISSILES, g_game->font22,"", al_map_rgb(255,255,255), "click");
	button[2] = new Button(img_button_repair,img_button_repair_over,img_button_repair,
							683+X_OFFSET, 230 , 0, EVENT_REPAIR_HULL, g_game->font22,"", al_map_rgb(255,255,255), "click");
	button[3] = new Button(img_button_repair,img_button_repair_over,img_button_repair,
							670+X_OFFSET, 325 , 0, EVENT_REPAIR_SHIELDS, g_game->font22,"", al_map_rgb(255,255,255), "click");
	button[4] = new Button(img_button_repair,img_button_repair_over,img_button_repair,
							150+X_OFFSET, 385 , 0, EVENT_REPAIR_ENGINES, g_game->font22,"", al_map_rgb(255,255,255), "click");

	for(int i = 0; i < 5; i++){
		if (button[i] == NULL){return false;}
		if (!button[i]->IsInitialized()){return false;}
	}

	//DEBUG CODE
	//g_game->gameState->m_ship.setHullIntegrity(60);
	//g_game->gameState->m_ship.setEngineIntegrity(60);
	//g_game->gameState->m_ship.setShieldIntegrity(60);
	//g_game->gameState->m_ship.setLaserIntegrity(60);
	//g_game->gameState->m_ship.setMissileLauncherIntegrity(60);
	//g_game->gameState->m_items.SetItemCount(ITEM_COBALT, 3);
	//g_game->gameState->m_items.SetItemCount(ITEM_MOLYBDENUM, 3);
	//g_game->gameState->m_items.SetItemCount(ITEM_ALUMINUM, 3);
	//g_game->gameState->m_items.SetItemCount(ITEM_TITANIUM, 3);
	//g_game->gameState->m_items.SetItemCount(ITEM_SILICA, 3);
	//g_game->gameState->officerEng->attributes.setEngineering(150);
	//g_game->gameState->officerCap->attributes.setEngineering(20);
	//g_game->gameState->officerEng->attributes.setVitality(0); //kill the engineer

	return true;
}

void ModuleEngineer::Close()
{
	try {
		if (text) {
			destroy_bitmap(text);
			text = NULL;
		}

		if (img_window) { al_destroy_bitmap(img_window); img_window = NULL; }
		if (img_bar_base) { al_destroy_bitmap(img_bar_base); img_bar_base = NULL; }
		if (img_bar_laser) { al_destroy_bitmap(img_bar_laser); img_bar_laser = NULL; }
		if (img_bar_missile) { al_destroy_bitmap(img_bar_missile); img_bar_missile = NULL; }
		if (img_bar_hull) { al_destroy_bitmap(img_bar_hull); img_bar_hull = NULL; }
		if (img_bar_armor) { al_destroy_bitmap(img_bar_armor); img_bar_armor = NULL; }
		if (img_bar_shield) { al_destroy_bitmap(img_bar_shield); img_bar_shield = NULL; }
		if (img_bar_engine) { al_destroy_bitmap(img_bar_engine); img_bar_engine = NULL; }
		if (img_ship) { al_destroy_bitmap(img_ship); img_ship = NULL; }
		if (img_button_repair) { al_destroy_bitmap(img_button_repair); img_button_repair = NULL; }
		if (img_button_repair_over) { al_destroy_bitmap(img_button_repair_over); img_button_repair_over = NULL; }

		for (int i=0; i < 5; i++){
			if (button[i]) {
				delete button[i];
				button[i] = NULL;
			}
		}
	}
	catch (std::exception e) {
		debug << e.what() << endl;
	}
	catch(...) {
		debug << "Unhandled exception in ModuleEngineer::Close" << endl;
	}
}

void ModuleEngineer::Update(){}

//return false if repair ceased due to lack of mineral, true otherwise
bool ModuleEngineer::useMineral(Ship &ship){

	GameState *gs = g_game->gameState;
	ShipPart repairing = ship.partInRepair;

	if ( repairing == PART_NONE ){
		debug << "engineer: [ERROR] useMineral() was called while no repair were in progress" << endl;
		return false;
	}

	//debug << "useMineral: system=%d, counter=%d, mineral=%d\n",
	//		repairing-1, ship.repairCounters[repairing-1], ship.neededMinerals[repairing-1]);
	
	//we do up to MAX_REPAIR_COUNT (defined as 3 in GameState.h) repair iterations before consuming one mineral
	if ( ship.repairCounters[repairing-1] < MAX_REPAIR_COUNT){
		ship.repairCounters[repairing-1]++;
		return true;

	}else{

		Officer *currentEngineer = g_game->gameState->getCurrentEng();
		std::string eng = currentEngineer->getLastName() + "-> ";
		int neededMineral = ship.repairMinerals[repairing-1];
		Item mineral;
		int num_mineral;
		
		gs->m_items.Get_Item_By_ID(neededMineral, mineral, num_mineral);

		if ( num_mineral == 0 ){
			//mineral not in the cargo hold, stop repair
			std::string mineralName = g_game->dataMgr->GetItemByID(neededMineral)->name;
			std::string msg = eng + "Repairing ceased due to lack of " + mineralName + ".";
			g_game->ShowMessageBoxWindow("",  msg );
			return false;

		}else{
			//consume the mineral
			std::string msg = eng + "Using one cubic meter of " + mineral.name + ".";
			g_game->printout(g_game->g_scrollbox, msg, GREEN, 1000);

			gs->m_items.RemoveItems(mineral.id, 1);
			Event e(CARGO_EVENT_UPDATE);
			g_game->modeMgr->BroadcastEvent(&e);

			//roll a new one
			switch (rand()%5){
				case 0: ship.repairMinerals[repairing-1] = ITEM_COBALT;     break;
				case 1: ship.repairMinerals[repairing-1] = ITEM_MOLYBDENUM; break;
				case 2: ship.repairMinerals[repairing-1] = ITEM_ALUMINUM;   break;
				case 3: ship.repairMinerals[repairing-1] = ITEM_TITANIUM;   break;
				case 4: ship.repairMinerals[repairing-1] = ITEM_SILICA;     break;
				default: ASSERT(0);
			}

			//increase engineering skill every 4 minerals consumed
			//doing it here ensures the player will never be able to get free skill increase
			currentEngineer->attributes.extra_variable++;
			if( currentEngineer->attributes.extra_variable >= 4 ){
				currentEngineer->attributes.extra_variable = 0;

				if(currentEngineer->SkillUp(SKILL_ENGINEERING))
					g_game->printout(g_game->g_scrollbox, eng + "I think I'm getting better at this.", PURPLE, 5000);
			}

			//reset the counter
			ship.repairCounters[repairing-1] = 0;

			return true;
		}
	}

	//UNREACHABLE
	ASSERT(0);
}

void ModuleEngineer::Draw()
{
	std::string s;

	if(g_game->gameState->getCurrentSelectedOfficer() != OFFICER_ENGINEER)
		module_active = false;

	if(viewer_offset_y > -VIEWER_TARGET_OFFSET){
		al_set_target_bitmap(g_game->GetBackBuffer()); al_draw_bitmap_region(img_window, 0, 0, VIEWER_WIDTH, VIEWER_HEIGHT, X_OFFSET, viewer_offset_y, 0);

		//draw the ship
		//al_set_target_bitmap(g_game->GetBackBuffer()); al_draw_bitmap_region(img_ship, 0, 0, img_ship->w, img_ship->h, 342+X_OFFSET, 95+viewer_offset_y, 0);
		al_set_target_bitmap(img_ship); al_draw_bitmap(g_game->GetBackBuffer(), 342+X_OFFSET, 95+viewer_offset_y, 0);

		al_set_target_bitmap(g_game->GetBackBuffer()); al_draw_bitmap_region(img_bar_base, 0, 0, al_get_bitmap_width(img_bar_base), al_get_bitmap_height(img_bar_base), 580+X_OFFSET, 135+viewer_offset_y, 0); //laser
		al_set_target_bitmap(g_game->GetBackBuffer()); al_draw_bitmap_region(img_bar_base, 0, 0, al_get_bitmap_width(img_bar_base), al_get_bitmap_height(img_bar_base), 175+X_OFFSET, 180+viewer_offset_y, 0); //missile
		al_set_target_bitmap(g_game->GetBackBuffer()); al_draw_bitmap_region(img_bar_base, 0, 0, al_get_bitmap_width(img_bar_base), al_get_bitmap_height(img_bar_base), 565+X_OFFSET, 230+viewer_offset_y, 0); //hull
		al_set_target_bitmap(g_game->GetBackBuffer()); al_draw_bitmap_region(img_bar_base, 0, 0, al_get_bitmap_width(img_bar_base), al_get_bitmap_height(img_bar_base), 155+X_OFFSET, 270+viewer_offset_y, 0); //Armor
		al_set_target_bitmap(g_game->GetBackBuffer()); al_draw_bitmap_region(img_bar_base, 0, 0, al_get_bitmap_width(img_bar_base), al_get_bitmap_height(img_bar_base), 550+X_OFFSET, 325+viewer_offset_y, 0); //shields
		al_set_target_bitmap(g_game->GetBackBuffer()); al_draw_bitmap_region(img_bar_base, 0, 0, al_get_bitmap_width(img_bar_base), al_get_bitmap_height(img_bar_base), 170+X_OFFSET, 385+viewer_offset_y, 0); //engines
		float percentage = 0;
		percentage = g_game->gameState->getShip().getLaserIntegrity() / 100.0f;
		al_set_target_bitmap(g_game->GetBackBuffer()); al_draw_bitmap_region(img_bar_laser, 0, 0, al_get_bitmap_width(img_bar_laser) * percentage, al_get_bitmap_height(img_bar_base), 580+X_OFFSET, 135+viewer_offset_y, 0); //laser
		
		percentage = g_game->gameState->getShip().getMissileLauncherIntegrity() / 100.0f;
		al_set_target_bitmap(g_game->GetBackBuffer()); al_draw_bitmap_region(img_bar_missile, 0, 0, al_get_bitmap_width(img_bar_missile) * percentage, al_get_bitmap_height(img_bar_base), 175+X_OFFSET, 180+viewer_offset_y, 0); //missile
		
		percentage = g_game->gameState->getShip().getHullIntegrity() / 100.0f;
		al_set_target_bitmap(g_game->GetBackBuffer()); al_draw_bitmap_region(img_bar_hull, 0, 0, al_get_bitmap_width(img_bar_hull) * percentage, al_get_bitmap_height(img_bar_base), 565+X_OFFSET, 230+viewer_offset_y, 0); //hull
		
		if(g_game->gameState->getShip().getMaxArmorIntegrity() <= 0){
			percentage = 0;
		}else{
			percentage = g_game->gameState->getShip().getArmorIntegrity() / g_game->gameState->getShip().getMaxArmorIntegrity();
		}
		al_set_target_bitmap(g_game->GetBackBuffer()); al_draw_bitmap_region(img_bar_armor, 0, 0, al_get_bitmap_width(img_bar_armor) * percentage, al_get_bitmap_height(img_bar_base), 155+X_OFFSET, 270+viewer_offset_y, 0); //Armor
		
		percentage = g_game->gameState->getShip().getShieldIntegrity() /  100.0f;
		al_set_target_bitmap(g_game->GetBackBuffer()); al_draw_bitmap_region(img_bar_shield, 0, 0, al_get_bitmap_width(img_bar_shield) * percentage, al_get_bitmap_height(img_bar_base), 550+X_OFFSET, 325+viewer_offset_y, 0); //shields
		
	percentage =  g_game->gameState->getShip().getEngineIntegrity() / 100.0f;
	al_set_target_bitmap(g_game->GetBackBuffer()); al_draw_bitmap_region(img_bar_engine, 0, 0, al_get_bitmap_width(img_bar_engine) * percentage, al_get_bitmap_height(img_bar_base), 170+X_OFFSET, 385+viewer_offset_y, 0); //engines
		al_draw_line(407+X_OFFSET, 104+viewer_offset_y, 560+X_OFFSET, 130+viewer_offset_y, GREEN, 1); //laser line
		al_draw_line(560+X_OFFSET, 130+viewer_offset_y, 690+X_OFFSET, 130+viewer_offset_y, GREEN, 1); //laser line

		al_draw_line(410+X_OFFSET, 175+viewer_offset_y, 175+X_OFFSET, 175+viewer_offset_y, GREEN, 1); //missile line

		al_draw_line(405+X_OFFSET, 250+viewer_offset_y, 540+X_OFFSET, 225+viewer_offset_y, GREEN, 1); //hull line
		al_draw_line(540+X_OFFSET, 225+viewer_offset_y, 675+X_OFFSET, 225+viewer_offset_y, GREEN, 1); //hull line

		al_draw_line(395+X_OFFSET, 235+viewer_offset_y, 280+X_OFFSET, 265+viewer_offset_y, GREEN, 1); //armor line
		al_draw_line(280+X_OFFSET, 265+viewer_offset_y, 155+X_OFFSET, 265+viewer_offset_y, GREEN, 1); //armor line

		al_draw_line(408+X_OFFSET, 320+viewer_offset_y, 660+X_OFFSET, 320+viewer_offset_y, GREEN, 1); //shield line

		al_draw_line(408+X_OFFSET, 355+viewer_offset_y, 275+X_OFFSET, 380+viewer_offset_y, GREEN, 1); //engine line
		al_draw_line(275+X_OFFSET, 380+viewer_offset_y, 170+X_OFFSET, 380+viewer_offset_y, GREEN, 1); //engine line
		button[0]->SetY(135 + viewer_offset_y);
		button[1]->SetY(180 + viewer_offset_y);
		button[2]->SetY(230 + viewer_offset_y);
		button[3]->SetY(325 + viewer_offset_y);
		button[4]->SetY(385 + viewer_offset_y); 
		for(int i=0; i<5; i++){
			button[i]->Run(g_game->GetBackBuffer());
		}
	}

	Officer *currentEngineer = g_game->gameState->getCurrentEng();
	std::string eng = currentEngineer->getLastName() + "-> ";
	Ship ship = g_game->gameState->getShip();
	float repair_time, repair_rate = 0;
	float repair_skill = g_game->gameState->CalcEffectiveSkill(SKILL_ENGINEERING);

	tlc_clear_to_transparent(text);
	s = "LASERS: " + ship.getLaserClassString();
	if(ship.partInRepair == PART_LASERS){
		if(ship.getLaserIntegrity() < 100 && ship.getLaserIntegrity() > 0){
			if(currentEngineer->CanSkillCheck() == true){
				currentEngineer->FakeSkillCheck();
				if(useMineral(ship)){
					repair_time = 8 * (6 - repair_skill/50);
					repair_rate = 100 / repair_time;
					ship.augLaserIntegrity(repair_rate);
				}
				else /* no more repair metal; stopping repair */
					ship.partInRepair = PART_NONE;
			}
		}else{
			ship.partInRepair = PART_NONE;
			g_game->printout(g_game->g_scrollbox, eng + "The lasers are now fully functional!", BLUE, 5000);
		}
		textout_ex(text, g_game->font22, s.c_str(), 580, 115, color_to_int(LTGREEN), -1);
	}else{
		textout_ex(text, g_game->font22, s.c_str(), 580, 115, color_to_int(LTBLUE), -1);
	}

	s = "MISSILES: " + ship.getMissileLauncherClassString();
	if(ship.partInRepair == PART_MISSILES){
		if(ship.getMissileLauncherIntegrity() < 100 && ship.getMissileLauncherIntegrity() > 0){
			if(currentEngineer->CanSkillCheck() == true){
				currentEngineer->FakeSkillCheck();
				if(useMineral(ship)){
					repair_time = 8 * (6 - repair_skill/50);
					repair_rate = 100 / repair_time;
					ship.augMissileLauncherIntegrity(repair_rate);
				}
				else /* no more repair metal; stopping repair */
					ship.partInRepair = PART_NONE;
			}
		}else{
			ship.partInRepair = PART_NONE;
			g_game->printout(g_game->g_scrollbox, eng + "The missile system is now fully functional!", BLUE,5000);
		}
		textout_ex(text, g_game->font22, s.c_str(), 175, 160, color_to_int(LTGREEN), -1);
	}else{
		textout_ex(text, g_game->font22, s.c_str(), 175, 160, color_to_int(LTBLUE), -1);
	}

	s = "HULL";
	if(ship.partInRepair == PART_HULL){
		if(ship.getHullIntegrity() < 100 && ship.getHullIntegrity() > 0){
			if(currentEngineer->CanSkillCheck() == true){
				currentEngineer->FakeSkillCheck();
				if(useMineral(ship)){
					repair_time = 25 * (6 - repair_skill/50);
					repair_rate = 100 / repair_time;
					ship.augHullIntegrity(repair_rate);
				}
				else /* no more repair metal; stopping repair */
					ship.partInRepair = PART_NONE;
			}
		}else{
			ship.partInRepair = PART_NONE;
			g_game->printout(g_game->g_scrollbox, eng + "The hull is now fully repaired!", BLUE, 5000);
		}
		alfont_textout_centre_ex(text, g_game->font22, s.c_str(), 565 + al_get_bitmap_width(img_bar_base)/2, 210, color_to_int(LTGREEN), -1);
	}else{
		alfont_textout_centre_ex(text, g_game->font22, s.c_str(), 565 + al_get_bitmap_width(img_bar_base)/2, 210, color_to_int(LTBLUE), -1);
	}

	s = "ARMOR: " + ship.getArmorClassString();
	textout_ex(text, g_game->font22, s.c_str(), 155, 250, color_to_int(LTBLUE), -1);

	s = "SHIELDS: " + ship.getShieldClassString();
	if(ship.partInRepair == PART_SHIELDS){
		if(ship.getShieldIntegrity() < 100 && ship.getShieldIntegrity() > 0){
			if(currentEngineer->CanSkillCheck() == true){
				currentEngineer->FakeSkillCheck();
				if(useMineral(ship)){
					repair_time = 10 * (6 - repair_skill/50);
					repair_rate = 100 / repair_time;
					ship.augShieldIntegrity(repair_rate);
					ship.setShieldCapacity(ship.getMaxShieldCapacity());
				}
				else /* no more repair metal; stopping repair */
					ship.partInRepair = PART_NONE;
			}
		}else{
			ship.partInRepair = PART_NONE;
			ship.setShieldCapacity(ship.getMaxShieldCapacity());
			g_game->printout(g_game->g_scrollbox, eng + "The shields are now fully functional!", BLUE,5000);
		}
		textout_ex(text, g_game->font22, s.c_str(), 550, 305, color_to_int(LTGREEN), -1);
	}else{
		textout_ex(text, g_game->font22, s.c_str(), 550, 305, color_to_int(LTBLUE), -1);
	}

	s = "ENGINES: " + ship.getEngineClassString();
	if(ship.partInRepair == PART_ENGINES){
		if(ship.getEngineIntegrity() < 100 && ship.getEngineIntegrity() > 0){
			if(currentEngineer->CanSkillCheck() == true){
				currentEngineer->FakeSkillCheck();
				if(useMineral(ship)){
					repair_time = 10 * (6 - repair_skill/50);
					repair_rate = 100 / repair_time;
					ship.augEngineIntegrity(repair_rate);
				}
				else /* no more repair metal; stopping repair */
					ship.partInRepair = PART_NONE;
			}
		}else{
			ship.partInRepair = PART_NONE;
			g_game->printout(g_game->g_scrollbox, eng + "The engines are now fully repaired!", BLUE, 5000);
		}
		textout_ex(text, g_game->font22, s.c_str(), 170, 365, color_to_int(LTGREEN), -1);
	}else{
		textout_ex(text, g_game->font22, s.c_str(), 170, 365, color_to_int(LTBLUE), -1);
	}
	g_game->gameState->setShip(ship);
	al_set_target_bitmap(g_game->GetBackBuffer()); al_draw_bitmap_region(text, 0, 0, VIEWER_WIDTH, VIEWER_HEIGHT, X_OFFSET, viewer_offset_y, 0);

	if(module_active){
		if(viewer_offset_y < -30)
			viewer_offset_y += VIEWER_MOVE_RATE;
	}else{
		if(viewer_offset_y > -VIEWER_TARGET_OFFSET)
			viewer_offset_y -= VIEWER_MOVE_RATE;
	}
}

void ModuleEngineer::OnEvent(Event *event)
{
	ShipPart repairing = g_game->gameState->m_ship.partInRepair;

	switch(event->getEventType()) {
		case 5001: //repair systems button
			module_active = !module_active;
			break;
		case EVENT_REPAIR_LASERS:
			if(g_game->gameState->getShip().getLaserIntegrity() > 0 
			&& g_game->gameState->getShip().getLaserIntegrity() < 100 
			&& g_game->gameState->officerEng->CanSkillCheck() == true
			&& repairing != PART_LASERS){
				repairing = PART_LASERS;
			}else{
				repairing = PART_NONE;
			}
			break;
		case EVENT_REPAIR_MISSILES:
			if(g_game->gameState->getShip().getMissileLauncherIntegrity() > 0 
			&& g_game->gameState->getShip().getMissileLauncherIntegrity() < 100
			&& g_game->gameState->officerEng->CanSkillCheck() == true
			&& repairing != PART_MISSILES){
				repairing = PART_MISSILES;
			}else{
				repairing = PART_NONE;
			}
			break;
		case EVENT_REPAIR_HULL:
			if(g_game->gameState->getShip().getHullIntegrity() > 0 
			&& g_game->gameState->getShip().getHullIntegrity() < 100
			&& g_game->gameState->officerEng->CanSkillCheck() == true
			&& repairing != PART_HULL){
				repairing = PART_HULL;
			}else{
				repairing = PART_NONE;
			}
			break;
		case EVENT_REPAIR_SHIELDS:
			if(g_game->gameState->getShip().getShieldIntegrity() > 0 
			&& g_game->gameState->getShip().getShieldIntegrity() < 100 
			&& g_game->gameState->officerEng->CanSkillCheck() == true
			&& repairing != PART_SHIELDS){
				repairing = PART_SHIELDS;
			}else{
				repairing = PART_NONE;
			}
			break;
		case EVENT_REPAIR_ENGINES:
			if(g_game->gameState->getShip().getEngineIntegrity() > 0 
			&& g_game->gameState->getShip().getEngineIntegrity() < 100
			&& g_game->gameState->officerEng->CanSkillCheck() == true
			&& repairing != PART_ENGINES){
				repairing = PART_ENGINES;
			}else{
				repairing = PART_NONE;
			}
			break;
		case 0: default:
			break;
	}

	if (g_game->gameState->m_ship.partInRepair != repairing)
		g_game->gameState->m_ship.partInRepair = repairing;
}

void ModuleEngineer::OnKeyPressed(int keyCode){}
void ModuleEngineer::OnKeyPress(int keyCode){}
void ModuleEngineer::OnKeyReleased(int keyCode){}

void ModuleEngineer::OnMouseMove(int x, int y)
{
	if (!module_active) return;

	for(int i=0; i<5; i++)
		button[i]->OnMouseMove(x,y);
}
void ModuleEngineer::OnMouseReleased(int button, int x, int y)
{
	if (!module_active) return;

	for(int i=0; i<5; i++)
		this->button[i]->OnMouseReleased(button, x, y);
}

void ModuleEngineer::OnMouseClick(int button, int x, int y){}
void ModuleEngineer::OnMousePressed(int button, int x, int y){}
void ModuleEngineer::OnMouseWheelUp(int x, int y) {}
void ModuleEngineer::OnMouseWheelDown(int x, int y) {}

