// SLDaemon.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#pragma comment(linker,"/subsystem:\"windows\"  /entry:\"mainCRTStartup\"" )

using namespace std; 

int main(int argc, char *argv[])
{
	STARTUPINFO si;
	DWORD returnCode;
	PROCESS_INFORMATION pi;		//进程信息 
	CString cmd;
	cmd.Format(L"cmd / c %s", argv[1]);
	LPWSTR lpCmd = cmd.GetBuffer();
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	do {
		// 创建子进程，判断是否执行成功 
		if (!CreateProcess(NULL, lpCmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		{
			return -1;
		}
		//进程执行成功，打印进程信息 
		//cout << "以下是子进程的信息：" << endl;
		//cout << "进程ID pi.dwProcessID: " << pi.dwProcessId << endl;
		//cout << "线程ID pi.dwThreadID : " << pi.dwThreadId << endl;
		// 等待知道子进程退出... 
		WaitForSingleObject(pi.hProcess, INFINITE);// 检测进程是否停止 
												   // WaitForSingleObject()函数检查对象的状态，如果是未确定的则等待至超时 
												   // 子进程退出 
		/*cout << "子进程已经退出" << endl;*/
		//获取子进程的返回值
		GetExitCodeProcess(pi.hProcess, &returnCode);
		/*std::cout << "process return code:" << returnCode << std::endl;*/
		// 关闭进程和句柄 
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		if (returnCode == 0)
			return 0;
	} while (true);				// 如果进程退出就再次执行方法 
}

