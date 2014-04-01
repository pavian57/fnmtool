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
#include <sys/stat.h>
#include <libgen.h>
#include <stdexcept>
//#include <cerrno>
#include "log.h"
#include "version.h"

using namespace std;
int CLog::intro()
{
	time(&tm);
	dt=gmtime(&tm);
	if (logopen) 
    fprintf(f_log, "---------- %i.%i.%i %s %s\n", dt->tm_mday+1, dt->tm_mon, dt->tm_year+1900, PRGNAME, VERSION);
  else 
    fprintf(stderr, "---------- %i.%i.%i %s %s\n", dt->tm_mday+1, dt->tm_mon, dt->tm_year+1900, PRGNAME, VERSION);  
	return 0;
}

int CLog::intro(string s_Prg)
{
	time(&tm);
	dt=gmtime(&tm);
	if (logopen) {
	  if (!s_Prg.find("ftn2rfc"))
      fprintf(f_log, "---------- %i.%i.%i %s %s\n", dt->tm_mday+1, dt->tm_mon, dt->tm_year+1900, PRGNAME, VERSION);
    else
      fprintf(f_log, "---------- %i.%i.%i %s %s\n", dt->tm_mday+1, dt->tm_mon, dt->tm_year+1900,RFC2FTN ,RFC2FTNVERSION ); 
  } else {
    if (!s_Prg.find("ftn2rfc"))
      fprintf(stderr, "---------- %i.%i.%i %s %s\n", dt->tm_mday+1, dt->tm_mon, dt->tm_year+1900, PRGNAME, VERSION);
    else
      fprintf(stderr, "---------- %i.%i.%i %s %s\n", dt->tm_mday+1, dt->tm_mon, dt->tm_year+1900,RFC2FTN ,RFC2FTNVERSION ); 
  }
	return 0;
}


int CLog::outro()
{
  if (logopen) 
    fprintf(f_log, "----------\n");
  else
    fprintf(stderr, "----------\n");
	return 0;
}

CLog::CLog()
{
  logopen = 0;
	f_log=fopen("/var/log/fnmtool.log", "a");

	if (f_log == NULL) {
	  cerr << "Open Log /var/log/fnmtool.log failed, trying alternative log in /tmp/" << endl;
    f_log=fopen("/tmp/fnmtool.log","a");
    if (f_log != NULL) {
      cerr << "Open Log /tmp/fnmtool.log succesfull" << endl;
      logopen = 1;     
    } else {
      cerr << "Open Log /tmp/fnmtool.log failed, logging to stderr" << endl;
    }
	} else {
    logopen = 1;	        
	}  
}

CLog::CLog(string s_File)
{
  logopen = 0;
	f_log=fopen(s_File.c_str(), "a");
	if (f_log == NULL) {
	  string newlogfile = "/tmp/" + s_File.substr( s_File.find_last_of( '/' ) +1); 
	  cerr << "Open Log " << newlogfile << " failed, trying alternative log in /tmp/" << endl;
    f_log=fopen(newlogfile.c_str(),"a");
    if (f_log != NULL) {
      cerr << "Open Log " <<  newlogfile << " succesfull" << endl;    
      logopen = 1;     
    } else {
      cerr << "Open Log " <<  newlogfile << " failed, loggin to stderr" << endl;         
    }
	} else {
    logopen = 1;	        
	} 
}

CLog::~CLog()
{
	if (!noclose && logopen) fclose(f_log);
}

int CLog::add(int type, string str)
{
	time(&tm);
	dt=gmtime(&tm);
	if (logopen)
	  fprintf(f_log, "%i %02u:%02u:%02u %s\n", type, dt->tm_hour+1, dt->tm_min, dt->tm_sec, str.c_str());
  else
    fprintf(stderr, "%i %02u:%02u:%02u %s\n", type, dt->tm_hour+1, dt->tm_min, dt->tm_sec, str.c_str());
	return 0;
}
