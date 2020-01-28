#include "common.h"
#include "log.h"
#include "d3d11hook.h"

void CreateConsole()
{
	if (!AllocConsole()) {
		// Add some error handling here.
		// You can call GetLastError() to get more info about the error.
		return;
	}

	// std::cout, std::clog, std::cerr, std::cin
	FILE* fDummy;
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	std::cout.clear();
	std::clog.clear();
	std::cerr.clear();
	std::cin.clear();

	// std::wcout, std::wclog, std::wcerr, std::wcin
	HANDLE hConOut = CreateFile(TEXT("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hConIn = CreateFile(TEXT("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
	SetStdHandle(STD_ERROR_HANDLE, hConOut);
	SetStdHandle(STD_INPUT_HANDLE, hConIn);
	std::wcout.clear();
	std::wclog.clear();
	std::wcerr.clear();
	std::wcin.clear();
}


void FindWindows(DWORD pid, std::vector<HWND>& wndList)
{
	wndList.clear();
	std::pair<std::vector<HWND>&, DWORD> params = { wndList, pid };
	// Enumerate the windows using a lambda to process each window
	BOOL bResult = EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
	{
		auto pParams = (std::pair<std::vector<HWND>&, DWORD>*)(lParam);
		DWORD processId;
		if (GetWindowThreadProcessId(hwnd, &processId) && processId == pParams->second)
		{
			pParams->first.push_back(hwnd);
		}
		return TRUE;
	}, (LPARAM)&params);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved)
{
	CreateConsole();

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		{
			LogUtils::Instance()->LogInfo("DLL_PROCESS_ATTACH");

			/*
			DWORD pid = GetCurrentProcessId();
			std::vector<HWND> wndList;
			FindWindows(pid, wndList);
			if (wndList.size() > 0)
			{
				for (auto& hWnd : wndList)
				{
					wchar_t wndName[1024];
					GetWindowText(hWnd, wndName, ARRAYSIZE(wndName));
					std::string strWnd = narrow(std::wstring(wndName));
					LogUtils::Instance()->LogInfo("EnumWindows! hWnd = %d, wndName = %s", hWnd, strWnd.c_str());
				}
			}
			*/

			LogUtils::Instance()->LogInfo("StartHookDX11!");
			StartHookDX11();
		}
		break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

