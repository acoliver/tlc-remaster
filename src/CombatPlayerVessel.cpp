
#include "CombatPlayerVessel.h"

CombatPlayerVessel::CombatPlayerVessel(lua_State *LuaVM, std::string ScriptName) : CombatObject(),
	forwardThrust(0),
	reverseThrust(0),
	turnRight(0),
	turnLeft(0)
{
	(void)LuaVM;
	(void)ScriptName;
}


CombatPlayerVessel::~CombatPlayerVessel() {}


void CombatPlayerVessel::Move()
{
	CombatObject::Update();
}


void CombatPlayerVessel::Draw(BITMAP *Canvas)
{
	CombatObject::Draw(Canvas);

}
