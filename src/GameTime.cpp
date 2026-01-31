
#include "env.h"
#include "GameTime.h"
#include "Util.h"


using namespace std;

GameTime::GameTime(): Requirement(),
	gametime(0)
{
	
};	
	
GameTime::GameTime(TiXmlElement *rootElement): Requirement(),
	gametime(0)
{
	TiXmlHandle reqHandle(rootElement);

	TiXmlText * text;

	text = reqHandle.FirstChild("GameTime").FirstChild().Text();
	if (text != NULL)
	{
		this->gametime = atoi(text->Value());
	}
};


GameTime::~GameTime()
{

}

void GameTime::RegisterSelf()
{

}

void GameTime::UnregisterSelf()
{

}

bool GameTime::Check()
{
	return true;
}

std::string GameTime::ToString()
{
	std::string str = "Total game time is atleast ";
	str += Util::ToString(gametime);
	str += " minutes";


	return str;
}
