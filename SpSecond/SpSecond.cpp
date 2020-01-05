
#include "framework.h"
#include "SpSecond.h"

// NOTE: Constants
#define FILE_MENU_EXIT 1
#define MAIN_WINDOW_CLASS L"MainWindow"
#define MAIN_WINDOW_TITLE L"SpSecond"

// NOTE: Prototypes
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM RegisterWindowClass(HINSTANCE hInstance);
BOOL InitializeWindow(HINSTANCE, int);
void SetMenu(HWND);
void SetControls(HWND);
void AppendWindowText(HWND, LPCTSTR);


// NOTE: Handlers
HINSTANCE hInstance;
HWND hMainWnd;
HMENU hMenu;
HWND hInputWnd;
HWND hResultWnd;
WNDPROC oldInputProc;

// NOTE: Main
int APIENTRY wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	MSG msg;
	hInstance = hInst;

	// NOTE: 1. Set window params
	RegisterWindowClass(hInst);

	// NOTE: 2. Trying to create window
	if (!InitializeWindow(hInstance, nCmdShow))
	{
		return FALSE;
	}

	// NOTE: 3. Handle msg
	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

// NOTE: Register window class
ATOM RegisterWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEX wc;														// window instance
	wc.cbSize = sizeof(wc);												// size
	wc.style = CS_HREDRAW | CS_VREDRAW;									// style
	wc.lpfnWndProc = WndProc;											// handler
	wc.lpszMenuName = NULL;												// menu name
	wc.lpszClassName = MAIN_WINDOW_CLASS;								// class name
	wc.cbWndExtra = NULL;
	wc.cbClsExtra = NULL;
	wc.hIcon = NULL;													// icon descriptior
	wc.hIconSm = NULL;													// tray icon descriptor
	wc.hCursor = NULL;													// cursor descriptor
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);				// background brush descriptor
	wc.hInstance = hInstance;											// ref to app

	return RegisterClassEx(&wc);
}

BOOL InitializeWindow(HINSTANCE hInst, int nCmdShow)
{
	hMainWnd = CreateWindow(
		MAIN_WINDOW_CLASS,				  // class name
		MAIN_WINDOW_TITLE,				  // title
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME | WS_VSCROLL | ES_MULTILINE | ES_READONLY | WS_CLIPSIBLINGS, // display mode
		CW_USEDEFAULT,					  // x-axis position
		CW_USEDEFAULT,					  // y-axis position
		CW_USEDEFAULT,					  // width
		CW_USEDEFAULT,					  // height
		(HWND)NULL,						  // ref to parent wnd
		NULL,							  // menu descriptor
		HINSTANCE(hInst),				  // app descriptor
		NULL);

	if (!hMainWnd)
	{
		return FALSE;
	}

	ShowWindow(hMainWnd, nCmdShow);
	UpdateWindow(hMainWnd);
	return TRUE;
}

// NOTE: Msg handler
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		SetMenu(hWnd);
		SetControls(hWnd);
		break;

	case WM_SIZE:
		break;
	case WM_COMMAND:
		switch (wParam)
		{
		case FILE_MENU_EXIT:
			SendMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;
		}
		break;

	case WM_PAINT:
		break;

	case WM_DESTROY:
		PostQuitMessage(NULL);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return NULL;
}

// NOTE: Edit control msg handlers 
LRESULT CALLBACK inputProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
			// TODO: Add file reading

			AppendWindowText(hResultWnd, (LPCTSTR)L"privet");
			break;
		}
	default:
		return CallWindowProc(oldInputProc, wnd, msg, wParam, lParam);
	}
	return 0;
}

void SetMenu(HWND hWnd)
{
	hMenu = CreateMenu();
	HMENU hFileMenu = CreateMenu();

	AppendMenu(hFileMenu, MF_STRING, FILE_MENU_EXIT, L"Exit");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");

	SetMenu(hWnd, hMenu);
}

void SetControls(HWND hWnd)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	int windowWidth = rect.right - rect.left;
	int windowHeight = rect.bottom - rect.top;

	// TODO: Relative positioning
	CreateWindowW(L"STATIC", L"Input file name from the root folder and press enter:", WS_VISIBLE | WS_CHILD | WS_BORDER, 0, 0, windowWidth, 30, hWnd, NULL, hInstance, NULL);
	hInputWnd = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 0, 30, windowWidth, 100, hWnd, NULL, hInstance, NULL);
	oldInputProc = (WNDPROC)SetWindowLongPtr(hInputWnd, GWLP_WNDPROC, (LONG_PTR)inputProc);
	CreateWindowW(L"STATIC", L"Result: ", WS_VISIBLE | WS_CHILD | WS_BORDER, 0, 130, windowWidth, 150, hWnd, NULL, hInstance, NULL);
	hResultWnd = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_BORDER | ES_READONLY, 0, 150, windowWidth, windowHeight, hWnd, NULL, hInstance, NULL);
}

void AppendWindowText(HWND hWnd, LPCTSTR str)
{
	SetWindowText(hWnd, str);
}
