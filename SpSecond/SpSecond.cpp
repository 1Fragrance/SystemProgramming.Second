
#include "framework.h"
#include "SpSecond.h"
#include "winioctl.h"
#include <string> 

// NOTE: Constants
#define FILE_MENU_EXIT 1
#define MAIN_WINDOW_CLASS L"MainWindow"
#define MAIN_WINDOW_TITLE L"SpSecond"
// #define FILE_NAME L"E:\\test.txt"


// NOTE: Prototypes
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM RegisterWindowClass(HINSTANCE hInstance);
BOOL InitializeWindow(HINSTANCE, int);
void SetMenu(HWND);
void SetControls(HWND);
void AppendWindowText(HWND, LPCTSTR);
void GetFileInfo(LPCTSTR path);
void ScanFileSystem(LPCTSTR filePath);
void AddToEditText(HWND, char*);

// NOTE: Handlers
HINSTANCE hInstance;
HWND hMainWnd;
HMENU hMenu;
HWND hInputWnd;
HWND hResultWnd;
HWND hTextResultWnd;
WNDPROC oldInputProc;


DWORD dwClusterSizeInBytes = NULL;

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

// NOTE: Initialize window
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

			TCHAR path[MAX_PATH];;
			GetWindowText(hInputWnd, path, MAX_PATH);
			GetFileInfo(path);
			break;
		}
	default:
		return CallWindowProc(oldInputProc, wnd, msg, wParam, lParam);
	}
	return 0;
}

// NOTE: Set menu for window
void SetMenu(HWND hWnd)
{
	hMenu = CreateMenu();
	HMENU hFileMenu = CreateMenu();

	AppendMenu(hFileMenu, MF_STRING, FILE_MENU_EXIT, L"Exit");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");

	SetMenu(hWnd, hMenu);
}

// NOTE: Set controls for window
void SetControls(HWND hWnd)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	int windowWidth = rect.right - rect.left;
	int windowHeight = rect.bottom - rect.top;

	// TODO: Relative positioning
	CreateWindowW(L"STATIC", L"Input file path and press enter:", WS_VISIBLE | WS_CHILD | WS_BORDER, 0, 0, windowWidth, 30, hWnd, NULL, hInstance, NULL);
	hInputWnd = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 0, 30, windowWidth, 100, hWnd, NULL, hInstance, NULL);
	oldInputProc = (WNDPROC)SetWindowLongPtr(hInputWnd, GWLP_WNDPROC, (LONG_PTR)inputProc);
	CreateWindowW(L"STATIC", L"File map: ", WS_VISIBLE | WS_CHILD | WS_BORDER, 0, 130, windowWidth / 2, 150, hWnd, NULL, hInstance, NULL);
	CreateWindowW(L"STATIC", L"File text: ", WS_VISIBLE | WS_CHILD | WS_BORDER, windowWidth / 2, 130, windowWidth, 150, hWnd, NULL, hInstance, NULL);
	hResultWnd = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_BORDER | ES_READONLY, 0, 150, windowWidth / 2, windowHeight, hWnd, NULL, hInstance, NULL);
	hTextResultWnd = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_BORDER | ES_READONLY, windowWidth / 2, 150, windowWidth, windowHeight, hWnd, NULL, hInstance, NULL);
}

// NOTE: Append text to the end (unicode)
void AppendWindowText(HWND hWnd, LPCTSTR str)
{
	// NOTE: get the current selection
	DWORD StartPos, EndPos;
	SendMessage(hWnd, EM_GETSEL, reinterpret_cast<WPARAM>(&StartPos), reinterpret_cast<WPARAM>(&EndPos));

	//  NOTE: move the caret to the end of the text
	int outLength = GetWindowTextLength(hWnd);
	SendMessage(hWnd, EM_SETSEL, outLength, outLength);

	//  NOTE: insert the text at the new caret position
	SendMessage(hWnd, EM_REPLACESEL, TRUE, (LPARAM)str);

	//  NOTE: restore the previous selection
	SendMessage(hWnd, EM_SETSEL, StartPos, EndPos);
}


// NOTE: Read file, get it map, parse text and show it
#define BUFFER_CHUNK 64 * 1024 * 1024
void GetFileInfo(LPCTSTR path)
{
	UINT32 bufferSize = BUFFER_CHUNK + sizeof(LARGE_INTEGER) * 2;
	RETRIEVAL_POINTERS_BUFFER* buffer;
	buffer = (RETRIEVAL_POINTERS_BUFFER*)malloc(bufferSize);;
	STARTING_VCN_INPUT_BUFFER vcnInputBuffer;
	vcnInputBuffer.StartingVcn.QuadPart = 0;

	DWORD junk = 0;
	HANDLE hDevice = INVALID_HANDLE_VALUE;
	int clustersCount = 0;

	// NOTE: Get cluster size;
	ScanFileSystem(path);

	if (dwClusterSizeInBytes != NULL)
	{
		// NOTE: Getting file handler
		hDevice = CreateFile(path,
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			0);
		while (true)
		{
			// NOTE: File not found
			if (hDevice == NULL || hDevice == INVALID_HANDLE_VALUE)
			{
				SetWindowText(hResultWnd, L"File not found");
				SetWindowText(hTextResultWnd, L"");
				return;
			}

			// NOTE: Get file pointers
			BOOL res = DeviceIoControl(hDevice, FSCTL_GET_RETRIEVAL_POINTERS, &vcnInputBuffer, sizeof(vcnInputBuffer), buffer, bufferSize, &junk, (LPOVERLAPPED)NULL);

			// NOTE: Smth goes wrong
			wchar_t outputMsg[MAX_PATH];
			if (res == FALSE && GetLastError() != ERROR_MORE_DATA)
			{
				_snwprintf_s(outputMsg, sizeof(outputMsg), L"Raised error, code: %ld. \nFile:  %ws \n ", GetLastError(), path);

				SetWindowText(hResultWnd, outputMsg);
				SetWindowText(hTextResultWnd, L"");
				return;
			}

			// NOTE: File is empty
			if (buffer == NULL || buffer->ExtentCount < 1 || GetLastError() == ERROR_HANDLE_EOF)
			{
				SetWindowText(hResultWnd, L"Empty file");
				SetWindowText(hTextResultWnd, L"");
				return;
			}

			// NOTE: Show file info
			_snwprintf_s(outputMsg, sizeof(outputMsg), L"Extents count: %ld.\n", buffer->ExtentCount);
			SetWindowText(hResultWnd, outputMsg);

			for (int i = 0; i < buffer->ExtentCount; i++)
			{
				wchar_t extendInfo[255];
				// NOTE: VCN - virtual cluster number(offsets within the file/stream space), LCN - logical cluster number(offsets within the volume space)
				_snwprintf_s(extendInfo, sizeof(extendInfo), L"\n%ld. LCN: %ld \r\n    NEXTVCN: %ld", i + 1, buffer->Extents[i].Lcn.QuadPart, buffer->Extents[i].NextVcn.QuadPart);
				AppendWindowText(hResultWnd, extendInfo); 
				clustersCount++;
			}

			if (res == TRUE)
			{
				break;
			}
		}

		// NOTE: Reading file
		DWORD dwBytesRead;
		hDevice = CreateFile(path,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			0,
			0);

		if (!hDevice)
		{
			SetWindowText(hTextResultWnd, L"Can't read file");
			return;
		}

		// NOTE: Read file context cluster by cluster
		SetWindowText(hTextResultWnd, L"");
		for (int i = 0; i < clustersCount; i++)
		{
			// NOTE: Move file pointer to the next cluster
			LARGE_INTEGER distance;
			distance.QuadPart = (dwClusterSizeInBytes * i);
			SetFilePointerEx(
				hDevice,
				distance,
				NULL,
				FILE_BEGIN
			);

			char* buff = new char[dwClusterSizeInBytes];
			for (int n = 0; n < dwClusterSizeInBytes; n++)
			{
				buff[n] = 0;
			}
			
			if (ReadFile(hDevice, buff, dwClusterSizeInBytes, &dwBytesRead, NULL) == FALSE)
			{
				SetWindowText(hTextResultWnd, L"Can't read file");
			}
			else
			{
				// NOTE: Append text
				AddToEditText(hTextResultWnd, buff);
			}

			delete[] buff;
		}

		CloseHandle(hDevice);
	}
}

// NOTE: Scan file system and get cluster size
void ScanFileSystem(LPCTSTR filePath)
{
	_TCHAR Buffer[MAX_PATH + 1] = { 0 };
	
	if (GetVolumePathNameW(filePath, Buffer, MAX_PATH))
	{
		DWORD dwSectorsPerCluster;
		DWORD dwBytesPerSector;
		DWORD dwNumberFreeClusters;
		DWORD dwTotalNumberClusters;
		GetDiskFreeSpaceW(Buffer, &dwSectorsPerCluster, &dwBytesPerSector, &dwNumberFreeClusters, &dwTotalNumberClusters);

		dwClusterSizeInBytes = dwSectorsPerCluster * dwBytesPerSector;
	}
	else 
	{
		SetWindowText(hResultWnd, L"Can't find file volume");
		SetWindowText(hTextResultWnd, L"");
	}

}

// NOTE: Append text to the edit (not unicode)
void AddToEditText(HWND hEdit, char* str)
{
	int iLen = GetWindowTextLength(hEdit) + strlen(str) + 1;
	char* newText = (char*)malloc(iLen);
	if (newText)
	{
		GetWindowTextA(hEdit, newText, iLen);
		strcat_s(newText, iLen, str);
		SetWindowTextA(hEdit, newText);
		free(newText);
	}
}
