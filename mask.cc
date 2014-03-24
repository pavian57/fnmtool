#include <iostream>
#include <string>
#include <algorithm>

using namespace std;
#include "strsep.h"
#include "mask.h"
#include "config.h"
#include "global.h"

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
        d_Flags = msk.d_Flags;
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
	d_Flags=msk.d_Flags;
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
	strseparate(&buf,",");
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
	bool attrMatch=true;
	
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
    cerr << endl << endl;
    cerr << "F_From=" << F_From << ": msk.F_From=" << msk.F_From << ": frmMatch=" << frmMatch  <<endl ;
    cerr << "F_To=" << F_To << ": msk.F_To=" << msk.F_To << ": toMatch=" << toMatch  <<endl ;
    
    cerr << "s_Sender=" << s_Sender << ": msk.s_Sender=" << msk.s_Sender << ": sndMatch=" << sndMatch  <<endl ;
    cerr << "s_Recipient=" << s_Recipient << ": msk.s_Recipient=" << msk.s_Recipient << ": rcpMatch=" << rcpMatch << endl ;
    cerr << "s_Subject=" << s_Subject << ": msk.s_Subject=" << msk.s_Subject << endl ;
    
    cerr << "sndMatch=" << sndMatch << ": rcpMatch=" << rcpMatch << ": frmMatch=" << frmMatch << ": toMatch=" << toMatch << ": subjMatch=" << subjMatch << ": attrMatch=" << attrMatch << endl;
  }


	/* do dirty checking with ~ */
	if (strchr(s_Sender.c_str(), '~')!=NULL)
	{
		if (strstr(msk.s_Sender.c_str(), strchr(s_Sender.c_str(),'~')+1)!=NULL)
			sndMatch=true;
	}

        if (strchr(s_Recipient.c_str(), '~')!=NULL)
        {
                if (strstr(msk.s_Recipient.c_str(), strchr(s_Recipient.c_str(),'~')+1)!=NULL)
                        rcpMatch=true;
        }

        if (strchr(s_Subject.c_str(), '~')!=NULL)
        {
          string strsubj, strsubjmask;
          strsubj = msk.s_Subject;
          strsubjmask = s_Subject;
          
          if (cfg->debug)          
            cerr << "msk.s_Subject=" << msk.s_Subject << ": s_Subject=" << s_Subject << endl;

           transform(strsubj.begin(), strsubj.end(), strsubj.begin(), std::ptr_fun<int, int>(std::toupper));
           transform(strsubjmask.begin(), strsubjmask.end(), strsubjmask.begin(), std::ptr_fun<int, int>(std::toupper));           

          if (cfg->debug)          
            cerr << "strsubjmask=" << strsubjmask << ": strsubj=" << strsubj << endl;


                if (strstr(strsubj.c_str(), strchr(strsubjmask.c_str(),'~')+1)!=NULL)
                        subjMatch=true;
                              
        }

        if (cfg->debug) {	        
          cerr << endl << "sndMatch=" << sndMatch << ": rcpMatch=" << rcpMatch << ": frmMatch=" << frmMatch << ": toMatch=" << toMatch << ": subjMatch=" << subjMatch << ": attrMatch=" << attrMatch << endl << endl;
        }
          


	if (sndMatch && rcpMatch && frmMatch && toMatch && subjMatch && attrMatch) 
		return true; 
	else return false;

}

int CMask::parseAttr(string s_Flags)
{
	bool mand=false;
	for (unsigned int i=0;i<s_Flags.size()-1;i+=2)
	{
		char prefix, flag;
		prefix=s_Flags[i];
		flag=s_Flags[i+1];
		if (prefix=='+') mand=true;
		if (flag=='p' || flag=='P')
		{
			if (mand) priv=1;
			else priv=-1;
		}
                if (flag=='c' || flag=='C')
                {
                        if (mand) crash=1;
                        else crash=-1;
                }
                if (flag=='s' || flag=='S')
                {
                        if (mand) sent=1;
                        else sent=-1;
                }
                if (flag=='r' || flag=='R')
                {
                        if (mand) received=1;
                        else received=-1;
                }
                if (flag=='a' || flag=='A')
                {
                        if (mand) attach=1;
                        else attach=-1;
                }
                if (flag=='i' || flag=='I')
                {
                        if (mand) immediate=1;
                        else immediate=-1;
                }
                if (flag=='o' || flag=='O')
                {
                        if (mand) orphan=1;
                        else orphan=-1;
                }
                if (flag=='k' || flag=='K')
                {
                        if (mand) kill=1;
                        else kill=-1;
                }
                if (flag=='l' || flag=='L')
                {
                        if (mand) local=1;
                        else local=-1;
                }
                if (flag=='h' || flag=='H')
                {
                        if (mand) hold=1;
                        else hold=-1;
                }
                if (flag=='f' || flag=='F')
                {
                        if (mand) file_request=1;
                        else file_request=-1;
                }
                if (flag=='n' || flag=='N')
                {
                        if (mand) file_request=1;
                        else file_request=-1;
                }
                if (flag=='d' || flag=='D')
                {
                        if (mand) file_request=1;
                        else file_request=-1;
                }
                if (flag=='u' || flag=='U')
                {
                        if (mand) file_request=1;
                        else file_request=-1;
                }
                if (flag=='q' || flag=='Q')
                {
                        if (mand) file_request=1;
                        else file_request=-1;
                }
                if (flag=='y' || flag=='Y')
                {
                        if (mand) file_request=1;
                        else file_request=-1;
                }
                if (flag=='e' || flag=='E')
                {
                        if (mand) file_request=1;
                        else file_request=-1;
                }
                if (flag=='m' || flag=='M')
                {
                        if (mand) file_request=1;
                        else file_request=-1;
                }
                if (flag=='t' || flag=='T')
                {
                        if (mand) file_request=1;
                        else file_request=-1;
                }
                if (flag=='z' || flag=='Z')
                {
                        if (mand) file_request=1;
                        else file_request=-1;
                }
	}
	return 0;
}
	
bool CMask::compareAttr(dword attr)
{
	return true;
}	
