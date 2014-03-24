#include <string>
#include <iostream>
#include <istream>
#include <ostream>
#include <iterator>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <algorithm>

#include "area.h"
#include "msg.h"
#include "msgid.h"
#include "version.h"
#include "config.h"
#include "global.h"
#include "rfcmail.h"
#include "iconvpp.h"

using namespace std;


CRfcmail::CRfcmail()
{
         r_From = "";
         r_To = "";
         s_From = "";
         s_To = "";
         s_Subject="no Subject found";
         s_Message = "";
         s_Ctrl = "";
         
};

int CRfcmail::parse(string sender,string recipient, string rawMail)
{

  string line, tmp;
  string zone, net, node, point, ftnaddr;
  int pos, index;

  pos = recipient.find("@");
  if (pos > 0) {
    s_To.assign(recipient,0,pos);
    line = recipient.substr(pos+1);
  } else return 1; //no at found, not valid rfc address
  replace( s_To.begin(), s_To.end(), '_', ' ');
  capitalize(s_To);  // nice name

// cut off all behind zone
  if ((pos = line.find(".fidonet")) != -1) {
    index = line.length()-pos;
    line.erase(pos,index);
  } else if ((pos = line.find(".z")) != -1) {
    if ((pos = line.find(".",pos+1)) != -1) {
      index = line.length()-pos;
      line.erase(pos,index);
    }
  }
  
// rkn@p99.f520.n301.z2.fidonet
  index = 0;
  point = "0";
  pos = line.find("p");
  if (pos != -1) {
    line.erase(pos,1);
    index = pos;
    pos = line.find(".f",index); 
    if (pos != -1) {
      pos = line.find(".");
      if (pos != -1) {
        point.assign(line,0,pos);
        line.erase(0,pos+1);
      }
      index = ++pos;
    }
  }

  pos = line.find(".f"); // extract node
  if (pos != -1) {
    line.erase(pos,1);
    index = line.find(".n");
  	if (index != -1) {
		  node.assign(line,pos+1,index-1);
    } else return 2; // no node info
  } else if ((pos = line.find("f")) != -1) {
	  line.erase(pos,1);
  	index = line.find(".n");
  	if (index != -1) {
		  node.assign(line,pos,index);
    } else return 2;
  } else return 2;  

  
  pos = line.find(".n"); // extract node
  if (pos != -1) {
    line.erase(pos+1,1);
    index = line.find(".z");
  	if (index != -1) {
		  net.assign(line,pos+1,index-pos-1);
    } else return 3; // no net info
  }  
  

  pos = line.find(".z"); // get the zone
  if (pos != -1) {
    line.erase(pos+1,1);
    line.erase(0,pos+1);
    zone.assign(line);
  }
  else return 4; // no zone info  


  ftnaddr = zone +":" + net + "/" + node + "." + point;
  F_To=const_cast<char*>(ftnaddr.c_str());

  
  pos = rawMail.find("Subject:");
  if (pos != -1) {
    line = rawMail.substr(pos);
    pos = line.find("\n");
    if (pos>8) 
      s_Subject.assign(line,9,pos-9);
    else s_Subject="no Subject found";
    line.clear();
  } 

  istringstream f(rawMail);
  int inmsg = 0;
  while (getline(f, line)) {
  cerr << line << endl;
    if (inmsg == 1) {
      s_Message.append(line);
      s_Message.append("\r");
    } else {
      if (!line.empty()) {
        pos = line.find_first_not_of(" \t");
        cerr << pos << endl;
//        if (pos == 1)
//          s_Ctrl.append("\001RFC:");                  
//        else
//          s_Ctrl.append("\001RFC-");
//        s_Ctrl.append(line);
      }     
    }
    
    if (line.compare("") == 0) inmsg = 1;
   }
 
   for (unsigned int i=0;i<cfg->A_Addrlist.size();i++)
   {
     if (cfg->A_Addrlist[i].rfcAddr.compare(sender) == 0) {
       s_From = cfg->A_Addrlist[i].fidoName;
       F_From = cfg->A_Addrlist[i].F_From;
       return 0;
     }
   }   
  return 255;
}

int CRfcmail::sendmail()
{

  CArea AArea;
  CMsg DestMsg;
  
  int rc = AArea.Open(cfg->s_Netmail);
  stringstream rcode ;//create a stringstream
  rcode << rc ;
	string logstr="Open Area returned=";
	logstr += rcode.str();
  log->add(2,logstr);
  if (rc==0) {
  
    DestMsg.New(AArea.GetArea());
    DestMsg.F_From=F_From;
    DestMsg.F_To=F_To;
    DestMsg.s_From=s_From;
    DestMsg.s_To=s_To;
    DestMsg.s_Subject=s_Subject;
    DestMsg.d_Attr= MSGLOCAL|MSGPRIVATE;
    
    string s_Kludges;
    /* write fmpt, topt, intl info */
    char *buf;
    buf = new char[200];

    sprintf(buf, "\001INTL %i:%i/%i %i:%i/%i",
            DestMsg.F_To.zone, DestMsg.F_To.net, DestMsg.F_To.node,
            DestMsg.F_From.zone, DestMsg.F_From.net, DestMsg.F_From.node);
    s_Kludges+=buf;

    if (DestMsg.F_From.point!=0)
    {
        sprintf(buf, "\001FMPT %i", DestMsg.F_From.point);
        s_Kludges+=buf;
    }
    if (DestMsg.F_To.point!=0)
    {
        sprintf(buf, "\001TOPT %i", DestMsg.F_To.point);
        s_Kludges+=buf;
    }


    if (DestMsg.F_From.point == 0)
        sprintf(buf,"\001MSGID: %i:%i/%i %08lx",DestMsg.F_From.zone, DestMsg.F_From.net, DestMsg.F_From.node ,createMsgId());
    else
        sprintf(buf,"\001MSGID: %i:%i/%i.%i %08lx",DestMsg.F_From.zone, DestMsg.F_From.net, DestMsg.F_From.node,DestMsg.F_From.point ,createMsgId());

    s_Kludges+=buf;

    sprintf(buf,"\001PID: %s %s",RFC2FTN,RFC2FTNVERSION);
    s_Kludges+=buf;
    sprintf(buf,"\001CHRS: %s",cfg->s_CharsetFtn.c_str());
    s_Kludges+=buf;    
    
    
    DestMsg.s_Ctrl += s_Kludges;
    DestMsg.s_Ctrl += s_Ctrl;    
    delete [] buf;
  
    string outmsg;
   
    converter conv(cfg->s_CharsetFtn,cfg->s_CharsetRfc);
    conv.convert(s_Message,outmsg);  
    
    DestMsg.s_MsgText = outmsg; 
    
    DestMsg.sent=true;
    DestMsg.Write();
    string logstr="Message written to Area " + cfg->s_Netmail;
    log->add(2,logstr);
    
    DestMsg.Close();
    AArea.Close(); 
 
  }  

  return 0;
}


CRfcmail::~CRfcmail()
{
};
