#include "env.h"
#include "env.h"
#include "MiniWindow.h"
#include "Game.h"


MiniWindow::MiniWindow() 
{
	SetPos(0,0);
	SetDimensions(200, 100);
	mwCorner = NULL;
	mwSide = NULL;
	mwInterior = NULL;
}

MiniWindow::MiniWindow(int width, int height) 
{
	SetPos(0,0);
	SetDimensions(width, height);
	mwCorner = NULL;
	mwSide = NULL;
	mwInterior = NULL;
}

MiniWindow::MiniWindow(int x, int y, int width, int height) 
{
	SetPos(x,y);
	SetDimensions(width, height);
	mwCorner = NULL;
	mwSide = NULL;
	mwInterior = NULL;
}

MiniWindow::MiniWindow(int x, int y, int width, int height, std::string cornerFilename, std::string sideFilename, std::string interiorFilename) 
{
	SetPos(x,y);
	SetDimensions(width, height);
	LoadCornerImage(cornerFilename.c_str());
	LoadSideImage(sideFilename.c_str());
	LoadInteriorImage(interiorFilename.c_str());
}

void MiniWindow::LoadCornerImage(std::string filename) 
{ 
	mwCorner = load_bitmap(filename.c_str(), NULL); 
}

void MiniWindow::LoadSideImage(std::string filename) 
{ 
	mwSide = load_bitmap(filename.c_str(), NULL); 
}

void MiniWindow::LoadInteriorImage(std::string filename)
{
	mwInterior = load_bitmap(filename.c_str(), NULL);
}

void MiniWindow::SetPos(int x, int y) 
{ 
	mwX = x; 
	mwY = y; 
}

void MiniWindow::SetDimensions(int width, int height) 
{ 
	mwWidth = width; 
	mwHeight = height; 
}


//Draws the box at the stored x/y position
void MiniWindow::Draw(BITMAP *destination)
{
	Draw(destination, mwX, mwY);
}

//Draws the box at the specified x/y position
void MiniWindow::Draw(BITMAP *destination, int x, int y)
{
	int PINK = (255 << 16) | (0 << 8) | 255;
	if (mwCorner == NULL || mwSide == NULL || mwInterior == NULL)
	{
		textout_ex(destination, g_game->font12, "Bitmaps not established", x, y, PINK, 01);
		return;
	}

	//create scratch pad
	BITMAP *buffer = create_bitmap(mwWidth, mwHeight);
	clear_to_color(buffer, PINK);
	
	//draw top/bottom sides
	for (int a = al_get_bitmap_width(mwCorner); a < mwWidth; a += al_get_bitmap_width(mwSide))
	{
		al_set_target_bitmap(buffer); al_draw_bitmap(mwSide, a, 0, 0);
		al_set_target_bitmap(buffer); al_draw_bitmap(mwSide, a, mwHeight - al_get_bitmap_height(mwSide), ALLEGRO_FLIP_VERTICAL);
	}

	//draw left/right sides
	for (int a = al_get_bitmap_height(mwCorner); a < mwHeight; a += al_get_bitmap_height(mwSide))
	{
		al_set_target_bitmap(buffer); 
		{ 
			float _cx = al_get_bitmap_width(mwSide) / 2.0f; 
			float _cy = al_get_bitmap_height(mwSide) / 2.0f; 
			float _angle_rad = ((float)fixtof(itofix(-64))) * ALLEGRO_PI * 2.0f / 256.0f; 
			al_draw_rotated_bitmap(mwSide, _cx, _cy, 0 + _cx, a + _cy, _angle_rad, 0); 
		}
		{ 
			float _cx = al_get_bitmap_width(mwSide) / 2.0f; 
			float _cy = al_get_bitmap_height(mwSide) / 2.0f; 
			float _angle_rad = ((float)fixtof(itofix(64))) * ALLEGRO_PI * 2.0f / 256.0f; 
			al_draw_rotated_bitmap(mwSide, _cx, _cy, mwWidth - al_get_bitmap_width(mwSide) + _cx, a + _cy, _angle_rad, 0); 
		}
	}
	
	//upper left corner
	al_set_target_bitmap(buffer);
	al_draw_filled_rectangle(0, 0, al_get_bitmap_width(mwCorner)-1+1, al_get_bitmap_height(mwCorner)-1+1, int_to_al_color(PINK));
	al_set_target_bitmap(buffer); al_draw_bitmap(mwCorner, 0, 0, 0);
	
	//upper right corner
	al_set_target_bitmap(buffer);
	al_draw_filled_rectangle(mwWidth - al_get_bitmap_width(mwCorner), 0, mwWidth-1+1, al_get_bitmap_height(mwCorner)-1+1, int_to_al_color(PINK));
	al_set_target_bitmap(buffer); al_draw_bitmap(mwCorner, mwWidth - al_get_bitmap_width(mwCorner), 0, ALLEGRO_FLIP_HORIZONTAL);
	//al_set_target_bitmap(buffer); { float _cx = al_get_bitmap_width(mwCorner) / 2.0f; float _cy = al_get_bitmap_height(mwCorner) / 2.0f; float _angle_rad = ((float)fixtof(itofix(64)) * ALLEGRO_PI * 2.0f / 256.0f; al_draw_rotated_bitmap(mwCorner, _cx, _cy, mwWidth - mwCorner->h + _cx, 0 + _cy, _angle_rad, 0); });
	
	//lower left corner
	al_set_target_bitmap(buffer);
	al_draw_filled_rectangle(0, mwHeight - al_get_bitmap_width(mwCorner), al_get_bitmap_height(mwCorner)-1+1, mwHeight-1+1, int_to_al_color(PINK));
	//al_set_target_bitmap(buffer); { float _cx = al_get_bitmap_width(mwCorner) / 2.0f; float _cy = al_get_bitmap_height(mwCorner) / 2.0f; float _angle_rad = ((float)fixtof(itofix(192)) * ALLEGRO_PI * 2.0f / 256.0f; al_draw_rotated_bitmap(mwCorner, _cx, _cy, 0 + _cx, mwHeight - mwCorner->h + _cy, _angle_rad, 0); });
	al_set_target_bitmap(buffer); al_draw_bitmap(mwCorner, 0, mwHeight - al_get_bitmap_height(mwCorner), ALLEGRO_FLIP_VERTICAL);
	
	//lower right corner
	al_set_target_bitmap(buffer);
	al_draw_filled_rectangle(mwWidth - al_get_bitmap_width(mwCorner), 0 + mwHeight - al_get_bitmap_height(mwCorner), mwWidth-1+1, mwHeight-1+1, int_to_al_color(PINK));
	//al_set_target_bitmap(buffer); { float _cx = al_get_bitmap_width(mwCorner) / 2.0f; float _cy = al_get_bitmap_height(mwCorner) / 2.0f; float _angle_rad = ((float)fixtof(itofix(128)) * ALLEGRO_PI * 2.0f / 256.0f; al_draw_rotated_bitmap(mwCorner, _cx, _cy, mwWidth - mwCorner->w + _cx, mwHeight - mwCorner->h + _cy, _angle_rad, 0); });
	al_set_target_bitmap(buffer); al_draw_bitmap(mwCorner, mwWidth - al_get_bitmap_width(mwCorner), mwHeight - al_get_bitmap_height(mwCorner), ALLEGRO_FLIP_HORIZONTAL | ALLEGRO_FLIP_VERTICAL);

	//draw interior tiles
	for (int a = al_get_bitmap_width(mwSide); a < mwWidth - al_get_bitmap_width(mwSide); a += al_get_bitmap_width(mwInterior))
	{
		for (int b = al_get_bitmap_height(mwSide); b < mwHeight - al_get_bitmap_height(mwSide); b += al_get_bitmap_height(mwInterior))
		{
			al_set_target_bitmap(buffer); al_draw_bitmap(mwInterior, a, b, 0);
		}
	}



	//draw scratchpad to destination
	al_set_target_bitmap(destination); al_draw_bitmap(buffer, x, y, 0);
	
	//delete scratchpad
	destroy_bitmap(buffer);
}
