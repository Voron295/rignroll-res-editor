#include "UI.h"
#include "resource.h"
#pragma comment(lib,"shell32")
#pragma comment(lib,"comctl32.lib")
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#pragma comment(lib,"nvtt.lib")
//#pragma comment(lib,"bc6h.lib")
//#pragma comment(lib,"bc7.lib")
//#pragma comment(lib,"nvcore.lib")
//#pragma comment(lib,"nvimage.lib")
//#pragma comment(lib,"nvmath.lib")
//#pragma comment(lib,"nvthread.lib")
#include "ResUI.h"

HINSTANCE hInst;
HWND hWnd;
HFONT hMainFont;
ResUI *g_pResUI = 0;
char g_pCmdLine[512];

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	MSG msg = {0};
	strcpy(g_pCmdLine, lpCmdLine);

	MyRegisterClass(hInstance);
	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	// Выполнить инициализацию приложения:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	//MessageBox(hWnd, lpCmdLine, "Open", MB_OK);

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (!TranslateAccelerator(
				hWnd,
				hAccel,
				&msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (g_pResUI)
				g_pResUI->Render();
		}
	}

	if (g_pResUI)
		delete g_pResUI;

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wcex.lpszClassName = "RnRResManager";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

	hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

	hWnd = CreateWindow("RnRResManager", "RigNRoll RES Editor", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX,
		CW_USEDEFAULT, 0, 800, 600, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	if (g_pResUI)
		if(g_pResUI->ProcessEvents(hWnd, message, wParam, lParam))
			return 0;

	switch (message)
	{
	case WM_CREATE:
		hMainFont = UI_CreateFont(TEXT("Tahoma"), 13);
		DragAcceptFiles(hWnd, TRUE);
		g_pResUI = new ResUI(hWnd);
		if (strlen(g_pCmdLine) > 0)
		{
			char *path = g_pCmdLine;
			std::vector<std::string> files;
			while (*path)
			{
				if (*path == '"')
				{
					std::string file = "";
					path++;
					while (*path != '"' && *path)
					{
						file += *path;
						path++;
					}
					files.push_back(file);
				}
				if (*path)
					path++;
			}
			g_pResUI->OpenModules(files);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: добавьте любой код отрисовки...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
