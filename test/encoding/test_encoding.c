// Test target for FVPKernel's encoding support.
//
// It is run through FVPLoaderGui.exe (which injects FVPKernel.dll) and writes
// what the hooked ANSI APIs produced to test_encoding.log next to itself.
//
// The four ANSI bytes below are GBK for "测试" (U+6D4B U+8BD5); other code
// pages decode them into different code units, which is what makes the
// configured code page observable.
//
// Note that the register/create-window hooks only kick in for the engine's
// hardcoded main window class, so the test uses that exact class name.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma warning(disable:4996)

#define GBK_TEST "\xB2\xE2\xCA\xD4"

static char   LogPath[MAX_PATH];
static HANDLE LogFile = INVALID_HANDLE_VALUE;

static void LogOpen(void)
{
	char* p;

	GetModuleFileNameA(NULL, LogPath, sizeof(LogPath));
	for (p = LogPath + lstrlenA(LogPath); p > LogPath; p--)
	{
		if (p[-1] == '\\' || p[-1] == '/')
			break;
	}

	lstrcpyA(p, "test_encoding.log");
	LogFile = CreateFileA(LogPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
}

static void Log(const char* Text)
{
	DWORD Written;

	if (LogFile != INVALID_HANDLE_VALUE)
		WriteFile(LogFile, Text, (DWORD)lstrlenA(Text), &Written, NULL);
}

static void LogLine(const char* Text)
{
	Log(Text);
	Log("\r\n");
}

static void LogWideHex(const WCHAR* Text, int Count)
{
	char Hex[8];
	int  i;

	for (i = 0; i < Count && i < 8; i++)
	{
		wsprintfA(Hex, "%04X ", (UINT)Text[i]);
		Log(Hex);
	}
	LogLine("");
}

static void LogBytesHex(const BYTE* Data, int Count)
{
	char Hex[8];
	int  i;

	for (i = 0; i < Count && i < 8; i++)
	{
		wsprintfA(Hex, "%02X ", (UINT)Data[i]);
		Log(Hex);
	}
	LogLine("");
}

static LRESULT CALLBACK TestWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProcA(hWnd, Message, wParam, lParam);
}

int main(void)
{
	WNDCLASSEXA    wc;
	HWND           hWnd;
	MENUITEMINFOA  mii;
	MENUITEMINFOW  wii;
	OSVERSIONINFOA osv;
	LOGFONTA       lf;
	HFONT          hFont;
	HMENU          hMenu;
	char           Line[256];
	WCHAR          Wide[64];
	int            Count;

	LogOpen();

	// 1. GetVersionExA is hooked to report Windows XP 5.1
	ZeroMemory(&osv, sizeof(osv));
	osv.dwOSVersionInfoSize = sizeof(osv);
	if (GetVersionExA(&osv))
		wsprintfA(Line, "VERSION=%u.%u.%u", osv.dwMajorVersion, osv.dwMinorVersion, osv.dwBuildNumber);
	else
		lstrcpyA(Line, "VERSION=FAILED");
	LogLine(Line);

	// 2. RegisterClassExA / CreateWindowExA are hooked for the engine class
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize        = sizeof(wc);
	wc.lpfnWndProc   = TestWndProc;
	wc.hInstance     = GetModuleHandleA(NULL);
	wc.lpszClassName = "HS_MAIN_WINDOW_CLASS00";
	wsprintfA(Line, "REGISTERCLASS=%d", RegisterClassExA(&wc) != 0);
	LogLine(Line);

	hWnd = CreateWindowExA(0, wc.lpszClassName, GBK_TEST, WS_OVERLAPPEDWINDOW,
		0, 0, 200, 100, NULL, NULL, wc.hInstance, NULL);
	wsprintfA(Line, "CREATEWINDOW=%d", hWnd != NULL);
	LogLine(Line);

	if (hWnd)
	{
		Wide[0] = 0;
		Count   = GetWindowTextW(hWnd, Wide, (int)(sizeof(Wide) / sizeof(Wide[0])));
		wsprintfA(Line, "TITLECCH=%d", Count);
		LogLine(Line);
		Log("TITLEW=");
		LogWideHex(Wide, Count);
	}

	// 3. CreateFontA is hooked, the charset comes from the configuration
	hFont = CreateFontA(12, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, GBK_TEST);
	ZeroMemory(&lf, sizeof(lf));
	GetObjectA(hFont, sizeof(lf), &lf);
	wsprintfA(Line, "FONTCHARSET=%02X", (UINT)(BYTE)lf.lfCharSet);
	LogLine(Line);
	Log("FONTFACE=");
	LogBytesHex((const BYTE*)lf.lfFaceName, 8);
	DeleteObject(hFont);

	// 4. SetMenuItemInfoA is hooked; read the result back with the raw W API
	hMenu = CreatePopupMenu();
	ZeroMemory(&mii, sizeof(mii));
	mii.cbSize     = sizeof(mii);
	mii.fMask      = MIIM_ID | MIIM_FTYPE | MIIM_STRING;
	mii.fType      = MFT_STRING;
	mii.wID        = 1;
	mii.dwTypeData = "placeholder";
	wsprintfA(Line, "INSERTMENU=%d", InsertMenuItemA(hMenu, 0, TRUE, &mii));
	LogLine(Line);

	ZeroMemory(&mii, sizeof(mii));
	mii.cbSize     = sizeof(mii);
	mii.fMask      = MIIM_STRING;
	mii.dwTypeData = GBK_TEST;
	mii.cch        = 4;
	wsprintfA(Line, "SETMENUITEM=%d", SetMenuItemInfoA(hMenu, 0, TRUE, &mii));
	LogLine(Line);

	ZeroMemory(Wide, sizeof(Wide));
	ZeroMemory(&wii, sizeof(wii));
	wii.cbSize     = sizeof(wii);
	wii.fMask      = MIIM_STRING;
	wii.dwTypeData = Wide;
	wii.cch        = (UINT)(sizeof(Wide) / sizeof(Wide[0]));
	wsprintfA(Line, "GETMENUITEMW=%d", GetMenuItemInfoW(hMenu, 0, TRUE, &wii));
	LogLine(Line);
	Log("MENUTEXTW=");
	LogWideHex(Wide, (int)wii.cch);

	DestroyMenu(hMenu);

	if (LogFile != INVALID_HANDLE_VALUE)
		CloseHandle(LogFile);

	return 0;
}
