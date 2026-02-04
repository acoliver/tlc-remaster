/*
	STARFLIGHT - THE LOST COLONY
	ModuleCrewHire.h - ?
	Author: ?
	Date: 9/21/07
*/

#ifndef MODULECREWHIRE_H
#define MODULECREWHIRE_H

#include "env.h"
#include "env.h"
#include "tinyxml/tinyxml.h"
#include "Module.h"

#include "GameState.h"
#include "Button.h"
#include "ScrollBox.h"
#include "EventMgr.h"


class Label;


class ModuleCrewHire : public Module
{
public:
	ModuleCrewHire();
	virtual ~ModuleCrewHire();
	virtual bool Init();
	void Update();
	virtual void Draw();
	virtual void OnKeyPress(int keyCode);
	virtual void OnKeyPressed(int keyCode);
	virtual void OnKeyReleased(int keyCode);
	virtual void OnMouseMove(int x, int y);
	virtual void OnMouseClick(int button, int x, int y);
	virtual void OnMousePressed(int button, int x, int y);
	virtual void OnMouseReleased(int button, int x, int y);
	void OnMouseWheelUp(int x, int y);
	void OnMouseWheelDown(int x, int y);
	virtual void OnEvent(Event *event);
	virtual void Close();

	Officer * FindOfficerType(OfficerType type);

private:
	void RefreshUnassignedCrewBox();
	void RefreshUnemployeedCrewBox();
	void DrawOfficerInfo(Officer *officer);

	int						currentScreen;
	int						selectedPosition;
	int						selectedPositionLastRun;
	int						selectedEntryLastRun;
	int						FALSEHover;
	int						lastEmployeeSpawn;
	int						currentVisit;

	Label					*title;
	Label					*slogan;
	Label					*directions;
	Label					*hiremoreDirections;
	Label					*stats;

	BITMAP 					*m_background;
	BITMAP 					*m_miniSkills;
	
	BITMAP					*exit_btn_norm;
	BITMAP					*exit_btn_over;
	BITMAP					*btn_norm;
	BITMAP					*btn_over;
	BITMAP					*btn_dis;
	BITMAP					*btn2_norm;
	BITMAP					*btn2_over;
	BITMAP					*btn2_dis;
	BITMAP					*catbtn_norm;
	BITMAP					*catbtn_over;
	BITMAP					*catbtn_dis;
	BITMAP					*icons_small;
	BITMAP					*icons_small_green;
	BITMAP					*icons_small_red;

	Button 					*m_exitBtn;
	Button					*m_hireBtn;
	Button 					*m_hiremoreBtn;
	Button 					*m_fireBtn;
	Button 					*m_unassignBtn;
	Button					*m_backBtn;

	Button					*m_PositionBtns[8];
	BITMAP					*posNormImages[8];
	BITMAP					*posOverImages[8];
	BITMAP					*posDisImages[8];

	std::vector<Officer*>	tOfficers;
	
	Officer					*selectedOfficer;

	ScrollBox::ScrollBox	*unassignedCrew;
	ScrollBox::ScrollBox	*unemployeed;
	ScrollBox::ScrollBox	*unemployeedType;


	ScrollBox::ColoredString coloredString;

};

#endif
