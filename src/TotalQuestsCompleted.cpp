
#include "env.h"
#include "TotalQuestsCompleted.h"
#include "Game.h"
#include "DataMgr.h"

using namespace std;

TotalQuestsCompleted::TotalQuestsCompleted(): Requirement(),
	totalQuests(0)
{
	
};	
	
TotalQuestsCompleted::TotalQuestsCompleted(TiXmlElement *rootElement): Requirement(),
	totalQuests(0)
{
	TiXmlHandle reqHandle(rootElement);

	TiXmlText * text;

	text = reqHandle.FirstChild("TotalQuests").FirstChild().Text();
	if (text != NULL)
	{
		this->totalQuests = atoi(text->Value());
	}
};


TotalQuestsCompleted::~TotalQuestsCompleted()
{
	
}


void TotalQuestsCompleted::RegisterSelf()
{
	//This requirement doesn't have an event
}

void TotalQuestsCompleted::UnregisterSelf()
{
	//This requirement doesn't have an event
}

bool TotalQuestsCompleted::Check()
{
	// The original code tracked per-quest completion history. The current GameState only tracks
	// completion for the active quest, so keep this requirement buildable with a conservative stub.
	int count = 0;


	if (count >= totalQuests)
		completed = true;
	else
		completed = false;

	return completed;
}

std::string TotalQuestsCompleted::ToString()
{
	std::string str = "You must have already completed ";
	str += std::to_string(totalQuests);
	str += " missions";

	return str;
}

