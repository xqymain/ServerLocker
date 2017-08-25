// SLDaemon.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "SLDaemon.h"

HINSTANCE m_hinstHookDll;    //    MonitorDll的实例句柄
void HookLoad();            //    加载HOOK      
BOOL installhook();
void HookUnload();            //    卸载HOOK

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

BOOL installhook()
{
	HINSTANCE hins = AfxGetInstanceHandle();
	HHOOK Hook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardProc, hins, 0);
	return (BOOL)Hook;
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	BOOL Discard = FALSE;

	if (nCode == HC_ACTION)
	{
		switch (wParam)
		{
		case WM_KEYDOWN:  case WM_SYSKEYDOWN:
		case WM_KEYUP:    case WM_SYSKEYUP:
			PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
			Discard =
				//Tab + Alt
				((p->vkCode == VK_TAB) && ((p->flags & LLKHF_ALTDOWN) != 0))
				//Esc + Alt
				|| ((p->vkCode == VK_ESCAPE) && ((p->flags & LLKHF_ALTDOWN) != 0))
				//Esc + Ctrl
				|| ((p->vkCode == VK_ESCAPE) && ((GetKeyState(VK_CONTROL) & 0x8000) != 0))
				//Ctrl + Space
				|| (((GetKeyState(VK_CONTROL) & 0x8000) != 0) && (p->vkCode == VK_SPACE));
			break;
		}
	}
	return(Discard ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam));
}

void HookLoad()
{
	m_hinstHookDll = LoadLibrary(_T("MonitorDll.dll"));
	CString loginfo;

	if (NULL == m_hinstHookDll)
	{
		loginfo.Format(_T("Load MonitorDll.dll failed with error code = [%d] "), GetLastError());
		AfxMessageBox(loginfo);
		return;
	}


	typedef BOOL(WINAPI* LoadMonitor)(HWND hwnd, DWORD dwProcessId);
	LoadMonitor loadMonitor = NULL;
	loadMonitor = (LoadMonitor)::GetProcAddress(m_hinstHookDll, "HookLoad");
	if (NULL == loadMonitor)
	{
		loginfo.Format(_T("Get function HookLoad failed with error code = [%d]"), GetLastError());
		AfxMessageBox(loginfo);
	}
	if (loadMonitor(m_hWnd, GetCurrentProcessId()))
	{
		loginfo.Format(_T("HOOK loaded successfully"));
		AfxMessageBox(loginfo);
	}
	else
	{
		loginfo.Format(_T("HOOK loading failed!"));
		AfxMessageBox(loginfo);
	}
}
void HookUnload()
{
	CString logInfo;
	if (m_hinstHookDll == NULL)
	{
		m_hinstHookDll = LoadLibrary(_T("MonitorDll.dll"));
		if (NULL == m_hinstHookDll)
		{
			logInfo.Format(_T("Load MonitorDll.dll failed with error code = [%d]"), GetLastError());
			AfxMessageBox(logInfo);
			return;
		}
	}

	typedef VOID(WINAPI* UnloadHook)();
	UnloadHook unloadHook = NULL;
	unloadHook = (UnloadHook)::GetProcAddress(m_hinstHookDll, "HookUnload");
	if (NULL == unloadHook)
	{
		logInfo.Format(_T("Get function HookLoad failed with error code = [%d]"), GetLastError());
		AfxMessageBox(logInfo);
		return;
	}

	unloadHook();

}
