/*
	STARFLIGHT - THE LOST COLONY
	main.cpp - main function that launches the game class
	Author: Coder
	Date:
*/

// env.h must be first - it includes allegro.h which sets up the magic main on Windows
#include "env.h"
// Do NOT include allegro.h again - env.h already did it
#include "Game.h"
#include "LogFile.h"

#include <cstring>
#include <cstdio>

#if defined(TLC_PLATFORM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#include <windows.h>
#include <direct.h>
#endif


//global engine object
Game *g_game;

#if defined(TLC_PLATFORM_WINDOWS)
static void SetWorkingDirectoryToExe()
{
	char exePath[MAX_PATH];
	DWORD length = GetModuleFileNameA(NULL, exePath, MAX_PATH);
	if (length == 0 || length == MAX_PATH)
	{
		return;
	}
	char *lastSlash = strrchr(exePath, '\\');
	if (lastSlash != NULL)
	{
		*lastSlash = '\0';
		_chdir(exePath);
	}
}
#endif


int main(int argc, char **argv)
{
#if defined(TLC_PLATFORM_WINDOWS)
	SetWorkingDirectoryToExe();
#endif
	debug.Print("main: start");
	// Quick checks before Allegro init
	if (argc > 1)
	{
		if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0)
		{
			printf("Starflight: The Lost Colony\n");
			printf("Version: 1.0.0-dev (macOS port)\n");
			printf("Built with Allegro Legacy\n");
			debug.Print("main: --version");

			return 0;
		}
		if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
		{
			printf("Starflight: The Lost Colony\n");
			printf("Usage: starflighttlc [options]\n");
			printf("Options:\n");
			printf("  --version, -v    Show version info\n");
			printf("  --help, -h       Show this help\n");
			debug.Print("main: --help");

			return 0;
		}
	}

	debug.Print("main: creating Game");
	g_game = new Game();
	debug.Print("main: entering Run");
	g_game->Run();
	debug.Print("main: Run returned");
	g_game = NULL;

	debug.Print("main: exit");
   return 0;
}
END_OF_MAIN()

