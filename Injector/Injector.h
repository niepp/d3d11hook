#pragma once

// Windows API
#include <Windows.h>

// C++ Standard Library
#include <string>

// Class to manage DLL injection into a remote process
class Injector
{
public:
	// Get singleton
	static Injector* Get();

	// Inject library
	void InjectLib(DWORD ProcID, const std::wstring& Path);
	void InjectLib(DWORD ProcID, const std::string& Path);

	// Get SeDebugPrivilege. Needed to inject properly.
	void GetSeDebugPrivilege();

protected:
	// Enforce singleton
	Injector() { }
	~Injector();
	Injector(const Injector&);
	Injector& operator= (const Injector&);
private:
	// Singleton
	static Injector* m_pSingleton;
};
