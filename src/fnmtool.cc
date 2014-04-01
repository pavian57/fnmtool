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
#include <string>

#include "area.h"
#include "msg.h"
#include "action.h"
#include "config.h"
#include "log.h"
#include "global.h"
#include "version.h"

void printhelp()
{
  cerr << PRGNAME << " " << VERSION << endl << endl;
  cerr << "Usage: fnmtool [options]" << endl;
  cerr << "fnmtool -s           -- run silent" << endl;
  cerr << "fnmtool -d           -- with debug output" << endl;
  cerr << "fnmtool -h -? --help -- this screen" << endl << endl;  
  exit(1);
}

int main(int argc, char** argv)
{
	int result = 0;
	cfg = new CConfig;
	for (int i = 1; i < argc; i++) {
	  if (strcmp(argv[1],"-h")==0 || strcmp(argv[1],"-?")==0 || strcmp(argv[1],"--help")==0)
     printhelp();
    if (strcmp(argv[i],"-s")==0)
    	cfg->silent = 1;
    if (strcmp(argv[i],"-d")==0)
      result = 1;    	
   }
	log = new CLog(cfg->s_Log);
	if (result) cfg->debug = 1;

 	if (!cfg->silent)	
		cerr << PRGNAME << " " << VERSION << endl;
	initApi();
	log->intro();
	for (unsigned int i=0;i<cfg->S_Scandir.size();i++)
	{
		result=cfg->S_Scandir[i].A_Area.Open();
		if (result==-1)
		{
		if (!cfg->silent)
		  cerr << "could not open area " << cfg->S_Scandir[i].A_Area.s_Path << endl;
			string logstr="could not open area ";
			logstr+=cfg->S_Scandir[i].A_Area.s_Path;
			logstr+="!";
			log->add(3, logstr);
			continue;
		}
		else 
		{
		if (!cfg->silent) 
		  cout  << "Scanning " << cfg->S_Scandir[i].A_Area.s_Path  << endl;
		   cfg->S_Scandir[i].A_Area.Scan(cfg->O_Op, cfg->A_Action, cfg->S_Scandir[i].firstMask, cfg->S_Scandir[i].lastMask);
		   cfg->S_Scandir[i].A_Area.Close();
		}
	}
	closeApi();
	delete cfg;
	log->add(9, "finishing");
	log->outro();
	delete log;
	return 0;
}
