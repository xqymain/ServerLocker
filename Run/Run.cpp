// Run.cpp : 定义控制台应用程序的入口点。
// For run ServerLocker System
// Pass TEST
//



#include "stdafx.h"



// Hide command prompt

#pragma comment(linker,"/subsystem:\"windows\"  /entry:\"mainCRTStartup\"" )



using namespace std;

FILE *fFile;
time_t now;
char *runname = "Run";
int li;
char *lch;
struct tm *newtime;
char tmpbuf[128];

void LogCreate()
{
	char *fileName = "status.log";
	ifstream fin(fileName);
	if (!fin)
	{
		ofstream fout(fileName);
		ifstream finn(fileName);
		if (!finn)
		{
			MessageBox(NULL, _T("Can not create a new log file,Return!"), _T("Error!"), MB_ICONERROR);
			exit(0);
		}
		fFile = fopen(fileName, "w");
	}
	fFile = fopen(fileName, "w");
}

void ThreadLog()
{
	now = time(0);
	newtime = localtime(&now);
	strftime(tmpbuf, 128, "%D %H %M %S", newtime);
	if (li == 0)
		fprintf(fFile, "%s[%s] :%s\n", tmpbuf, runname, lch);
	if (li == 1)
		fprintf(fFile, "%s[%s] RUN::%s\n", tmpbuf, runname, lch);
	return;}

int main()
{
	LogCreate();
	STARTUPINFO si;
	DWORD returnCode;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	CString cmd;
	cmd.Format(L"SLDaemon.exe ServerLocker.exe");
	LPWSTR lpCmd = cmd.GetBuffer();
	// 创建子进程，判断是否执行成功 

	if (!CreateProcess(NULL, lpCmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))

	{
		HANDLE h; //线程句柄
		li = 0;
		lch = (char*)"Creating sub-thread failed,Return!";
		h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadLog, NULL, 1, 0); //创建子线程
		ResumeThread(h);  //启动子线程
		return -1;
	}
	now = time(0);
	newtime = localtime(&now);
	strftime(tmpbuf, 128,"%D %H %M %S", newtime);
	fprintf(fFile, "%s[%s] :Started SLDaemon.exe.\n", tmpbuf, runname);
	// 进程执行成功，打印进程信息 

	now = time(0);

	newtime = localtime(&now);
	strftime(tmpbuf, 128, "%D %H %M %S", newtime);
	fprintf(fFile, "%s[%s] :Process is successful and prints the process information.\n", tmpbuf,runname);
	//cout << "以下是子进程的信息：" << endl;

	now = time(0);

	newtime = localtime(&now);
	strftime(tmpbuf, 128, "%D %H %M %S", newtime);
	fprintf(fFile, "%s[%s] :Process ID pi.dwProcessID %Id.\n", tmpbuf, runname, pi.dwProcessId);
	//cout << "进程ID pi.dwProcessID: " << pi.dwProcessId << endl;

	now = time(0);

	newtime = localtime(&now);
	strftime(tmpbuf, 128, "%D %H %M %S", newtime);
	fprintf(fFile, "%s[%s] :dwThread ID pi.dwThreadId %Id.\n", tmpbuf, runname, pi.dwThreadId);
	//cout << "线程ID pi.dwThreadID : " << pi.dwThreadId << endl;

	now = time(0);

	newtime = localtime(&now);
	strftime(tmpbuf, 128, "%D %H %M %S", newtime);
	fprintf(fFile, "%s[%s] :Output is finished!\n", tmpbuf, runname);
	now = time(0);
	newtime = localtime(&now);
	strftime(tmpbuf, 128, "%D %H %M %S", newtime);
	fprintf(fFile, "%s[%s] :Return 0.\n", tmpbuf, runname);
	fclose(fFile);
    return 0;
}
