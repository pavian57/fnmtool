#include <iostream>
#include <string>
#include <algorithm>

using namespace std;
#include "strsep.h"
#include "mask.h"
#include "config.h"
#include "global.h"
extern "C"
{
#include <smapi/msgapi.h>
}

CMask::CMask()
{
}

CMask::~CMask()
{

}

CMask::CMask(const CMask& msk)
{
        s_Sender = msk.s_Sender;
        s_Recipient = msk.s_Recipient;
        F_From = msk.F_From;
        F_To = msk.F_To;
        s_Subject = msk.s_Subject;
        s_Flags = msk.s_Flags;
}

CMask::CMask(string sndr, string recpt, CFtnAddr frm, CFtnAddr to, string subj, dword flgs)
{
	s_Sender = sndr;
	s_Recipient = recpt;
	F_From = frm;
	F_To = to;
	s_Subject = subj;
	d_Flags = flgs;
}

CMask& CMask::operator=(const CMask& msk)
{
	if ((this)==(&msk)) return (*this);
	s_Sender = msk.s_Sender;
	s_Recipient = msk.s_Recipient;
	s_Subject=msk.s_Subject;
	F_From=msk.F_From;
	F_To=msk.F_To;
	s_Flags=msk.s_Flags;
	d_Flags= msk.d_Flags;
	return (*this);
}

CMask& CMask::operator=(const string& str)
{
	char *buf;
	buf = new char[128];
	buf=const_cast<char*>(str.c_str());
	s_Sender=strseparate(&buf, ",");
	F_From=strseparate(&buf, ",");
	s_Recipient=strseparate(&buf, ",");
	F_To=strseparate(&buf, ",");
	s_Subject=strseparate(&buf, ",");
	s_Flags = strseparate(&buf,",");
	delete [] buf;
	return (*this);
}

bool CMask::operator==(const CMask& msk)
{
	bool sndMatch=false;
	bool rcpMatch=false;
	bool frmMatch=false;
	bool toMatch=false;
	bool subjMatch=false;
	bool attrMatch=false;

  if (s_Flags.empty()) {
    attrMatch=true;
  } else {
	  attrMatch = checkAttr(s_Flags,msk.d_Flags);
  }
	
	/* try to determine matches simply */
	if (msk.s_Sender == s_Sender) sndMatch=true;
	if (s_Sender=="*") sndMatch=true;
	
	if (msk.s_Recipient == s_Recipient) rcpMatch=true;
	if (s_Recipient=="*") rcpMatch=true;
	
	if (F_From == msk.F_From) frmMatch=true;
	if (F_From.zone == 0 || F_From.net ==0) frmMatch=true;
	if (F_From =="*") frmMatch=true;
	
	if (F_To == msk.F_To) toMatch=true;
	if (F_To.zone == 0 || F_To.net ==0) toMatch=true;
	
	if (msk.s_Subject == s_Subject) subjMatch=true;
	if (s_Subject=="*") subjMatch=true;
	

  if (cfg->debug) {	
    cerr << "F_From=" << F_From << ": msk.F_From=" << msk.F_From << ": frmMatch=" << frmMatch  <<endl ;
    cerr << "F_To=" << F_To << ": msk.F_To=" << msk.F_To << ": toMatch=" << toMatch  <<endl ;
    cerr << "s_Sender=" << s_Sender << ": msk.s_Sender=" << msk.s_Sender << ": sndMatch=" << sndMatch  <<endl ;
    cerr << "s_Recipient=" << s_Recipient << ": msk.s_Recipient=" << msk.s_Recipient << ": rcpMatch=" << rcpMatch << endl ;
    cerr << "s_Subject=" << s_Subject << ": msk.s_Subject=" << msk.s_Subject << ": subjMatch=" << subjMatch  << endl ;
    cerr << "s_Flags=" << s_Flags << ": msk.s_Flags=" << s_MsgFlags << ": attrMatch=" << attrMatch  << endl ; 
    cerr << "do dirty checking with ~" << endl;
  }


	/* do dirty checking with ~ */
	
	
	if (strchr(s_Sender.c_str(), '~')!=NULL)
	{
		if (strstr(msk.s_Sender.c_str(), strchr(s_Sender.c_str(),'~')+1)!=NULL)
			sndMatch=true;
    if (cfg->debug) 
      cerr << "s_Sender=" << s_Sender << ": msk.s_Sender=" << msk.s_Sender << ": sndMatch=" << sndMatch  <<endl ;
	}

  if (strchr(s_Recipient.c_str(), '~')!=NULL)
  {
    if (strstr(msk.s_Recipient.c_str(), strchr(s_Recipient.c_str(),'~')+1)!=NULL)  
       rcpMatch=true;
    if (cfg->debug)
      cerr << "s_Recipient=" << s_Recipient << ": msk.s_Recipient=" << msk.s_Recipient << ": rcpMatch=" << rcpMatch << endl ;
  }

  if (strchr(s_Subject.c_str(), '~')!=NULL)
  {
    string strsubj, strsubjmask;
    strsubj = msk.s_Subject;
    strsubjmask = s_Subject;
          
    transform(strsubj.begin(), strsubj.end(), strsubj.begin(), std::ptr_fun<int, int>(std::toupper));
    transform(strsubjmask.begin(), strsubjmask.end(), strsubjmask.begin(), std::ptr_fun<int, int>(std::toupper));           

    if (strstr(strsubj.c_str(), strchr(strsubjmask.c_str(),'~')+1)!=NULL)
      subjMatch=true;
    if (cfg->debug)
      cerr << "strsubjmask=" << strsubjmask << ": strsubj=" << strsubj << ": s_Subject=" << s_Subject << ": msk.s_Subject=" << msk.s_Subject << ": subjMatch=" << subjMatch  << endl ;    
                              
  }
     
  if (cfg->debug)
     cerr << "sndMatch=" << sndMatch << ": rcpMatch=" << rcpMatch << ": frmMatch=" << frmMatch << ": toMatch=" << toMatch << ": subjMatch=" << subjMatch << ": attrMatch=" << attrMatch << endl;

	if (sndMatch && rcpMatch && frmMatch && toMatch && subjMatch && attrMatch) 
		return true; 
	else return false;

}

bool CMask::checkAttr(string s_Flags,dword attr)
{
	bool mand=false;
  bool result=false;
  s_MsgFlags.clear();  
	for (unsigned int i=0;i<s_Flags.size()-1;i+=2)
	{
		char prefix, flag;
		prefix=s_Flags[i];
		flag=s_Flags[i+1];
		if (prefix=='+') mand=true;
		if (flag=='p' || flag=='P') { //MSGPRIVATE
		  if (mand) {
        if ((attr & MSGPRIVATE) != 0) result = true; else result = false;
        if (result == true) s_MsgFlags += "+p"; else s_MsgFlags += "-p";
      } else {
        if ((attr & MSGPRIVATE) == 0) result = true; else result = false;
        if (result != true) s_MsgFlags += "+p"; else s_MsgFlags += "-p";
      }
    } else if (flag=='c' || flag=='C') { //MSGCRASH
		  if (mand) {
        if ((attr & MSGCRASH) != 0) result = true; else result = false;
        if (result == true) s_MsgFlags += "+c"; else s_MsgFlags += "-c";
      } else {
        if ((attr & MSGCRASH) == 0) result = true; else result = false;
        if (result != true) s_MsgFlags += "+c"; else s_MsgFlags += "-c";
      }
    } else if (flag=='s' || flag=='S') { //MSGSENT
			if (mand) {
        if ((attr & MSGSENT) != 0) result = true; else result = false;
        if (result == true) s_MsgFlags += "+s"; else s_MsgFlags += "-s";
      } else {
        if ((attr & MSGSENT) == 0) result = true; else result = false;
        if (result != true) s_MsgFlags += "+s"; else s_MsgFlags += "-s";
      }
    } else if (flag=='r' || flag=='R') { //MSGREAD
      if (mand) {
        if ((attr & MSGREAD) != 0) result = true; else result = false;
        if (result == true) s_MsgFlags += "+r"; else s_MsgFlags += "-r";
      } else { 
        if ((attr & MSGREAD) == 0) result = true; else result = false;
        if (result != true) s_MsgFlags += "+r"; else s_MsgFlags += "-r";
      }
    } else  if (flag=='a' || flag=='A') { //MSGFILE
      if (mand) {
        if ((attr & MSGFILE) != 0) result = true; else result = false;
        if (result == true) s_MsgFlags += "+a"; else s_MsgFlags += "-a";
      } else {       
        if ((attr & MSGFILE) == 0) result = true; else result = false;
        if (result != true) s_MsgFlags += "+a"; else s_MsgFlags += "-a";
      }    
    }else if (flag=='i' || flag=='I') { //MSGFWD
      if (mand) {
        if ((attr & MSGFWD) != 0) result = true; else result = false;
        if (result == true) s_MsgFlags += "+i"; else s_MsgFlags += "-i";
      } else { 
        if ((attr & MSGFWD) == 0) result = true; else result = false;
        if (result != true) s_MsgFlags += "+i"; else s_MsgFlags += "-i";
      }    
    } else if (flag=='o' || flag=='O') { //MSGORPHAN
      if (mand) {
        if ((attr & MSGORPHAN) != 0) result = true; else result = false;
        if (result == true) s_MsgFlags += "+o"; else s_MsgFlags += "-o";
      } else { 
        if ((attr & MSGORPHAN) == 0) result = true; else result = false;
        if (result != true) s_MsgFlags += "+o"; else s_MsgFlags += "-o";
      }    
    } if (flag=='k' || flag=='K') {//MSGKILL
      if (mand) {
        if ((attr & MSGKILL) != 0) result = true; else result = false;
        if (result == true) s_MsgFlags += "+k"; else s_MsgFlags += "-k";
      } else { 
        if ((attr & MSGKILL) == 0) result = true; else result = false;
        if (result != true) s_MsgFlags += "+k"; else s_MsgFlags += "-k";
      }    
    } else if (flag=='l' || flag=='L') { //MSGLOCAL
      if (mand) {
        if ((attr & MSGLOCAL) != 0) result = true; else result = false;
        if (result == true) s_MsgFlags += "+l"; else s_MsgFlags += "-l";
      } else {
			  if ((attr & MSGLOCAL) == 0) result = true; else result = false;
			  if (result != true) s_MsgFlags += "+l"; else s_MsgFlags += "-l";
      }
    } else if (flag=='h' || flag=='H') {//MSGHOLD
      if (mand) {
        if ((attr & MSGHOLD) != 0) result = true; else result = false;
        if (result == true) s_MsgFlags += "+h"; else s_MsgFlags += "-h";
      } else { 
        if ((attr & MSGHOLD) == 0) result = true; else result = false;
        if (result != true) s_MsgFlags += "+h"; else s_MsgFlags += "-h";
      }        
    } else if (flag=='f' || flag=='F') {//MSGFRQ
      if (mand) {
        if ((attr & MSGFRQ) != 0) result = true; else result = false;
        if (result == true) s_MsgFlags += "+f"; else s_MsgFlags += "-f";
      } else { 
        if ((attr & MSGFRQ) == 0) result = true; else result = false;
        if (result != true) s_MsgFlags += "+f"; else s_MsgFlags += "-f";
      }    
    } else if (flag=='n' || flag=='N') {//MSGSCANNED
      if (mand) {
        if ((attr & MSGSCANNED) != 0) result = true; else result = false;
        if (result == true) s_MsgFlags += "+n"; else s_MsgFlags += "-n";
      } else { 
        if ((attr & MSGSCANNED) == 0) result = true; else result = false;
        if (result != true) s_MsgFlags += "+n"; else s_MsgFlags += "-n";
      }    
    } else if (flag=='d' || flag=='D') {//MSGXX2
      if (mand) {
        if ((attr & MSGXX2) != 0) result = true; else result = false;
        if (result == true) s_MsgFlags += "+d"; else s_MsgFlags += "-d";
      } else { 
        if ((attr & MSGXX2) == 0) result = true; else result = false;
        if (result != true) s_MsgFlags += "+d"; else s_MsgFlags += "-d";
      }    
    } else if (flag=='u' || flag=='U') {//MSGURQ
      if (mand) {
        if ((attr & MSGURQ) != 0) result = true; else result = false;
        if (result == true) s_MsgFlags += "+u"; else s_MsgFlags += "-u";
      } else { 
        if ((attr & MSGURQ) == 0) result = true; else result = false;
        if (result != true) s_MsgFlags += "+u"; else s_MsgFlags += "-u";
      }    
    } else if (flag=='q' || flag=='Q') {//MSGRRQ
      if (mand) {
        if ((attr & MSGRRQ) != 0) result = true; else result = false;
        if (result == true) s_MsgFlags += "+q"; else s_MsgFlags += "-q";
      } else { 
        if ((attr & MSGRRQ) == 0) result = true; else result = false;
        if (result != true) s_MsgFlags += "+q"; else s_MsgFlags += "-q";
      }    
    } else if (flag=='y' || flag=='Y') {//MSGCPT
      if (mand) {
        if ((attr & MSGCPT) != 0) result = true; else result = false;
        if (result == true) s_MsgFlags += "+y"; else s_MsgFlags += "-y";
      } else { 
        if ((attr & MSGCPT) == 0) result = true; else result = false;
        if (result != true) s_MsgFlags += "+y"; else s_MsgFlags += "-y";
      }    
    } else if (flag=='e' || flag=='E') {//; e = KFS (erase)
    } else if (flag=='m' || flag=='M') {//; m = iMmediate
    } else if (flag=='t' || flag=='T') {//; t = TFS
    } else if (flag=='z' || flag=='Z') {//; z = Archive sent
    }
	}
	return result;
}
	

/*
#define MSGPRIVATE 0x0001
#define MSGCRASH   0x0002
#define MSGREAD    0x0004
#define MSGSENT    0x0008
#define MSGFILE    0x0010
#define MSGFWD     0x0020     // is also called intransit flag 
#define MSGORPHAN  0x0040
#define MSGKILL    0x0080
#define MSGLOCAL   0x0100
#define MSGHOLD    0x0200
#define MSGXX2     0x0400     // you can use this flag as "Direct" attribute 
#define MSGFRQ     0x0800
#define MSGRRQ     0x1000
#define MSGCPT     0x2000
#define MSGARQ     0x4000
#define MSGURQ     0x8000

*/