//  _    _           _                       _            
// | |  | |         | |                     | |           
// | |__| |   __ _  | |   __ _   ___   ____ | | __   __ _ 
// |  __  |  / _` | | |  / _` | / __| |_  / | |/ /  / _` |
// | |  | | | (_| | | | | (_| | \__ \  / /  |   <  | (_| |
// |_|  |_|  \__,_| |_|  \__,_| |___/ /___| |_|\_\  \__,_|

#include <Windows.h>
#include <fstream>
#include <string>
#include <thread>

const UINT IDM_CHANGECOLOR = 69;
const UINT IDM_CLEAR = 70;
const UINT IDM_CLOSE = 71;
const UINT IDM_MINIMIZE = 72;

using namespace std;

//Main-Window Global-Variables
const int SizeY = 800;
const int SizeX = 800;

const HBRUSH main_bg = CreateSolidBrush(RGB(255, 255, 255));

COLORREF farbe = RGB(255, 0, 0);
//WndProc (Prototyp)
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//changeColor
int changeColor(HWND hwnd) {
	CHOOSECOLORA cc = {};
	COLORREF custColors[16];

	cc.hwndOwner = hwnd;
	cc.lStructSize = sizeof(cc);
	cc.rgbResult = farbe;
	cc.lpCustColors = custColors;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColorA(&cc)) {
		MessageBoxA(NULL, "Color changed!", "Color", MB_OK);

		farbe = cc.rgbResult;

		if (farbe == RGB(255, 0, 0)) {
			MessageBoxA(NULL, "Farbe nicht geändert!", "Fehler", MB_ICONERROR);
		}
	}

	return 0;
}
//createPaintyMenu
int createPaintyMenu(HWND hwnd) {
	HMENU hBar;
	HMENU hMenu;

	HMENU WindowMenu;

	//Initialise HMENU´s
	hBar = CreateMenu();
	hMenu = CreateMenu();

	WindowMenu = CreateMenu();

	//Create the Menus
	AppendMenuA(hMenu, MF_STRING, IDM_CHANGECOLOR, "Change Color");

	AppendMenuA(hBar, MF_POPUP, (UINT_PTR)hMenu, "Color");

	//Fenster Menü
	AppendMenuA(WindowMenu, MF_STRING, IDM_CLEAR, "Clear");
	AppendMenuA(WindowMenu, MF_MENUBREAK, NULL, NULL);

	AppendMenuA(WindowMenu, MF_STRING, IDM_CLOSE, "Close Window");
	AppendMenuA(WindowMenu, MF_STRING, IDM_MINIMIZE, "Minimize");
	
	//Set to WindowBar
	AppendMenuA(hBar, MF_POPUP, (UINT_PTR)WindowMenu, "Window");
	
	BOOL rc = SetMenu(hwnd, hBar);

	if (rc == NULL) {
		return -1;
	}
	else {
		return 0;
	}


}

//WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
	//Main-Window | Variablen
	HWND hwnd;
	WNDCLASS wc = {};
	const wchar_t szMain[] = L"Painty";
	MSG msg;

	//"wc" WNDCLASS definine
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = szMain;
	wc.hbrBackground = main_bg;
	wc.hCursor = LoadCursor(NULL, IDC_CROSS);

	//Register "wc"
	RegisterClass(&wc);

	//Create the Main-Window
	hwnd = CreateWindow(szMain, L"Painty", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, SizeX, SizeY, NULL, NULL, hInstance, NULL);

	//Show the Main Window
	ShowWindow(hwnd, iCmdShow);

	//Get Windows-Messages
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

//==================================================================================================
//====> WndProc <====
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static HINSTANCE hInstance = GetModuleHandle(NULL);

	static POINT cursorPoint;


	switch (msg) {
		//======> WM_CLOSE <=======
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;

		//======> WM_DESTROY <=======
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		//======> WM_GETMINMAXINFO <=======
	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = 300;
		lpMMI->ptMinTrackSize.y = 300;

		return 0;
	}


		//======> WM_CREATE <=======
	case WM_CREATE:
		//Set the Menu
		createPaintyMenu(hwnd);

		return 0;


		//======> WM_COMMAND <=======
	case WM_COMMAND:

		//===> IDM_FARBEWECHSELN
		if(LOWORD(wParam) == IDM_CHANGECOLOR) {
			changeColor(hwnd);
		}
		//===> IDM_CLOSE
		if (LOWORD(wParam) == IDM_CLOSE) {
			DestroyWindow(hwnd);
		}
		//===> IDM_MINIMIZE
		if (LOWORD(wParam) == IDM_MINIMIZE) {
			ShowWindow(hwnd, 6);
		}
		//===> IDM_CLEAR
		if (LOWORD(wParam) == IDM_CLEAR) {
			InvalidateRect(hwnd, NULL, TRUE);
		}

		return 0;


		//======> WM_LBUTTONDOWN <=======
	case WM_LBUTTONDOWN: {
		//Get the Cursor Position in X- and Y-Coordinate
		cursorPoint.x = LOWORD(lParam);
		cursorPoint.y = HIWORD(lParam);

		//Get HDC
		HDC hdc = GetDC(hwnd);

		//Set Pixel in the Color at Point
		SetPixel(hdc, cursorPoint.x, cursorPoint.y, farbe);

		//Release HDC
		ReleaseDC(hwnd, hdc);

		return 0;
	}

		//======> WM_MOUSEMOVE <=======
	case WM_MOUSEMOVE: {
		if (wParam & MK_LBUTTON) {
			cursorPoint.x = LOWORD(lParam);
			cursorPoint.y = HIWORD(lParam);

			//Get HDC
			HDC hdc = GetDC(hwnd);

			//Set Pixel in the Color at Point
			SetPixel(hdc, cursorPoint.x, cursorPoint.y, farbe);

			for (int i = 0; i != 5; i++) {
				SetPixel(hdc, cursorPoint.x + 1 * i, cursorPoint.y, farbe);
			}

			//Release HDC
			ReleaseDC(hwnd, hdc);
		}

		return 0;
	}

		

	}

	//Return DefProc
	return DefWindowProc(hwnd, msg, wParam, lParam);
}