#include <string>
#include <iostream>
#ifdef __GNUC__
#include <cstring>
#include <cstdio>
#else
#include <string.h>
#include <stdio.h>
#endif
#include <ctype.h>
#include "config.h"
#include "strsep.h"
#include "mask.h"

using namespace std;	


bool CConfig::openConfig(string path)
{

	cfgFile=fopen(path.c_str(), "r");
	if (cfgFile==NULL) {
		return false;
	} else {
		return true;
	}
	return false; 
}

bool CConfig::closeConfig()
{
	if (cfgFile!=NULL) fclose(cfgFile);
	return true;
}

string CConfig::getLine()
{
	char *line;
	string order;
	unsigned char counter=0;
	line = new char[256];
	if (fgets(line, 256, cfgFile)==NULL) return "\n";
	for (;counter<strlen(line);counter++) if (line[counter]==92) line[counter]='\\';
	if (line[0]==' ') strseparate(&line, " ");
	line = strseparate(&line, "\n");
	order = line;
	delete [] line;
	return order;
}	

param CConfig::getParam(string line)
{
	string token;
	param parm;
	while (line[0]==' ') line.erase(0,1);
	if (line[line.size()]=='\n') line.erase(line.size(), 1);
	do 
	{
		token += tolower(line[0]);
		line.erase(0,1);
	} while (line[0]!=' ');

	while (line[0]==' ') line.erase(0,1);	
	parm.s_Token=token;
	parm.s_RestOfLine=line;
	for (unsigned int i=0;i<parm.s_Token.length();i++) parm.s_Token[i]=tolower(parm.s_Token[i]);
	return parm;
}

CScandir::CScandir(string s_Path)
{
	if (s_Path[0]=='$') {
    A_Area.i_type=MSGTYPE_SQUISH;
    s_Path.erase(0,1);
	}	else if (s_Path[0]=='!') {
    A_Area.i_type=MSGTYPE_JAM;
    s_Path.erase(0,1);
  } else 
    A_Area.i_type=MSGTYPE_SDM;
	A_Area.s_Path=s_Path;
}	

CScandir::~CScandir()
{
}

COperation::COperation(string s_MaskStr)
{
	CMask TempMask;
	TempMask=s_MaskStr;
	M_Mask=TempMask;
}


COperation::~COperation()
{
}

CAddresslist::CAddresslist(string s_AddrStr)
{
//  cout << s_AddrStr << endl;
	int pos = s_AddrStr.find(";");
  rfcAddr.assign(s_AddrStr,0,pos);
  s_AddrStr.erase(0,pos+1);
//  cout << s_AddrStr << endl;
  
  pos = s_AddrStr.find(";");
  fidoName.assign(s_AddrStr,0,pos);
  s_AddrStr.erase(0,pos+1);
//  cout << s_AddrStr << endl;
  
  F_From = const_cast<char*>(s_AddrStr.c_str());
    
//  cout << "rfcAddr=" << rfcAddr << ": fidoName=" << fidoName <<": Fidoaddresse=" << F_From << ":" << endl;
}


CAddresslist::~CAddresslist()
{
}


CConfig::CConfig(int dbg)
{
	param parm;
	string s_Line;
	string s_Token;
	int scn=-1, msk=-1, actn=-1, rfcfido=-1;
  s_CharsetFtn = "LATIN1";
  s_CharsetRfc = "UTF-8";
  debug = dbg;
	silent = 0;
	openConfig(CONFIGDIR);
if (debug) cerr << "Dump of configfile " << CONFIGDIR << endl;			
	while ((s_Line=getLine())!="\n") 
	{
		if (s_Line == "") continue;
		if (s_Line[0] == ';') continue;

		parm=getParam(s_Line);
		if (parm.s_Token=="home")
		{
			s_Home=parm.s_RestOfLine;
			F_Home=const_cast<char*>(s_Home.c_str());
if (debug) cerr << "home =" << s_Home << ": home=" << F_Home << ":" << endl;			
		}
		if (parm.s_Token=="netmail")
		{
			s_Netmail=parm.s_RestOfLine;
if (debug) cerr << "netmail=" << s_Netmail << ":" << endl;			
		}
		else if (parm.s_Token=="silent")
		{
			silent=atoi(parm.s_RestOfLine.c_str());
if (debug) cerr << "silent=" << silent << ":" << endl;			
		}
		else if (parm.s_Token=="debug")
		{
			if (!debug)
			debug=atoi(parm.s_RestOfLine.c_str());
if (debug) cerr << "debug=" << debug << ":" << endl;			
		}		
		else if (parm.s_Token=="log")
		{
			s_Log=parm.s_RestOfLine;
if (debug) cerr << "log=" << s_Log << ":" << endl;			
		}
		else if (parm.s_Token=="ftncharset")
		{
      s_CharsetFtn =parm.s_RestOfLine;
if (debug) cerr << "ftncharset=" << s_CharsetFtn << ":" << endl;			
		}
		else if (parm.s_Token=="rfccharset")
		{
      s_CharsetRfc = parm.s_RestOfLine;
if (debug) cerr << "rfccharset=" << s_CharsetRfc << ":" << endl;			
		}
		else if (parm.s_Token=="outbound")
		{
			s_Outbound=parm.s_RestOfLine;
if (debug) cerr << "outbound=" << s_Outbound << ":" << endl;			
		}
		else if (parm.s_Token=="inbound")
		{
			s_Inbound=parm.s_RestOfLine;
if (debug) cerr << "inbound=" << s_Inbound << ":" << endl;			
		}
		else if (parm.s_Token=="scandir")
		{
			scn++;
			CScandir scndr(parm.s_RestOfLine);
if (debug) cerr << "scandir=" << parm.s_RestOfLine << ":" << endl;			
			S_Scandir.push_back(scndr);
			if (scn==0) 
				S_Scandir[scn].firstMask=0;
			else
			{
				S_Scandir[scn].firstMask=msk+1;
				S_Scandir[scn-1].lastMask=msk;
			}
		}
		else if (parm.s_Token=="mask")
		{
			msk++;
			COperation op(parm.s_RestOfLine);
if (debug) cerr << "mask=" << parm.s_RestOfLine << ":" << endl;			
			O_Op.push_back(op);
			if (msk==0)
				O_Op[msk].firstAction=0;
			else
			{
				O_Op[msk].firstAction=actn+1;
				O_Op[msk-1].lastAction=actn;
			}
		}
		else if (parm.s_Token=="action")
		{
			actn++;
			CAction act;
			act.param=parm.s_RestOfLine;
if (debug) cerr << "action=" << parm.s_RestOfLine << ":" << endl;			
			A_Action.push_back(act);
		}
		else if (parm.s_Token=="rfc2ftn")
		{
			rfcfido++;
			CAddresslist taddrlist(parm.s_RestOfLine);
if (debug) cerr << "rfc2ftn=" << parm.s_RestOfLine << ":" << endl;			
			if (rfcfido==0)
			  taddrlist.firstAddr = 0;
			else
			  taddrlist.lastAddr = rfcfido;
      A_Addrlist.push_back(taddrlist);
		}
	}
	if (S_Scandir.size()==1) S_Scandir[0].lastMask=O_Op.size()-1; 
	if (O_Op.size()==1) O_Op[0].lastAction=A_Action.size()-1;
	S_Scandir[S_Scandir.size()-1].lastMask=O_Op.size()-1;
	O_Op[O_Op.size()-1].lastAction=A_Action.size()-1;
if (debug) cerr << "End of Dump " << CONFIGDIR  << endl;			
	closeConfig();	 
}
