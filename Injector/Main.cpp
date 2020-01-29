// Injector
#include "Injector.h"

// Windows API
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <Shlwapi.h>

// C++ Standard Library
#include <iostream>
#include <string>
#include <locale>

struct wstr
{
	wchar_t *ws;
	wstr(const char* s)
	{
		int len = mbstowcs(NULL, s, 0) + 1;
		ws = (wchar_t *)malloc(len * sizeof(wchar_t));
		mbstowcs(ws, s, len);
	}

	~wstr()
	{
		free(ws);
		ws = nullptr;
	}

};

DWORD LanuchProcess(LPTSTR exeFile, LPTSTR workPath)
{

	SetCurrentDirectory(workPath);

	LPTSTR sConLin = new TCHAR[MAX_PATH];
	wcscpy(sConLin, exeFile);

	STARTUPINFO stStartUpInfo = { sizeof(STARTUPINFO) };
	ZeroMemory(&stStartUpInfo, sizeof(STARTUPINFO));
	stStartUpInfo.cb = sizeof(STARTUPINFO);
	stStartUpInfo.dwFlags = STARTF_USESHOWWINDOW;
	stStartUpInfo.wShowWindow = 1;//������ʾΪ0����ʾ 

	PROCESS_INFORMATION pProcessInfo;
	ZeroMemory(&pProcessInfo, sizeof(PROCESS_INFORMATION));

	//����һ���½���  
	if (CreateProcess(
		NULL,   //  ָ��һ��NULL��β�ġ�����ָ����ִ��ģ��Ŀ��ֽ��ַ���  
		sConLin, // �������ַ���  
		NULL, //    ָ��һ��SECURITY_ATTRIBUTES�ṹ�壬����ṹ������Ƿ񷵻صľ�����Ա��ӽ��̼̳С�  
		NULL, //    ���lpProcessAttributes����Ϊ�գ�NULL������ô������ܱ��̳С�<ͬ��>  
		false,//    ָʾ�½����Ƿ�ӵ��ý��̴��̳��˾����   
		0,  //  ָ�����ӵġ���������������ͽ��̵Ĵ����ı�  
			//  CREATE_NEW_CONSOLE  �¿���̨���ӽ���  
			//  CREATE_SUSPENDED    �ӽ��̴��������ֱ������ResumeThread����  
		NULL, //    ָ��һ���½��̵Ļ����顣����˲���Ϊ�գ��½���ʹ�õ��ý��̵Ļ���  
		workPath, //    ָ���ӽ��̵Ĺ���·��  
		&stStartUpInfo, // �����½��̵������������ʾ��STARTUPINFO�ṹ��  
		&pProcessInfo  // �����½��̵�ʶ����Ϣ��PROCESS_INFORMATION�ṹ��  
	))
	{
		std::cout << "create process success" << std::endl;

		//�������йرվ������������̺��½��̵Ĺ�ϵ����Ȼ�п��ܲ�С�ĵ���TerminateProcess�����ص��ӽ���  
		CloseHandle(pProcessInfo.hProcess);
		CloseHandle(pProcessInfo.hThread);
	}
	else {
		std::cerr << "failed to create process" << std::endl;
	}

	return pProcessInfo.dwProcessId;

}

// Entry point
int main(int argc, char* argv[])
{
    try
    {
		LPTSTR curDir = new TCHAR[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, curDir);

		wstr exeFile(argv[1]);
		wstr workPath(argv[2]);

		// Variable to store process ID
		DWORD ProcID = LanuchProcess(exeFile.ws, workPath.ws);

		SetCurrentDirectory(curDir);

        // Get privileges required to perform the injection
        Injector::Get()->GetSeDebugPrivilege();

		std::wstring dllPath(curDir);
		dllPath += L"/dllhook.dll";

		//std::string dllPath(curDir);
		Injector::Get()->InjectLib(ProcID, dllPath);
		// If we get to this point then no exceptions have been thrown so we
		// assume success.
		std::cout << "Successfully injected module!" << std::endl;
    
    }
    // Catch STL-based exceptions.
    catch (const std::exception& e)
    {
        std::string TempError(e.what());
        std::string Error(TempError.begin(), TempError.end());
        std::cout << "General Error:" << std::endl
            << Error << std::endl;
    }
    catch (...)
    {
        std::cout << "Unknown error!" << std::endl;
    }

    // Return success
    return ERROR_SUCCESS;
}
