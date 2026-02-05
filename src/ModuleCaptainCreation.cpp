#include "env.h"
#include "ModuleCaptainCreation.h"
#include "Button.h"
#include "AudioSystem.h"
#include "Events.h"
#include "ModeMgr.h"
#include "Game.h"
#include "Label.h"
using namespace std;


// Removed DATAFILE defines - using direct bitmap pointers



#define PROFBTN_WIDTH 329
#define PROFBTN_HEIGHT 168
#define PROFBTN_SCIENTIFIC_X 13
#define PROFBTN_SCIENTIFIC_Y 219
#define PROFBTN_FREELANCE_X 349
#define PROFBTN_FREELANCE_Y 219
#define PROFBTN_MILITARY_X 686
#define PROFBTN_MILITARY_Y 219
#define BACKBTN_WIDTH 82
#define BACKBTN_HEIGHT 58

#define BACKBTN_X 12
#define BACKBTN_Y 698

#define FINISHBTN_X 860
#define FINISHBTN_Y 585

#define EVENT_NONE 0
#define EVENT_FINISH 101

#define TEXTCOL al_map_rgb(0,255,255)

#define TEXTHEIGHT_TITLES 60
#define TEXTHEIGHT_PROFESSIONNAMES 40
#define TEXTHEIGHT_NAME 30
#define TEXTHEIGHT_ATTRIBUTES 30

#define PROFESSIONAMES_VERT_SPACE 10

#define NAME_X 287
#define NAME_Y 184
#define NAME_MAXLEN 15

#define CURSOR_Y NAME_Y
#define CURSOR_DELAY 10

#define ATTS_X 287
#define ATTS_Y 144

#define ATTS_COMMON_X			287
#define ATTS_VALS_COMMON_X		487
#define ATTS_PLUS_COMMON_X		547
#define ATTS_MAX_COMMON_X		627
#define ATTS_AVAILPTS_COMMON_X  740
#define ATTS_Y_BASE			224
#define ATTS_Y_SPACING			50
#define ATTS_PLUS_Y_BASE		(ATTS_Y_BASE - 7)

#define RESET_X 868
#define RESET_Y 542

#define PROFINFO_X 200
#define PROFINFO_Y 400

#define DURABILITY_X		ATTS_COMMON_X
#define DURABILITY_Y		(ATTS_Y_BASE + (ATTS_Y_SPACING*0))
#define PLUS_DURABILITY_X  ATTS_PLUS_COMMON_X
#define PLUS_DURABILITY_Y  (ATTS_PLUS_Y_BASE + (ATTS_Y_SPACING*0))
#define LEARNRATE_X		ATTS_COMMON_X
#define LEARNRATE_Y		(ATTS_Y_BASE + (ATTS_Y_SPACING*1))
#define PLUS_LEARNRATE_X	ATTS_PLUS_COMMON_X
#define PLUS_LEARNRATE_Y	(ATTS_PLUS_Y_BASE + (ATTS_Y_SPACING*1))

#define SCIENCE_X				ATTS_COMMON_X
#define SCIENCE_Y				(ATTS_Y_BASE + (ATTS_Y_SPACING*3))
#define PLUS_SCIENCE_X		ATTS_PLUS_COMMON_X
#define PLUS_SCIENCE_Y		(ATTS_PLUS_Y_BASE + (ATTS_Y_SPACING*3))
#define NAVIGATION_X			ATTS_COMMON_X
#define NAVIGATION_Y			(ATTS_Y_BASE + (ATTS_Y_SPACING*4))
#define PLUS_NAVIGATION_X		ATTS_PLUS_COMMON_X
#define PLUS_NAVIGATION_Y		(ATTS_PLUS_Y_BASE + (ATTS_Y_SPACING*4))
#define TACTICS_X				ATTS_COMMON_X
#define TACTICS_Y				(ATTS_Y_BASE + (ATTS_Y_SPACING*5))
#define PLUS_TACTICS_X		ATTS_PLUS_COMMON_X
#define PLUS_TACTICS_Y		(ATTS_PLUS_Y_BASE + (ATTS_Y_SPACING*5))
#define ENGINEERING_X			ATTS_COMMON_X
#define ENGINEERING_Y			(ATTS_Y_BASE + (ATTS_Y_SPACING*6))
#define PLUS_ENGINEERING_X	ATTS_PLUS_COMMON_X
#define PLUS_ENGINEERING_Y	(ATTS_PLUS_Y_BASE + (ATTS_Y_SPACING*6))
#define COMMUNICATION_X		ATTS_COMMON_X
#define COMMUNICATION_Y		(ATTS_Y_BASE + (ATTS_Y_SPACING*7))
#define PLUS_COMMUNICATION_X  ATTS_PLUS_COMMON_X
#define PLUS_COMMUNICATION_Y  (ATTS_PLUS_Y_BASE + (ATTS_Y_SPACING*7))
#define MEDICAL_X				ATTS_COMMON_X
#define MEDICAL_Y				(ATTS_Y_BASE + (ATTS_Y_SPACING*8))
#define PLUS_MEDICAL_X		ATTS_PLUS_COMMON_X
#define PLUS_MEDICAL_Y		(ATTS_PLUS_Y_BASE + (ATTS_Y_SPACING*8))

#define BASEATT_SCIENTIFIC_DURABILITY		5
#define BASEATT_SCIENTIFIC_LEARNRATE		5
#define BASEATT_SCIENTIFIC_SCIENCE		15
#define BASEATT_SCIENTIFIC_NAVIGATION		5
#define BASEATT_SCIENTIFIC_TACTICS		0
#define BASEATT_SCIENTIFIC_ENGINEERING	5
#define BASEATT_SCIENTIFIC_COMMUNICATION  15
#define BASEATT_SCIENTIFIC_MEDICAL		10

#define MAXATT_SCIENTIFIC_DURABILITY		10
#define MAXATT_SCIENTIFIC_LEARNRATE		10
#define MAXATT_SCIENTIFIC_SCIENCE			250
#define MAXATT_SCIENTIFIC_NAVIGATION		95
#define MAXATT_SCIENTIFIC_TACTICS			65
#define MAXATT_SCIENTIFIC_ENGINEERING		95
#define MAXATT_SCIENTIFIC_COMMUNICATION	250
#define MAXATT_SCIENTIFIC_MEDICAL			125

#define BASEATT_FREELANCE_DURABILITY		5
#define BASEATT_FREELANCE_LEARNRATE		5
#define BASEATT_FREELANCE_SCIENCE			5
#define BASEATT_FREELANCE_NAVIGATION		15
#define BASEATT_FREELANCE_TACTICS			5
#define BASEATT_FREELANCE_ENGINEERING		10
#define BASEATT_FREELANCE_COMMUNICATION	15
#define BASEATT_FREELANCE_MEDICAL			0

#define MAXATT_FREELANCE_DURABILITY		10
#define MAXATT_FREELANCE_LEARNRATE		10
#define MAXATT_FREELANCE_SCIENCE			95
#define MAXATT_FREELANCE_NAVIGATION		250
#define MAXATT_FREELANCE_TACTICS			95
#define MAXATT_FREELANCE_ENGINEERING		125
#define MAXATT_FREELANCE_COMMUNICATION	250
#define MAXATT_FREELANCE_MEDICAL			65

#define BASEATT_MILITARY_DURABILITY		5
#define BASEATT_MILITARY_LEARNRATE		5
#define BASEATT_MILITARY_SCIENCE			0
#define BASEATT_MILITARY_NAVIGATION		10
#define BASEATT_MILITARY_TACTICS			15
#define BASEATT_MILITARY_ENGINEERING		10
#define BASEATT_MILITARY_COMMUNICATION	15
#define BASEATT_MILITARY_MEDICAL			0

#define MAXATT_MILITARY_DURABILITY		10
#define MAXATT_MILITARY_LEARNRATE		10
#define MAXATT_MILITARY_SCIENCE			65
#define MAXATT_MILITARY_NAVIGATION		125
#define MAXATT_MILITARY_TACTICS			250
#define MAXATT_MILITARY_ENGINEERING		125
#define MAXATT_MILITARY_COMMUNICATION	250
#define MAXATT_MILITARY_MEDICAL			65

#define INITIAL_AVAIL_PTS 5
#define INITIAL_AVAIL_PROF_PTS 25

#define EVENT_MINUS						10100

ModuleCaptainCreation::ModuleCaptainCreation(void)
: m_wizPage(WP_NONE)
, m_mouseOverImg(NULL)
, m_cursorIdx(0)
, m_cursorIdxDelay(0)
, m_profInfoBox(NULL)
{
}

ModuleCaptainCreation::~ModuleCaptainCreation(void){}

bool ModuleCaptainCreation::Init()
{
	// Load profession choice background
    m_professionChoiceBackground = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_professionbackground.bmp");
	if (m_professionChoiceBackground == NULL)
    {
        debug << "captaincreation: error loading profession background" << endl;
		return false;
    }

	// Load scientific button bitmaps
	m_scientificBtn = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_scientific.bmp");
	if (m_scientificBtn == NULL)
	{
		g_game->message("CaptainCreation: Error loading scientific button");
		return false;
	}

	m_scientificBtnMouseOver = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_scientific_mouseover.bmp");
	if (m_scientificBtnMouseOver == NULL)
	{
		g_game->message("CaptainCreation: Error loading scientific button mouseover");
		return false;
	}

	// Load freelance button bitmaps
	m_freelanceBtn = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_freelance.bmp"); 
	if (m_freelanceBtn == NULL)
	{
		g_game->message("CaptainCreation: Error loading freelance button");
		return false;
	}

	m_freelanceBtnMouseOver = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_freelance_mouseover.bmp");
	if (m_freelanceBtnMouseOver == NULL)
	{
		g_game->message("CaptainCreation: Error loading freelance button mouseover");
		return false;
	}

	// Load military button bitmaps
	m_militaryBtn = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_military.bmp");
	if (m_militaryBtn == NULL)
	{
		g_game->message("CaptainCreation: Error loading military button");
		return false;
	}

	m_militaryBtnMouseOver = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_military_mouseover.bmp");
	if (m_militaryBtnMouseOver == NULL)
	{
		g_game->message("CaptainCreation: Error loading military button mouseover");
		return false;
	}

	m_profInfoScientific = new Label("Even though the universe regresses towards smaller and smaller components, it is still plenty large to hide a few mysteries. The Scientific Officer represents the pinnacle of Myrrdanian brainpower. Armed with wit, cunning, intelligence... and a stun gun these brave souls explore the edges of the galaxy documenting planets and capturing life forms for study. Not to mention, the ability to recommend a planet for colonization comes with monetary and retirement perks. Mostly monetary seeing as distant planet construction usually takes some time to kick start.",
		150, 420, 750, 400, WHITE, g_game->font24);
	if (m_profInfoScientific == NULL)
		return false;
	m_profInfoScientific->Refresh();

	m_profInfoFreelance = new Label("There is a lot of money to be made in the Gamma Sector and the Freelancer's job is to get his hands on some. This jack of all trades profession is easily the most versatile Captain type in the galaxy. Capable of interstellar combat and properly equipped with modern scanning and exploring technology there is ample opportunity for the Freelancer to respond to most situations. One distinguishing feature is the greatly expanded cargo room which, of course, makes all those lowly Glush Cola shipments that much more profitable.",
		150, 420, 750, 400, WHITE, g_game->font24);
	if (m_profInfoFreelance == NULL)
		return false;
	m_profInfoFreelance->Refresh();

	m_profInfoMilitary =  new Label("The galaxy is a rough and tumble place where there is hardly ever a shortage of conflict. The Military Officer is the spear point of Myrrdan's influence and is often called upon to serve 'for the greater good.' Trained in tactical combat and given access to some of the highest class weaponry in the sector, it is never a bad time to be at the helm of a Wraith class warship. Being in front of it, however, is another scenario entirely.",
		150, 420, 750, 400, WHITE, g_game->font24);
	if (m_profInfoMilitary == NULL)
		return false;
	m_profInfoMilitary->Refresh();

	// Load details background
    m_detailsBackground = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_detailsbackground.bmp");
	if (m_detailsBackground == NULL)
    {
        debug << "captaincreation: error loading details background" << endl;
		return false;
    }

	// Load reset button bitmaps
	m_resetBtn = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_reset.bmp");
	if (m_resetBtn == NULL)
	{
		g_game->message("CaptainCreation: Error loading reset button");
		return false;
	}

	m_resetBtnMouseOver = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_reset_mouseover.bmp");
	if (m_resetBtnMouseOver == NULL)
	{
		g_game->message("CaptainCreation: Error loading reset button mouseover");
		return false;
	}

	// Load finish button bitmaps
	m_finishBtnNorm = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_finish.bmp");
	if (m_finishBtnNorm == NULL)
	{
		g_game->message("CaptainCreation: Error loading finish button");
		return false;
	}
	
	m_finishBtnOver = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_finish_mouseover.bmp");
	if (m_finishBtnOver == NULL)
	{
		g_game->message("CaptainCreation: Error loading finish button mouseover");
		return false;
	}
	
	m_finishBtnDis = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_finish_disabled.bmp");
	if (m_finishBtnDis == NULL)
	{
		g_game->message("CaptainCreation: Error loading finish button disabled");
		return false;
	}
	
	m_finishBtn = new Button(m_finishBtnNorm, m_finishBtnOver, m_finishBtnDis,
		FINISHBTN_X,FINISHBTN_Y,EVENT_NONE,EVENT_FINISH,"",false,true);
	if (m_finishBtn == NULL) return false;
	if (!m_finishBtn->IsInitialized()) return false;

	// Load cursor bitmaps
	m_cursor[0] = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_cursor0.bmp");
	if (m_cursor[0] == NULL)
	{
		g_game->message("CaptainCreation: Error loading cursor0");
		return false;
	}

	m_cursor[1] = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_cursor1.bmp");
	if (m_cursor[1] == NULL)
	{
		g_game->message("CaptainCreation: Error loading cursor1");
		return false;
	}

	// Load back button bitmaps
	m_backBtn = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_back.bmp");
	if (m_backBtn == NULL)
	{
		g_game->message("CaptainCreation: Error loading back button");
		return false;
	}

	m_backBtnMouseOver = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_back_mouseover.bmp");
	if (m_backBtnMouseOver == NULL)
	{
		g_game->message("CaptainCreation: Error loading back button mouseover");
		return false;
	}

	// Load plus button bitmaps
	m_plusBtn = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_plus.bmp");
	if (m_plusBtn == NULL)
	{
		g_game->message("CaptainCreation: Error loading plus button");
		return false;
	}

	m_plusBtnMouseOver = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_plus_mouseover.bmp");
	if (m_plusBtnMouseOver == NULL)
	{
		g_game->message("CaptainCreation: Error loading plus button mouseover");
		return false;
	}

	m_minusBtnNorm = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_minus.bmp");
	if (m_minusBtnNorm == NULL)
	{
		g_game->message("CaptainCreation: Error loading minus button");
		return false;
	}

	m_minusBtnOver = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_minus_mouseover.bmp");
	if (m_minusBtnOver == NULL)
	{
		g_game->message("CaptainCreation: Error loading minus button mouseover");
		return false;
	}

	m_minusBtnDis = (BITMAP*)al_load_bitmap("data/captaincreation/captaincreation_minus_disabled.bmp");
	if (m_minusBtnDis == NULL)
	{
		g_game->message("CaptainCreation: Error loading minus button disabled");
		return false;
	}
	
	m_minusBtns[0] = new Button(m_minusBtnNorm, m_minusBtnOver, m_minusBtnDis,
							 PLUS_DURABILITY_X + 42, PLUS_DURABILITY_Y, EVENT_NONE, EVENT_MINUS + 0,"");
	m_minusBtns[1] = new Button(m_minusBtnNorm, m_minusBtnOver, m_minusBtnDis,
							 PLUS_LEARNRATE_X + 42, PLUS_LEARNRATE_Y, EVENT_NONE, EVENT_MINUS + 1,"");
	m_minusBtns[2] = new Button(m_minusBtnNorm, m_minusBtnOver, m_minusBtnDis,
							 PLUS_SCIENCE_X + 42, PLUS_SCIENCE_Y, EVENT_NONE, EVENT_MINUS + 2,"");
	m_minusBtns[3] = new Button(m_minusBtnNorm, m_minusBtnOver, m_minusBtnDis,
							 PLUS_NAVIGATION_X + 42, PLUS_NAVIGATION_Y, EVENT_NONE, EVENT_MINUS + 3,"");
	m_minusBtns[4] = new Button(m_minusBtnNorm, m_minusBtnOver, m_minusBtnDis,
							 PLUS_TACTICS_X + 42, PLUS_TACTICS_Y, EVENT_NONE, EVENT_MINUS + 4,"");
	m_minusBtns[5] = new Button(m_minusBtnNorm, m_minusBtnOver, m_minusBtnDis,
							 PLUS_ENGINEERING_X + 42, PLUS_ENGINEERING_Y, EVENT_NONE, EVENT_MINUS + 5,"");
	m_minusBtns[6] = new Button(m_minusBtnNorm, m_minusBtnOver, m_minusBtnDis,
							 PLUS_COMMUNICATION_X + 42, PLUS_COMMUNICATION_Y, EVENT_NONE, EVENT_MINUS + 6,"");
	m_minusBtns[7] = new Button(m_minusBtnNorm, m_minusBtnOver, m_minusBtnDis,
							 PLUS_MEDICAL_X + 42, PLUS_MEDICAL_Y, EVENT_NONE, EVENT_MINUS + 7,"");

	for(int i = 0; i < 8; i++){
		if (m_minusBtns[i] == NULL) return false;
		if (!m_minusBtns[i]->IsInitialized()) return false;
	}


	//load audio files
	m_sndBtnClick = g_game->audioSystem->Load("data/captaincreation/buttonclick.ogg");
	if (!m_sndBtnClick) {
		g_game->message("Error loading data/captaincreation_buttonclick.ogg");
		return false;
	}

	m_sndClick = g_game->audioSystem->Load("data/captaincreation/click.ogg");
	if (!m_sndClick) {
		g_game->message("Error loading data/captaincreation_click.ogg");
		return false;
	}

	m_sndErr = g_game->audioSystem->Load("data/captaincreation/error.ogg");
	if (!m_sndErr) {
		g_game->message("Error loading data/captaincreation/error.ogg");
		return false;
	}

	m_mouseOverImg = NULL;
	m_profInfoBox = NULL;
	m_name = "";
	m_wizPage = WP_PROFESSION_CHOICE;

	return true;
}


void ModuleCaptainCreation::Update()
{
	Module::Update();

}

void ModuleCaptainCreation::Draw()
{
	static bool help1 = true;
	static bool help2 = true;

	switch(m_wizPage)
	{
	case WP_PROFESSION_CHOICE:
		{
			al_set_target_bitmap(g_game->GetBackBuffer());
			al_draw_bitmap_region(m_professionChoiceBackground, 0, 0, al_get_display_width(al_get_current_display()), al_get_display_height(al_get_current_display()), 0, 0, 0);
			al_draw_bitmap_region(m_scientificBtn, 0, 0, PROFBTN_WIDTH, PROFBTN_HEIGHT, PROFBTN_SCIENTIFIC_X, PROFBTN_SCIENTIFIC_Y, 0);
			al_draw_bitmap_region(m_freelanceBtn, 0, 0, PROFBTN_WIDTH, PROFBTN_HEIGHT, PROFBTN_FREELANCE_X, PROFBTN_FREELANCE_Y, 0);
			al_draw_bitmap_region(m_militaryBtn, 0, 0, PROFBTN_WIDTH, PROFBTN_HEIGHT, PROFBTN_MILITARY_X, PROFBTN_MILITARY_Y, 0);
			al_draw_bitmap_region(m_backBtn, 0, 0, BACKBTN_WIDTH, BACKBTN_HEIGHT, BACKBTN_X, BACKBTN_Y, 0);

			if (m_profInfoBox != NULL)
			{
				m_profInfoBox->Draw(g_game->GetBackBuffer());
			}

			if (m_mouseOverImg != NULL)
			{
				al_draw_bitmap_region(m_mouseOverImg, 0, 0, al_get_bitmap_width(m_mouseOverImg), al_get_bitmap_height(m_mouseOverImg), m_mouseOverImgX, m_mouseOverImgY, 0);
			}

			//display tutorial help messages for beginners
			if ( (!g_game->gameState->firstTimeVisitor || g_game->gameState->getActiveQuest() > 1) ) help1 = false;
			if ( help1 )
			{
				help1 = false;
				string str = "Okay, let's create a new character for you, starting with your choice of profession. Choose a Science, Freelance, or Military career.";
				g_game->ShowMessageBoxWindow("",  str, 400, 300, YELLOW, 600, 400, false);
			}

		}
		break;

	case WP_DETAILS:
		{
			al_set_target_bitmap(g_game->GetBackBuffer());
			al_draw_bitmap_region(m_detailsBackground, 0, 0, al_get_bitmap_width(g_game->GetBackBuffer()), al_get_bitmap_height(g_game->GetBackBuffer()), 0, 0, 0);

			al_draw_bitmap_region(m_backBtn, 0, 0, BACKBTN_WIDTH, BACKBTN_HEIGHT, BACKBTN_X, BACKBTN_Y, 0);

		alfont_set_font_size(g_game->font10, TEXTHEIGHT_TITLES);
		alfont_textout_centre(g_game->GetBackBuffer(),g_game->font10,"Captain Details",al_get_bitmap_width(g_game->GetBackBuffer())/2,30,color_to_int(TEXTCOL));

		alfont_set_font_size(g_game->font10,TEXTHEIGHT_NAME);
		char n[128];
		sprintf(n,"Name: %s", m_name.c_str());
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,NAME_X,NAME_Y,color_to_int(TEXTCOL),n);

			int nlen = alfont_text_length(g_game->font10,n);
			al_draw_bitmap_region(m_cursor[m_cursorIdx], 0, 0, al_get_bitmap_width(m_cursor[m_cursorIdx]), al_get_bitmap_height(m_cursor[m_cursorIdx]), NAME_X+nlen+2, CURSOR_Y, 0);

			if (++m_cursorIdxDelay > CURSOR_DELAY)
			{
			m_cursorIdxDelay = 0;
			m_cursorIdx++;
			if (m_cursorIdx > 1)
				m_cursorIdx = 0;
			}

			alfont_set_font_size(g_game->font10,TEXTHEIGHT_ATTRIBUTES);

//			alfont_textprintf(m_canvas,g_game->font10,ATTS_X,ATTS_Y,color_to_int(TEXTCOL),"Starting Attributes");
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,DURABILITY_X,DURABILITY_Y,color_to_int(TEXTCOL),"Durability");
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,LEARNRATE_X,LEARNRATE_Y,color_to_int(TEXTCOL),"Learn Rate");
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,SCIENCE_X,SCIENCE_Y,color_to_int(TEXTCOL),"Science");
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,NAVIGATION_X,NAVIGATION_Y,color_to_int(TEXTCOL),"Navigation");
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,TACTICS_X,TACTICS_Y,color_to_int(TEXTCOL),"Tactics");
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,ENGINEERING_X,ENGINEERING_Y,color_to_int(TEXTCOL),"Engineering");
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,COMMUNICATION_X,COMMUNICATION_Y,color_to_int(TEXTCOL),"Communication");
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,MEDICAL_X,MEDICAL_Y,color_to_int(TEXTCOL),"Medical");

		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,ATTS_AVAILPTS_COMMON_X+20,DURABILITY_Y,color_to_int(TEXTCOL),"%d available",m_availPts);
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,ATTS_VALS_COMMON_X,DURABILITY_Y,color_to_int(TEXTCOL),"%d",m_attributes.durability);
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,ATTS_VALS_COMMON_X,LEARNRATE_Y,color_to_int(TEXTCOL),"%d",m_attributes.learnRate);
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,ATTS_AVAILPTS_COMMON_X+20,SCIENCE_Y,color_to_int(TEXTCOL),"%d available",m_availProfPts);
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,ATTS_VALS_COMMON_X,SCIENCE_Y,color_to_int(TEXTCOL),"%d",m_attributes.science);
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,ATTS_VALS_COMMON_X,NAVIGATION_Y,color_to_int(TEXTCOL),"%d",m_attributes.navigation);
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,ATTS_VALS_COMMON_X,TACTICS_Y,color_to_int(TEXTCOL),"%d",m_attributes.tactics);
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,ATTS_VALS_COMMON_X,ENGINEERING_Y,color_to_int(TEXTCOL),"%d",m_attributes.engineering);
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,ATTS_VALS_COMMON_X,COMMUNICATION_Y,color_to_int(TEXTCOL),"%d",m_attributes.communication);
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,ATTS_VALS_COMMON_X,MEDICAL_Y,color_to_int(TEXTCOL),"%d",m_attributes.medical);

		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,ATTS_MAX_COMMON_X,DURABILITY_Y,color_to_int(TEXTCOL)," (%d max)",m_attributesMax.durability);
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,ATTS_MAX_COMMON_X,LEARNRATE_Y,color_to_int(TEXTCOL)," (%d max)",m_attributesMax.learnRate);
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,ATTS_MAX_COMMON_X,SCIENCE_Y,color_to_int(TEXTCOL)," (%d max)",m_attributesMax.science);
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,ATTS_MAX_COMMON_X,NAVIGATION_Y,color_to_int(TEXTCOL)," (%d max)",m_attributesMax.navigation);
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,ATTS_MAX_COMMON_X,TACTICS_Y,color_to_int(TEXTCOL)," (%d max)",m_attributesMax.tactics);
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,ATTS_MAX_COMMON_X,ENGINEERING_Y,color_to_int(TEXTCOL)," (%d max)",m_attributesMax.engineering);
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,ATTS_MAX_COMMON_X,COMMUNICATION_Y,color_to_int(TEXTCOL)," (%d max)",m_attributesMax.communication);
		alfont_textprintf(g_game->GetBackBuffer(),g_game->font10,ATTS_MAX_COMMON_X,MEDICAL_Y,color_to_int(TEXTCOL)," (%d max)",m_attributesMax.medical);

			al_set_target_bitmap(g_game->GetBackBuffer());
			al_draw_bitmap_region(m_plusBtn, 0, 0, al_get_bitmap_width(m_plusBtn), al_get_bitmap_height(m_plusBtn), PLUS_DURABILITY_X, PLUS_DURABILITY_Y, 0);
			al_draw_bitmap_region(m_plusBtn, 0, 0, al_get_bitmap_width(m_plusBtn), al_get_bitmap_height(m_plusBtn), PLUS_LEARNRATE_X, PLUS_LEARNRATE_Y, 0);
			al_draw_bitmap_region(m_plusBtn, 0, 0, al_get_bitmap_width(m_plusBtn), al_get_bitmap_height(m_plusBtn), PLUS_SCIENCE_X, PLUS_SCIENCE_Y, 0);
			al_draw_bitmap_region(m_plusBtn, 0, 0, al_get_bitmap_width(m_plusBtn), al_get_bitmap_height(m_plusBtn), PLUS_NAVIGATION_X, PLUS_NAVIGATION_Y, 0);
			al_draw_bitmap_region(m_plusBtn, 0, 0, al_get_bitmap_width(m_plusBtn), al_get_bitmap_height(m_plusBtn), PLUS_TACTICS_X, PLUS_TACTICS_Y, 0);
			al_draw_bitmap_region(m_plusBtn, 0, 0, al_get_bitmap_width(m_plusBtn), al_get_bitmap_height(m_plusBtn), PLUS_ENGINEERING_X, PLUS_ENGINEERING_Y, 0);
			al_draw_bitmap_region(m_plusBtn, 0, 0, al_get_bitmap_width(m_plusBtn), al_get_bitmap_height(m_plusBtn), PLUS_COMMUNICATION_X, PLUS_COMMUNICATION_Y, 0);
			al_draw_bitmap_region(m_plusBtn, 0, 0, al_get_bitmap_width(m_plusBtn), al_get_bitmap_height(m_plusBtn), PLUS_MEDICAL_X, PLUS_MEDICAL_Y, 0);

			al_draw_bitmap_region(m_resetBtn, 0, 0, al_get_bitmap_width(m_resetBtn), al_get_bitmap_height(m_resetBtn), RESET_X, RESET_Y, 0);

			if ((m_availPts == 0) &&
				(m_availProfPts == 0) &&
				(m_name.size() > 0))
			{
			m_finishBtn->SetEnabled(true);
			}
			else
			{
			m_finishBtn->SetEnabled(false);
			}

			m_finishBtn->Run(g_game->GetBackBuffer());

			if (m_mouseOverImg != NULL)
			{
			al_draw_bitmap_region(m_mouseOverImg, 0, 0, al_get_bitmap_width(m_mouseOverImg), al_get_bitmap_height(m_mouseOverImg), m_mouseOverImgX, m_mouseOverImgY, 0);
			}
			for(int i=0; i<8; i++){
				m_minusBtns[i]->Run(g_game->GetBackBuffer());
			}

			//display tutorial help messages for beginners
			if ( (!g_game->gameState->firstTimeVisitor || g_game->gameState->getActiveQuest() > 1) ) help2 = false;
			if ( help2 )
			{
				help2 = false;
				string str = "Next, you need to enter a name for your captain, and then set your attribute points: 5 points to Durability or Learning Rate, and 25 points to all the rest. You must allocate all of the points before continuing.";
				g_game->ShowMessageBoxWindow("",  str, 400, 300, YELLOW, 10, 250, false);
			}
		}
		break;
	}
}


void ModuleCaptainCreation::Close()
{
	try {
        if (m_professionChoiceBackground!=NULL)
        {
            al_destroy_bitmap(m_professionChoiceBackground);
            m_professionChoiceBackground=NULL;
        }
        if (m_detailsBackground!=NULL)
        {
            al_destroy_bitmap(m_detailsBackground);
            m_detailsBackground=NULL;
        }
		if (m_scientificBtn != NULL)
		{
			al_destroy_bitmap(m_scientificBtn);
			m_scientificBtn = NULL;
		}
		if (m_scientificBtnMouseOver != NULL)
		{
			al_destroy_bitmap(m_scientificBtnMouseOver);
			m_scientificBtnMouseOver = NULL;
		}
		if (m_freelanceBtn != NULL)
		{
			al_destroy_bitmap(m_freelanceBtn);
			m_freelanceBtn = NULL;
		}
		if (m_freelanceBtnMouseOver != NULL)
		{
			al_destroy_bitmap(m_freelanceBtnMouseOver);
			m_freelanceBtnMouseOver = NULL;
		}
		if (m_militaryBtn != NULL)
		{
			al_destroy_bitmap(m_militaryBtn);
			m_militaryBtn = NULL;
		}
		if (m_militaryBtnMouseOver != NULL)
		{
			al_destroy_bitmap(m_militaryBtnMouseOver);
			m_militaryBtnMouseOver = NULL;
		}
		if (m_resetBtn != NULL)
		{
			al_destroy_bitmap(m_resetBtn);
			m_resetBtn = NULL;
		}
		if (m_resetBtnMouseOver != NULL)
		{
			al_destroy_bitmap(m_resetBtnMouseOver);
			m_resetBtnMouseOver = NULL;
		}
		if (m_finishBtnNorm != NULL)
		{
			al_destroy_bitmap(m_finishBtnNorm);
			m_finishBtnNorm = NULL;
		}
		if (m_finishBtnOver != NULL)
		{
			al_destroy_bitmap(m_finishBtnOver);
			m_finishBtnOver = NULL;
		}
		if (m_finishBtnDis != NULL)
		{
			al_destroy_bitmap(m_finishBtnDis);
			m_finishBtnDis = NULL;
		}
		if (m_cursor[0] != NULL)
		{
			al_destroy_bitmap(m_cursor[0]);
			m_cursor[0] = NULL;
		}
		if (m_cursor[1] != NULL)
		{
			al_destroy_bitmap(m_cursor[1]);
			m_cursor[1] = NULL;
		}
		if (m_backBtn != NULL)
		{
			al_destroy_bitmap(m_backBtn);
			m_backBtn = NULL;
		}
		if (m_backBtnMouseOver != NULL)
		{
			al_destroy_bitmap(m_backBtnMouseOver);
			m_backBtnMouseOver = NULL;
		}
		if (m_plusBtn != NULL)
		{
			al_destroy_bitmap(m_plusBtn);
			m_plusBtn = NULL;
		}
		if (m_plusBtnMouseOver != NULL)
		{
			al_destroy_bitmap(m_plusBtnMouseOver);
			m_plusBtnMouseOver = NULL;
		}
		// Note: minus button bitmaps are NULL, no need to destroy
		if (m_profInfoScientific != NULL)
		{
			delete m_profInfoScientific;
			m_profInfoScientific = NULL;
		}
		if (m_profInfoFreelance != NULL)
		{
			delete m_profInfoFreelance;
			m_profInfoFreelance = NULL;
		}
		if (m_profInfoMilitary != NULL)
		{
			delete m_profInfoMilitary;
			m_profInfoMilitary = NULL;
		}
		if (m_finishBtn != NULL)
		{
			delete m_finishBtn;
			m_finishBtn = NULL;
		}
		if (m_sndBtnClick != NULL)
		{
			m_sndBtnClick = NULL;
		}
		if (m_sndClick != NULL)
		{
			m_sndClick = NULL;
		}
		if (m_sndErr != NULL)
		{
			m_sndErr = NULL;
		}
		for (int i=0; i < 8; i++){
			delete m_minusBtns[i];
			m_minusBtns[i] = NULL;
		}
	}
	catch(std::exception e) {
		debug << e.what() << endl;
	}
	catch(...) {
		debug << "Unhandled exception in CaptainCreation::Close" << endl;
	}


}




void ModuleCaptainCreation::OnKeyPress(int keyCode)
{
	Module::OnKeyPress(keyCode);
}

void ModuleCaptainCreation::OnKeyPressed(int keyCode)
{
	Module::OnKeyPressed(keyCode);

	switch(m_wizPage)
	{
	case WP_DETAILS:
		{
			bool playKeySnd = false;
			bool playErrSnd = false;

			if (((keyCode >= KEY_A) && (keyCode <= KEY_9_PAD)) || (keyCode == KEY_SPACE))
			{
			if (m_name.size() < NAME_MAXLEN)
			{
				char c = (char)scancode_to_ascii(keyCode);

				ALLEGRO_KEYBOARD_STATE keyboard_state;
				al_get_keyboard_state(&keyboard_state);
				if ((al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT) || al_key_down(&keyboard_state, ALLEGRO_KEY_RSHIFT)) && (keyCode < KEY_0) && (keyCode != KEY_SPACE))
				{
					c -= 32;
				}

				m_name.push_back(c);

				playKeySnd = true;
			}
			else
				playErrSnd = true;
			}
			else if (keyCode == KEY_BACKSPACE)
			{
			if (m_name.size() > 0)
			{
				m_name.erase(--(m_name.end()));

				playKeySnd = true;
			}
			else
				playErrSnd = true;
			}

			if (playKeySnd)
			{		
				g_game->audioSystem->Play(m_sndClick);
			}

			if (playErrSnd)
			{		
				g_game->audioSystem->Play(m_sndErr); 
			}
		}
		break;
	}
}

void ModuleCaptainCreation::OnKeyReleased(int keyCode)
{
	Module::OnKeyReleased(keyCode);
}

void ModuleCaptainCreation::OnMouseMove(int x, int y)
{
	Module::OnMouseMove(x,y);

	switch(m_wizPage)
	{
	case WP_PROFESSION_CHOICE:
		{
			if ((x >= PROFBTN_SCIENTIFIC_X) &&
				(x < (PROFBTN_SCIENTIFIC_X + PROFBTN_WIDTH)) &&
				(y >= PROFBTN_SCIENTIFIC_Y) &&
				(y < (PROFBTN_SCIENTIFIC_Y + PROFBTN_HEIGHT)))
			{
			m_mouseOverImg = m_scientificBtnMouseOver;
			m_profInfoBox = m_profInfoScientific;
			m_mouseOverImgX = PROFBTN_SCIENTIFIC_X;
			m_mouseOverImgY = PROFBTN_SCIENTIFIC_Y;
			}
			else if ((x >= PROFBTN_FREELANCE_X) &&
				(x < (PROFBTN_FREELANCE_X + PROFBTN_WIDTH)) &&
				(y >= PROFBTN_FREELANCE_Y) &&
				(y < (PROFBTN_FREELANCE_Y + PROFBTN_HEIGHT)))
			{
			m_mouseOverImg = m_freelanceBtnMouseOver;
			m_profInfoBox = m_profInfoFreelance;
			m_mouseOverImgX = PROFBTN_FREELANCE_X;
			m_mouseOverImgY = PROFBTN_FREELANCE_Y;
			}
			else if ((x >= PROFBTN_MILITARY_X) &&
				(x < (PROFBTN_MILITARY_X + PROFBTN_WIDTH)) &&
				(y >= PROFBTN_MILITARY_Y) &&
				(y < (PROFBTN_MILITARY_Y + PROFBTN_HEIGHT)))
			{
			m_mouseOverImg = m_militaryBtnMouseOver;
			m_profInfoBox = m_profInfoMilitary;
			m_mouseOverImgX = PROFBTN_MILITARY_X;
			m_mouseOverImgY = PROFBTN_MILITARY_Y;
			}
			else if ((x >= BACKBTN_X) &&
				(x < (BACKBTN_X + BACKBTN_WIDTH)) &&
				(y >= BACKBTN_Y) &&
				(y < (BACKBTN_Y + BACKBTN_HEIGHT)))
			{
			m_mouseOverImg = m_backBtnMouseOver;
			m_mouseOverImgX = BACKBTN_X;
			m_mouseOverImgY = BACKBTN_Y;
			}
			else
			{
			m_mouseOverImg = NULL;
			m_profInfoBox = NULL;
			}
		}
		break;

	case WP_DETAILS:
		{
			for(int i=0; i<8; i++){
				m_minusBtns[i]->OnMouseMove(x,y);
			}
			if ((x >= BACKBTN_X) &&
				(x < (BACKBTN_X + BACKBTN_WIDTH)) &&
				(y >= BACKBTN_Y) &&
				(y < (BACKBTN_Y + BACKBTN_HEIGHT)))
			{
			m_mouseOverImg = m_backBtnMouseOver;
			m_mouseOverImgX = BACKBTN_X;
			m_mouseOverImgY = BACKBTN_Y;
			}
			else if ((x >= PLUS_DURABILITY_X) &&
				(x < (PLUS_DURABILITY_X + al_get_bitmap_width(m_plusBtn))) &&
				(y >= PLUS_DURABILITY_Y) &&
				(y < (PLUS_DURABILITY_Y + al_get_bitmap_height(m_plusBtn))))
			{
			m_mouseOverImg = m_plusBtnMouseOver;
			m_mouseOverImgX = PLUS_DURABILITY_X;
			m_mouseOverImgY = PLUS_DURABILITY_Y;
			}
			else if ((x >= PLUS_LEARNRATE_X) &&
				(x < (PLUS_LEARNRATE_X + al_get_bitmap_width(m_plusBtn))) &&
				(y >= PLUS_LEARNRATE_Y) &&
				(y < (PLUS_LEARNRATE_Y + al_get_bitmap_height(m_plusBtn))))
			{
			m_mouseOverImg = m_plusBtnMouseOver;
			m_mouseOverImgX = PLUS_LEARNRATE_X;
			m_mouseOverImgY = PLUS_LEARNRATE_Y;
			}
			else if ((x >= PLUS_SCIENCE_X) &&
				(x < (PLUS_SCIENCE_X + al_get_bitmap_width(m_plusBtn))) &&
				(y >= PLUS_SCIENCE_Y) &&
				(y < (PLUS_SCIENCE_Y + al_get_bitmap_height(m_plusBtn))))
			{
			m_mouseOverImg = m_plusBtnMouseOver;
			m_mouseOverImgX = PLUS_SCIENCE_X;
			m_mouseOverImgY = PLUS_SCIENCE_Y;
			}
			else if ((x >= PLUS_NAVIGATION_X) &&
				(x < (PLUS_NAVIGATION_X + al_get_bitmap_width(m_plusBtn))) &&
				(y >= PLUS_NAVIGATION_Y) &&
				(y < (PLUS_NAVIGATION_Y + al_get_bitmap_height(m_plusBtn))))
			{
			m_mouseOverImg = m_plusBtnMouseOver;
			m_mouseOverImgX = PLUS_NAVIGATION_X;
			m_mouseOverImgY = PLUS_NAVIGATION_Y;
			}
			else if ((x >= PLUS_TACTICS_X) &&
				(x < (PLUS_TACTICS_X + al_get_bitmap_width(m_plusBtn))) &&
				(y >= PLUS_TACTICS_Y) &&
				(y < (PLUS_TACTICS_Y + al_get_bitmap_height(m_plusBtn))))
			{
			m_mouseOverImg = m_plusBtnMouseOver;
			m_mouseOverImgX = PLUS_TACTICS_X;
			m_mouseOverImgY = PLUS_TACTICS_Y;
			}
			else if ((x >= PLUS_ENGINEERING_X) &&
				(x < (PLUS_ENGINEERING_X + al_get_bitmap_width(m_plusBtn))) &&
				(y >= PLUS_ENGINEERING_Y) &&
				(y < (PLUS_ENGINEERING_Y + al_get_bitmap_height(m_plusBtn))))
			{
			m_mouseOverImg = m_plusBtnMouseOver;
			m_mouseOverImgX = PLUS_ENGINEERING_X;
			m_mouseOverImgY = PLUS_ENGINEERING_Y;
			}
			else if ((x >= PLUS_COMMUNICATION_X) &&
				(x < (PLUS_COMMUNICATION_X + al_get_bitmap_width(m_plusBtn))) &&
				(y >= PLUS_COMMUNICATION_Y) &&
				(y < (PLUS_COMMUNICATION_Y + al_get_bitmap_height(m_plusBtn))))
			{
			m_mouseOverImg = m_plusBtnMouseOver;
			m_mouseOverImgX = PLUS_COMMUNICATION_X;
			m_mouseOverImgY = PLUS_COMMUNICATION_Y;
			}
			else if ((x >= PLUS_MEDICAL_X) &&
				(x < (PLUS_MEDICAL_X + al_get_bitmap_width(m_plusBtn))) &&
				(y >= PLUS_MEDICAL_Y) &&
				(y < (PLUS_MEDICAL_Y + al_get_bitmap_height(m_plusBtn))))
			{
			m_mouseOverImg = m_plusBtnMouseOver;
			m_mouseOverImgX = PLUS_MEDICAL_X;
			m_mouseOverImgY = PLUS_MEDICAL_Y;
			}
			else if ((x >= RESET_X) &&
				(x < (RESET_X + al_get_bitmap_width(m_resetBtn))) &&
				(y >= RESET_Y) &&
				(y < (RESET_Y + al_get_bitmap_height(m_resetBtn))))
			{
			m_mouseOverImg = m_resetBtnMouseOver;
			m_mouseOverImgX = RESET_X;
			m_mouseOverImgY = RESET_Y;
			}
			else
			{
			m_mouseOverImg = NULL;
			}

			m_finishBtn->OnMouseMove(x,y);
		}
		break;
	}
}

void ModuleCaptainCreation::OnMouseClick(int button, int x, int y)
{
	Module::OnMouseClick(button,x,y);

}

void ModuleCaptainCreation::OnMousePressed(int button, int x, int y)
{
	Module::OnMousePressed(button, x, y);
}

void ModuleCaptainCreation::OnMouseReleased(int button, int x, int y)
{
	Module::OnMouseReleased(button, x, y);

	switch(m_wizPage)
	{
	case WP_PROFESSION_CHOICE:
		{
			if ((x >= PROFBTN_SCIENTIFIC_X) &&
				(x < (PROFBTN_SCIENTIFIC_X + PROFBTN_WIDTH)) &&
				(y >= PROFBTN_SCIENTIFIC_Y) &&
				(y < (PROFBTN_SCIENTIFIC_Y + PROFBTN_HEIGHT)))
			{
				g_game->audioSystem->Play(m_sndBtnClick);
				m_wizPage = WP_DETAILS;
				m_mouseOverImg = NULL;
				chooseScience();
			}
			else if ((x >= PROFBTN_FREELANCE_X) &&
				(x < (PROFBTN_FREELANCE_X + PROFBTN_WIDTH)) &&
				(y >= PROFBTN_FREELANCE_Y) &&
				(y < (PROFBTN_FREELANCE_Y + PROFBTN_HEIGHT)))
			{
				g_game->audioSystem->Play(m_sndBtnClick);
				m_wizPage = WP_DETAILS;
				m_mouseOverImg = NULL;
				chooseFreelance();
			}
			else if ((x >= PROFBTN_MILITARY_X) &&
				(x < (PROFBTN_MILITARY_X + PROFBTN_WIDTH)) &&
				(y >= PROFBTN_MILITARY_Y) &&
				(y < (PROFBTN_MILITARY_Y + PROFBTN_HEIGHT)))
			{
				g_game->audioSystem->Play(m_sndBtnClick);
				m_wizPage = WP_DETAILS;
				m_mouseOverImg = NULL;
				chooseMilitary();
			}
			else if ((x >= BACKBTN_X) &&
				(x < (BACKBTN_X + BACKBTN_WIDTH)) &&
				(y >= BACKBTN_Y) &&
				(y < (BACKBTN_Y + BACKBTN_HEIGHT)))
			{
				g_game->audioSystem->Play(m_sndBtnClick);
				m_wizPage = WP_PROFESSION_CHOICE;
				m_mouseOverImg = NULL;

				//return player to previous screen
				g_game->modeMgr->LoadModule(g_game->modeMgr->GetPrevModuleName());
				return;
			}
		}
		break;

	case WP_DETAILS:
		{
			bool playSnd = false;
			bool playErrSnd = false;

			for(int i=0; i<8; i++){
				m_minusBtns[i]->OnMouseReleased(button, x,y);
			}

			if ((x >= BACKBTN_X) &&
				(x < (BACKBTN_X + BACKBTN_WIDTH)) &&
				(y >= BACKBTN_Y) &&
				(y < (BACKBTN_Y + BACKBTN_HEIGHT)))
			{
			playSnd = true;
			m_wizPage = WP_PROFESSION_CHOICE;
			m_mouseOverImg = NULL;
			m_profInfoBox = NULL;
			}
			else if ((x >= PLUS_DURABILITY_X) &&
				(x < (PLUS_DURABILITY_X + al_get_bitmap_width(m_plusBtn))) &&
				(y >= PLUS_DURABILITY_Y) &&
				(y < (PLUS_DURABILITY_Y + al_get_bitmap_height(m_plusBtn))))
			{
			if ((m_availPts > 0) && (m_attributes.durability < m_attributesMax.durability))
			{
				playSnd = true;
				m_attributes.durability++;
				m_availPts--;
			}
			else
				playErrSnd = true;
			}
			else if ((x >= PLUS_LEARNRATE_X) &&
				(x < (PLUS_LEARNRATE_X + al_get_bitmap_width(m_plusBtn))) &&
				(y >= PLUS_LEARNRATE_Y) &&
				(y < (PLUS_LEARNRATE_Y + al_get_bitmap_height(m_plusBtn))))
			{
			if ((m_availPts > 0) && (m_attributes.learnRate < m_attributesMax.learnRate))
			{
				playSnd = true;
				m_attributes.learnRate++;
				m_availPts--;
			}
			else
				playErrSnd = true;
			}
			else if ((x >= PLUS_SCIENCE_X) &&
				(x < (PLUS_SCIENCE_X + al_get_bitmap_width(m_plusBtn))) &&
				(y >= PLUS_SCIENCE_Y) &&
				(y < (PLUS_SCIENCE_Y + al_get_bitmap_height(m_plusBtn))))
			{
			if ((m_availProfPts > 0) && (m_attributes.science < m_attributesMax.science))
			{
				playSnd = true;
				m_attributes.science++;
				m_availProfPts--;
			}
			else
				playErrSnd = true;
			}
			else if ((x >= PLUS_NAVIGATION_X) &&
				(x < (PLUS_NAVIGATION_X + al_get_bitmap_width(m_plusBtn))) &&
				(y >= PLUS_NAVIGATION_Y) &&
				(y < (PLUS_NAVIGATION_Y + al_get_bitmap_height(m_plusBtn))))
			{
			if ((m_availProfPts > 0) && (m_attributes.navigation < m_attributesMax.navigation))
			{
				playSnd = true;
				m_attributes.navigation++;
				m_availProfPts--;
			}
			else
				playErrSnd = true;
			}
			else if ((x >= PLUS_TACTICS_X) &&
				(x < (PLUS_TACTICS_X + al_get_bitmap_width(m_plusBtn))) &&
				(y >= PLUS_TACTICS_Y) &&
				(y < (PLUS_TACTICS_Y + al_get_bitmap_height(m_plusBtn))))
			{
			if ((m_availProfPts > 0) && (m_attributes.tactics < m_attributesMax.tactics))
			{
				playSnd = true;
				m_attributes.tactics++;
				m_availProfPts--;
			}
			else
				playErrSnd = true;
			}
			else if ((x >= PLUS_ENGINEERING_X) &&
				(x < (PLUS_ENGINEERING_X + al_get_bitmap_width(m_plusBtn))) &&
				(y >= PLUS_ENGINEERING_Y) &&
				(y < (PLUS_ENGINEERING_Y + al_get_bitmap_height(m_plusBtn))))
			{
			if ((m_availProfPts > 0) && (m_attributes.engineering < m_attributesMax.engineering))
			{
				playSnd = true;
				m_attributes.engineering++;
				m_availProfPts--;
			}
			else
				playErrSnd = true;
			}
			else if ((x >= PLUS_COMMUNICATION_X) &&
				(x < (PLUS_COMMUNICATION_X + al_get_bitmap_width(m_plusBtn))) &&
				(y >= PLUS_COMMUNICATION_Y) &&
				(y < (PLUS_COMMUNICATION_Y + al_get_bitmap_height(m_plusBtn))))
			{
			if ((m_availProfPts > 0) && (m_attributes.communication < m_attributesMax.communication))
			{
				playSnd = true;
				m_attributes.communication++;
				m_availProfPts--;
			}
			else
				playErrSnd = true;
			}
			else if ((x >= PLUS_MEDICAL_X) &&
				(x < (PLUS_MEDICAL_X + al_get_bitmap_width(m_plusBtn))) &&
				(y >= PLUS_MEDICAL_Y) &&
				(y < (PLUS_MEDICAL_Y + al_get_bitmap_height(m_plusBtn))))
			{
			if ((m_availProfPts > 0) && (m_attributes.medical < m_attributesMax.medical))
			{
				playSnd = true;
				m_attributes.medical++;
				m_availProfPts--;
			}
			else
				playErrSnd = true;
			}
			else if ((x >= RESET_X) &&
				(x < (RESET_X + al_get_bitmap_width(m_resetBtn))) &&
				(y >= RESET_Y) &&
				(y < (RESET_Y + al_get_bitmap_height(m_resetBtn))))
			{
			playSnd = true;
			m_attributes = m_attributesInitial;
			m_availPts = INITIAL_AVAIL_PTS;
			m_availProfPts = INITIAL_AVAIL_PROF_PTS;
			}

			m_finishBtn->OnMouseReleased(button,x,y);

			if (playSnd)
			{
				g_game->audioSystem->Play(m_sndBtnClick);
			}

			if (playErrSnd)
			{
				g_game->audioSystem->Play(m_sndErr);
			}
		}
		break;
	}
}

void ModuleCaptainCreation::OnMouseWheelUp(int x, int y)
{
	Module::OnMouseWheelUp(x, y);
}

void ModuleCaptainCreation::OnMouseWheelDown(int x, int y)
{
	Module::OnMouseWheelDown(x, y);
}





void ModuleCaptainCreation::chooseFreelance()
{
	//set freelance attributes
	m_profession = PROFESSION_FREELANCE;
	m_attributes.durability = BASEATT_FREELANCE_DURABILITY;
	m_attributes.learnRate = BASEATT_FREELANCE_LEARNRATE;
	m_attributes.science = BASEATT_FREELANCE_SCIENCE;
	m_attributes.navigation = BASEATT_FREELANCE_NAVIGATION;
	m_attributes.tactics = BASEATT_FREELANCE_TACTICS;
	m_attributes.engineering = BASEATT_FREELANCE_ENGINEERING;
	m_attributes.communication = BASEATT_FREELANCE_COMMUNICATION;
	m_attributes.medical = BASEATT_FREELANCE_MEDICAL;
	m_attributesInitial = m_attributes;

	// set attribute max values
	m_attributesMax.durability = MAXATT_FREELANCE_DURABILITY;
	m_attributesMax.learnRate = MAXATT_FREELANCE_LEARNRATE;
	m_attributesMax.science = MAXATT_FREELANCE_SCIENCE;
	m_attributesMax.navigation = MAXATT_FREELANCE_NAVIGATION;
	m_attributesMax.tactics = MAXATT_FREELANCE_TACTICS;
	m_attributesMax.engineering = MAXATT_FREELANCE_ENGINEERING;
	m_attributesMax.communication = MAXATT_FREELANCE_COMMUNICATION;
	m_attributesMax.medical = MAXATT_FREELANCE_MEDICAL;

	m_availPts = INITIAL_AVAIL_PTS;
	m_availProfPts = INITIAL_AVAIL_PROF_PTS;

	//store attributes in gamestate	
	g_game->gameState->setProfession(m_profession);
	g_game->gameState->officerCap->attributes = m_attributes;

	//set ship name and properties based on profession
	Ship ship = g_game->gameState->getShip();
	ship.setName("Acquisition");
	int value = g_game->getGlobalNumber("PROF_FREELANCE_ARMOR");
	ship.setArmorClass(value);
	ship.setArmorIntegrity(100.0);
	value = g_game->getGlobalNumber("PROF_FREELANCE_ENGINE");
	ship.setEngineClass(value);
	ship.setEngineIntegrity(100.0);
	value = g_game->getGlobalNumber("PROF_FREELANCE_SHIELD");
	ship.setShieldClass(value);
	ship.setShieldIntegrity(100.0);
	value = g_game->getGlobalNumber("PROF_FREELANCE_LASER");
	ship.setLaserClass(value);
	ship.setLaserIntegrity(100.0);
	value = g_game->getGlobalNumber("PROF_FREELANCE_MISSILE");
	ship.setMissileLauncherClass(value);
	ship.setMissileLauncherIntegrity(100.0);
	value = g_game->getGlobalNumber("PROF_FREELANCE_PODS");
	ship.setCargoPodCount(value);

	int maxEngineClass=0, maxShieldClass=0, maxArmorClass=0, maxMissileLauncherClass=0, maxLaserClass=0;
	maxEngineClass          = g_game->getGlobalNumber("PROF_FREELANCE_ENGINE_MAX");
	maxShieldClass          = g_game->getGlobalNumber("PROF_FREELANCE_SHIELD_MAX");
	maxArmorClass           = g_game->getGlobalNumber("PROF_FREELANCE_ARMOR_MAX");
	maxMissileLauncherClass = g_game->getGlobalNumber("PROF_FREELANCE_MISSILE_MAX");
	maxLaserClass           = g_game->getGlobalNumber("PROF_FREELANCE_LASER_MAX");
	ship.setMaxEngineClass(maxEngineClass);
	ship.setMaxShieldClass(maxShieldClass);
	ship.setMaxArmorClass(maxArmorClass);
	ship.setMaxMissileLauncherClass(maxMissileLauncherClass);
	ship.setMaxLaserClass(maxLaserClass);

	//Roll random repair minerals and set the repair counters
	ship.initializeRepair();

	g_game->gameState->setShip(ship);
}



void ModuleCaptainCreation::chooseMilitary()
{
	//set military attributes
	m_profession = PROFESSION_MILITARY;
	m_attributes.durability = BASEATT_MILITARY_DURABILITY;
	m_attributes.learnRate = BASEATT_MILITARY_LEARNRATE;
	m_attributes.science = BASEATT_MILITARY_SCIENCE;
	m_attributes.navigation = BASEATT_MILITARY_NAVIGATION;
	m_attributes.tactics = BASEATT_MILITARY_TACTICS;
	m_attributes.engineering = BASEATT_MILITARY_ENGINEERING;
	m_attributes.communication = BASEATT_MILITARY_COMMUNICATION;
	m_attributes.medical = BASEATT_MILITARY_MEDICAL;
	m_attributesInitial = m_attributes;

	// maximum attribute values
	m_attributesMax.durability = MAXATT_MILITARY_DURABILITY;
	m_attributesMax.learnRate = MAXATT_MILITARY_LEARNRATE;
	m_attributesMax.science = MAXATT_MILITARY_SCIENCE;
	m_attributesMax.navigation = MAXATT_MILITARY_NAVIGATION;
	m_attributesMax.tactics = MAXATT_MILITARY_TACTICS;
	m_attributesMax.engineering = MAXATT_MILITARY_ENGINEERING;
	m_attributesMax.communication = MAXATT_MILITARY_COMMUNICATION;
	m_attributesMax.medical = MAXATT_MILITARY_MEDICAL;

	m_availPts = INITIAL_AVAIL_PTS;
	m_availProfPts = INITIAL_AVAIL_PROF_PTS;

	//store attributes in gamestate	
	g_game->gameState->setProfession(m_profession);
	g_game->gameState->officerCap->attributes = m_attributes;

	//set ship name and properties based on profession
	Ship ship = g_game->gameState->getShip();
	ship.setName("Devastator");
	int value = g_game->getGlobalNumber("PROF_MILITARY_ARMOR");
	ship.setArmorClass(value);
	ship.setArmorIntegrity(100.0);
	value = g_game->getGlobalNumber("PROF_MILITARY_ENGINE");
	ship.setEngineClass(value);
	ship.setEngineIntegrity(100.0);
	value = g_game->getGlobalNumber("PROF_MILITARY_SHIELD");
	ship.setShieldClass(value);
	ship.setShieldIntegrity(100.0);
	value = g_game->getGlobalNumber("PROF_MILITARY_LASER");
	ship.setLaserClass(value);
	ship.setLaserIntegrity(100.0);
	value = g_game->getGlobalNumber("PROF_MILITARY_MISSILE");
	ship.setMissileLauncherClass(value);
	ship.setMissileLauncherIntegrity(100.0);
	value = g_game->getGlobalNumber("PROF_MILITARY_PODS");
	ship.setCargoPodCount(value);

	int maxEngineClass=0, maxShieldClass=0, maxArmorClass=0, maxMissileLauncherClass=0, maxLaserClass=0;
	maxEngineClass          = g_game->getGlobalNumber("PROF_MILITARY_ENGINE_MAX");
	maxShieldClass          = g_game->getGlobalNumber("PROF_MILITARY_SHIELD_MAX");
	maxArmorClass           = g_game->getGlobalNumber("PROF_MILITARY_ARMOR_MAX");
	maxMissileLauncherClass = g_game->getGlobalNumber("PROF_MILITARY_MISSILE_MAX");
	maxLaserClass           = g_game->getGlobalNumber("PROF_MILITARY_LASER_MAX");
	ship.setMaxEngineClass(maxEngineClass);
	ship.setMaxShieldClass(maxShieldClass);
	ship.setMaxArmorClass(maxArmorClass);
	ship.setMaxMissileLauncherClass(maxMissileLauncherClass);
	ship.setMaxLaserClass(maxLaserClass);

	//Roll random repair minerals and set the repair counters
	ship.initializeRepair();

	g_game->gameState->setShip(ship);
}

void ModuleCaptainCreation::chooseScience()
{
	//set science attributes
	m_profession = PROFESSION_SCIENTIFIC;
	m_attributes.durability = BASEATT_SCIENTIFIC_DURABILITY;
	m_attributes.learnRate = BASEATT_SCIENTIFIC_LEARNRATE;
	m_attributes.science = BASEATT_SCIENTIFIC_SCIENCE;
	m_attributes.navigation = BASEATT_SCIENTIFIC_NAVIGATION;
	m_attributes.tactics = BASEATT_SCIENTIFIC_TACTICS;
	m_attributes.engineering = BASEATT_SCIENTIFIC_ENGINEERING;
	m_attributes.communication = BASEATT_SCIENTIFIC_COMMUNICATION;
	m_attributes.medical = BASEATT_SCIENTIFIC_MEDICAL;
	m_attributesInitial = m_attributes;

	// maximum attribute values
	m_attributesMax.durability = MAXATT_SCIENTIFIC_DURABILITY;
	m_attributesMax.learnRate = MAXATT_SCIENTIFIC_LEARNRATE;
	m_attributesMax.science = MAXATT_SCIENTIFIC_SCIENCE;
	m_attributesMax.navigation = MAXATT_SCIENTIFIC_NAVIGATION;
	m_attributesMax.tactics = MAXATT_SCIENTIFIC_TACTICS;
	m_attributesMax.engineering = MAXATT_SCIENTIFIC_ENGINEERING;
	m_attributesMax.communication = MAXATT_SCIENTIFIC_COMMUNICATION;
	m_attributesMax.medical = MAXATT_SCIENTIFIC_MEDICAL;

	m_availPts = INITIAL_AVAIL_PTS;
	m_availProfPts = INITIAL_AVAIL_PROF_PTS;

	//store attributes in gamestate	
	g_game->gameState->setProfession(m_profession);
	g_game->gameState->officerCap->attributes = m_attributes;

	//set ship name and properties based on profession
	Ship ship = g_game->gameState->getShip();
	ship.setName("Expedition");
	int value = g_game->getGlobalNumber("PROF_SCIENCE_ARMOR");
	ship.setArmorClass(value);
	ship.setArmorIntegrity(100.0);
	value = g_game->getGlobalNumber("PROF_SCIENCE_ENGINE");
	ship.setEngineClass(value);
	ship.setEngineIntegrity(100.0);
	value = g_game->getGlobalNumber("PROF_SCIENCE_SHIELD");
	ship.setShieldClass(value);
	ship.setShieldIntegrity(100.0);
	value = g_game->getGlobalNumber("PROF_SCIENCE_LASER");
	ship.setLaserClass(value);
	ship.setLaserIntegrity(100.0);
	value = g_game->getGlobalNumber("PROF_SCIENCE_MISSILE");
	ship.setMissileLauncherClass(value);
	ship.setMissileLauncherIntegrity(100.0);
	value = g_game->getGlobalNumber("PROF_SCIENCE_PODS");
	ship.setCargoPodCount(value);

	int maxEngineClass=0, maxShieldClass=0, maxArmorClass=0, maxMissileLauncherClass=0, maxLaserClass=0;
	maxEngineClass          = g_game->getGlobalNumber("PROF_SCIENCE_ENGINE_MAX");
	maxShieldClass          = g_game->getGlobalNumber("PROF_SCIENCE_SHIELD_MAX");
	maxArmorClass           = g_game->getGlobalNumber("PROF_SCIENCE_ARMOR_MAX");
	maxMissileLauncherClass = g_game->getGlobalNumber("PROF_SCIENCE_MISSILE_MAX");
	maxLaserClass           = g_game->getGlobalNumber("PROF_SCIENCE_LASER_MAX");
	ship.setMaxEngineClass(maxEngineClass);
	ship.setMaxShieldClass(maxShieldClass);
	ship.setMaxArmorClass(maxArmorClass);
	ship.setMaxMissileLauncherClass(maxMissileLauncherClass);
	ship.setMaxLaserClass(maxLaserClass);

	//Roll random repair minerals and set the repair counters
	ship.initializeRepair();

	g_game->gameState->setShip(ship);
}



void ModuleCaptainCreation::OnEvent(Event *event)
{
	bool playBtnSnd = false;
	bool creationComplete = false;
	bool playErrSnd = false;

	if( m_availPts < INITIAL_AVAIL_PTS ) {
		if(event->getEventType() == EVENT_MINUS){
			//durability
			if ( m_attributes.durability > m_attributesInitial.durability ){
				playBtnSnd = true;
				m_attributes.durability--;
				m_availPts++;
			}else{
				playErrSnd = true;
			}
		}else if(event->getEventType() == EVENT_MINUS + 1){
			//learn rate
			if ( m_attributes.learnRate > m_attributesInitial.learnRate ){
				playBtnSnd = true;
				m_attributes.learnRate--;
				m_availPts++;
			}else{
				playErrSnd = true;
			}
		}
	}else{
		if(event->getEventType() == EVENT_MINUS || event->getEventType() == EVENT_MINUS + 1){
			playErrSnd = true;
		}
	}
	if( m_availProfPts < INITIAL_AVAIL_PROF_PTS ){
		if(event->getEventType() == EVENT_MINUS + 2){
			//science
			if ( m_attributes.science > m_attributesInitial.science ){
				playBtnSnd = true;
				m_attributes.science--;
				m_availProfPts++;
			}else{
				playErrSnd = true;
			}
		}else if(event->getEventType() == EVENT_MINUS + 3){
			//navigation
			if ( m_attributes.navigation > m_attributesInitial.navigation ){
				playBtnSnd = true;
				m_attributes.navigation--;
				m_availProfPts++;
			}else{
				playErrSnd = true;
			}
		}else if(event->getEventType() == EVENT_MINUS + 4){
			//tactics
			if ( m_attributes.tactics > m_attributesInitial.tactics ){
				playBtnSnd = true;
				m_attributes.tactics--;
				m_availProfPts++;
			}else{
				playErrSnd = true;
			}
		}else if(event->getEventType() == EVENT_MINUS + 5){
			//engineering 
			if ( m_attributes.engineering > m_attributesInitial.engineering ){
				playBtnSnd = true;
				m_attributes.engineering--;
				m_availProfPts++;
			}else{
				playErrSnd = true;
			}
		}else if(event->getEventType() == EVENT_MINUS + 6){
			//communication
			if ( m_attributes.communication > m_attributesInitial.communication ){
				playBtnSnd = true;
				m_attributes.communication--;
				m_availProfPts++;
			}else{
				playErrSnd = true;
			}
		}else if(event->getEventType() == EVENT_MINUS + 7){
			//medical
			if ( m_attributes.medical > m_attributesInitial.medical ){
				playBtnSnd = true;
				m_attributes.medical--;
				m_availProfPts++;
			}else{
				playErrSnd = true;
			}
		}
	}else{
		for(int i=2; i<8; i++){
			if(event->getEventType() == EVENT_MINUS+i){
				playErrSnd = true;
			}
		}
	}

	if (playErrSnd){
		g_game->audioSystem->Play(m_sndErr);
	}

	if (event->getEventType() == EVENT_FINISH)
	{
		playBtnSnd = true;

		/*GameState gs;
		gs.Reset();
		gs.m_profession = m_profession;
		gs.officerCap->name = m_name;
		gs.officerCap->attributes = m_attributes;
		gs.m_captainSelected = true;
		gs.SaveGame("newcaptain.dat");*/

		//this ends up calling g_game->gameState->m_ship.Reset()
		//so most of the changes we did on the ship are thrown out
		g_game->gameState->Reset();
		g_game->gameState->m_profession = m_profession;
		g_game->gameState->officerCap->name = m_name;
		g_game->gameState->officerCap->attributes = m_attributes;
		g_game->gameState->m_captainSelected = true;

        //give new player 20 Endurium (and override any prior amount added)
        const int ITEM_ENDURIUM = 54;
        Item endurium;
        int amount=0;
        g_game->gameState->m_items.Get_Item_By_ID(ITEM_ENDURIUM, endurium, amount);
        if (amount > 20) amount = 0;
        if (amount > 0)
            amount = 20 - amount;
        else
            amount = 20;
        g_game->gameState->m_items.AddItems(ITEM_ENDURIUM, amount);
		g_game->gameState->SaveGame("newcaptain.dat");

		creationComplete = true;
	}

	if (playBtnSnd)
	{
		g_game->audioSystem->Play(m_sndBtnClick);
	}

	if (creationComplete)
	{
		g_game->gameState->m_captainSelected = true;
		g_game->modeMgr->LoadModule(MODULE_CAPTAINSLOUNGE);
		return;
	}
}
