// Windows Includes
#include <Windows.h>
#include <TlHelp32.h>
#include <malloc.h>
#include <Tchar.h>

// C++ Standard Library
#include <vector>
#include <iostream>
#include <string>

// StealthLib
#include "Injector.h"
#include "EnsureCleanup.h"

// Static data
Injector* Injector::m_pSingleton = 0;

// Get injector singleton
Injector* Injector::Get()
{
	if (!m_pSingleton)
		m_pSingleton = new Injector();
	return m_pSingleton;
}

uintptr_t FindRemoteDLL(DWORD pid, const std::wstring libName)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;

	// up to 10 retries
	for (int i = 0; i < 10; i++)
	{
		hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
		if (hModuleSnap == INVALID_HANDLE_VALUE)
		{
			DWORD err = GetLastError();
			std::cout << "CreateToolhelp32Snapshot(" << pid << ") -> " << err << std::endl;

			// retry if error is ERROR_BAD_LENGTH
			if (err == ERROR_BAD_LENGTH)
				continue;
		}

		// didn't retry, or succeeded
		break;
	}

	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		std::cout << "Couldn't create toolhelp dump of modules in process " << pid << std::endl;
		return 0;
	}

	MODULEENTRY32 me32;
	memset(&me32, 0, sizeof(me32));
	me32.dwSize = sizeof(MODULEENTRY32);

	BOOL success = Module32First(hModuleSnap, &me32);

	if (success == FALSE)
	{
		DWORD err = GetLastError();

		std::cout << "Couldn't get first module in process " << pid << "err: " << err << std::endl;
		CloseHandle(hModuleSnap);
		return 0;
	}

	uintptr_t ret = 0;

	int numModules = 0;

	do
	{
		wchar_t modnameLower[MAX_MODULE_NAME32 + 1];
		memset(&modnameLower, 0, sizeof(modnameLower));
		wcsncpy_s(modnameLower, me32.szModule, MAX_MODULE_NAME32);

		wchar_t *wc = &modnameLower[0];
		while (*wc)
		{
			*wc = towlower(*wc);
			wc++;
		}

		numModules++;

		if (wcsstr(modnameLower, libName.c_str()) == modnameLower)
		{
			ret = (uintptr_t)me32.modBaseAddr;
		}
	} while (ret == 0 && Module32Next(hModuleSnap, &me32));

	if (ret == 0)
	{
		HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);

		DWORD exitCode = 0;

		if (h)
			GetExitCodeProcess(h, &exitCode);

		if (h == NULL || exitCode != STILL_ACTIVE)
		{
			std::cout <<
				"Error injecting into remote process with PID " << pid << "which is no longer available.\n"
				"Possibly the process has crashed during early startup, or is missing DLLs to run?" << std::endl;
		}
		else
		{
			std::cout << "Couldn't find module \'" << libName.data() << "\' among " << numModules << " modules" << std::endl;
		}

		if (h)
			CloseHandle(h);
	}

	CloseHandle(hModuleSnap);

	return ret;
}


// Injects a module (fully qualified path) via process id
void Injector::InjectLib(DWORD ProcID, const std::wstring& Path)
{
	// Get a handle for the target process.
	EnsureCloseHandle Process(OpenProcess(
		PROCESS_QUERY_INFORMATION |   // Required by Alpha
		PROCESS_CREATE_THREAD     |   // For CreateRemoteThread
		PROCESS_VM_OPERATION      |   // For VirtualAllocEx/VirtualFreeEx
		PROCESS_VM_WRITE          |   // For WriteProcessMemory
		PROCESS_VM_READ			  |
		SYNCHRONIZE,
		FALSE, ProcID));
	if (!Process) 
		throw std::runtime_error("Could not get handle to process.");

	// Calculate the number of bytes needed for the DLL's pathname
	size_t Size  = (Path.length() + 1) * sizeof(wchar_t);

	// Allocate space in the remote process for the pathname
	EnsureReleaseRegionEx LibFileRemote(VirtualAllocEx(Process, NULL, Size, MEM_COMMIT, PAGE_READWRITE),
		Process);
	if (!LibFileRemote)
		throw std::runtime_error("Could not allocate memory in remote process.");

	// Copy the DLL's pathname to the remote process' address space
	if (!WriteProcessMemory(Process, LibFileRemote, 
		Path.c_str(), Size, NULL))
		throw std::runtime_error("Could not write to memory in remote process.");

	// Get the real address of LoadLibraryW in Kernel32.dll
	HMODULE hKernel32 = GetModuleHandle(TEXT("Kernel32"));
	if (!hKernel32)
		throw std::runtime_error("Could not get handle to Kernel32.");
	PTHREAD_START_ROUTINE pfnThreadRtn = reinterpret_cast<PTHREAD_START_ROUTINE>
		(GetProcAddress(hKernel32, "LoadLibraryW"));
	if (!pfnThreadRtn)
		throw std::runtime_error("Could not get pointer to LoadLibraryW.");

	// Create a remote thread that calls LoadLibraryW(DLLPathname)
	HANDLE Thread = CreateRemoteThread(Process, NULL, 0, pfnThreadRtn,
		LibFileRemote, 0, NULL);
	if (!Thread)
		throw std::runtime_error("Could not create thread in remote process.");

	// Wait for the remote thread to terminate
	WaitForSingleObject(Thread, INFINITE);
	CloseHandle(Thread);

	auto func = FindRemoteDLL(ProcID, L"dllhook.dll");
	if (func == 0)
	{
		throw std::runtime_error("cannot find remotedll.");
	}

}

// MBCS version of InjectLib
void Injector::InjectLib(DWORD ProcID, const std::string& Path)
{
	// Convert path to unicode
	std::wstring UnicodePath(Path.begin(),Path.end());

	// Call the Unicode version of the function to actually do the work.
	InjectLib(ProcID, UnicodePath);
}

// Gives the current process the SeDebugPrivelige so we can get the 
// required process handle.
// Note: Requires administrator rights
void Injector::GetSeDebugPrivilege()
{
	// Open current process token with adjust rights
	HANDLE TempToken;
	BOOL RetVal = OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES
		| TOKEN_QUERY, &TempToken);
	if (!RetVal) 
		throw std::runtime_error("Could not open process token.");
	EnsureCloseHandle Token(TempToken);

	// Get the LUID for SE_DEBUG_NAME 
	LUID Luid = { NULL }; // Locally unique identifier
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Luid)) 
		throw std::runtime_error("Could not look up privilege value for SeDebugName.");
	if (Luid.LowPart == NULL && Luid.HighPart == NULL) 
		throw std::runtime_error("Could not get LUID for SeDebugName.");

	// Process privileges
	TOKEN_PRIVILEGES Privileges = { NULL };
	// Set the privileges we need
	Privileges.PrivilegeCount = 1;
	Privileges.Privileges[0].Luid = Luid;
	Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Apply the adjusted privileges
	if (!AdjustTokenPrivileges(Token, FALSE, &Privileges,
		sizeof (Privileges), NULL, NULL)) 
		throw std::runtime_error("Could not adjust token privileges.");
}
