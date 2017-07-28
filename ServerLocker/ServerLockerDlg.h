// ServerLockToolsPsDlg.h : 头文件
//

#pragma once


// CServerLockToolsPsDlg 对话框
class CServerLockToolsPsDlg : public CDialogEx
{
// 构造
public:
	CServerLockToolsPsDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SERVERLOCKTOOLSPS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	HINSTANCE m_hinstHookDll;    //    MonitorDll的实例句柄
	void HookLoad();            //    加载HOOK            
	void HookUnload();            //    卸载HOOK

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();        //    关闭程序的时候卸载DLL !!!!!
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSetlock();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedExitsystem();
};
