// Run.cpp : 定义控制台应用程序的入口点。
// For run ServerLocker System
//

#include "stdafx.h"

#pragma comment(linker,"/subsystem:\"windows\"  /entry:\"mainCRTStartup\"" )

using namespace std;

FILE *fFile;
time_t now;
char *runname = "Run";

int main()
{
	system("SLDaemon.exe ServerLocker.exe");
	ifstream fin("status.log");
	if (!fin)
	{
		char *fileName = "status.log";
		ifstream fin("status.log");
		if (!fin)
		{
			MessageBox(_T("Can not create a new log file,Return!"), _T("Error!"), MB_ICONERROR);
			return -1;
		}
		fFile = fopen(fileName, "w");
	}
	now = time(0);
	fprintf(fFile, "%d[%s] :Started SLDaemon.exe.",now,runname);
	now = time(0);
	fprintf(fFile, "%d[%s] :Return 0.", now, runname);
	fclose(fFile);
    return 0;
}

