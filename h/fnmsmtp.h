#ifndef _FNMSMTP_H_
#define _FNMSMTP_H_

#ifdef __GNUC__
#include <cstdio>
#else
#include <stdio.h>
#endif
#include<iostream>    //cout
#ifdef LINUX
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include<netdb.h> //hostent
#else
#include <winsock2.h>
#endif

#include <string>
#include <time.h>
#include "version.h"

using namespace std;
class CSmtp
{
private:
    string address;
    string s_ServerMessage;
    string s_LocalHostname;
    string s_UserAgent;
    string s_ContentType;
    string DOT;
    string CRLF;
       
    struct sockaddr_in server;
		int i_Socket;
    int open_con();
		int close_con();
    int send_data(string str);
    int receive();
    
	
	int i_Port;
	string s_MailServer;
	string s_MailFrom;
	string s_MailTo;
	string s_Subject;
	string s_MailText;
	string s_ErrorMsg;
	string s_Message;
	int i_ReturnCode;	

public:	
//Methods	
	CSmtp();
	~CSmtp();
	int setPort(int port);
	int setContentType(string str);
	int setMailServer(string str);
	int setMailFrom(string str);
	int setMailTo(string str);
	int setMailSubject(string str);
	int setMailText(string str);
	string getErrorMessage();
	int sendmail();
};
#endif

