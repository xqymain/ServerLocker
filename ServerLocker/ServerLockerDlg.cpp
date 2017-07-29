
// ServerLockerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ServerLocker.h"
#include "ServerLockerDlg.h"
#include "afxdialogex.h"
#include <iostream>
#include <windows.h>
#include "thirdparty/openssl/sha.h"
#include <string>

#pragma comment(lib,"libcrypto64MT.lib")
#pragma comment(lib,"libssl64MT.lib")

using namespace std;

#pragma warning(disable:4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Declare global variables
CBrush m_bkbrush;
int userstatus = 0;    // Unlocked=0 Locked=1
CString SetPassword;
CString ConfirmPassword;
CString UnlockPassword;
std::wstring strValue;
int ru=0;
char *ch;
char content[256];      // Query the contents of the registry key
DWORD dwType = REG_SZ;  // Define Data Type
DWORD dwLength = 256;
struct HKEY__*RootKey;  // Registry Key name
TCHAR *SubKey;          // To open the address of the registry key
TCHAR *KeyName;         // To Set the name of the Item
TCHAR *ValueName;       // To Set the name of the Value
LPBYTE SetContent_S;    // String Type
CRect rct;              // Control the Screen Area
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

// The CAboutDlg dialog box for the application "About" menu item

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog box data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Support

// Achieve
protected:
	DECLARE_MESSAGE_MAP()
};


CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CServerLockerDlg Dialog

CServerLockerDlg::CServerLockerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CServerLockerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerLockerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CServerLockerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SETLOCK, &CServerLockerDlg::OnBnClickedSetlock)
	ON_BN_CLICKED(IDC_EXITSYSTEM, &CServerLockerDlg::OnBnClickedExitsystem)
END_MESSAGE_MAP()

// CServerLockerDlg Message handler


BOOL CServerLockerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add the "About ..." menu item to the system menu.

	// IDM_ABOUTBOX Must be within the scope of the system command.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog box. When the application main window is not a dialog box, the frame will automatically
	//  Do this 
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add additional initialization code here
	
	// Raise permissions

	HANDLE hToken;
	// Open access token for the current process
	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		return FALSE;
	}
	TOKEN_PRIVILEGES tkp;
	// View current process permissions

	if(!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid))
	{
		return FALSE;
	}
	// Modify privilege array
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Modify access token
	if (AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, 0))
	{
		char regname[] = "Software\\ServerLockToolsPs";
		char ValueName[] = "InitPassword";
		HKEY hKey= HKEY_LOCAL_MACHINE;
		HKEY subKey,subCKey;
		BYTE ValueData[64];
		DWORD Buffer;
		// Open Registry    
		if(RegOpenKeyEx(hKey, regname, 0, KEY_ALL_ACCESS, &subKey) != ERROR_SUCCESS)
		{
			MessageBox("Insufficient permissions to open the registry!", "Error", MB_ICONERROR);
			OnOK();
		}
		else
		{
			// Read the original key       
			Buffer = sizeof(ValueData);
			if (RegQueryValueEx(hKey, ValueName, 0, NULL, ValueData, &Buffer) != ERROR_SUCCESS)
			{
				// Not Found      
				// Create a new SZ with a value of none      
				if (RegCreateKey(hKey, regname, &subCKey) != ERROR_SUCCESS)
				{
					MessageBoxEx(NULL, "Can not create new registry value!", "Error", MB_OK | MB_ICONERROR, NULL);
					OnOK();
				}
				else
				{
					// SUCCESS
					char *temp = "";
					if (RegSetValueEx(subCKey, ValueName, 0, REG_SZ, (const unsigned char *)temp, sizeof(temp)) != ERROR_SUCCESS)
					{
						MessageBoxEx(NULL, "unknown error(1)", "Error", MB_OK | MB_ICONERROR, NULL);
						OnOK();
					}
				}
				ru = 1;
				RegCloseKey(hKey);
			}
			else
			{
			    //Found       
			//	//改变值
			//	DWORD temp = 0;
			//	if (RegSetValueEx(hKey, ValueName, 0, REG_DWORD, (LPBYTE)&temp, sizeof(DWORD)) != ERROR_SUCCESS)
			//	{
			//		MessageBox("无法更改注册表值！", "错误", MB_ICONERROR);
			//		RegCloseKey(hKey);
			//		OnOK();
		    //}
				HookLoad();    // Load HOOK
				ShowContent(hKey, regname, ValueName);
				string stemp = sha512(content);
				ConfirmPassword = SetPassword = stemp.c_str();
				SetDlgItemText(IDC_MESSAGE, "Due to last unlocked\n Please verify your last password!");
				SendDlgItemMessage(IDC_SET, EM_SETREADONLY, 1);
				SendDlgItemMessage(IDC_SETAGIN, EM_SETREADONLY, 1);
				SendDlgItemMessage(IDC_UNLOCK, EM_SETREADONLY, 0);
				SetDlgItemText(IDC_SETLOCK, "System Unlock");
				SetDlgItemText(IDC_SET, "");
				SetDlgItemText(IDC_SETAGIN, "");
				GetDlgItem(IDC_EXITSYSTEM)->EnableWindow(false);
				userstatus = 1;
				GetWindowRect(rct); // Relative to the upper left corner of the screen position
									// The mouse is limited to a rectangular area on the screen
				ClipCursor(rct);
				// Write the new system settings to the user profile
				SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, true, 0, SPIF_UPDATEINIFILE);
				// Operation shell function implementation
				::ShowWindow(::FindWindow("Shell_TrayWnd", NULL), SW_HIDE);
				GetDlgItem(IDC_UNLOCK)->SetFocus();
				//HANDLE hThread = CreateThread(NULL, 0, Fun, NULL, 0, NULL);
				return TRUE;
			}
		}
	}
	else
	{
		MessageBox("You aren't an administrator!", "Error", MB_OK|MB_ICONERROR);
		return FALSE;
		OnOK();
	}

	////重启
	//if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0))
	//{
	//	return FALSE;
	//}
	// Disable the use of controls
	SendDlgItemMessage(IDC_UNLOCK, EM_SETREADONLY, 1);
	// Change the child window
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CServerLockerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

//  If you add a minimize button to a dialog box, you need the following code
//  To draw the icon. For MFC applications that use the Document / View model,
//  This will be done automatically by the framework.

void CServerLockerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Used to draw the device context

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Make the icon centered in the work area rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to get the cursor when the user drags the minimized window
// to show.
HCURSOR CServerLockerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CServerLockerDlg::OnBnClickedSetlock()
{
	// TODO:Add the lock function handler code here
	if (userstatus == 0)
	{
		GetDlgItemText(IDC_SET, SetPassword); // The password entered by the user is stored directly into SetPassword
		GetDlgItemText(IDC_SETAGIN, ConfirmPassword);// Store the confirmation password entered by the user to ConfirmPassword
		if (SetPassword != ConfirmPassword)
		{
			SetDlgItemText(IDC_MESSAGE, "Two passwords are inconsistent!\nCan not be successfully locked!");
			SetDlgItemText(IDC_SET, "");
			SetDlgItemText(IDC_SETAGIN, "");

			// Position the cursor directly into the input IDC_SET control
			GetDlgItem(IDC_SET)->SetFocus();
			return;
		}
		if (SetPassword == "")// To determine whether it is empty password
		{
			SetDlgItemText(IDC_MESSAGE, "Password can not be blank!\nWas it intentional?");
			// Position the cursor directly into the input IDC_SET control
			GetDlgItem(IDC_SET)->SetFocus();
			return;
		}
		char *temp = SetPassword.GetBuffer(SetPassword.GetLength());
		string stemp = sha512(temp);
		ConfirmPassword = SetPassword = stemp.c_str();
		const char *ctemp = stemp.c_str();
		strcpy(temp, ctemp);
		char regname[] = "Software\\ServerLockToolsPs";
		char ValueName[] = "InitPassword";
		HKEY hKey = HKEY_LOCAL_MACHINE;
		if (RegOpenKeyEx(hKey, regname, 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
		{
			if (RegCreateKeyEx(hKey, regname, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS)
			{
				MessageBox("Insufficient permissions to open the registry!", "Error", MB_ICONERROR);
				OnOK();
			}
		}
		else
		{
			if (RegSetValueEx(hKey, ValueName, 0, REG_SZ, (const unsigned char *)temp, sizeof(temp)) != ERROR_SUCCESS)
			{
				MessageBox("Can not change the registry value!", "Error", MB_ICONERROR);
				RegCloseKey(hKey);
				OnOK();
			}
		}
		SetDlgItemText(IDC_MESSAGE, "Operating system has been successfully locked!\nPlease come back early!");
		SendDlgItemMessage(IDC_SET, EM_SETREADONLY, 1); 
		SendDlgItemMessage(IDC_SETAGIN, EM_SETREADONLY, 1);
		SendDlgItemMessage(IDC_UNLOCK, EM_SETREADONLY, 0);
		SetDlgItemText(IDC_SETLOCK, "System Unlock");
		SetDlgItemText(IDC_SET, "");
		SetDlgItemText(IDC_SETAGIN, "");
		GetDlgItem(IDC_EXITSYSTEM)->EnableWindow(false);
		userstatus = 1;
		GetWindowRect(rct); // Relative to the upper left corner of the screen position
		// Limit the mouse to a rectangular area on the screen
		ClipCursor(rct);
		// Write the new system settings to the user profile
		SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, true, 0, SPIF_UPDATEINIFILE);
		// Operation shell function implementation
		::ShowWindow(::FindWindow("Shell_TrayWnd", NULL), SW_HIDE);
		GetDlgItem(IDC_UNLOCK)->SetFocus();
		return;
	}

	if (userstatus == 1)   //Unlock
	{
		GetDlgItemText(IDC_UNLOCK, UnlockPassword);
		char *temp = UnlockPassword.GetBuffer(UnlockPassword.GetLength());
		string stemp = sha512(temp);
		UnlockPassword = stemp.c_str();
		if (SetPassword != UnlockPassword)
		{
			SetDlgItemText(IDC_MESSAGE, "Unlock password input error!\nUnable to unlock!");
			SetDlgItemText(IDC_UNLOCK, "");
			GetDlgItem(IDC_UNLOCK)->SetFocus(); // Cursor positioning
			return;
		}
		if (SetPassword == "")// To determine whether it is empty password
		{
			SetDlgItemText(IDC_MESSAGE, "Unlock password can not be empty!\nIs this intentional?");
			// Position the cursor directly into the input IDC_UNLOCK control
			GetDlgItem(IDC_UNLOCK)->SetFocus();
			return;
		}
		SetDlgItemText(IDC_MESSAGE, "Unlock success!\nDo not I help?");
		SendDlgItemMessage(IDC_SET, EM_SETREADONLY, 0);
		SendDlgItemMessage(IDC_SETAGIN, EM_SETREADONLY, 0);
		SendDlgItemMessage(IDC_UNLOCK, EM_SETREADONLY, 1);
		SetDlgItemText(IDC_SETLOCK, "Achieve lock");
		SetDlgItemText(IDC_UNLOCK, "");
		GetDlgItem(IDC_EXITSYSTEM)->EnableWindow(true);
		userstatus = 0;
		ClipCursor(NULL);
		SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, false, 0,SPIF_UPDATEINIFILE);
		::ShowWindow(::FindWindow("Shell_TrayWnd", NULL), SW_SHOW);

	}
}
BOOL CServerLockerDlg::PreTranslateMessage(MSG * pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		// Shield the specified button 
		switch (pMsg->wParam)
		{
		case VK_ESCAPE:
			return FALSE;
		case VK_RETURN:
			return FALSE;
		default:
			break;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
void CServerLockerDlg::OnBnClickedExitsystem()
{
	OnOK();
}
int ShowContent(struct HKEY__*ReRootKey, TCHAR *ReSubKey, TCHAR *ReValueName)
{
	HKEY hKey = HKEY_LOCAL_MACHINE;
	int i = 0; // Operation results:0==succeed
	if (RegOpenKeyEx(hKey, ReSubKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hKey, ReValueName, NULL, &dwType, (unsigned char *)content, &dwLength) != ERROR_SUCCESS)
		{
			AfxMessageBox("Error: Can not query the relevant registry information");
			i = 1;
		}
		RegCloseKey(hKey);
	}
	else
	{
		AfxMessageBox("Error: unable to open the relevant hKey");
		i = 1;
	}
	return i;
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
void CServerLockerDlg::OnClose()
{
	// TODO: Add the message handler code here and / or call the default value
	HookUnload();    // Exit the window to uninstall HOOK
	CDialogEx::OnClose();
}

void CServerLockerDlg::HookLoad()
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
void CServerLockerDlg::HookUnload()
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

string sha512(const string str)
{
	char buf[129];
	unsigned char hash[SHA512_DIGEST_LENGTH];
	SHA512_CTX sha512;
	SHA512_Init(&sha512);
	SHA512_Update(&sha512, str.c_str(), str.size());
	SHA512_Final(hash, &sha512);
	string NewString = "";
	for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
	{
		sprintf(buf, "%02x", hash[i]);
		NewString = NewString + buf;
	}
	//CString ctemp;
	//ctemp = NewString.c_str();
	//MessageBox(NULL, ctemp, "Message", MB_OK);
	return NewString;
}
