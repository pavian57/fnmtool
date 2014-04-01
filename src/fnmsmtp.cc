#ifndef IS_OLDGCC
#include <iostream>
#include <sstream>
#include <string>
#else
#include <iostream.h>
#endif
#ifdef __GNUC__
#include <cstdio>
#include <cerrno>
#else
#include <errno.h>
#include <stdio.h>
#endif
#include <time.h>
#include <sys/stat.h>
#include <libgen.h>
#include <stdexcept>
#include <string.h>
#include <unistd.h>
#include "fnmsmtp.h"
#include "global.h"







using namespace std;

CSmtp::CSmtp()
{
    s_MailServer.clear();
    i_Socket = -1;
    i_Port = 25;
    address = "";
		s_UserAgent = "X-Mailer: " ;
		s_UserAgent += PRGNAME;
    s_UserAgent += "/";
		s_UserAgent += OS;
    s_UserAgent += " ";
		s_UserAgent += VERSION;
    s_ContentType = "Content-Type: text/plain;charset=UTF-8";
    DOT = ".";
    CRLF = "\r\n";

}

CSmtp::~CSmtp()
{
}

int CSmtp::setPort(int port=25)
{
  i_Port = port;
  s_ErrorMsg.clear();
  i_ReturnCode = 0;
	return i_ReturnCode;
}

int CSmtp::setContentType(string str)
{
  s_ContentType = "Content-Type: text/plain;charset=" + str;
  s_ErrorMsg.clear();
  i_ReturnCode = 0;
  return i_ReturnCode;
}


int CSmtp::setMailServer(string str)
{
  s_MailServer = str;
  s_ErrorMsg.clear();
  i_ReturnCode = 0;
  return i_ReturnCode;
}

int CSmtp::setMailFrom(string str)
{
  s_MailFrom = str;
  s_ErrorMsg.clear();
  i_ReturnCode = 0;
  return i_ReturnCode;
}

int CSmtp::setMailTo(string str)
{
  s_MailTo = str;
  s_ErrorMsg.clear();
  i_ReturnCode = 0;
  return i_ReturnCode;
}

int CSmtp::setMailSubject(string str)
{
  s_Subject = str;
  s_ErrorMsg.clear();
  i_ReturnCode = 0;
  return i_ReturnCode;
}

int CSmtp::setMailText(string str)
{
  s_MailText = str;
  s_ErrorMsg.clear();
  i_ReturnCode = 0;
  return i_ReturnCode;
}


string CSmtp::getErrorMessage()
{
	return s_ErrorMsg;
}

int CSmtp::sendmail()
{
  s_ErrorMsg.clear();
  i_ReturnCode = 0;
  if (i_Port < 0) i_Port = 25;
  if (s_MailServer.empty()) s_MailServer = "localhost";
	
	i_ReturnCode = open_con();
  if (i_ReturnCode != 0) {
    //Problem
  }

  i_ReturnCode = receive();
	if (i_ReturnCode != 220) {
		//Problem 
	}
  send_data("HELO "+s_LocalHostname+"\r\n");
	i_ReturnCode = receive();
  if (i_ReturnCode != 250) {
    //Problem
  }
 
	send_data("MAIL FROM:  "+s_MailFrom+"\r\n");
  i_ReturnCode = receive();
  if (i_ReturnCode != 250) {
    //Problem
  }

	send_data("RCPT TO:  "+s_MailTo+"\r\n");
  i_ReturnCode = receive();
  if (i_ReturnCode != 250) {
    //Problem
  }

  send_data("DATA\r\n");
  i_ReturnCode = receive();
  if (i_ReturnCode != 354) {
    //Problem
  }
	send_data("From: "+s_MailFrom+CRLF);
	send_data("To: "+s_MailTo+CRLF);
  send_data("X-Sender: fnmtool@"+s_LocalHostname + CRLF);
	send_data(s_UserAgent + CRLF);
	send_data(s_ContentType + CRLF);
  send_data("Subject:"+s_Subject+CRLF+CRLF);
  send_data(s_MailText);
  send_data(CRLF+DOT+CRLF);

  i_ReturnCode = receive();
  if (i_ReturnCode != 250) {
    //Problem
  }

  send_data("QUIT"+CRLF);
	i_ReturnCode = receive();
  if (i_ReturnCode != 221) {
    //Problem
  }

	i_ReturnCode = close_con();
  if (i_ReturnCode != 0) {
    //Problem
  }
  return i_ReturnCode;
}

int CSmtp::open_con()
{
	s_ErrorMsg.clear();
	i_ReturnCode = 0;

	char local_hostname[64];
  //determine local host name
  i_ReturnCode = gethostname(local_hostname, sizeof(local_hostname));
  if (i_ReturnCode == -1) {
     strcpy(local_hostname, "myhost.domain");
  }
 	s_LocalHostname = local_hostname;

	if(i_Socket == -1) {
        //Create socket
		i_Socket = socket(AF_INET , SOCK_STREAM , 0); 
		if (i_Socket == -1) {
            s_ErrorMsg = "Could not create socket";
			i_ReturnCode = -1;
        }
	} else    {   /* OK , nothing */  }

    //setup address structure
	if(inet_addr(s_MailServer.c_str()) == INADDR_NONE) {
		struct hostent *he;
    struct in_addr **addr_list;

        //resolve the hostname, its not an ip address
    if ( (he = gethostbyname( s_MailServer.c_str() ) ) == NULL) {
            //gethostbyname failed
			i_ReturnCode = -1;
      s_ErrorMsg = "Failed to resolve hostname";
      return i_ReturnCode;
		}

        //Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
    addr_list = (struct in_addr **) he->h_addr_list;

    for(int i = 0; addr_list[i] != NULL; i++) {
            //strcpy(ip , inet_ntoa(*addr_list[i]) );
			server.sin_addr = *addr_list[i];

			s_Message = address + " resolved to " + inet_ntoa(*addr_list[i]);

			break;
        }
    } else {
			server.sin_addr.s_addr = inet_addr( s_MailServer.c_str() );
	}

  server.sin_family = AF_INET;
  server.sin_port = htons( i_Port );

    //Connect to remote server
  i_ReturnCode = connect(i_Socket , (struct sockaddr *)&server , sizeof(server));
  if (i_ReturnCode != 0) {
    i_ReturnCode = errno;
    s_ErrorMsg = strerror(errno);
		return i_ReturnCode;
	}
	return i_ReturnCode;
}

int CSmtp::close_con()
{
  s_ErrorMsg.clear();
  i_ReturnCode = 0;
 	i_ReturnCode = shutdown(i_Socket,SHUT_RDWR);
  if (i_ReturnCode != 0) {
		i_ReturnCode = errno;
		s_ErrorMsg = strerror(errno);
	}
  return i_ReturnCode;
}


int CSmtp::send_data(string str)
{
	s_ErrorMsg.clear();
	i_ReturnCode = 0;

	if( send(i_Socket , str.c_str() , strlen( str.c_str() ) , 0) < 0) {
		i_ReturnCode = -1;
		s_ErrorMsg = "Send failed";
    return i_ReturnCode;
	}
	if (cfg->debug)
		cerr << "rc=" << i_ReturnCode << ": send_data=" << str;

	return i_ReturnCode;
}

int CSmtp::receive()
{
  s_ErrorMsg.clear();
	s_ServerMessage.clear();
  i_ReturnCode = 0;
	int len=128;

	char *recvData = new char[len + 1];
	int lenRead = recv(i_Socket, recvData, len, 0);
	if (lenRead < len)
    recvData[lenRead] = '\0';
	else
    recvData[len] = '\0'; 

  s_ServerMessage = recvData;
	int pos =  s_ServerMessage.find(" ");
  istringstream ( s_ServerMessage.substr(0,pos) ) >> i_ReturnCode;
	if (cfg->debug)
		cerr << "rc=" << i_ReturnCode << ": recvData=" << recvData << ": lenRead=" << lenRead <<  endl;

  return i_ReturnCode;
}

