// ServerLockerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ServerLocker.h"
#include "ServerLockerDlg.h"
#include "afxdialogex.h"


using namespace std;

#pragma warning(disable:4996)

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif


// Declare global variables
CBrush m_bkbrush;
int userstatus = 0;    // Unlocked=0 Locked=1
CString SetPassword;
CString ConfirmPassword;
CString UnlockPassword;
std::wstring strValue;
int ru = 0;
int attn = 0; // Number of attempts
int attv = 0;
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
FILE *fFile;
time_t now;
char *runname = "ServerLocker";

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
	now = time(0);
	fprintf(fFile, "%Id[%s] RUN::CAboutDlg()", now, runname);
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	now = time(0);
	fprintf(fFile, "%Id[%s] RUN::DoDataExchange()", now, runname);
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
	// Check Log File
	ifstream fin("status.log");

	// Build Log File
	if (!fin)
	{
		char *fileName = "status.log";
		ifstream fin("status.log");
		if (!fin)
		{
			MessageBox("Can not create a new log file,Return!", "Error!", MB_ICONERROR);
			return -1;
		}
		fFile = fopen(fileName, "w");
	}
	fFile = fopen(fileName, "w");
	now = time(0);
	fprintf(fFile, "%Id[%s] RUN::OnInitDialog()", now, runname);
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
	now = time(0);
	fprintf(fFile, "%Id[%s] :Successful access to administrator privileges.", now, runname);
	// Modify access token
	if (AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, 0))
	{
		char regname[] = "SOFTWARE\\ServerLockToolsPs";
		char ValueName[] = "InitPassword";
		HKEY hKey= HKEY_LOCAL_MACHINE;
		HKEY subKey,subCKey;
		BYTE ValueData[64];
		DWORD Buffer;
		// Open Registry    
		if(RegOpenKeyEx(hKey, regname, 0, KEY_ALL_ACCESS, &subKey) != ERROR_SUCCESS)
		{
			auto backnum = RegOpenKeyEx(hKey, regname, 0, KEY_ALL_ACCESS, &subKey);
			if (backnum)
			{
				CString output;
				output.Format("You do not have permission to open the registry! Return code:%Id .", backnum);
				MessageBox(output, "Error", MB_ICONERROR);
				now = time(0);
				fprintf(fFile, "%Id[%s] :%s", now, runname,output.GetBuffer(0));
				OnOK();
			}
			else {
				now = time(0);
				fprintf(fFile, "%Id[%s] :Retry to open registry...", now, runname);
				goto success;
			}
		}
		else
		{
		success:
			now = time(0);
			fprintf(fFile, "%Id[%s] :Successfully open registry.", now, runname);
			// Read the original key       
			Buffer = sizeof(ValueData);
			if (RegQueryValueEx(hKey, ValueName, 0, NULL, ValueData, &Buffer) != ERROR_SUCCESS)
			{
				// Not Found      
				now = time(0);
				fprintf(fFile, "%Id[%s] :Not Found registry value:%s .", now, runname, ValueName);
				// Create a new SZ with a value of none
				now = time(0);
				fprintf(fFile, "%Id[%s] :Creating a new registry value:%s ...", now, runname,ValueName);
				if (RegCreateKey(hKey, regname, &subCKey) != ERROR_SUCCESS)
				{
					now = time(0);
					fprintf(fFile, "%Id[%s] :Failed to create registry key:%s Return!", now, runname, ValueName);
					MessageBox("Can not create new registry value!", "Error", MB_OK | MB_ICONERROR);
					fclose(fFile);
					OnOK();
				}
				else
				{
					// SUCCESS
					char *temp = "";
					if (RegSetValueEx(subCKey, ValueName, 0, REG_SZ, (const unsigned char *)temp, sizeof(temp)) != ERROR_SUCCESS)
					{
						MessageBox("unknown error(1)", "Error", MB_OK | MB_ICONERROR);
						now = time(0);
						fprintf(fFile, "%Id[%s] :Unknown error,Return!", now, runname);
						fclose(fFile);
						OnOK();
					}
				}
				ru = 1;
				RegCloseKey(hKey);
			}
			else
			{
			    //Found       
			//	// Modify Value
			//	DWORD temp = 0;
			//	if (RegSetValueEx(hKey, ValueName, 0, REG_DWORD, (LPBYTE)&temp, sizeof(DWORD)) != ERROR_SUCCESS)
			//	{
			//		MessageBox("Can't modify Regedit Value!", "ERROR!", MB_ICONERROR);
			//		RegCloseKey(hKey);
			//		OnOK();
		    //}
				now = time(0);
				fprintf(fFile, "%Id[%s] :Found registry value,Lock!", now, runname);
				fprintf(fFile, "%Id[%s] :System is Locked!", now, runname);
				ShowContent(hKey, regname, ValueName);
				string stemp = content;
				unsigned char *md5_sha512_passwd = NULL;
				ConfirmPassword = SetPassword = stemp.c_str();
				MD5((const unsigned char*)stemp.c_str(), 65, md5_sha512_passwd);
				now = time(0);
				fprintf(fFile, "%Id[%s] :Password-SHA512-MD5:%s", now, runname, md5_sha512_passwd);
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
				now = time(0);
				fprintf(fFile, "%Id[%s] :L Shell_TrayWnd Status bar modified successfully.", now, runname);
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
		now = time(0);
		fprintf(fFile, "%Id[%s] Do not have administrator privileges,Return!", now, runname);
		fclose(fFile);
		return FALSE;
		OnOK();
	}

	////Reboot
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
	now = time(0);
	fprintf(fFile, "%Id[%s] RUN::OnSysCommand()", now, runname);
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
	now = time(0);
	fprintf(fFile, "%Id[%s] RUN::OnPaint()", now, runname);
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
	now = time(0);
	fprintf(fFile, "%Id[%s] RUN::OnQueryDragIcon()", now, runname);
	return static_cast<HCURSOR>(m_hIcon);
}



void CServerLockerDlg::OnBnClickedSetlock()
{
	now = time(0);
	fprintf(fFile, "%Id[%s] RUN::OnBnClickedSetlock()", now, runname);
	// TODO: Add the lock function handler code here
	if (userstatus == 0)
	{
		GetDlgItemText(IDC_SET, SetPassword); // The password entered by the user is stored directly into SetPassword
		GetDlgItemText(IDC_SETAGIN, ConfirmPassword);// Store the confirmation password entered by the user to ConfirmPassword
		if (SetPassword != ConfirmPassword)
		{
			SetDlgItemText(IDC_MESSAGE, "Two passwords are inconsistent!\nCan not be successfully locked!");
			SetDlgItemText(IDC_SET, "");
			SetDlgItemText(IDC_SETAGIN, "");
			now = time(0);
			fprintf(fFile, "%Id[%s] :Two passwords are inconsistent.", now, runname);
			// Position the cursor directly into the input IDC_SET control
			GetDlgItem(IDC_SET)->SetFocus();
			return;
		}
		if (SetPassword == "")// To determine whether it is empty password
		{
			SetDlgItemText(IDC_MESSAGE, "Password can not be blank!\nWas it intentional?");
			now = time(0);
			fprintf(fFile, "%Id[%s] :Blank Password.", now, runname);
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
				now = time(0);
				fprintf(fFile, "%Id[%s] :Insufficient permissions to open the registry,Return!", now, runname);
				fclose(fFile);
				OnOK();
			}
		}
		else
		{
			if (RegSetValueEx(hKey, ValueName, 0, REG_SZ, (const unsigned char *)temp, sizeof(temp)) != ERROR_SUCCESS)
			{
				MessageBox("Can not change the registry value!", "Error", MB_ICONERROR);
				now = time(0);
				fprintf(fFile, "%Id[%s] :Can not change the registry value,Return!", now, runname);
				fclose(fFile);
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
		now = time(0);
		fprintf(fFile, "%Id[%s] :L Shell_TrayWnd Status bar modified successfully.", now, runname);
		GetDlgItem(IDC_UNLOCK)->SetFocus();
		return;
	}

	if (userstatus == 1)   //Unlock
	{
		now = time(0);
		fprintf(fFile, "%Id[%s] :Has entered the wait unlock mode.", now, runname);
		if(attn == 5 && attv == 0)
	 	{
		    SetDlgItemText(IDC_MESSAGE, "Try too many times, please try again later!");
		    SendDlgItemMessage(IDC_UNLOCK, EM_SETREADONLY, 0);
		    fprintf(fFile, "%s[%s] :The number of attempts to unlock exceeds the threshold. Lock five minutes.\n", tmpbuf, runname);
		    Sleep(300000);
                    SendDlgItemMessage(IDC_UNLOCK, EM_SETREADONLY, 1);
		    now = time(0);
 		    newtime = localtime(&now);
		    strftime(tmpbuf, 128, "%D %H %M %S", newtime);
                    
		    attv++;
			
		    fprintf(fFile, "%s[%s] :Modfied ATTV count.\n", tmpbuf, runname);
		}
		else if(attn == 10 && attv==1)
		{
	            SetDlgItemText(IDC_MESSAGE, "Try too many times, please try again later!");
		    SendDlgItemMessage(IDC_UNLOCK, EM_SETREADONLY, 0);
		    fprintf(fFile, "%s[%s] :The number of attempts to unlock exceeds the threshold. Lock thirty minutes.\n", tmpbuf, runname);
		    Sleep(1800000);
                    SendDlgItemMessage(IDC_UNLOCK, EM_SETREADONLY, 1);
		    now = time(0);
 		    newtime = localtime(&now);
		    strftime(tmpbuf, 128, "%D %H %M %S", newtime);
                    
		    attv++;
			
		    fprintf(fFile, "%s[%s] :Modfied ATTV count.\n", tmpbuf, runname);
		}
		else if(attn > 10 && attv >= 2)
		{
	            SetDlgItemText(IDC_MESSAGE, "Try too many times, please try again later!");
		    SendDlgItemMessage(IDC_UNLOCK, EM_SETREADONLY, 0);
		    fprintf(fFile, "%s[%s] :The number of attempts to unlock exceeds the threshold. Lock three hours.\n", tmpbuf, runname);
		    Sleep(10800000);
                    SendDlgItemMessage(IDC_UNLOCK, EM_SETREADONLY, 1);
		    now = time(0);
 		    newtime = localtime(&now);
		    strftime(tmpbuf, 128, "%D %H %M %S", newtime);
                    
		    attv++;
			
		    fprintf(fFile, "%s[%s] :Modfied ATTV count.\n", tmpbuf, runname);
		}
		GetDlgItemText(IDC_UNLOCK, UnlockPassword);
		char *temp = UnlockPassword.GetBuffer(UnlockPassword.GetLength());
		string stemp = sha512(temp);
		UnlockPassword = stemp.c_str();
		if (SetPassword != UnlockPassword)
		{
			attn++;
			now = time(0);
 		        newtime = localtime(&now);
		        strftime(tmpbuf, 128, "%D %H %M %S", newtime);
			SetDlgItemText(IDC_MESSAGE, "Unlock password input error!\nUnable to unlock!");
			SetDlgItemText(IDC_UNLOCK, "");
			fprintf(fFile, "%s[%s] :Unlock Password wrong, modfied ATTN count.\n", tmpbuf, runname);
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
		now = time(0);
		fprintf(fFile, "%Id[%s] :System is Unlocked.", now, runname);
		userstatus = 0;
		ClipCursor(NULL);
		::ShowWindow(::FindWindow("Shell_TrayWnd", NULL), SW_SHOW);
		now = time(0);
		fprintf(fFile, "%Id[%s] :U Shell_TrayWnd Status bar modified successfully.", now, runname);
	}
}
BOOL CServerLockerDlg::PreTranslateMessage(MSG * pMsg)
{
	now = time(0);
	fprintf(fFile, "%Id[%s] RUN::PreTranslateMessage()", now, runname);
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
	now = time(0);
	fprintf(fFile, "%Id[%s] RUN::OnBnClickedExitsystem()", now, runname);
	OnOK();
}
int CServerLockerDlg::ShowContent(struct HKEY__*ReRootKey, TCHAR *ReSubKey, TCHAR *ReValueName)
{
	now = time(0);
	fprintf(fFile, "%Id[%s] RUN::ShowContent()", now, runname);
	HKEY hKey = HKEY_LOCAL_MACHINE;
	int i = 0; // Operation results : 0 == succeed
	if (RegOpenKeyEx(hKey, ReSubKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hKey, ReValueName, NULL, &dwType, (unsigned char *)content, &dwLength) != ERROR_SUCCESS)
		{
			AfxMessageBox("Error: Can not query the relevant registry information");
			now = time(0);
			fprintf(fFile, "%Id[%s:%s] :Can not query the relevant registry information.", now, runname,"ShowContent");
			i = 1;
		}
		RegCloseKey(hKey);
	}
	else
	{
		AfxMessageBox("Error: unable to open the relevant hKey");
		now = time(0);
		fprintf(fFile, "%Id[%s:%s] :unable to open the relevant hKey.", now, runname,"ShowContent");
		i = 1;
	}
	return i;
}
BOOL CServerLockerDlg::installhook()
{
	now = time(0);
	fprintf(fFile, "%Id[%s] RUN::installhook()", now, runname);
	HINSTANCE hins = AfxGetInstanceHandle();
	HHOOK Hook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardProc, hins, 0);
	now = time(0);
	fprintf(fFile, "%Id[%s] :Keyboard hook mounted successfully.", now, runname);
	return (BOOL)Hook;
}
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	now = time(0);
	fprintf(fFile, "%Id[%s] RUN::KeyboardProc()", now, runname);
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
	now = time(0);
	fprintf(fFile, "%Id[%s] RUN::OnClose()", now, runname);
	// TODO: Add the message handler code here and / or call the default value
	fclose(fFile);
	CDialogEx::OnClose();
}

string CServerLockerDlg::sha512(const string str)
{
	now = time(0);
	fprintf(fFile, "%Id[%s] RUN::sha512()", now, runname);
	char buf[64]; 
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
	now = time(0);
	fprintf(fFile, "%Id[%s] :SHA512 hash calculation is complete.", now, runname);
	return NewString;
}



// Thanks for your read!
