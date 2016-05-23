//
//软渲染程序
//2016年5月 by moonightlei
//

#include<Windows.h>
#include<iostream>
#include<stdlib.h>
#include<time.h>
#include"3dlib.h"
#include"setting.h"
#include"rasterization.h"

#pragma warning(disable : 4996)
#pragma comment( lib,"winmm.lib" )


using namespace std;

//ColorRgb g_framebuffer[WINDOW_WIDTH][WINDOW_HEIGHT];
extern ColorRgb g_framebuffer[WINDOW_WIDTH][WINDOW_HEIGHT];

//=================================================================================
//  全局变量声明
HDC g_hdc = NULL,g_mdc = NULL,g_bufdc; //全局设备环境句柄
RECT g_rect;
HBITMAP g_bitmap;

FILE *_stream;

DWORD g_tPre=0,g_tNow=0;
int g_iNum = 0;
HFONT g_hFont;

// 全局变量声明结束 =================================================================

//===============================================================================
//   函数声明
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL Program_Init(HWND hwnd);
VOID Program_Paint(HWND hwnd);
BOOL Program_CleanUp(HWND hwnd);
FLOAT Get_FPS();
//   函数声明 ========================================================================

//应用程序入口函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//实例化窗口类，并设定参数
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc; //设定窗口过程函数的函数指针
	wndClass.cbWndExtra = 0;
	wndClass.cbClsExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor(NULL,IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = L"MainInterfaceWnd";

	//注册窗口类
	if (!RegisterClassEx(&wndClass))	return -1;

	//创建窗口
	HWND hwnd = CreateWindow(L"MainInterfaceWnd", WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL,NULL, hInstance, NULL);

	MoveWindow(hwnd, 250, 80, WINDOW_WIDTH, WINDOW_HEIGHT, true);
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	if (!Program_Init(hwnd)) {
		MessageBox(hwnd, L"资源初始化失败", L"消息窗口", 0);
		return FALSE;
	}

	//if (AllocConsole())
	//{
	//	//MessageBox(hwnd, L"开启控制台成功", L"消息窗口", 0);
	//	//freopen("CONIN$", "r+t", stdin);
	//	//freopen_s(&_stream,"CONOUT$", "w+t", stdout);
	//	freopen("CONOUT$", "w", stdout);
	//	//printf("hello hplonline!-_-\n");
	//	//std::cout << "test" << std::endl;
	//}

	//消息循环
	MSG msg = { 0 };
	while(msg.message != WM_QUIT ){ 
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			/*g_tNow = GetTickCount();
			if(g_tNow-g_tPre >= 1/60.0)*/
				Program_Paint(hwnd);


		}
	}

	UnregisterClass(L"MainInterfaceWnd", wndClass.hInstance);


	return 0;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParm)
{
	PAINTSTRUCT painStruct;
	switch (message)
	{
	//case WM_PAINT://客户区重绘消息
	//	g_hdc = BeginPaint(hwnd, &painStruct);
	//	Program_Paint(hwnd);
	//	EndPaint(hwnd,&painStruct);
	//	ValidateRect(hwnd, NULL);
	//	break;
	case WM_KEYDOWN://按键消息
		if (wParam == VK_ESCAPE)//ESC键
			DestroyWindow(hwnd);
		break;
	case WM_DESTROY://窗口销毁消息
		Program_CleanUp(hwnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParm);
	}

	return 0;
}


BOOL Program_Init(HWND hwnd)
{
	srand((unsigned)time(NULL));//初始化随机种子

	

	g_hdc = GetDC(hwnd);
	g_mdc = CreateCompatibleDC(g_hdc);

	
	//g_bufdc = CreateCompatibleDC(g_hdc);

	
	


	//GetClientRect(hwnd, &g_rect);

	HFONT hFont;
	hFont = CreateFont(20,0,0,0,700,0,0,0,GB2312_CHARSET,0,0,0,0,TEXT("微软雅黑"));
	SelectObject(g_mdc,hFont);
	SetBkMode(g_mdc,TRANSPARENT);

	for (int i = 0; i < WINDOW_WIDTH; ++i)
		for (int j = 0; j < WINDOW_HEIGHT; ++j)
			g_framebuffer[i][j] = { 0,0,0 };

	Program_Paint(hwnd);

	return TRUE;
}

VOID Program_Paint(HWND hwnd)
{
	//清空像素缓存
	/*
	for (int i = 0; i < WINDOW_WIDTH; ++i)
		for (int j = 0; j < WINDOW_HEIGHT; ++j)
			g_framebuffer[i][j].setColor(0,0,0);
	*/
	//绘图
	HBITMAP bmp;
	bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
	SelectObject(g_mdc, bmp);
	
	
	/*rasterzization::bresenham_DrawLine(
		rand()%WINDOW_WIDTH, rand() % WINDOW_WIDTH,
		rand() % WINDOW_HEIGHT, rand() % WINDOW_HEIGHT,g_mdc);*/

	//绘制到屏幕上
	/*
	for (int i = 0; i < WINDOW_WIDTH; ++i)
		for (int j = 0; j < WINDOW_HEIGHT; ++j)
			SetPixel(g_mdc, i, j,RGB(g_framebuffer[i][j].r,g_framebuffer[i][j].b, g_framebuffer[i][j].b));
			//g_framebuffer[i][j].setColor(0, 0, 0);
	*/

	//绘制文字
	wchar_t str[100];
	swprintf_s(str, L"FPS:%.2f", Get_FPS());
	SetTextColor(g_mdc, RGB(10, 10, 255));
	TextOut(g_mdc, WINDOW_WIDTH - 150, 50, str, wcslen(str));


	//交换缓冲区
	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, 0, 0, SRCCOPY);
	g_tPre = GetTickCount();
	//g_iNum++;
	//g_mdc = NULL;
	DeleteObject(bmp);
}

BOOL Program_CleanUp(HWND hwnd)
{

	//FreeConsole();
	DeleteDC(g_bufdc);
	DeleteDC(g_mdc);
	ReleaseDC(hwnd, g_hdc);
	return TRUE;
}

FLOAT Get_FPS()
{
	static float fps = 0;
	static int frameCount = 0;
	static float currentTime = 0.0f;
	static float lastTime = 0.0f;


	frameCount++;
	currentTime = timeGetTime() * 0.001f;
	if (currentTime - lastTime > 1.0f) {
		fps = (float)frameCount / (currentTime - lastTime);
		lastTime = currentTime;
		frameCount = 0;
	}
	return fps;
}