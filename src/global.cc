#include "global.h"
extern "C"
{
#include <smapi/msgapi.h>
}


CConfig * cfg;
CLog * log;

using namespace std;

void capitalize (string &s) { 
  bool cap = true;

  for(unsigned int i = 0; i <= s.length(); i++)
  {
    if (isalpha(s[i]) && cap == true)
    {
        s[i] = toupper(s[i]);
        cap = false;
    }
    else if (isspace(s[i]))
    {  
        cap = true;
    }
  }
}

string printAttr (dword attr)
{
  string attrList;
  if ((attr & MSGPRIVATE) == 0) attrList += "-p"; else attrList += "+p";
  if ((attr & MSGCRASH) == 0) attrList += "-c"; else attrList += "+c";  
  if ((attr & MSGREAD) == 0) attrList += "-r"; else attrList += "+r";
  if ((attr & MSGSENT) == 0) attrList += "-s"; else attrList += "+s";  
  if ((attr & MSGFILE) == 0) attrList += "-a"; else attrList += "+a";  
  if ((attr & MSGFWD) == 0) attrList += "-i"; else attrList += "+i";  
  if ((attr & MSGORPHAN) == 0) attrList += "-o"; else attrList += "+o";  
  if ((attr & MSGKILL) == 0) attrList += "-k"; else attrList += "+k";  
  if ((attr & MSGLOCAL) == 0) attrList += "-l"; else attrList += "+l";
  if ((attr & MSGHOLD) == 0) attrList += "-h"; else attrList += "+h";  
  if ((attr & MSGXX2) == 0) attrList += "-d"; else attrList += "+d";  
  if ((attr & MSGFRQ) == 0) attrList += "-f"; else attrList += "+f";  
  if ((attr & MSGSCANNED) == 0) attrList += "-n"; else attrList += "+n";  
  if ((attr & MSGURQ) == 0) attrList += "-u"; else attrList += "+u";  
  if ((attr & MSGRRQ) == 0) attrList += "-q"; else attrList += "+q";  
  if ((attr & MSGCPT) == 0) attrList += "-y"; else attrList += "+y";  
  
  return attrList;
}

int FileExist( const std::string& Name )
{

#ifndef __linux__
    struct _stat buf;
    return  _stat( Name.c_str(), &buf );
#else
    struct stat buf;
    return stat( Name.c_str(), &buf );
#endif
    
}
