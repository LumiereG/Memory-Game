#include <dwmapi.h>
#include <stdexcept>
#include "app.h"
#include "resource.h"

std::wstring const app::s_class_name{ L"MemoryGame" };

// Register the window class if not already registered
bool app::register_class() {
	WNDCLASSEXW desc{};
	if (GetClassInfoExW(m_instance, s_class_name.c_str(),&desc) != 0) 
		return true;

	desc = {
	.cbSize = sizeof(WNDCLASSEXW),
	.lpfnWndProc = window_proc_static,
	.hInstance = m_instance,
	.hIcon = LoadIconW(m_instance, MAKEINTRESOURCEW(ID_APPICON)),
	.hCursor = LoadCursorW(nullptr, L"IDC_ARROW"),
	.hbrBackground =
		CreateSolidBrush(RGB(255, 253, 208)),
		.lpszClassName = s_class_name.c_str()
	};

	return RegisterClassExW(&desc) != 0;
}

// Create the main application window
HWND app::create_window(DWORD style){
	RECT size{ 0, 0, m_board->width, m_board->height };
	AdjustWindowRectEx(&size, style, true, 0);
	
	HWND window = CreateWindowExW(
		0,
		s_class_name.c_str(),
		L"Memory Game",
		WS_OVERLAPPED | WS_SYSMENU,
		(GetSystemMetrics(SM_CXSCREEN) - m_board->width) / 2, (GetSystemMetrics(SM_CYSCREEN) - m_board->height) / 2,
		size.right - size.left, size.bottom - size.top,
		nullptr,
		nullptr,
		m_instance,
		this);
	
	// Create board cells (fields)
	for (int i = 0; i < n * n; i++) {
		auto& f = m_board->fields()[i];
		cells[i] = CreateWindowExW(
			0, L"STATIC", nullptr,
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			f.position.left, f.position.top,
			f.position.right - f.position.left,
			f.position.bottom - f.position.top,
			window, nullptr, m_instance, nullptr);

		SetClassLongPtrW(cells[i], GCLP_HBRBACKGROUND, (LONG_PTR)m_field_brush);
	}

	return window;
}

// Static window procedure wrapper
LRESULT app::window_proc_static(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	app* ap_p = nullptr;
	if (message == WM_NCCREATE)
	{
		auto p = reinterpret_cast<LPCREATESTRUCTW>(lparam);
		ap_p = static_cast<app*>(p->lpCreateParams);
		SetWindowLongPtrW(window, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(ap_p));
	}
	else
	{
		ap_p = reinterpret_cast<app*>(
			GetWindowLongPtrW(window, GWLP_USERDATA));
	}
	if (ap_p != nullptr)
	{
		return ap_p->window_proc(window, message,
			wparam, lparam);
	}
	return DefWindowProcW(window, message, wparam, lparam);
}

// Main window procedure
LRESULT app::window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam){
	switch (message) {
		case WM_CREATE:
			SetWindowText(window, L"Press ESC to Start");
			break;
		case WM_CLOSE:
			DestroyWindow(window);
			return 0;
		case WM_DESTROY:
			if (window == m_main)
				PostQuitMessage(EXIT_SUCCESS);
			return 0;
		case WM_CTLCOLORSTATIC:
			return reinterpret_cast<INT_PTR>(m_field_brush);
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE) {
				ResetGame(window);
			}
			break;
		case WM_LBUTTONUP:
			if (!isShowingSequence) {
				int x = LOWORD(lparam) / (m_board->field_size + m_board->margin);
				int y = HIWORD(lparam) / (m_board->field_size + m_board->margin);
				if (x < n && y < n) {
					CheckClick(window, x, y);
				}
			}
			break;
		case WM_TIMER:
			HandleTimer(window);
			break;
	}
	return DefWindowProcW(window, message, wparam, lparam);
}

// Constructor
app::app(HINSTANCE instance, int N)
	: m_instance{ instance }, m_main{}, m_field_brush{ CreateSolidBrush(RGB(124, 10, 2)) } {
	m_board = new board(N);
	cells = new HWND[N * N];
	n = N;
	register_class();
	
	DWORD main_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
	m_main = create_window(main_style);
}

// Run the application
int app::run(int show_command)
{
	ShowWindow(m_main, show_command);
	MSG msg{};
	BOOL result = TRUE;
	while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0)
	{
		if (result == -1)
			return EXIT_FAILURE;
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return EXIT_SUCCESS;
}

// Reset the game state
void app::ResetGame(HWND hwnd) {
	score = 0;
	sequence.clear();
	userInput.clear();
	SetWindowTextW(hwnd, L"Score: 0 Memorize!");
	StartGame(hwnd);
}

// Start a new game round
void app::StartGame(HWND hwnd) {
	for(int i = 0; i < n*n; i++) ShowWindow(cells[i], SW_HIDE);
	sequence.push_back(rand() % (n * n));
	currentStep = 0;
	isShowingSequence = true;
	SetTimer(hwnd, TIMER_ID, 1000, nullptr);
}

// Show the sequence to the player
void app::ShowSequence(HWND hwnd) {
	currentStep = 0;
	isShowingSequence = true;
	SetTimer(hwnd, TIMER_ID, 1000, nullptr);
}

// Handle player input
void app::CheckClick(HWND hwnd, int x, int y) {
	int idx = y * n + x;
	ShowWindow(cells[previousi], SW_HIDE);
	ShowWindow(cells[n * y + x], SW_SHOW);
	userInput.push_back(idx);

	if (userInput[userInput.size() - 1] != sequence[userInput.size() - 1]) {
		KillTimer(hwnd, TIMER_ID);
		SetWindowTextW(hwnd, L"Memory Game - Wrong! ESC to restart!");
		return;
	}
	previousi = idx;
}

// Handle timer events (sequence display and user input handling)
void app::HandleTimer(HWND hwnd) {
	if (isShowingSequence) {
		if (currentStep < sequence.size()) {
			ShowWindow(cells[previousi], SW_HIDE);
			int idx = sequence[currentStep];
			ShowWindow(cells[idx], SW_SHOW);
			previousi = idx;
			currentStep++;
		}
		else {
			ShowWindow(cells[previousi], SW_HIDE);
			isShowingSequence = false;
			SetWindowTextW(hwnd, (L"Score: " + std::to_wstring(score) + L" Guess!").c_str());
		}
	}
	else {
		ShowWindow(cells[previousi], SW_HIDE);
		if (userInput.size() == sequence.size()) {
			score++;
			SetWindowTextW(hwnd, (L"Score: " + std::to_wstring(score) + L" Memorize!").c_str());
			userInput.clear();
			sequence.push_back(rand() % (n * n));
			ShowSequence(hwnd);
		}
	}
}