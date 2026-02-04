#pragma once
#include <string>
#include "env.h"

class Button;
class Label;
//struct BITMAP;

class MessageBoxWindow
{
public:
	//ctors
	MessageBoxWindow(
        std::string heading,
		std::string initText,
		int initX, 
		int initY, 
		int initWidth, 
		int initHeight, 
		ALLEGRO_COLOR initTextColor, 
		bool initCentered);

	~MessageBoxWindow();

	//accessors
	std::string GetText() const;
	int GetX() const;
	int GetY() const;
	int GetWidth() const;
	int GetHeight() const;
	ALLEGRO_COLOR GetTextColor() const;
	bool IsCentered() const;
	bool IsVisible() const;

	//mutators
	void SetText(std::string initText);
	void SetX(int initX);
	void SetY(int initY);
	void SetWidth(int initWidth);
	void SetHeight(int initHeight);
	void SetTextColor(ALLEGRO_COLOR initTextColor);
	void SetCentered(bool initCentered);
	void SetVisible(bool visibility);


	//other funcs
	bool OnMouseMove(int x, int y);
	bool OnMouseReleased(int button, int x, int y);
	bool OnMouseClick(int button, int x, int y);
	bool OnMousePressed(int button, int x, int y);
	bool OnKeyPress(int keyCode);

	void Update();
	void Draw();

	static Button	*button1;
	static Button	*button2;
	static BITMAP	*bg;
	static BITMAP	*bar;

private:

    std::string     heading;
	std::string		text;
	int				x;
	int 			y;
	int 			width;
	int 			height;
	ALLEGRO_COLOR	textColor;
	bool			centered;
	bool			visible;
	Label			*labelText;
    Label           *labelHeading;

};

