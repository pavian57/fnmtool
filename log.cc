#ifndef IS_OLDGCC
#include <iostream>
#else
#include <iostream.h>
#endif
#ifdef __GNUC__
#include <cstdio>
#else
#include <stdio.h>
#endif
#include <time.h>
#include "log.h"
#include "version.h"

using namespace std;
int CLog::intro()
{
	time(&tm);
	dt=gmtime(&tm);
        fprintf(f_log, "---------- %i.%i.%i %s %s\n", dt->tm_mday+1, dt->tm_mon, dt->tm_year+1900, PRGNAME, VERSION);
	return 0;
}

int CLog::intro(string s_Prg)
{
	time(&tm);
	dt=gmtime(&tm);
	if (!s_Prg.find("ftn2rfc"))
    fprintf(f_log, "---------- %i.%i.%i %s %s\n", dt->tm_mday+1, dt->tm_mon, dt->tm_year+1900, PRGNAME, VERSION);
  else
    fprintf(f_log, "---------- %i.%i.%i %s %s\n", dt->tm_mday+1, dt->tm_mon, dt->tm_year+1900,RFC2FTN ,RFC2FTNVERSION ); 
	return 0;
}


int CLog::outro()
{
        fprintf(f_log, "----------\n");
	return 0;
}

CLog::CLog()
{
	f_log=fopen("/var/log/fnmtool.log", "a");
}

CLog::CLog(string s_File)
{
	f_log=fopen(s_File.c_str(), "a");
}

CLog::~CLog()
{
	if (!noclose) fclose(f_log);
}

int CLog::add(int type, string str)
{
	time(&tm);
	dt=gmtime(&tm);
	fprintf(f_log, "%i %02u:%02u:%02u %s\n", type, dt->tm_hour+1, dt->tm_min, dt->tm_sec, str.c_str());
	return 0;
}
