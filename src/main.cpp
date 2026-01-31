/*
	STARFLIGHT - THE LOST COLONY
	main.cpp - main function that launches the game class
	Author: Coder
	Date:
*/

#include "env.h"
#include <allegro.h>
#include "Game.h"
#include "LogFile.h"

#include <cstring>
#include <cstdio>


//global engine object
Game *g_game;

int main(int argc, char **argv)
{
	// Quick checks before Allegro init
	if (argc > 1)
	{
		if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0)
		{
			printf("Starflight: The Lost Colony\n");
			printf("Version: 1.0.0-dev (macOS port)\n");
			printf("Built with Allegro Legacy\n");

			return 0;
		}
		if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
		{
			printf("Starflight: The Lost Colony\n");
			printf("Usage: starflighttlc [options]\n");
			printf("Options:\n");
			printf("  --version, -v    Show version info\n");
			printf("  --help, -h       Show this help\n");


			return 0;
		}
	}

	g_game = new Game();
	g_game->Run();
	g_game = NULL;

   return 0;
}
END_OF_MAIN()

