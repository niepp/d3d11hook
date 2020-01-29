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
	stStartUpInfo.wShowWindow = 1;//窗口显示为0不显示 

	PROCESS_INFORMATION pProcessInfo;
	ZeroMemory(&pProcessInfo, sizeof(PROCESS_INFORMATION));

	//创建一个新进程  
	if (CreateProcess(
		NULL,   //  指向一个NULL结尾的、用来指定可执行模块的宽字节字符串  
		sConLin, // 命令行字符串  
		NULL, //    指向一个SECURITY_ATTRIBUTES结构体，这个结构体决定是否返回的句柄可以被子进程继承。  
		NULL, //    如果lpProcessAttributes参数为空（NULL），那么句柄不能被继承。<同上>  
		false,//    指示新进程是否从调用进程处继承了句柄。   
		0,  //  指定附加的、用来控制优先类和进程的创建的标  
			//  CREATE_NEW_CONSOLE  新控制台打开子进程  
			//  CREATE_SUSPENDED    子进程创建后挂起，直到调用ResumeThread函数  
		NULL, //    指向一个新进程的环境块。如果此参数为空，新进程使用调用进程的环境  
		workPath, //    指定子进程的工作路径  
		&stStartUpInfo, // 决定新进程的主窗体如何显示的STARTUPINFO结构体  
		&pProcessInfo  // 接收新进程的识别信息的PROCESS_INFORMATION结构体  
	))
	{
		std::cout << "create process success" << std::endl;

		//下面两行关闭句柄，解除本进程和新进程的关系，不然有可能不小心调用TerminateProcess函数关掉子进程  
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
