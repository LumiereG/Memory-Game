#include <windows.h>
#include "app.h"

/**
 * Entry point for the Memory Game application.
 * Initializes and starts the game.
 *
 * @param instance - Handle to the application instance.
 * @param hInstPrev - Unused parameter (legacy Windows compatibility).
 * @param lpCmdLine - Command line arguments (expected to contain board size N).
 * @param show_command - Specifies how the window should be displayed.
 * @return Exit status code.
 */

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE hInstPrev, PSTR  lpCmdLine, int show_command)
{
	int N = 3; // Default board size

	// Parse command-line argument to set board size, ensuring it doesn't exceed 10
	if (lpCmdLine && *lpCmdLine) {
		N = atoi(lpCmdLine) > 10 ? N : atoi(lpCmdLine);
	}
	app app{ instance, N };

	return app.run(show_command);

}