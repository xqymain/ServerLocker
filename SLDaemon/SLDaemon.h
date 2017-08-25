// ServerLockerDlg.h : 头文件
//

#include <windows.h>

#pragma once

class CDaemon
{
	// 构造
public:
	CDaemon(CWnd* pParent = NULL);	// 标准构造函数

protected:
	HWND m_hWnd;
	HINSTANCE m_hinstHookDll;    //    MonitorDll的实例句柄
	void HookLoad();             //    加载HOOK      
	BOOL installhook();
	void HookUnload();           //    卸载HOOK
};
