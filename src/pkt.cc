#include <string>
#ifdef __GNUC__
#include <cstdio>
#else
#include <stdio.h>
#endif
#include "global.h"
#include "pkt.h"
#include "version.h"
extern "C" 
{
//#include "../smapi/progprot.h"
#include <smapi/msgapi.h>
};

using namespace std;


int CPkt::openPkt()
{
	string completename;
	char name[9];
	sprintf(name, "%04x%04x.out", toNode.net, toNode.node);
	if (dir=="") completename=cfg->s_Outbound + '/' + name;
	else completename=dir + '/' + name;
	f_pkt=fopen(completename.c_str(), "r");
	if (f_pkt==NULL) newPkt=true;
	else
	{
		fclose(f_pkt);
		newPkt=false;
	}

	if (newPkt==false) 
	{
                f_pkt=fopen(completename.c_str(), "ab");
                appendMessage();
	}
	else
	{
		f_pkt=fopen(completename.c_str(), "wb");
                writeHeader();
                appendMessage();

	}
	if (f_pkt==NULL) return -1;
	return 0;
}

int CPkt::writeHeader()
{
	time_t tm;
	struct tm *pkttime;

	/* write node info */
	fwrite(&fromNode.node, 2, 1, f_pkt);
        fwrite(&toNode.node, 2, 1, f_pkt);

	/* write time info */
	time(&tm);
	pkttime=localtime(&tm);
	int year=1900+pkttime->tm_year;
	fwrite(&year, 2, 1, f_pkt);
	fwrite(&pkttime->tm_mon, 2, 1, f_pkt);
	fwrite(&pkttime->tm_mday, 2, 1, f_pkt);
	fwrite(&pkttime->tm_hour, 2, 1, f_pkt);
	fwrite(&pkttime->tm_min, 2, 1, f_pkt);
	fwrite(&pkttime->tm_sec, 2, 1, f_pkt);
	/* write baud info */
	fputc(0, f_pkt); fputc(0, f_pkt);

	/* write pkt version */
	int pktver=2;
	fwrite(&pktver, 2, 1, f_pkt);

	/* write net information */
        fwrite(&fromNode.net, 2, 1, f_pkt);
        fwrite(&toNode.net, 2, 1, f_pkt);

	/* write low-order productcode and major revision */
	unsigned char l_pcode=0xfe;
	unsigned char h_pcode=0x00;
	
	unsigned char minor_revision=1;
	unsigned char major_revision=0;

	fputc(l_pcode, f_pkt);
	fputc(major_revision, f_pkt);
	
	/* write password info */
	for (unsigned int i=0;i<8;i++) 
	{	
		if (password.length()>i)
			fputc(password[i], f_pkt);
		else
			fputc('\0', f_pkt);
	}
	
	/* write zone info */
        fwrite(&fromNode.zone, 2, 1, f_pkt);
        fwrite(&toNode.zone, 2, 1, f_pkt);
	fwrite(&fromNode.zone, 2, 1, f_pkt);
	
	/* write CW copy */
	int capabilityWord=1;
	int dummy=capabilityWord / 256;
	fputc(dummy, f_pkt);
	dummy= capabilityWord%256;
	fputc(dummy, f_pkt);
//	fwrite(&capabilityWord, 2, 1, f_pkt);

	/* write high-order productCode and minor revision */
	fputc(h_pcode, f_pkt);
	fputc(minor_revision, f_pkt);

	/* write CW */
        fwrite(&capabilityWord, 2, 1, f_pkt);
	
	/* write zone info */
        fwrite(&fromNode.zone, 2, 1, f_pkt);
        fwrite(&toNode.zone, 2, 1, f_pkt);
	
	/* write point info */
        fwrite(&fromNode.point, 2, 1, f_pkt);
        fwrite(&toNode.point, 2, 1, f_pkt);

	/* write product specific data */
	int data=0;
	fwrite(&data, 4, 1, f_pkt);

	/* write double \0 to finalize packet */
	fputc(0, f_pkt);
	fputc(0, f_pkt);

	return 0;
}

int CPkt::appendMessage()
{
	
	/* go two chars back to kill last \0 \0 */	
	fseek(f_pkt, -2, SEEK_END);

	/* msg header */
	fputc(2, f_pkt);
	fputc(0, f_pkt);
	
	/* write header */

	/* write node info */
	fwrite(&Message.F_From.node, 2, 1, f_pkt);
        fwrite(&Message.F_To.node, 2, 1, f_pkt);

	/* write net info */
        fwrite(&Message.F_From.net, 2, 1, f_pkt);
        fwrite(&Message.F_To.net, 2, 1, f_pkt);
	
	/* write attributes */
	fwrite(&Message.d_Attr, 2, 1, f_pkt);

	/* write cost */
	int cost=12;
	fwrite(&cost, 2, 1, f_pkt);
	
	/* NOTE: REWRITE THIS CODE! write real msg date instead of current!!!! */
	/* date time */
	time_t tm;
	struct tm *dt;
        time(&tm);
        dt=localtime(&tm);
	char datetime[20];
	if (dt->tm_mon==0) sprintf(datetime, "%02u Jan %02u %02u:%02u:%02u", 
			dt->tm_mday, dt->tm_year, dt->tm_hour, dt->tm_min, dt->tm_sec);
        if (dt->tm_mon==1) sprintf(datetime, "%02u Feb %02u %02u:%02u:%02u",
                        dt->tm_mday, dt->tm_year, dt->tm_hour, dt->tm_min, dt->tm_sec);
        if (dt->tm_mon==2) sprintf(datetime, "%02u Mar %02u %02u:%02u:%02u",
                        dt->tm_mday, dt->tm_year, dt->tm_hour, dt->tm_min, dt->tm_sec);
        if (dt->tm_mon==3) sprintf(datetime, "%02u Apr %02u %02u:%02u:%02u",
                        dt->tm_mday, dt->tm_year, dt->tm_hour, dt->tm_min, dt->tm_sec);
        if (dt->tm_mon==4) sprintf(datetime, "%02u May %02u %02u:%02u:%02u",
                        dt->tm_mday, dt->tm_year, dt->tm_hour, dt->tm_min, dt->tm_sec);
        if (dt->tm_mon==5) sprintf(datetime, "%02u Jun %02u %02u:%02u:%02u",
                        dt->tm_mday, dt->tm_year, dt->tm_hour, dt->tm_min, dt->tm_sec);
        if (dt->tm_mon==6) sprintf(datetime, "%02u Jul %02u %02u:%02u:%02u",
                        dt->tm_mday, dt->tm_year, dt->tm_hour, dt->tm_min, dt->tm_sec);
        if (dt->tm_mon==7) sprintf(datetime, "%02u Aug %02u %02u:%02u:%02u",
                        dt->tm_mday, dt->tm_year, dt->tm_hour, dt->tm_min, dt->tm_sec);
        if (dt->tm_mon==8) sprintf(datetime, "%02u Sep %02u %02u:%02u:%02u",
                        dt->tm_mday, dt->tm_year, dt->tm_hour, dt->tm_min, dt->tm_sec);
        if (dt->tm_mon==9) sprintf(datetime, "%02u Oct %02u %02u:%02u:%02u",
                        dt->tm_mday, dt->tm_year, dt->tm_hour, dt->tm_min, dt->tm_sec);
        if (dt->tm_mon==10) sprintf(datetime, "%02u Nov %02u %02u:%02u:%02u",
                        dt->tm_mday, dt->tm_year, dt->tm_hour, dt->tm_min, dt->tm_sec);
        if (dt->tm_mon==11) sprintf(datetime, "%02u Dec %02u %02u:%02u:%02u",
                        dt->tm_mday, dt->tm_year, dt->tm_hour, dt->tm_min, dt->tm_sec);
	if (strlen(datetime)<20)
		for (int i=strlen(datetime);i<19;i++) datetime[i]=0;
	for (int i=0;i<19;i++) fputc(datetime[i], f_pkt);

	/* write sender name */
	if (Message.s_To.length()<36) 
	{
	        int j = Message.s_To.length();
		for (int i=0;i<j;i++)
			fputc(Message.s_To[i], f_pkt);
		fputc(0, f_pkt);
	}
	else 
	{
		for (int i=0;i<36;i++) fputc(Message.s_To[i], f_pkt);
		fputc(0, f_pkt);
	}
	
	/* write recipient name */
        if (Message.s_From.length()<36) 
	{
	        int j = Message.s_From.length();
                for (int i=0;i<j;i++)
                        fputc(Message.s_From[i], f_pkt);
                fputc(0, f_pkt);
	}
        else
        {
                for (int i=0;i<36;i++) fputc(Message.s_From[i], f_pkt);
                fputc(0, f_pkt);
        }
 
	/* write subject line */
        if (Message.s_Subject.length()<72) 
	{
	        int j = Message.s_Subject.length();
                for (int i=0;i<j;i++)
                        fputc(Message.s_Subject[i], f_pkt);
                fputc(0, f_pkt);
 
	}
        else
        {
                for (int i=0;i<72;i++) fputc(Message.s_Subject[i], f_pkt);
                fputc(0, f_pkt);
        }

	/* write messagetext */
	string msgText;
	/* TODO: check if kludges are already present, when exporting */	
	/* if neccessary write point and zone info */
	msgText+="\001";
	for (unsigned int i=1;i<Message.s_Ctrl.length();i++)
	{
		if (Message.s_Ctrl[i]!='\001')
			msgText+=Message.s_Ctrl[i];
		else 
			msgText+="\r\001";
	}
	msgText+='\r';
	msgText+='\r';
	if (msgText[msgText.length()-1]!='\0') msgText.erase(msgText.length()-1,1);
/*	if ((Message.F_From.point!=0) && (msgText.find("FMPT")==-1))
	{
		char fmpt[6];
		sprintf(fmpt, "%i", Message.F_From.point);
		msgText+="\001FMPT ";
		msgText+=fmpt;
		msgText+=0x0d;
	}
        if ((Message.F_To.point!=0) && !msgText.find("TOPT")==-1)
        {
                char topt[6];
                sprintf(topt, "%i", Message.F_To.point);
                msgText+="\001TOPT ";
                msgText+=topt;
		msgText+=0x0d;
        }*/
        if (strstr(msgText.c_str(), "INTL ")==NULL)
        {
                char intl[48];
                sprintf(intl, "%i:%i/%i.%i %i:%i/%i.%i", 
			Message.F_From.zone, Message.F_From.net, Message.F_From.node, Message.F_From.point,
			Message.F_To.zone, Message.F_To.net, Message.F_To.node, Message.F_To.point);
                msgText+="\001INTL ";
                msgText+=intl;
		msgText+=0x0d;
        }

	/* write message text */
	msgText+=Message.s_MsgText;	
	char buf[128];
        sprintf(buf, "\001Via %u:%u/%u.%u @%04u%02u%02u.%02u%02u%02u.UTC %s %s",
           Message.F_From.zone, Message.F_From.net, Message.F_From.node, Message.F_From.point,
           dt->tm_year + 1900, dt->tm_mon + 1, dt->tm_mday, dt->tm_hour+1, dt->tm_min, dt->tm_sec, PRGNAME, VERSION);
 	
	msgText+=buf;
	
	fputs(msgText.c_str(), f_pkt);
	/* write two \0 */
	fputc(0, f_pkt);

	return 0;
}

int CPkt::closePkt()
{
	if (f_pkt!=NULL) fclose(f_pkt);
	return 0;
}


int CPkt::create()
{
	openPkt();
	closePkt();
	return 0;
}
