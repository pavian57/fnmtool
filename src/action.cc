#include <iostream>
#include <sstream> // for ostringstream
#include <algorithm>
#ifdef __GNUC__
#include <cstdio>
#else
#include <stdio.h>
#endif
#include <cstring>

#include <time.h>
extern "C"
{
#include <smapi/msgapi.h>
}
#include "ftnaddr.h"
#include "mask.h"
#include "area.h"
#include "action.h"
#include "msg.h"
#include "version.h"
#include "pkt.h"
#include "global.h"
#include "msgid.h"
#include "iconvpp.h"
#include "fnmsmtp.h"

using namespace std;


CAction::CAction()
{
}

CAction::CAction(const CAction & act)
{
	param=act.param;
}

CAction::~CAction()
{
}

int CFileAction::run()
{
	CMsg SrcMsg;
	SrcMsg.Open(msgnum, Area);
	f_txtFile=fopen(s_Filename.c_str(), "w");
	if (f_txtFile != NULL) {
    fputs("From: ",f_txtFile);
    fputs(SrcMsg.s_From.c_str(),f_txtFile);
    fprintf(f_txtFile, ", %i:%i/%i.%i", SrcMsg.F_From.zone, SrcMsg.F_From.net, SrcMsg.F_From.node, SrcMsg.F_From.point);
    fputs("\n",f_txtFile);
    fputs("To  : ",f_txtFile);
    fputs(SrcMsg.s_To.c_str(),f_txtFile);
    fprintf(f_txtFile, ", %i:%i/%i.%i", SrcMsg.F_To.zone, SrcMsg.F_To.net, SrcMsg.F_To.node, SrcMsg.F_To.point);
    fputs("\n",f_txtFile);
    fputs("Subject: ",f_txtFile);
    fputs(SrcMsg.s_Subject.c_str(),f_txtFile);
    fputs("\n",f_txtFile);
    fputs("---------------------\n",f_txtFile);
    for (unsigned int i=0; i<SrcMsg.s_MsgText.length(); i++)
    {
        if (SrcMsg.s_MsgText[i]=='\r') SrcMsg.s_MsgText[i]='\n';
    }
    fputs(SrcMsg.s_MsgText.c_str(),f_txtFile);
    SrcMsg.read = true;
    SrcMsg.WriteAttr();
    SrcMsg.Close();
    string logstr="Writing Message to File " + s_Filename;
    if (!cfg->silent) cout << logstr << endl
    log->add(2,logstr);
    fclose(f_txtFile);
    return 0;
  } else {
    string logstr="Open File " + s_Filename + " failed!";
    log->add(2,logstr);
  }
  return -1;
}

int CHdrFileAction::run()
{
    CMsg SrcMsg;
    SrcMsg.Open(msgnum, Area);
    f_txtFile=fopen(s_Filename.c_str(), "w");
    if (f_txtFile != NULL) {
      fputs("From:      ",f_txtFile);
      fputs(SrcMsg.s_From.c_str(),f_txtFile);
      fprintf(f_txtFile, ", %i:%i/%i.%i", SrcMsg.F_From.zone, SrcMsg.F_From.net, SrcMsg.F_From.node, SrcMsg.F_From.point);
      fputs("\n",f_txtFile);
      fputs("To  :      ",f_txtFile);
      fputs(SrcMsg.s_To.c_str(),f_txtFile);
      fprintf(f_txtFile, ", %i:%i/%i.%i", SrcMsg.F_To.zone, SrcMsg.F_To.net, SrcMsg.F_To.node, SrcMsg.F_To.point);
      fputs("\n",f_txtFile);
      fputs("Subject:   ",f_txtFile);
      fputs(SrcMsg.s_Subject.c_str(),f_txtFile);
      fputs("\n",f_txtFile);
      fputs("Attribute: ",f_txtFile);
      fputs(printAttr(SrcMsg.d_Attr).c_str(),f_txtFile);
      fputs("\n",f_txtFile);
      fputs("----------------------\n", f_txtFile);
      SrcMsg.read = true;
      SrcMsg.WriteAttr();
      SrcMsg.Close();
      string logstr="Writing Header to File " + s_Filename;
			if (!cfg->silent) cout << logstr << endl
      log->add(2,logstr);
      fclose(f_txtFile);
      return 0;
    } else {
       string logstr="Open File " + s_Filename + " failed!";
    	 log->add(2,logstr);
    }
    return -1;
}

int CBounceAction::run()
{
    CMsg SrcMsg;
    CMsg DestMsg;
    /* open messages */
    SrcMsg.Open(msgnum, Area);
    DestMsg.New(Area);
    if (!cfg->silent) cerr << "created message" << endl;
    /* Write Headers */
    DestMsg.F_From=SrcMsg.F_To;
    DestMsg.s_From=SrcMsg.s_To;
    DestMsg.F_To=SrcMsg.F_From;
    DestMsg.s_To=SrcMsg.s_From;

    /* Write Subject */
    DestMsg.s_Subject="[bounce]";
    DestMsg.s_Subject+=SrcMsg.s_Subject;

    /* write attributes */
    DestMsg.d_Attr=MSGPRIVATE;

    string s_Text;
    string s_Kludges;
    if (!cfg->silent) cerr << "writing kludges" << endl;
    /* write fmpt, topt, intl info */
    char buf[25];
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
    sprintf(buf, "\001INTL %i:%i/%i %i:%i/%i",
            DestMsg.F_To.zone, DestMsg.F_To.net, DestMsg.F_To.node,
            DestMsg.F_From.zone, DestMsg.F_From.net, DestMsg.F_From.node);
    s_Kludges+=buf;
    if (!cfg->silent) cerr  << "wrote kludges!" << endl;
    string s_Temp;
    if (!cfg->silent) cerr  << "writing messageText" << endl;
    DestMsg.s_Ctrl+=s_Kludges;

    DestMsg.Write();
    DestMsg.Close();
    SrcMsg.Close();
    return 0;
}

int CPingPongAction::run()
{
    CMsg SrcMsg;
    CMsg DestMsg;
    /* open messages */
    SrcMsg.Open(msgnum, Area);
    DestMsg.New(Area);
    if (cfg->debug) cerr << "created message" << endl;
    /* Write Headers */

    DestMsg.F_From=SrcMsg.F_To;
    DestMsg.F_To=SrcMsg.F_From;

    DestMsg.F_To=SrcMsg.F_From;
    DestMsg.s_From="Ping Manager";// SrcMsg.s_To;
    DestMsg.s_To=SrcMsg.s_From;
    DestMsg.s_Subject="Your Ping request"; //  SrcMsg.s_Subject;
    DestMsg.d_Attr= MSGLOCAL|MSGPRIVATE|MSGKILL;

    /*

    #define MSGKILL    0x0080
    #define MSGLOCAL   0x0100
    #define MSGRRQ     0x1000

    */
    string s_Kludges;
    if (cfg->debug) cerr << "writing kludges" << endl;
    /* write fmpt, topt, intl info */
    char *buf;
    buf = new char[50];

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

    sprintf(buf,"\001PID: %s %s",PRGNAME,VERSION);
    s_Kludges+=buf;

    DestMsg.s_Ctrl+=s_Kludges;

    if (cfg->debug) cerr  << "wrote kludges!" << endl;
    delete [] buf;

    buf = new char[SrcMsg.s_MsgText.length()+SrcMsg.s_Ctrl.length()+500]();

    sprintf(buf,"Hello %s.\r\r",SrcMsg.s_From.c_str());
    sprintf(buf+strlen(buf),"Your ping-message received by my system at %02d:%02d\r\r", SrcMsg.t_DateArrived.time.hh ,SrcMsg.t_DateArrived.time.mm);
    sprintf(buf+strlen(buf),"Original message\r\r");
    sprintf(buf+strlen(buf),"============================================================================\r");
    sprintf(buf+strlen(buf),"From         : %s",SrcMsg.s_From.c_str());
    sprintf(buf+strlen(buf), ", %i:%i/%i.%i\r", SrcMsg.F_From.zone, SrcMsg.F_From.net, SrcMsg.F_From.node, SrcMsg.F_From.point);
    sprintf(buf+strlen(buf),"To           : %s",SrcMsg.s_To.c_str());
    sprintf(buf+strlen(buf),", %i:%i/%i.%i\r", SrcMsg.F_To.zone, SrcMsg.F_To.net, SrcMsg.F_To.node, SrcMsg.F_To.point);
    sprintf(buf+strlen(buf),"Subject      : %s\r",SrcMsg.s_Subject.c_str());
    sprintf(buf+strlen(buf), "Date written : %02d.%02d.%04d\r", SrcMsg.t_DateWritten.date.da ,SrcMsg.t_DateWritten.date.mo,SrcMsg.t_DateWritten.date.yr+1980);
    sprintf(buf+strlen(buf), "Date arrived : %02d.%02d.%04d\r", SrcMsg.t_DateArrived.date.da ,SrcMsg.t_DateArrived.date.mo,SrcMsg.t_DateArrived.date.yr+1980);
    sprintf(buf+strlen(buf),"============================================================================\r");

//check for ^A and change to @
    char* token		= NULL;
    char  delims[]	= "\1";
    string ctrlline;
    // During the first read, establish the char string and get the first token.
    char *dup = strdup(SrcMsg.s_Ctrl.c_str());
    token = strtok(dup,"\1");
    // While there are any tokens left in "char_line"...
    while (token != NULL)
    {
// seperate controllines
        ctrlline = "@";
        ctrlline += token;
//write ctrl to buf
        sprintf(buf+strlen(buf),"%s\r",ctrlline.c_str());
        token = strtok(NULL, delims);
    }
    free(dup);

    //check for ^A to @ lf to cr
    for (unsigned int i=0; i<SrcMsg.s_MsgText.length(); i++)
    {
        if (SrcMsg.s_MsgText[i]=='\n') SrcMsg.s_MsgText[i]='\r';
        if (SrcMsg.s_MsgText[i]=='\1') SrcMsg.s_MsgText[i]='@';
    }

    // invalidate tear -!- and origin $ backwards
    int tearl = 0;
    for (unsigned int i=SrcMsg.s_MsgText.length(); i>0 ; i--)
    {
        if (SrcMsg.s_MsgText[i]=='*')
            SrcMsg.s_MsgText[i]='$';
        if (SrcMsg.s_MsgText[i]=='-')
        {
            tearl++;
            if (tearl == 3)
            {
                SrcMsg.s_MsgText[i+1]='!';
                break;
            }
        }
        else tearl = 0;
    }
//write original message to buf
    sprintf(buf+strlen(buf),"%s",SrcMsg.s_MsgText.c_str());
    sprintf(buf+strlen(buf),"============================================================================\r");
//	sprintf(buf+strlen(buf),"--- %s %s",PRGNAME,VERSION);

    if (cfg->debug) cerr  << "Message=\n" << buf<< endl;

    DestMsg.s_MsgText=buf;

    if (cfg->debug) cerr  << "writing messageText" << endl;
    DestMsg.Write();

    //sprintf(buf), " %i:%i/%i.%i", SrcMsg.F_From.zone, SrcMsg.F_From.net, SrcMsg.F_From.node, SrcMsg.F_From.point);

    stringstream node;//create a stringstream
    node  << SrcMsg.F_From ;
    string logstr="PINGRequest: wrote message to=";
    logstr += node.str();

    if (!cfg->silent) cout << logstr << endl;
    log->add(2,logstr);
    DestMsg.Close();

    SrcMsg.d_Attr = SrcMsg.d_Attr | MSGREAD;
    SrcMsg.read = true;
    SrcMsg.WriteAttr();
    SrcMsg.Delete(Area);
    SrcMsg.deleted=true;
//	SrcMsg.Close();

    delete [] buf;
    return 0;
}


int CCopyAction::run()
{
    CArea AArea;
    CMsg SrcMsg;
    CMsg DestMsg;
    destarea=param;
    AArea.Open(destarea);
    SrcMsg.Open(msgnum, Area);
    DestMsg.New(AArea.GetArea());
    DestMsg.F_From=SrcMsg.F_From;
    DestMsg.F_To=SrcMsg.F_To;
    DestMsg.s_From=SrcMsg.s_From;
    DestMsg.s_To=SrcMsg.s_To;
    DestMsg.s_Subject=SrcMsg.s_Subject;
    DestMsg.s_MsgText=SrcMsg.s_MsgText;
    DestMsg.s_Ctrl=SrcMsg.s_Ctrl;
    DestMsg.d_Attr=SrcMsg.d_Attr;
    DestMsg.sent=true;
    DestMsg.Write();
    string logstr="Copied Message to Area " + destarea;
    SrcMsg.read = true;
    SrcMsg.WriteAttr();
    if (!cfg->silent) cout << logstr << endl
    log->add(2,logstr);
    DestMsg.Close();
    SrcMsg.Close();
    AArea.Close(); 
    return 0;
}

int CMoveAction::run()
{
    CArea AArea;
    CMsg SrcMsg;
    CMsg DestMsg;
    destarea=param;
    AArea.Open(destarea);
    SrcMsg.Open(msgnum, Area);
    DestMsg.New(AArea.GetArea());
    DestMsg.F_From=SrcMsg.F_From;
    DestMsg.F_To=SrcMsg.F_To;
    DestMsg.s_From=SrcMsg.s_From;
    DestMsg.s_To=SrcMsg.s_To;
    DestMsg.s_Subject=SrcMsg.s_Subject;
    DestMsg.s_MsgText=SrcMsg.s_MsgText;
    DestMsg.s_Ctrl=SrcMsg.s_Ctrl;
    DestMsg.d_Attr=SrcMsg.d_Attr;
    DestMsg.sent=true;
    DestMsg.Write();
    string logstr="Moved message to Area " + destarea;
    if (!cfg->silent) cout << logstr << endl;
    log->add(2, logstr);
    SrcMsg.Delete(Area);
    SrcMsg.deleted=true;
    DestMsg.Close();
    AArea.Close();
    return 0;
}

int CPackmailAction::run()
{
    CPkt pkt;
    string fromnode, tonode, passwd;
    string temp=param;
    while (temp[0]==' ') temp.erase(0,1);
    do
    {
        /*	   if (temp[0]==(*temp.end()))
        	   {
        		string logstr="Invalid packmail statement while processing from-address!";
        		log->add(5, logstr);
        		if (!cfg->silent) cerr << logstr << endl;
        		exit(0);
        	   }*/
        fromnode+=temp[0];
        temp.erase(0,1);
    }
    while (temp[0]!=' ');
    while (temp[0]==' ') temp.erase(0,1);
    do
    {
        /*           if (temp[0]==(*temp.end()))
                   {
                        string logstr="Invalid packmail statement while processing uplink!";
                        log->add(5, logstr);
                        if (!cfg->silent) cerr << logstr << endl;
                        exit(0);
                   }*/
        tonode+=temp[0];
        temp.erase(0,1);
    }
    while (temp[0]!=' ');
    while(temp[0]==' ') temp.erase(0,1);
    passwd=temp;

    pkt.fromNode=const_cast<char*>(fromnode.c_str());
    pkt.toNode=const_cast<char*>(tonode.c_str());
    pkt.password=passwd;
    pkt.Message.Open(msgnum, Area);
    char taddr[20];
    char faddr[20];
    char viaaddr[20];
    sprintf(faddr, "%i:%i/%i.%i",
            pkt.Message.F_From.zone, pkt.Message.F_From.net, pkt.Message.F_From.node, pkt.Message.F_From.point);
    sprintf(taddr, "%i:%i/%i.%i",
            pkt.Message.F_To.zone, pkt.Message.F_To.net, pkt.Message.F_To.node, pkt.Message.F_To.point);
    sprintf(viaaddr, "%i:%i/%i.%i",
            pkt.toNode.zone, pkt.toNode.net, pkt.toNode.node, pkt.toNode.point);
    string logstr="Packed Message from ";
    logstr+=faddr;
    logstr+=" to ";
    logstr+= taddr;
    logstr+=" via ";
    logstr+=viaaddr;
    log->add(2, logstr);
    pkt.create();
    pkt.Message.d_Attr |= MSGSENT;
    pkt.Message.Write();
    pkt.Message.Close();
    return 0;
}

int CMovemailAction::run()
{
    CPkt pkt;
    string fromnode, tonode, passwd, dir;
    string temp=param;
    while (temp[0]==' ') temp.erase(0,1);
    do
    {
        if (temp[0]==(*temp.end()))
        {
            string logstr="Invalid movemail statement! ToNode not specified";
            log->add(5, logstr);
            if (!cfg->silent) cerr << logstr << endl;
            exit(0);
        }

        fromnode+=temp[0];
        temp.erase(0,1);
    }
    while (temp[0]!=' ');
    while (temp[0]==' ') temp.erase(0,1);
    do
    {
        if (temp[0]==(*temp.end()))
        {
            string logstr="Invalid movemail statement! FromNode not specified";
            log->add(5, logstr);
            if (!cfg->silent)  cerr << logstr << endl;
            exit(0);
        }

        tonode+=temp[0];
        temp.erase(0,1);
    }
    while (temp[0]!=' ');
    while(temp[0]==' ') temp.erase(0,1);
    do
    {
        if (temp[0]==(*temp.end()))
        {
            string logstr="Invalid movemail statement! wrong directory";
            log->add(5, logstr);
            if (!cfg->silent) cerr << logstr << endl;
            exit(0);
        }

        dir+=temp[0];
        temp.erase(0,1);
    }
    while (temp[0]!=' ');

    passwd=temp;

    pkt.fromNode=const_cast<char*>(fromnode.c_str());
    pkt.toNode=const_cast<char*>(tonode.c_str());
    pkt.password=passwd;
    pkt.dir=dir;
    pkt.Message.Open(msgnum, Area);
    char taddr[20];
    char faddr[20];
    char viaaddr[20];
    sprintf(faddr, "%i:%i/%i.%i",
            pkt.Message.F_From.zone, pkt.Message.F_From.net, pkt.Message.F_From.node, pkt.Message.F_From.point);
    sprintf(taddr, "%i:%i/%i.%i",
            pkt.Message.F_To.zone, pkt.Message.F_To.net, pkt.Message.F_To.node, pkt.Message.F_To.point);
    sprintf(viaaddr, "%i:%i/%i.%i",
            pkt.toNode.zone, pkt.toNode.net, pkt.toNode.node, pkt.toNode.point);
    string logstr="Packed Message from ";
    logstr+=faddr;
    logstr+=" to ";
    logstr+= taddr;
    logstr+=" via ";
    logstr+=viaaddr;
    log->add(2, logstr);
    pkt.create();
    pkt.Message.d_Attr |= MSGSENT;
    pkt.Message.Write();
    pkt.Message.Close();
    return 0;
}


int CEmailAction::run()
{
    CMsg Message;
  	CSmtp sendmail;
    string smtpserver, smtpport, mailto;
    string temp=param;
    while (temp[0]==' ') temp.erase(0,1);
    do
    {
        if (temp[0]==(*temp.end()))
        {
            string logstr="Invalid email statement! smtpserver not specified";
            log->add(5, logstr);
            if (!cfg->silent) cerr << logstr << endl;
            exit(0);
        }

        smtpserver+=temp[0];
        temp.erase(0,1);
    }
    while (temp[0]!=' ');


    while (temp[0]==' ') temp.erase(0,1);
    do
    {
        if (temp[0]==(*temp.end()))
        {
            string logstr="Invalid email statement! smtpport not specified";
            log->add(5, logstr);
            if (!cfg->silent) cerr << logstr << endl;
            exit(0);
        }

        smtpport+=temp[0];
        temp.erase(0,1);
    }
    while (temp[0]!=' ');

    while (temp[0]==' ') temp.erase(0,1);
    mailto=temp;
    char *buf;
    Message.Open(msgnum, Area);
    buf = new char[Message.s_MsgText.length()+400]();
    sprintf(buf,"From: %s",Message.s_From.c_str());
    sprintf(buf+strlen(buf), ", %i:%i/%i.%i\n", Message.F_From.zone, Message.F_From.net, Message.F_From.node, Message.F_From.point);
    sprintf(buf+strlen(buf),"To  : %s",Message.s_To.c_str());
    sprintf(buf+strlen(buf),", %i:%i/%i.%i\n", Message.F_To.zone, Message.F_To.net, Message.F_To.node, Message.F_To.point);
    sprintf(buf+strlen(buf),"Subject: %s\n",Message.s_Subject.c_str());
    sprintf(buf+strlen(buf),"---------------------\n");

    converter conv(cfg->s_CharsetRfc, Message.s_Charset);
    if (cfg->debug) cerr  << "Message.s_Charset=" << Message.s_Charset << endl;

    if (cfg->debug) cerr  << "Message.s_MsgText.length()=" << Message.s_MsgText.length() << endl;
    for (unsigned int i=0; i<Message.s_MsgText.length(); i++)
    {
        if (Message.s_MsgText[i]=='\r') Message.s_MsgText[i]='\n';
        if (Message.s_MsgText[i]=='\1') Message.s_MsgText[i]='@';
    }
    
    string outmsg;
    conv.convert(Message.s_MsgText,outmsg);  
    
    sprintf(buf+strlen(buf),"%s",outmsg.c_str());  //Message.s_MsgText.c_str());

    if (cfg->debug) cerr  << Message.s_MsgText << endl;

    Message.read = true;
    Message.WriteAttr();
    Message.Close();

    char *mailfrom;
    mailfrom = new char[100]();
    replace(Message.s_From.begin(),Message.s_From.end(),' ','_');
    sprintf(mailfrom,"%s@",Message.s_From.c_str());
    sprintf(mailfrom+strlen(mailfrom), "p%i.f%i.n%i.z%i.fidonet", Message.F_From.point, Message.F_From.node, Message.F_From.net, Message.F_From.zone);


		sendmail.setMailServer(smtpserver);
		sendmail.setPort(atoi(smtpport.c_str()));

    sendmail.setContentType(cfg->s_CharsetRfc);
	  sendmail.setMailFrom(mailfrom); 
		sendmail.setMailTo(mailto);
		sendmail.setMailSubject(Message.s_Subject);
		sendmail.setMailText(buf);




 		int rc = sendmail.sendmail();

		if (rc != 0) {
    	string logstr="Error__ sending e-mail: ";
//      logstr += errmsg;
      log->add(5,logstr);
    } else {
    	ostringstream out;
	    string  areaname;
  	  areaname.append(basename(s_Area.c_str()));
	    out << "Sending message #: " << msgnum << " from Area: " << areaname << " to: " << mailto;
  	  log->add(2,out.str());
	    if (!cfg->silent) cout << out.str()  << endl;
		}

    delete [] buf;
    return 0;
}


int CRewriteAction::run()
{
    CMask newMask;
    newMask=param;
    CMsg Message;
    Message.Open(msgnum, Area);
    /* change sender */
    if (newMask.s_Sender[0]!='*')
        Message.s_From=newMask.s_Sender;
    /* change recipient */
    if (newMask.s_Recipient[0]!='*')
        Message.s_To=newMask.s_Recipient;
    /* change fromA */
    char *eaddr=new char[24];
    strcpy(eaddr,"65535:65535/65535.65535");
    if (newMask.F_From==eaddr) {}
    else
        Message.F_From=newMask.F_From;
    /* change toA */
    if (newMask.F_To==eaddr) {}
    else
        Message.F_To=newMask.F_To;

    /* change subject */
    if (newMask.s_Subject[0]!='*')
        Message.s_Subject=newMask.s_Subject;
    Message.Write();
    Message.Close();
    delete eaddr;
    return 0;
}

int CDisplayAction::run()
{
	CMsg SrcMsg;
	SrcMsg.Open(msgnum, Area);
  cout << "From   : " << SrcMsg.s_From << "  " << SrcMsg.F_From.zone << ":" << SrcMsg.F_From.net << "/" << SrcMsg.F_From.node << "." << SrcMsg.F_From.point << endl;
  cout << "To     : " << SrcMsg.s_To   << "  " << SrcMsg.F_To.zone << ":" <<  SrcMsg.F_To.net << "/" << SrcMsg.F_To.node<< "." <<  SrcMsg.F_To.point << endl;
  cout << "Subject: " << SrcMsg.s_Subject << endl;
  cout << "==========================================" <<  endl;
  for (unsigned int i=0; i<SrcMsg.s_MsgText.length(); i++)
  {
     if (SrcMsg.s_MsgText[i]=='\r') SrcMsg.s_MsgText[i]='\n';
  }
  cout << SrcMsg.s_MsgText << endl;
//  SrcMsg.d_Attr = SrcMsg.d_Attr | MSGREAD;
  SrcMsg.read = true;
  SrcMsg.WriteAttr();
  SrcMsg.Close();
  string logstr="Writing Message to screen";
	if (!cfg->silent) cout << logstr << endl
  log->add(2,logstr);
  return 0;
}

int CSemaphoreAction::run()
{
	 CMsg SrcMsg;
	f_semFile=fopen(param.c_str(), "w");
  if (f_semFile != NULL) {
  	SrcMsg.Open(msgnum, Area);
//  SrcMsg.d_Attr = SrcMsg.d_Attr | MSGREAD;
    SrcMsg.read = true;
    SrcMsg.WriteAttr();
    SrcMsg.Close();
    string logstr="Semaphore "+param +" written ";
		if (!cfg->silent) cout  << logstr << endl
    log->add(2,logstr);
    fclose(f_semFile);
    return 0;
  } else {
    string logstr="Writing Semaphore " + param + " failed!";
    log->add(2,logstr);
  }
  return -1;
}

/*int CEchoCopyAction::run()
{
   CArea AArea;
   CMsg SrcMsg;
   CMsg DestMsg;
   destarea=param;
   AArea.Open(destarea);
   SrcMsg.Open(msgnum, Area);
   DestMsg.New(AArea.GetArea());
   DestMsg.F_From=SrcMsg.F_From;
   DestMsg.F_To=SrcMsg.F_To;
   DestMsg.s_From=SrcMsg.s_From;
   DestMsg.s_To=SrcMsg.s_To;
   DestMsg.s_Subject=SrcMsg.s_Subject;
   DestMsg.s_MsgText=SrcMsg.s_MsgText;
   DestMsg.s_Ctrl=SrcMsg.s_Ctrl;
   DestMsg.d_Attr=SrcMsg.d_Attr;
   DestMsg.sent=true;
   DestMsg.Write();
   string logstr="Copied Message to Area " + destarea;
   log->add(2,logstr);
   DestMsg.Close();
   SrcMsg.Close();
   AArea.Close();
   return 0;
}

int CEchoMoveAction::run()
{
   CArea AArea;
   CMsg SrcMsg;
   CMsg DestMsg;
   destarea=param;
   AArea.Open(destarea);
   SrcMsg.Open(msgnum, Area);
   DestMsg.New(AArea.GetArea());
   DestMsg.F_From=SrcMsg.F_From;
   DestMsg.F_To=SrcMsg.F_To;
   DestMsg.s_From=SrcMsg.s_From;
   DestMsg.s_To=SrcMsg.s_To;
   DestMsg.s_Subject=SrcMsg.s_Subject;
   DestMsg.s_MsgText=SrcMsg.s_MsgText;
   DestMsg.s_Ctrl=SrcMsg.s_Ctrl;
   DestMsg.d_Attr=SrcMsg.d_Attr;
   DestMsg.sent=true;
   DestMsg.Write();
   string logstr="Moved message to Area " + destarea;
   log->add(2, logstr);
   SrcMsg.Delete(Area);
   SrcMsg.deleted=true;
   DestMsg.Close();
   AArea.Close();
   return 0;
}*/

int CDeleteAction::run()
{
    CMsg SrcMsg;
    char logstr[64];
    SrcMsg.Open(msgnum, Area);
    SrcMsg.Delete(Area);
    SrcMsg.deleted=true;
    sprintf(logstr,"Deleted message #%d",msgnum);
    if (!cfg->silent) cout << logstr << endl;
    log->add(2, logstr);
    return 0;
}

int CTwitAction::run()
{
		CMsg SrcMsg;
    CMsg DestMsg;
    /* open messages */
    SrcMsg.Open(msgnum, Area);
    DestMsg.New(Area);
    if (cfg->debug) cerr << "CTwitAction, created message" << endl;
    /* Write Headers */

    DestMsg.F_From=SrcMsg.F_To;
    DestMsg.F_To=SrcMsg.F_From;

    DestMsg.F_To=SrcMsg.F_From;
    DestMsg.s_From=SrcMsg.s_To;
    DestMsg.s_To=SrcMsg.s_From;
    DestMsg.s_Subject=SrcMsg.s_Subject;
    DestMsg.d_Attr= MSGLOCAL|MSGPRIVATE|MSGKILL;

    string s_Kludges;
    if (cfg->debug) cerr << "writing kludges" << endl;
    /* write fmpt, topt, intl info */
    char *buf;
    buf = new char[50];

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

    sprintf(buf,"\001PID: %s %s",PRGNAME,VERSION);
    s_Kludges+=buf;

    DestMsg.s_Ctrl+=s_Kludges;

    if (cfg->debug) cerr  << "wrote kludges!" << endl;
    delete [] buf;

    buf = new char[200]();

    sprintf(buf,"Hello %s.\r\r",SrcMsg.s_From.c_str());
    sprintf(buf+strlen(buf),"Your message has been succesfully ignored and deleted\r\r\r");

    if (cfg->debug) cerr  << "CTwitAction Message=\n" << buf<< endl;

    DestMsg.s_MsgText=buf;

    if (cfg->debug) cerr  << "CTwitAction writing messageText" << endl;
    DestMsg.Write();

    //sprintf(buf), " %i:%i/%i.%i", SrcMsg.F_From.zone, SrcMsg.F_From.net, SrcMsg.F_From.node, SrcMsg.F_From.point);

    stringstream node;//create a stringstream
    node  << SrcMsg.F_From ;
    string logstr="CTwitAction wrote message to=";
    logstr += node.str();

    if (!cfg->silent) cout << logstr << endl;
    log->add(2,logstr);
    DestMsg.Close();

    SrcMsg.d_Attr = SrcMsg.d_Attr | MSGREAD;
    SrcMsg.read = true;
    SrcMsg.WriteAttr();
    SrcMsg.Delete(Area);
    SrcMsg.deleted=true;
//	SrcMsg.Close();

    delete [] buf;
    return 0;
}

