// Injector
#include "Injector.h"
#include "Seh.h"
#include "StringWrap.h"
#include "argh.h"
#include "StringUtil.h"

// Windows API
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <Shlwapi.h>

// C++ Standard Library
#include <iostream>
#include <string>
#include <locale>

DWORD LanuchProcess(LPTSTR exeFile, LPTSTR workPath)
{

	LPTSTR curDir = new TCHAR[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, curDir);

	SetCurrentDirectory(workPath);

	LPTSTR sConLin = new TCHAR[MAX_PATH];
	strcpy(sConLin, exeFile);

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

	SetCurrentDirectory(curDir);
	return pProcessInfo.dwProcessId;

}

// Entry point
int main(int, char* argv[])
{
	try
	{

	 //HMODULE hmod =	LoadLibraryW(L"dllhook.dll");


	// Needed to proxy SEH exceptions to C++ exceptions
	SehGuard Guard;

        // Variable to store process ID
        DWORD ProcID = LanuchProcess("D:/Need for Speed Most Wanted/NFS13.exe", "D:/Need for Speed Most Wanted/");
   

        // Get privileges required to perform the injection
        Injector::Get()->GetSeDebugPrivilege();

        // Inject module
        Injector::Get()->InjectLib(ProcID, "dllhook.dll");
        // If we get to this point then no exceptions have been thrown so we
        // assume success.
        std::tcout << "Successfully injected module!" << std::endl;

    }
    // Catch STL-based exceptions.
    catch (const std::exception& e)
    {
        std::string TempError(e.what());
        std::tstring Error(TempError.begin(), TempError.end());
        std::tcout << "General Error:" << std::endl
            << Error << std::endl;
    }
    // Catch custom SEH-proxy exceptions.
    // Currently only supports outputting error code.
    // TODO: Convert to string and dump more verbose output.
    catch (const SehException& e)
    {
        std::tcout << "SEH Error:" << std::endl
            << e.GetCode() << std::endl;
    }
    // Catch any other unknown exceptions.
    // TODO: Find a better way to handle this. Should never happen anyway, but
    // you never know.
    // Note: Could use SetUnhandledExceptionFilter but would potentially be 
    // messy.
    catch (...)
    {
        std::tcout << "Unknown error!" << std::endl;
    }

    // Return success
    return ERROR_SUCCESS;
}
