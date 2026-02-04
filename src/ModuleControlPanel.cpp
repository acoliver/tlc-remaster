/*
	STARFLIGHT - THE LOST COLONY
	ModuleControlPabel.cpp
	Author: coder1024
	Date: April, 07
*/

#include "env.h"
#include "ModuleControlPanel.h"
#include "AudioSystem.h"
#include "Events.h"
#include "ModeMgr.h"
#include "GameState.h"
#include "Game.h"

using namespace std;


#define GUI_CONTROLPANEL_POS_X 817
#define GUI_CONTROLPANEL_POS_Y 477
#define OFFICERICON_UL_X GUI_CONTROLPANEL_POS_X + 2
#define OFFICERICON_UL_Y GUI_CONTROLPANEL_POS_Y + 2
#define CMDBUTTONS_UL_X GUI_CONTROLPANEL_POS_X + 48
#define CMDBUTTONS_UL_Y GUI_CONTROLPANEL_POS_Y + 50
#define CP_OFFICER_TOOLTIP_X GUI_CONTROLPANEL_POS_X + 45
#define CP_OFFICER_TOOLTIP_Y GUI_CONTROLPANEL_POS_Y + 4
#define CP_COMMAND_TOOLTIP_X GUI_CONTROLPANEL_POS_X + 47
#define CP_COMMAND_TOOLTIP_Y GUI_CONTROLPANEL_POS_Y + 270


#define TRANSPARENTCLR al_map_rgb(255,0,255)

ModuleControlPanel::ModuleControlPanel(void)
{
	controlPanelBackgroundImg = NULL;
	mouseOverOfficer = NULL;
	selectedOfficer = NULL;
	mouseOverCommand = NULL;
	selectedCommand = NULL;
	bEnabled = true;
}

ModuleControlPanel::~ModuleControlPanel(void){}

bool ModuleControlPanel::Init()
{
	int bx = GUI_CONTROLPANEL_POS_X;
	int by = GUI_CONTROLPANEL_POS_Y;
	

	//load background image
	controlPanelBackgroundImg = (BITMAP *)load_bitmap("data/controlpanel/gui_controlpanel.bmp",NULL);
	if (controlPanelBackgroundImg == NULL) 
    {
		g_game->message("Error loading controlpanel background");
		return false;
	}

	const int officerIconWidth = 40;
	const int officerIconHeight = 41;
	int officerIconX = OFFICERICON_UL_X;
	int officerIconY = OFFICERICON_UL_Y;

	if (!CommandButton::InitCommon())
		return false;

	if (!OfficerButton::InitCommon())
		return false;

	//reusable button object
	CommandButton* cbtn;


    //these are the 80x80 command buttons
    const int buttonSpacingX = CommandButton::GetCommonWidth() + 1;
    const int buttonSpacingY = CommandButton::GetCommonHeight() + 1;


	/*
	* CAPTAIN
	*/
	OfficerButton *captainBtn;
	captainBtn = new OfficerButton(*this, OFFICER_CAPTAIN, 
        "data/controlpanel/cp_captain_mo.bmp",
        "data/controlpanel/cp_captain_select.bmp", 
        officerIconX, officerIconY);

		selectedOfficer = captainBtn;
		officerButtons.push_back(captainBtn);

        //starting position of captain buttons
		int cix = CMDBUTTONS_UL_X;
		int ciy = CMDBUTTONS_UL_Y;

		//LAUNCH BUTTON
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_captain_launch.bmp", "Break orbit", cix,ciy);
		cbtn->setEventID(EVENT_CAPTAIN_LAUNCH);
		captainBtn->commandButtons.push_back(cbtn);

		//DESCEND BUTTON
		cix += buttonSpacingX;
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_captain_descend.bmp", "Descend to surface", cix,ciy);
		cbtn->setEventID(EVENT_CAPTAIN_DESCEND);
		captainBtn->commandButtons.push_back(cbtn);

		//CARGO HOLD BUTTON
		cix = CMDBUTTONS_UL_X;
        ciy += buttonSpacingY;
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_captain_cargo.bmp", "Cargo hold", cix,ciy);
		cbtn->setEventID(EVENT_CAPTAIN_CARGO);
		captainBtn->commandButtons.push_back(cbtn);

		//QUESTLOG BUTTON
		cix += buttonSpacingX;
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_com_questlog.bmp", "Quest log", cix,ciy);
		cbtn->setEventID(EVENT_CAPTAIN_QUESTLOG);
		captainBtn->commandButtons.push_back(cbtn);

		//LOG PLANET BUTTON
		cix = CMDBUTTONS_UL_X;
		ciy += buttonSpacingY;
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_captain_logplanet.bmp", "Log planet", cix,ciy);
		cbtn->setEventID(EVENT_CAPTAIN_LOG);
		captainBtn->commandButtons.push_back(cbtn);


	/*
	* SCIENCE OFFICER
	*/
    //bump science button relative to captain button
	officerIconY += officerIconHeight;

	OfficerButton *scienceBtn = new OfficerButton(*this, OFFICER_SCIENCE,
        "data/controlpanel/cp_science_mo.bmp",
        "data/controlpanel/cp_science_select.bmp",
        officerIconX, officerIconY);

	officerButtons.push_back(scienceBtn);

        //starting position of science buttons
		cix = CMDBUTTONS_UL_X;
		ciy = CMDBUTTONS_UL_Y;

		//SCAN BUTTON
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_science_scan.bmp", "Sensor scan", cix,ciy);
		cbtn->setEventID(EVENT_SCIENCE_SCAN);
		scienceBtn->commandButtons.push_back(cbtn);

		//ANALYSIS BUTTON
		cix += buttonSpacingX;
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_science_analysis.bmp", "Sensor analysis", cix,ciy);
		cbtn->setEventID(EVENT_SCIENCE_ANALYSIS);
		scienceBtn->commandButtons.push_back(cbtn);

	/*
	* NAVIGATOR
	*/
    //bump button relative to previous button
	officerIconY += officerIconHeight;

	OfficerButton *navBtn = new OfficerButton(*this, OFFICER_NAVIGATION,
        "data/controlpanel/cp_navigation_mo.bmp",
        "data/controlpanel/cp_navigation_select.bmp",
        officerIconX, officerIconY);

	officerButtons.push_back(navBtn);

        //starting position of NAV buttons
		cix = CMDBUTTONS_UL_X;
		ciy = CMDBUTTONS_UL_Y;

		//ORBIT BUTTON
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_nav_orbit.bmp", "Orbit planet", cix,ciy);
		cbtn->setEventID(EVENT_NAVIGATOR_ORBIT);
		navBtn->commandButtons.push_back(cbtn);

		//STARPORT DOCK BUTTON
		cix += buttonSpacingX;
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_nav_dock.bmp", "Dock with Starport", cix,ciy);
		cbtn->setEventID(EVENT_NAVIGATOR_DOCK);
		navBtn->commandButtons.push_back(cbtn);

		//HYPERSPACE ENGINE BUTTON
		cix = CMDBUTTONS_UL_X;
        ciy += buttonSpacingY;
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_nav_hyperspace.bmp", "Hyperspace engine", cix,ciy);
		cbtn->setEventID(EVENT_NAVIGATOR_HYPERSPACE);
		navBtn->commandButtons.push_back(cbtn);

		//STARMAP BUTTON
		cix += buttonSpacingX;
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_nav_starmap.bmp", "Starmap", cix,ciy);
		cbtn->setEventID(EVENT_NAVIGATOR_STARMAP);
		navBtn->commandButtons.push_back(cbtn);


	/*
	* TACTICAL
	*/
    //bump button relative to previous button
	officerIconY += officerIconHeight;

	OfficerButton *tacBtn = new OfficerButton(*this,OFFICER_TACTICAL,
        "data/controlpanel/cp_tactical_mo.bmp",
        "data/controlpanel/cp_tactical_select.bmp",
        officerIconX,officerIconY);

	officerButtons.push_back(tacBtn);

        //starting position of TAC buttons
		cix = CMDBUTTONS_UL_X;
		ciy = CMDBUTTONS_UL_Y;

		//SHIELDS BUTTON
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_tac_shields.bmp", "Raise/Lower Shields", cix,ciy);
		cbtn->setEventID(EVENT_TACTICAL_SHIELDS);
		tacBtn->commandButtons.push_back(cbtn);

		//WEAPONS BUTTON
		cix += buttonSpacingX;
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_tac_weapons.bmp", "Arm/Disarm Weapons", cix,ciy);
		cbtn->setEventID(EVENT_TACTICAL_WEAPONS);
		tacBtn->commandButtons.push_back(cbtn);

	/*
	* ENGINEER
	*/
    //bump button relative to previous button
	officerIconY += officerIconHeight;

	OfficerButton *engBtn = new OfficerButton(*this,OFFICER_ENGINEER,
        "data/controlpanel/cp_engineer_mo.bmp",
        "data/controlpanel/cp_engineer_select.bmp",
        officerIconX,officerIconY);

	officerButtons.push_back(engBtn);

        //starting position of ENG buttons
		cix = CMDBUTTONS_UL_X;
		ciy = CMDBUTTONS_UL_Y;

		//REPAIR BUTTON
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_eng_repair.bmp", "Repair systems", cix,ciy);
		cbtn->setEventID(EVENT_ENGINEER_REPAIR);
		engBtn->commandButtons.push_back(cbtn);

		//INJECT FUEL BUTTON
		cix += buttonSpacingX;
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_com_respond.bmp", "Inject fuel", cix, ciy);
		cbtn->setEventID(EVENT_ENGINEER_INJECT);
		engBtn->commandButtons.push_back(cbtn);

	/*
	* COMMUNICATIONS
	*/
    //bump button relative to previous button
	officerIconY += officerIconHeight;

	OfficerButton *comBtn = new OfficerButton(*this,OFFICER_COMMUNICATION,
        "data/controlpanel/cp_comm_mo.bmp",
        "data/controlpanel/cp_comm_select.bmp",
        officerIconX,officerIconY);

	officerButtons.push_back(comBtn);

        //starting position of COM buttons
		cix = CMDBUTTONS_UL_X;
		ciy = CMDBUTTONS_UL_Y;

		//POSTURE BUTTON
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_com_posture.bmp", "Change posture", cix,ciy);
		cbtn->setEventID(EVENT_COMM_POSTURE);
		comBtn->commandButtons.push_back(cbtn);

		//HAIL BUTTON
		cix += buttonSpacingX;
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_com_hail.bmp", "Hail or respond", cix,ciy);
		cbtn->setEventID(EVENT_COMM_HAIL);
		comBtn->commandButtons.push_back(cbtn);

		//QUESTION BUTTON
		cix = CMDBUTTONS_UL_X;
        ciy += buttonSpacingY;
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_com_question.bmp", "Ask a question", cix,ciy);
		cbtn->setEventID(EVENT_COMM_QUESTION);
		comBtn->commandButtons.push_back(cbtn);

		//TERMINATE BUTTON
		cix += buttonSpacingX;
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_com_terminate.bmp", "End communication", cix,ciy);
		cbtn->setEventID(EVENT_COMM_TERMINATE);
		comBtn->commandButtons.push_back(cbtn);

		//DISTRESS BUTTON
		cix = CMDBUTTONS_UL_X;
        ciy += buttonSpacingY;
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_com_distress.bmp", "Send distress signal", cix,ciy);
		cbtn->setEventID(EVENT_COMM_DISTRESS);
		comBtn->commandButtons.push_back(cbtn);

		//STATEMENT BUTTON
		//cix += CommandButton::GetCommonWidth();
		//cbtn = new CommandButton(*this, COMMANDICON_COM_STATEMENT.bmp", "Make a statement", cix,ciy);
		//cbtn->setEventID(EVENT_COMM_STATEMENT);
		//comBtn->commandButtons.push_back(cbtn);


	/*
	* MEDICAL
	*/
    //bump button relative to previous button
	officerIconY += officerIconHeight;

	OfficerButton *medBtn = new OfficerButton(*this,OFFICER_MEDICAL,
        "data/controlpanel/cp_medical_mo.bmp",
        "data/controlpanel/cp_medical_select.bmp",
        officerIconX,officerIconY);

	officerButtons.push_back(medBtn);

        //starting position of MED buttons
		cix = CMDBUTTONS_UL_X;
		ciy = CMDBUTTONS_UL_Y;

		//EXAMINE BUTTON
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_med_examine.bmp", "Examine crew", cix,ciy);
		cbtn->setEventID(EVENT_DOCTOR_EXAMINE);
		medBtn->commandButtons.push_back(cbtn);

		//TREAT BUTTON
		cix += buttonSpacingX;
		cbtn = new CommandButton(*this, "data/controlpanel/commandicon_med_treat.bmp", "Treat crew", cix,ciy);
		cbtn->setEventID(EVENT_DOCTOR_TREAT);
		medBtn->commandButtons.push_back(cbtn);



	//initialize all of the buttons 
	for (vector<OfficerButton*>::iterator i = officerButtons.begin(); i != officerButtons.end(); ++i)
	{
		OfficerButton *officerButton = *i;

		if (officerButton == NULL)
			return false;

		if (!officerButton->InitButton())
			return false;
	}

	//load audio files
	sndOfficerSelected = g_game->audioSystem->Load("data/controlpanel/officer_selected.ogg");
	if (!sndOfficerSelected) 
    {
		g_game->message("ControlPanel: Error loading officer_selected");
		return false;
	}
	sndOfficerCommandSelected = g_game->audioSystem->Load("data/controlpanel/officer_command_selected.ogg");
	if (!sndOfficerCommandSelected) 
    {
		g_game->message("ControlPanel: Error loading officer_command_selected");
		return false;
	}

	return true;
}


void ModuleControlPanel::SetButton(int ButtonID, bool enabled)
{
	for (vector<CommandButton*>::iterator i = selectedOfficer->commandButtons.begin(); i != selectedOfficer->commandButtons.end(); ++i)
	{
		CommandButton* button = *i;
		if (button->getEventID() == ButtonID) 
		{
			button->SetEnabled(enabled);
		}
	}
}


void ModuleControlPanel::Update()
{
	Module::Update();

	/**
	 * Set gameState variable to keep track of currently selected officer
	 * this is needed by the Status Window module, among other places.
	**/
	if (selectedOfficer != NULL)
		g_game->gameState->setCurrentSelectedOfficer(selectedOfficer->GetOfficerType());
}

void ModuleControlPanel::Draw()
{
	if (g_game->gameState->getCurrentModule() == MODULE_ENCOUNTER &&
		g_game->doShowControls() == false)
			return;
	
	static int lastMode = 0;

	//set CP buttons when mode change takes place
	if (g_game->gameState->player->controlPanelMode != lastMode) 
    {
		lastMode = g_game->gameState->player->controlPanelMode;
		//this->ToggleButtons();
	}	

	// render CP background with transparency
	static int gcpx = GUI_CONTROLPANEL_POS_X;
	static int gcpy = GUI_CONTROLPANEL_POS_Y;
	if (controlPanelBackgroundImg)
    {
		al_set_target_bitmap(g_game->GetBackBuffer());
		al_draw_bitmap_region(controlPanelBackgroundImg, 0, 0, 
			al_get_bitmap_width(controlPanelBackgroundImg), al_get_bitmap_height(controlPanelBackgroundImg),
			gcpx, gcpy, 0);
    }

	// render command buttons for the selected officer
	if (selectedOfficer != NULL)
	{
		for (vector<CommandButton*>::iterator i = selectedOfficer->commandButtons.begin(); i != selectedOfficer->commandButtons.end(); ++i)
		{
			CommandButton *commandButton = *i;

			if (commandButton->GetEnabled()) 
			{
			    if (selectedCommand == commandButton)
			    {
				    commandButton->RenderSelected(g_game->GetBackBuffer());
			    }
			    else if (mouseOverCommand == commandButton)
			    {
				    commandButton->RenderMouseOver(g_game->GetBackBuffer());
			    }
			    else
			    {
				    commandButton->RenderPlain(g_game->GetBackBuffer());
			    }
			}
			else
			{
			    commandButton->RenderDisabled(g_game->GetBackBuffer());
			}
		}
	}

	// render officer buttons
	for (vector<OfficerButton*>::iterator i = officerButtons.begin(); i != officerButtons.end(); ++i)
	{
		OfficerButton *officerButton = *i;
		if (officerButton == selectedOfficer)
		{
			officerButton->RenderSelected(g_game->GetBackBuffer());
		}
		else if (officerButton == mouseOverOfficer)
		{
			officerButton->RenderMouseOver(g_game->GetBackBuffer());
		}
		else
		{
		    if (officerButton->imgMouseOver)
            {
			    al_set_target_bitmap(g_game->GetBackBuffer());
			    al_draw_bitmap_region(officerButton->imgMouseOver, 0, 0,
                    al_get_bitmap_width(officerButton->imgMouseOver), al_get_bitmap_height(officerButton->imgMouseOver),
                    officerButton->posX, officerButton->posY, 0);
            }
		}
	}
}

void ModuleControlPanel::Close()
{
	try {

        destroy_bitmap(controlPanelBackgroundImg);


		for (vector<OfficerButton*>::iterator i = officerButtons.begin(); i != officerButtons.end(); ++i)
		{
			if (*i != NULL)
			{
				(*i)->DestroyButton();
				delete *i;
			}
		}
		officerButtons.clear();

		CommandButton::DestroyCommon();
		OfficerButton::DestroyCommon();

		selectedOfficer = NULL;
		
	}
	catch (std::exception e) 
    {
		debug << e.what() << endl;
	}
	catch(...) {
		debug << "Unhandled exception in ControlPanel::Close" << endl;
	}
}

void ModuleControlPanel::OnKeyPress(int keyCode){}
void ModuleControlPanel::OnKeyPressed(int keyCode){}

void ModuleControlPanel::OnKeyReleased(int keyCode)
{
	Module::OnKeyReleased(keyCode);
	switch (keyCode) 
    {
		case KEY_F1: //select the captain
			g_game->gameState->setCurrentSelectedOfficer(OFFICER_CAPTAIN);
			for (vector<OfficerButton*>::iterator i = officerButtons.begin(); i != officerButtons.end(); ++i)
            {
				OfficerButton *officerButton = *i;
				if(officerButton->GetOfficerType() == OFFICER_CAPTAIN)
                {
					//change the officer
					selectedOfficer = officerButton;
					break;
				}	
			}
			break;
		case KEY_F2: //select the science officer
			g_game->gameState->setCurrentSelectedOfficer(OFFICER_SCIENCE);
			for (vector<OfficerButton*>::iterator i = officerButtons.begin(); i != officerButtons.end(); ++i)
            {
				OfficerButton *officerButton = *i;
				if(officerButton->GetOfficerType() == OFFICER_SCIENCE)
                {
					//change the officer
					selectedOfficer = officerButton;
					break;
				}	
			}
			break;
		case KEY_F3: //select the navigator
			g_game->gameState->setCurrentSelectedOfficer(OFFICER_NAVIGATION);
			for (vector<OfficerButton*>::iterator i = officerButtons.begin(); i != officerButtons.end(); ++i)
            {
				OfficerButton *officerButton = *i;
				if(officerButton->GetOfficerType() == OFFICER_NAVIGATION)
                {
					//change the officer
					selectedOfficer = officerButton;
					break;
				}	
			}
			break;
		case KEY_F4: //select the tactician
			g_game->gameState->setCurrentSelectedOfficer(OFFICER_TACTICAL);
			for (vector<OfficerButton*>::iterator i = officerButtons.begin(); i != officerButtons.end(); ++i)
            {
				OfficerButton *officerButton = *i;
				if(officerButton->GetOfficerType() == OFFICER_TACTICAL)
                {
					//change the officer
					selectedOfficer = officerButton;
					break;
				}	
			}
			break;
		case KEY_F5: //select the engineer
			g_game->gameState->setCurrentSelectedOfficer(OFFICER_ENGINEER);
			for (vector<OfficerButton*>::iterator i = officerButtons.begin(); i != officerButtons.end(); ++i)
            {
				OfficerButton *officerButton = *i;
				if(officerButton->GetOfficerType() == OFFICER_ENGINEER)
                {
					//change the officer
					selectedOfficer = officerButton;
					break;
				}	
			}
			break;
		case KEY_F6: //select the comms officer
			g_game->gameState->setCurrentSelectedOfficer(OFFICER_COMMUNICATION);
			for (vector<OfficerButton*>::iterator i = officerButtons.begin(); i != officerButtons.end(); ++i)
            {
				OfficerButton *officerButton = *i;
				if(officerButton->GetOfficerType() == OFFICER_COMMUNICATION)
                {
					//change the officer
					selectedOfficer = officerButton;
					break;
				}	
			}
			break;
		case KEY_F7: //select the doctor
			g_game->gameState->setCurrentSelectedOfficer(OFFICER_MEDICAL);
			for (vector<OfficerButton*>::iterator i = officerButtons.begin(); i != officerButtons.end(); ++i)
            {
				OfficerButton *officerButton = *i;
				if(officerButton->GetOfficerType() == OFFICER_MEDICAL)
                {
					//change the officer
					selectedOfficer = officerButton;
					break;
				}	
			}
			break;

		case KEY_M: // "map" button
			g_game->gameState->setCurrentSelectedOfficer(OFFICER_NAVIGATION);
			for (vector<OfficerButton*>::iterator i = officerButtons.begin(); i != officerButtons.end(); ++i)
            {
				OfficerButton *officerButton = *i;
				if(officerButton->GetOfficerType() == OFFICER_NAVIGATION)
                {
					//change the officer
					selectedOfficer = officerButton;
					break;
				}
			}
			Event e(EVENT_NAVIGATOR_STARMAP);
			g_game->modeMgr->BroadcastEvent(&e);
			break;

	}
}

void ModuleControlPanel::OnMouseMove(int x, int y)
{
	//Module::OnMouseMove(x,y);

	// look for officer button mouse over
	mouseOverOfficer = NULL;
	for (vector<OfficerButton*>::iterator i = officerButtons.begin(); (i != officerButtons.end()) && (mouseOverOfficer == NULL); ++i)
	{
		OfficerButton *officerButton = *i;

		if (officerButton->IsInButton(x,y))
		{
			mouseOverOfficer = officerButton;
		}
	}

	// look for command button mouse over
	mouseOverCommand = NULL;
	if (selectedOfficer != NULL)
	{
		for (vector<CommandButton*>::iterator i = selectedOfficer->commandButtons.begin(); i != selectedOfficer->commandButtons.end(); ++i)
		{
			CommandButton *commandButton = *i;

			if (commandButton->IsInButton(x,y))
			{
			    mouseOverCommand = commandButton;
			}
		}
	}
}

void ModuleControlPanel::OnMouseClick(int button, int x, int y)
{
	//Module::OnMouseClick(button,x,y);
}

void ModuleControlPanel::OnMousePressed(int button, int x, int y)
{
	//Module::OnMousePressed(button, x, y);

	if (button != 0)
		return;

	// select officer
	for (vector<OfficerButton*>::iterator i = officerButtons.begin(); i != officerButtons.end(); ++i)
	{
		OfficerButton *officerButton = *i;

		if (officerButton->IsInButton(x,y))
		{
			//change the officer
			selectedOfficer = officerButton;

			//enable command buttons
			//this->ToggleButtons();

			//play sound
			g_game->audioSystem->Play(sndOfficerSelected);

			break;
		}
	}
//jjh - maybe here to force navigator when entering hyperspace
	// set command to pressed
	if (selectedOfficer != NULL)
	{
		for (vector<CommandButton*>::iterator i = selectedOfficer->commandButtons.begin(); i != selectedOfficer->commandButtons.end(); ++i)
		{
			CommandButton *commandButton = *i;

			if (commandButton->IsInButton(x,y) && commandButton->GetEnabled())
			{
			selectedCommand = commandButton;

			g_game->audioSystem->Play(sndOfficerCommandSelected);
			}
		}
	}

}

void ModuleControlPanel::OnMouseReleased(int button, int x, int y)
{
	//Module::OnMouseReleased(button, x, y);

	//launch event based on button ID so all modules in this mode will be notified
	if ( selectedCommand ){
		Event e(selectedCommand->getEventID());
		g_game->modeMgr->BroadcastEvent(&e);
	}

	selectedCommand = NULL;
}

void ModuleControlPanel::OnMouseWheelUp(int x, int y)
{
	//Module::OnMouseWheelUp(x, y);
}

void ModuleControlPanel::OnMouseWheelDown(int x, int y)
{
	//Module::OnMouseWheelDown(x, y);
}



void ModuleControlPanel::OnEvent(Event * event){}


//******************************************************************************
// CommandButton
//******************************************************************************

#define CMDBUTTON_LABEL_CLR			al_map_rgb(0,0,0)

BITMAP* ModuleControlPanel::CommandButton::imgBackground = NULL;
BITMAP* ModuleControlPanel::CommandButton::imgBackgroundDisabled = NULL;
BITMAP* ModuleControlPanel::CommandButton::imgBackgroundMouseOver = NULL;
BITMAP* ModuleControlPanel::CommandButton::imgBackgroundSelected = NULL;

ModuleControlPanel::CommandButton::CommandButton(ModuleControlPanel& outer, std::string imgFileCmdIcon, std::string cmdName, int posX, int posY)
: outer(outer)
{
	this->imgFileCmdIcon = imgFileCmdIcon;
	this->cmdName = cmdName;
	this->posX = posX;
	this->posY = posY;
	this->imgCmdIcon = NULL;
	this->enabled = true;
}

ModuleControlPanel::CommandButton::~CommandButton(){}

bool ModuleControlPanel::CommandButton::InitCommon()
{
	imgBackground = (BITMAP*)load_bitmap("data/controlpanel/command_button_bg.bmp",NULL);
	if (imgBackground == NULL) 
    {
		g_game->message("Error in control panel");
		return false;
	}

	imgBackgroundDisabled = (BITMAP*)load_bitmap("data/controlpanel/command_button_bg_disabled.bmp",NULL);
	if (imgBackgroundDisabled == NULL) 
    {
		g_game->message("Error in control panel");
		return false;
	}

	imgBackgroundMouseOver = (BITMAP*)load_bitmap("data/controlpanel/command_button_bg_mo.bmp",NULL);
	if (imgBackgroundMouseOver == NULL) 
    {
		g_game->message("Error in control panel");
		return false;
	}

	imgBackgroundSelected = (BITMAP*)load_bitmap("data/controlpanel/command_button_bg_select.bmp",NULL);
	if (imgBackgroundSelected == NULL) 
    {
		g_game->message("Error in control panel");
		return false;
	}

	return true;
}

int ModuleControlPanel::CommandButton::GetCommonWidth()
{
	return al_get_bitmap_width(imgBackground);
}

int ModuleControlPanel::CommandButton::GetCommonHeight()
{
	return al_get_bitmap_height(imgBackground);
}

bool ModuleControlPanel::CommandButton::InitButton()
{
	imgCmdIcon = (BITMAP*)load_bitmap(imgFileCmdIcon.c_str(),NULL);
	if (imgCmdIcon == NULL) 
    {
		g_game->message("Error in control panel");
		return false;
	}

	return true;
}

void ModuleControlPanel::CommandButton::DestroyButton()
{
	if (imgCmdIcon != NULL)
	{
		destroy_bitmap(imgCmdIcon);
		imgCmdIcon = NULL;
	}
}

void ModuleControlPanel::CommandButton::DestroyCommon()
{
	if (imgBackground != NULL)
	{
		destroy_bitmap(imgBackground);
		imgBackground = NULL;
	}

	if (imgBackgroundDisabled != NULL)
	{
		destroy_bitmap(imgBackgroundDisabled);
		imgBackgroundDisabled = NULL;
	}

	if (imgBackgroundMouseOver != NULL)
	{
		destroy_bitmap(imgBackgroundMouseOver);
		imgBackgroundMouseOver = NULL;
	}

	if (imgBackgroundSelected != NULL)
	{
		destroy_bitmap(imgBackgroundSelected);
		imgBackgroundSelected = NULL;
	}
}

void ModuleControlPanel::CommandButton::RenderPlain(BITMAP* canvas)
{
	Render(canvas,imgBackground);
}

void ModuleControlPanel::CommandButton::RenderDisabled(BITMAP* canvas)
{
	Render(canvas,imgBackgroundDisabled);
}

void ModuleControlPanel::CommandButton::RenderMouseOver(BITMAP* canvas)
{
	Render(canvas,imgBackgroundMouseOver);

	static int x = CP_COMMAND_TOOLTIP_X;
	static int y = CP_COMMAND_TOOLTIP_Y; 

    //print command tooltip text
	g_game->Print18(canvas, x, y, cmdName.c_str(), WHITE);
}

void ModuleControlPanel::CommandButton::RenderSelected(BITMAP* canvas)
{
	Render(canvas,imgBackgroundSelected, true);
}

bool ModuleControlPanel::CommandButton::IsInButton(int x, int y)
{
	if ((x >= posX) && (x < posX+al_get_bitmap_width(imgBackground)) &&
		(y >= posY) && (y < posY+al_get_bitmap_height(imgBackground)))
	{
		return true;
	}

	return false;
}

void ModuleControlPanel::CommandButton::SetEnabled(bool enabled)
{
	this->enabled = enabled;
}

bool ModuleControlPanel::CommandButton::GetEnabled()
{
	return enabled;
}

void ModuleControlPanel::CommandButton::Render(BITMAP *canvas, BITMAP *imgBackground, bool down)
{
	// draw button background and command icon image
	al_set_target_bitmap(canvas);
	al_draw_bitmap_region(imgBackground, 0, 0, al_get_bitmap_width(imgBackground), al_get_bitmap_height(imgBackground), posX, posY, 0);
	
	if (down)
		al_draw_bitmap_region(imgCmdIcon, 0, 0, al_get_bitmap_width(imgCmdIcon), al_get_bitmap_height(imgCmdIcon), posX, posY, 0);
	else
		al_draw_bitmap_region(imgCmdIcon, 0, 2, al_get_bitmap_width(imgCmdIcon), al_get_bitmap_height(imgCmdIcon), posX, posY, 0);

}




//******************************************************************************
// OfficerButton
//******************************************************************************

#define OFFICER_MOUSEOVERTIP_SPACEFROMBTN_X		3
#define OFFICER_MOUSEOVERTIP_SPACEFROMBTN_Y		0
#define OFFICER_MOUSEOVERTIP_BORDER_THICKNESS	2
#define OFFICER_MOUSEOVERTIP_BORDER_CLR			al_map_rgb(0,0,0)
#define OFFICER_MOUSEOVERTIP_BACKGROUND_CLR		al_map_rgb(200,200,200)
#define OFFICER_MOUSEOVERTIP_TEXT_CLR			al_map_rgb(255,255,0)
#define OFFICER_MOUSEOVERTIP_INNER_SPACING		5
#define OFFICER_MOUSEOVERTIP_BAR_HEIGHT			10
#define OFFICER_MOUSEOVERTIP_HEALTH_CLR			al_map_rgb(255,0,0)
#define OFFICER_MOUSEOVERTIP_LABEL_CLR			al_map_rgb(255,255,255)
#define OFFICER_MOUSEOVERTIP_TEXTOFFSET_X		6
#define OFFICER_MOUSEOVERTIP_TEXTOFFSET_Y		6

ModuleControlPanel::OfficerButton::OfficerButton(ModuleControlPanel& outer, OfficerType officerType, std::string imgFileMouseOver, std::string imgFileSelected, int posX, int posY)
: outer(outer)
{
	this->officerType = officerType;
	this->imgFileMouseOver = imgFileMouseOver;
	this->imgFileSelected = imgFileSelected;
	this->posX = posX;
	this->posY = posY;
	this->imgMouseOver = NULL;
	this->imgSelected = NULL;
}

ModuleControlPanel::OfficerButton::~OfficerButton(){}

bool ModuleControlPanel::OfficerButton::InitCommon()
{
	return true;
}

bool ModuleControlPanel::OfficerButton::InitButton()
{
	imgMouseOver = (BITMAP*)load_bitmap(imgFileMouseOver.c_str(),NULL);
	if (imgMouseOver == NULL) 
    {
		g_game->message("Error in control panel");
		return false;
	}

	imgSelected = (BITMAP*)load_bitmap(imgFileSelected.c_str(),NULL);
	if (imgSelected == NULL) 
    {
		g_game->message("Error in control panel");
		return false;
	}

	for (vector<CommandButton*>::iterator i = commandButtons.begin(); i != commandButtons.end(); ++i)
	{
		CommandButton* commandButton = *i;

		if (commandButton == NULL)
			return false;

		if (!commandButton->InitButton())
			return false;
	}

	return true;
}

void ModuleControlPanel::OfficerButton::RenderMouseOver(BITMAP *canvas)
{
	std::string name = "Unknown Officer";
	std::string title = "Unknown Title";
	Officer* officer = NULL;

	// mouse-over button image
	al_set_target_bitmap(canvas);
	al_draw_bitmap_region(imgMouseOver, 0, 0, al_get_bitmap_width(imgMouseOver), al_get_bitmap_height(imgMouseOver), posX, posY, 0);

	try {
		// get the officer associated with this button
		officer = g_game->gameState->getOfficer(this->officerType);
		name = officer->name;
		title = officer->GetTitle();
	}
	catch(...) { }

	// determine location for tip window
	static int x = CP_OFFICER_TOOLTIP_X;
	static int y = CP_OFFICER_TOOLTIP_Y;

	// clear background
	rectfill(canvas, x, y, x+155, y+32, color_to_int(al_map_rgb(57,59,134)));

	// draw tooltip of crew position/name
	g_game->Print18(canvas, x + 5, y, officer->GetTitle().c_str(), OFFICER_MOUSEOVERTIP_TEXT_CLR);
	g_game->Print18(canvas, x + 5, y + 15, name.c_str(), OFFICER_MOUSEOVERTIP_LABEL_CLR);

}

void ModuleControlPanel::OfficerButton::RenderSelected(BITMAP *canvas)
{
	std::string name = "Unknown Officer";
	std::string title = "Unknown Title";
	Officer* officer = NULL;

	// selected button image
	al_set_target_bitmap(canvas);
	al_draw_bitmap_region(imgSelected, 0, 0, al_get_bitmap_width(imgMouseOver), al_get_bitmap_height(imgMouseOver), posX, posY, 0);

	try {
		// get the officer associated with this button
		officer = g_game->gameState->getOfficer(this->officerType);
		name = officer->name;
		title = officer->GetTitle();
	}
	catch(...) { }

	// determine location for tip window
	static int x = CP_OFFICER_TOOLTIP_X;
	static int y = CP_OFFICER_TOOLTIP_Y;

	// clear background
	rectfill(canvas, x, y, x+155, y+32, color_to_int(al_map_rgb(57,59,134)));

    // draw tooltip of crew position/name
	g_game->Print18(canvas, x + 5, y, officer->GetTitle().c_str(), OFFICER_MOUSEOVERTIP_TEXT_CLR);
	g_game->Print18(canvas, x + 5, y + 15, name.c_str(), OFFICER_MOUSEOVERTIP_LABEL_CLR);

}

bool ModuleControlPanel::OfficerButton::IsInButton(int x, int y)
{
	if ((x >= posX) && (x < posX+al_get_bitmap_width(imgMouseOver)) &&
		(y >= posY) && (y < posY+al_get_bitmap_height(imgMouseOver)))
	{
		return true;
	}

	return false;
}

void ModuleControlPanel::OfficerButton::DestroyButton()
{
	if (imgMouseOver != NULL)
	{
		destroy_bitmap(imgMouseOver);
		imgMouseOver = NULL;
	}
	if (imgSelected != NULL)
	{
		destroy_bitmap(imgSelected);
		imgSelected = NULL;
	}

	for (vector<CommandButton*>::iterator i = commandButtons.begin(); i != commandButtons.end(); ++i)
	{
		(*i)->DestroyButton();
		delete *i;
	}
	commandButtons.clear();
}

void ModuleControlPanel::OfficerButton::DestroyCommon()
{
}



