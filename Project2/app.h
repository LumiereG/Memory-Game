#pragma once
#include <windows.h>
#include <string>
#include "board.h"
#include <chrono>
#include <vector>

/**
 * Main application class for the Memory Game.
 * Handles window creation, event processing, and game logic.
 */

class app {
private:
    // Registers the window class if not already registered
    bool register_class();
    static std::wstring const s_class_name;

    // Static window procedure (used to forward messages to the instance method)
    static LRESULT CALLBACK window_proc_static(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

    // Instance window procedure
    LRESULT window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

    // Creates the main application window
    HWND create_window(DWORD style);

    // Array of cell windows (game fields)
    HWND* cells;

    // Board size (n x n)
    LONG n;

    // Handle to the main application window
    HWND m_main;

    // Handle to the application instance
    HINSTANCE m_instance;

    // Pointer to the game board object
    board* m_board;

    // Brush for coloring game fields
    HBRUSH m_field_brush;

    // Stores the sequence to be memorized
    std::vector<int> sequence;

    // Stores the user's input sequence
    std::vector<int> userInput;

    // Flag indicating whether the sequence is currently being shown
    BOOL isShowingSequence = false;

    // Timer identifier
    int TIMER_ID = 0;

    // Current step in the sequence
    int currentStep = 0;

    // Index of the previously displayed field
    int previousi = 0;

    // Player's score
    int score = 0;

    // Handles timer events (sequence display and user input handling)
    void HandleTimer(HWND hwnd);

public:
    /**
     * Constructor: Initializes the application and creates the main window.
     * @param instance - Handle to the application instance.
     * @param N - Board size (NxN grid).
     */
    app(HINSTANCE instance, int N);

    /**
     * Runs the main event loop of the application.
     * @param show_command - Specifies how the window is to be shown.
     * @return Exit status code.
     */
    int run(int show_command);

    /**
     * Resets the game state and starts a new round.
     * @param hwnd - Handle to the main application window.
     */
    void ResetGame(HWND hwnd);

    /**
     * Starts the game by displaying the first sequence.
     * @param hwnd - Handle to the main application window.
     */
    void StartGame(HWND hwnd);

    /**
     * Displays the sequence for the player to memorize.
     * @param hwnd - Handle to the main application window.
     */
    void ShowSequence(HWND hwnd);

    /**
     * Handles player's click input and checks if it matches the sequence.
     * @param hwnd - Handle to the main application window.
     * @param x - X-coordinate of the clicked field.
     * @param y - Y-coordinate of the clicked field.
     */
    void CheckClick(HWND hwnd, int x, int y);
};
