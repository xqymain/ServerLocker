// ServerLockerDlg.h : 头文件
//

#include <windows.h>

#pragma once

HINSTANCE m_hinstHookDll;    //    MonitorDll的实例句柄
void HookLoad();            //    加载HOOK      
BOOL installhook();
void HookUnload();            //    卸载HOOK