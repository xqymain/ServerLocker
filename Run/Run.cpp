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
			fFile = fopen(fileName, "w");
	}
	now = time(0);
	fprintf(fFile, "%d[%s] :Started SLDaemon.exe.",now,runname);
	now = time(0);
	fprintf(fFile, "%d[%s] :Return 0.", now, runname);
    return 0;
}

