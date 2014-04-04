#include <iostream>
#include <string>
#ifdef __GNUC__
#include <cstring>
#include <cstdio>
#else
#include <string.h>
#include <stdio.h>
#endif
#include <time.h>
#include "msg.h"

extern "C"
{
#include <smapi/msgapi.h>
}

using namespace std;

CMsg::CMsg()
{
	s_From="";
	s_To="";
	s_Subject="";
	s_Charset="";
	F_From="0:0/0.0";
	F_To="0:0/0.0";
	d_Attr=0;
	s_Ctrl="";
	s_MsgText="";
}

CMsg::CMsg(int i_num)
{
//	Open(i_num, &A_Area);
}

int CMsg::Open(int i_num, HAREA area)
{
	char *buf1, *buf2;
	string charset;
	hmsg = MsgOpenMsg(area, MOPEN_RW, i_num);
	if (hmsg==NULL) 
	{
		cerr << "could not open message" << endl;
		return -1;
	}
	i_CtrlLen=MsgGetCtrlLen(hmsg);
	i_TextLen = MsgGetTextLen(hmsg);
	
	buf1=new char[i_CtrlLen+1]();
	buf2=new char[i_TextLen+1]();
	MsgReadMsg(hmsg, &xmsg, 0, i_TextLen, (unsigned char *) buf2, i_CtrlLen, (unsigned char *)buf1);
	s_MsgText=buf2;
	s_Ctrl=buf1;
	s_Charset="IBM437";
  charset = s_Ctrl;
	int pos = charset.find("CHRS: ");
	if (pos != -1) {
    charset.erase(0,pos+6);
    pos = charset.find(" ");
    if (pos != -1) {
      charset.erase(pos,-1);
    } 
  } else {
    s_Charset = "LATIN1";
  }
  
  pos = s_Charset.find("IBMPC");
  if (pos > -1) {s_Charset = "IBM437";}
  pos = s_Charset.find("LATIN-1");
  if (pos > -1) {s_Charset = "LATIN1";}
         
	                              
	s_From=(char *)xmsg.from;
	s_To=(char *)xmsg.to;
	s_Subject=(char *)xmsg.subj;
	F_From=xmsg.orig;
	F_To=xmsg.dest;
	d_Attr=xmsg.attr;
  t_DateWritten=xmsg.date_written;
	t_DateArrived=xmsg.date_arrived;
	 
	i_number=i_num;
	deleted=false;
	delete [] buf1;
	delete [] buf2;
	return 0;
}

int CMsg::New(HAREA area)
{
	hmsg= MsgOpenMsg(area, MOPEN_CREATE, 0);
	if (hmsg==NULL) 
	{
		cerr << "Unable to create message!";
		return -1;
	}
	return 0;
}

int CMsg::Write()
{
	unsigned char from[36];
	unsigned char to[36];
	unsigned char subj[72];

	time_t tm;
	struct tm *dt;

	time(&tm);
	dt=gmtime(&tm);

//19 Jun 13  12:25:40
	char buffer [20] = {0};
	strftime (buffer ,20,"%d %b %y  %X",dt);

	strncpy((char *)xmsg.__ftsc_date,(char *)buffer,20);

 	xmsg.date_written.date.da=dt->tm_mday;
	xmsg.date_written.date.mo=dt->tm_mon+1;
	xmsg.date_written.date.yr=dt->tm_year;
	xmsg.date_written.time.ss=dt->tm_sec;
	xmsg.date_written.time.mm=dt->tm_min;
	xmsg.date_written.time.hh=dt->tm_hour+1;

	xmsg.date_arrived.date.da=dt->tm_mday;
  xmsg.date_arrived.date.mo=dt->tm_mon+1;
  xmsg.date_arrived.date.yr=dt->tm_year;
  xmsg.date_arrived.time.ss=dt->tm_sec;
  xmsg.date_arrived.time.mm=dt->tm_min;
  xmsg.date_arrived.time.hh=dt->tm_hour+1;

	xmsg.replyto=0;

	strcpy((char *)subj, (s_Subject.c_str()));
	strcpy((char *)from, (s_From.c_str()));
	strcpy((char *)to,(s_To.c_str()));
	if (sent)
		xmsg.attr=MSGPRIVATE|MSGLOCAL|MSGSENT;
	else
		xmsg.attr=MSGPRIVATE|MSGLOCAL;

	strcpy((char *)xmsg.from,(char *)from);
        strcpy((char *)xmsg.to,(char *)to);
        strcpy((char *)xmsg.subj,(char *)subj);
	xmsg.orig.zone=F_From.zone;
        xmsg.orig.net=F_From.net;
        xmsg.orig.node=F_From.node;
        xmsg.orig.point=F_From.point;
        xmsg.dest.zone=F_To.zone;
        xmsg.dest.net=F_To.net;
        xmsg.dest.node=F_To.node;
        xmsg.dest.point=F_To.point;
      	xmsg.attr=d_Attr ;
	i_TextLen=s_MsgText.length()+1;
	i_CtrlLen=s_Ctrl.length()+1;
	
	MsgWriteMsg(hmsg, 0, &xmsg, const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(s_MsgText.c_str())), i_TextLen, i_TextLen+i_CtrlLen, i_CtrlLen, (byte *)const_cast<char*>(s_Ctrl.c_str()));
	return 0;
}


int CMsg::WriteAttr()
{
  if (read) {
  	xmsg.attr=xmsg.attr|MSGREAD;
  }
  int rc = MsgWriteMsg(hmsg, 0, &xmsg, NULL, 0,0,0,NULL);
	return rc;
}


CMask CMsg::GetMask()
{
	CMask msk;
	msk.s_Sender=s_From;
	msk.s_Recipient=s_To;
	msk.s_Subject=s_Subject;
	msk.F_From=F_From;
	msk.F_To=F_To;
	msk.d_Flags=d_Attr;
	return msk;
}

int CMsg::Close()
{
	if (hmsg!=NULL && deleted==false) MsgCloseMsg(hmsg);
	return 0;
}


int CMsg::Delete(HAREA Area)
{
	if (hmsg!=NULL) {
		MsgKillMsg(Area, i_number);
/*		MsgCloseMsg(hmsg);
		MsgKillMsg(Area, i_number);*/
	}
	return 0;
}
