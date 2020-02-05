// WindowsProject1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "WindowsProject1.h"

#include <windows.h>
#include <Winspool.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <mmsystem.h>
#pragma comment(lib, "d2d1")

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Winspool.lib")

#pragma _CRT_SECURE_NO_WARNINGS

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 700
ID2D1Factory* pD2DFactory = NULL;
ID2D1SolidColorBrush* ecset;

#define ID_TIMER1 101

int bar1Y, labdaX, labdaY, labda_sebessegX, labda_sebessegY;

void init_game(void);
void render_scene(void);
void labda_frissites(void);

//***********STANDARD WIN32API************

ID2D1HwndRenderTarget* pRT = NULL;
#define HIBA_00 TEXT("Error:Program initialisation process.")
HINSTANCE hInstGlob;
int SajatiCmdShow;
HWND Form1; //Ablak kezeloje
LRESULT CALLBACK WndProc0(HWND, UINT, WPARAM, LPARAM);
//******************************************************

//*********************************
//Foprogram belepesi pont
//*********************************
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("StdWinClassName");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass0;
	SajatiCmdShow = iCmdShow;
	hInstGlob = hInstance;

	//*********************************
	//Ablak elokeszitese
	//*********************************
	wndclass0.style = CS_HREDRAW | CS_VREDRAW;
	wndclass0.lpfnWndProc = WndProc0;
	wndclass0.cbClsExtra = 0;
	wndclass0.cbWndExtra = 0;
	wndclass0.hInstance = hInstance;
	wndclass0.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass0.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass0.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wndclass0.lpszMenuName = NULL;
	wndclass0.lpszClassName = TEXT("WIN0");

	//*********************************
	//Ablak regisztrralasa
	//*********************************
	if (!RegisterClass(&wndclass0))
	{
		MessageBox(NULL, HIBA_00, TEXT("Program Start"), MB_ICONERROR);
		return 0;
	}

	//*********************************
	//Ablak letrehozasaCreating the window
	//*********************************
	Form1 = CreateWindow(TEXT("WIN0"),
		TEXT("JATEKVEZERLOK PROGRAMOZASA - PELDAPROGRAM"),
		(WS_OVERLAPPED | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX),
		0,
		0,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	//*********************************
	//Ablak megjelenitese
	//*********************************
	ShowWindow(Form1, SajatiCmdShow);
	UpdateWindow(Form1);

	//*********************************
	//Ablak mukodtetese
	//*********************************
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//*********************************
//Uzenetkezeles
//*********************************
LRESULT CALLBACK WndProc0(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	unsigned int yPos;

	DWORD            dwNeeded, dwReturned;
	HDC              hdcprinter;
	PRINTER_INFO_4 *pinfo4=NULL;
	PBYTE buffer[99];
	LPWSTR szoveg[1024];

	switch (message)
	{
		//*********************************
		//Ablak letrehozasa
		//*********************************
	case WM_CREATE:
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
		pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
				hwnd, D2D1::SizeU(SCREEN_WIDTH, SCREEN_HEIGHT)),
			&pRT);
		pRT->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
			&ecset);
		init_game();
		if ((joyGetNumDevs()) > 0) joySetCapture(hwnd, JOYSTICKID1, NULL, FALSE);//20-NULL
		SetTimer(hwnd, ID_TIMER1, 10, (TIMERPROC)NULL);
		Sleep(1000);
		EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, (LPBYTE)buffer,dwNeeded, &dwNeeded, &dwReturned);
		strcpy((char *)szoveg,(CONST CHAR *)pinfo4->pPrinterName);
		return 0;
		//*********************************
		//Villodzas ellen
		//*********************************
	case WM_ERASEBKGND:
		return (LRESULT)1;
	case WM_TIMER:
		switch (wParam)
		{
		case ID_TIMER1:
			labda_frissites();
			render_scene();
			break;
		}
		return 0;
	case MM_JOY1MOVE:
		yPos = HIWORD(lParam);
		if (yPos == 65535) {
			if (bar1Y < SCREEN_HEIGHT - 80)
			{
				bar1Y += 10;
			}
		}
		else if (yPos == 0) {
			if (bar1Y > 0)
			{
				bar1Y -= 10;
			}
		}
		break;
		//*********************************
		//Ablak ujrarajzolasa
		//*********************************
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		render_scene();
		return 0;
		//*********************************
		//Ablak bezarasa
		//*********************************
	case WM_CLOSE:
		KillTimer(hwnd, ID_TIMER1);
		pRT->Release();
		ecset->Release();
		pD2DFactory->Release();
		DestroyWindow(hwnd);
		return 0;
		//*********************************
		//Ablak megsemmisitese
		//*********************************
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

//********************************
//Kezdobeallitasok
//********************************
void init_game(void)
{
	bar1Y = SCREEN_HEIGHT / 2;
	labdaX = SCREEN_WIDTH / 2;
	labdaY = SCREEN_HEIGHT / 2;
	labda_sebessegX = 4;
	labda_sebessegY = 2;
}

void render_scene(void)
{
	pRT->BeginDraw();
	pRT->Clear(D2D1::ColorF(D2D1::ColorF::White));
	D2D1_RECT_F rectangle = D2D1::RectF(
		10.0f,
		bar1Y,
		30.0f,
		bar1Y + 70);
	pRT->FillRectangle(&rectangle, ecset);
	D2D1_RECT_F rectangle3 = D2D1::RectF(
		labdaX,
		labdaY,
		labdaX + 10,
		labdaY + 10);
	pRT->FillRectangle(&rectangle3, ecset);
	pRT->EndDraw();
}

void labda_frissites(void)
{
	//altalanos mozgatas
	labdaX += labda_sebessegX;
	labdaY += labda_sebessegY;
	if (labdaX < 0) init_game();
	else if (labdaX > SCREEN_WIDTH - 10) {
		labda_sebessegX += 2;
		if (labda_sebessegX == 15) labda_sebessegX = 14;
		labda_sebessegX *= -1;
	}

	if (labdaY > SCREEN_HEIGHT - 10) labda_sebessegY *= -1;
	else if (labdaY < 10) labda_sebessegY *= -1;


	//utkozesdetektalas
	if (labdaX < 30 && labdaX > 10 && labdaY >= bar1Y && labdaY <= bar1Y + 70) labda_sebessegX *= -1;
}