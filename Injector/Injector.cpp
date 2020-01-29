// Windows Includes
#include <Windows.h>
#include <TlHelp32.h>
#include <malloc.h>
#include <Tchar.h>

// C++ Standard Library
#include <vector>

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

// Injects a module (fully qualified path) via process id
void Injector::InjectLib(DWORD ProcID, const std::string& Path)
{
	// Get a handle for the target process.
	EnsureCloseHandle Process(OpenProcess(
		PROCESS_QUERY_INFORMATION |   // Required by Alpha
		PROCESS_CREATE_THREAD     |   // For CreateRemoteThread
		PROCESS_VM_OPERATION      |   // For VirtualAllocEx/VirtualFreeEx
		PROCESS_VM_WRITE,             // For WriteProcessMemory
		FALSE, ProcID));
	if (!Process) 
		throw std::runtime_error("Could not get handle to process.");

	// Calculate the number of bytes needed for the DLL's pathname
	size_t Size  = (Path.length() + 1) * sizeof(char);

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
		(GetProcAddress(hKernel32, TEXT("LoadLibraryA")));
	if (!pfnThreadRtn)
		throw std::runtime_error("Could not get pointer to LoadLibraryW.");

	// Create a remote thread that calls LoadLibraryW(DLLPathname)
	EnsureCloseHandle Thread(CreateRemoteThread(Process, NULL, 0, pfnThreadRtn, 
		LibFileRemote, 0, NULL));
	if (!Thread)
		throw std::runtime_error("Could not create thread in remote process.");

	// Wait for the remote thread to terminate
	DWORD WaitRet = WaitForSingleObject(Thread, INFINITE);

	// Get thread exit code
	DWORD ExitCode;
	if (!GetExitCodeThread(Thread, &ExitCode))
		throw std::runtime_error("Could not get thread exit code.");

	// Check LoadLibrary succeeded and returned a module base
	if (!ExitCode) {
		DWORD ExitErr = GetLastError();
		throw std::runtime_error("Call to LoadLibrary in remote process failed.");
	}
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