// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Modern Warfare 2 patches: console-based console
//
// Initial author: NTAuthority
// Started: 2011-01-20
// ==========================================================

#include "StdInc.h"
#undef MOUSE_MOVED

#include "Hooking.h"
#include "Tool.h"
#include "s10e5.h"

#include <curses.h>
#include <cctype>
#include <clocale>

struct WinConData
{
	HWND__ *hWnd;
	HWND__ *hwndBuffer;
	HWND__ *codLogo;
	HFONT__ *hfBufferFont;
	HWND__ *hwndInputLine;
	char errorString[512];
	char consoleText[512];
	char returnedText[512];
	int windowWidth;
	int windowHeight;
	int(__stdcall *SysInputLineWndProc)(HWND__ *, unsigned int, unsigned int, int);
};

WinConData* s_wcd = (WinConData*)0x64A3288;

// should use funcionality?
extern bool console;

void Sys_Print(const char* message)
{
	Com_Printf(message);
}

extern void Com_Quit();
void Sys_Error(const char* format, ...)
{
	static char buffer[32768];
	va_list va;
	va_start(va, format);
	//vprintf(format, va);
	_vsnprintf(buffer, sizeof(buffer), format, va);
	va_end(va);

	Com_Error(true, "%s", buffer);
}

void GrabCommand(const char* str)
{
	const char* cmd = str + 1;
	Cbuf_AddText(0, cmd);
	Com_Printf(str);
}

void ConsoleCommands_Init();

void InitConsole()
{
	call(0x4914B0, Sys_Print, PATCH_JUMP);
	call(0x43D570, Sys_Error, PATCH_JUMP);
	call(0x47025A, GrabCommand, PATCH_CALL);

	if (!console) return;

	FreeConsole();

	*(DWORD*)0x42895D = 440; // resize window

	DWORD showConsole = 0x4305E0;
	__asm call showConsole;

	SetWindowText(s_wcd->hWnd, "ZoneBuilder " MAJOR_VERSION_STR "." MINOR_VERSION_STR "." COMMIT_STR " Console");

	// for some reason LoadImage returns NULL and doesn't set the last error if the format is corupted (AKA exported by GIMP)
	HANDLE logo = LoadImage(NULL, "zonebuilder.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (logo)
		SendMessageA(s_wcd->codLogo, STM_SETIMAGE, NULL, (LPARAM)logo);

	const char* credit = "ZoneBuilder by TheApadayo, DidUknowiPwn && momo5502. (Build: " COMMIT_STR " " __DATE__ " - " __TIME__ ")";

	HFONT font = CreateFont(-10, 0, 0, 0, 0, 0, 0, 0, 1, 8, 0, 5, 2, "Verdana");
	HWND static1 = CreateWindowEx(0, "static", credit, 0x50000000, 6, 425, 600, 25, s_wcd->hWnd, 0, GetModuleHandle(NULL), 0);
	SendMessageA(static1, WM_SETFONT, (WPARAM)font, 0);
}



void RunConsole()
{
	ConsoleCommands_Init();

	MSG msg;
	while (GetMessage(&msg, s_wcd->hWnd, NULL, NULL) > 0){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		Cbuf_Execute(0, 0);
	}

}