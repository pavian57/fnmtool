#ifndef _PKT_H_
#define _PKT_H_
#include <string>
#ifndef IS_NOTGCC
#include <cstdio>
#else
#include <stdio.h>
#endif
#include "mask.h"
#include "msg.h"
#include "ftnaddr.h"

class CPkt
{
	public:
	   CMsg Message;
	   CFtnAddr toNode;
	   CFtnAddr fromNode;
	   string password;
	   string dir;	   
	   int create();
	protected:
	   FILE *f_pkt;
	   bool newPkt;
	   int openPkt();
	   int writeHeader();
	   int appendMessage();
	   int closePkt();
};
#endif
