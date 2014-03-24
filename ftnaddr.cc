/******************************************************************************/
/*   _______ _______                                                          */
/*   |     | |     |            This file is part of the TTrack distribution  */
/*   --| |-- --| |--                                                          */
/*     | |     | |               Copyright (C) 1999 Michael Mleczko           */
/*     |_|     |_| RACK                                                       */
/*                                                                            */
/*     Fido     : 2:2444/1101.47                                              */
/*     Internet : 667@atomicfront.de                                          */
/*     Snail    : Hustadtring 63, 44801 Bochum                                */
/*                                                                            */
/******************************************************************************/
#include <string>
#include <fstream>
#include <iostream>
extern "C"
{
#include <smapi/msgapi.h>
}
#include "ftnaddr.h"
using namespace std;

CFtnAddr::CFtnAddr()
{
	// initialize with default values
	zone = 65535;
        anyzone = false;
	net = 65535;
	anynet = false;
	node = 65535;
	anynode = false;
	point = 65535;
	anypoint = false;
	domain = "";
	anydomain = false;
}

CFtnAddr::CFtnAddr(const CFtnAddr & in)
{
	// initialize with values from in
	zone = in.zone;
        anyzone = in.anyzone;
	net = in.net;
	anynet = in.anynet;
	node = in.node;
	anynode = in.anynode;
	point = in.point;
        anypoint = in.anypoint;
	domain = in.domain;
	anydomain = in.anydomain;
}

CFtnAddr::CFtnAddr(const string& str)
{
	// get data from string
	this->getFromStr(str);
}

CFtnAddr::~CFtnAddr()
{
}
	
ostream&	operator<<(ostream& os, CFtnAddr out)
{
	os << out.Zone() << ":" << out.Net() << "/" << out.Node() << "." << out.Point() << "@" << out.Domain();
	return os;
}

string& operator>>(string& str, CFtnAddr& in)
{
	in.getFromStr(str);
	return str;
}

istream&	operator>>(istream& is, CFtnAddr& in)
{
   string str;
	is >> str;
	in.getFromStr(str);
	return is;
}

CFtnAddr &CFtnAddr::operator=(const CFtnAddr &in)
{
	zone = in.zone;
        anyzone = in.anyzone;
	net = in.net;
        anynet = in.anynet;
	node = in.node;
        anynode = in.anynode;
	point = in.point;
        anypoint = in.anypoint;
	domain = in.domain;
	anydomain = in.anydomain;

	return (*this);
}

CFtnAddr &CFtnAddr::operator=(const string &str)
{
	getFromStr(str);
	return (*this);
}

CFtnAddr &CFtnAddr::operator=(const NETADDR &addr)
{
	zone = addr.zone;
        anyzone = false;
	net = addr.net;
	anynet = false;
	node = addr.node;
	anynode = false;
	point = addr.point;
	anypoint = false;
	domain = "";
	anydomain = false;

	return (*this);
}

CFtnAddr &CFtnAddr::operator=(const char *str)
{
  string str2;

  str2 = str;
  getFromStr(str2);

  return (*this);
}



bool operator==(const CFtnAddr& arg1, const CFtnAddr& arg2)
{
  bool equal = true;

  if ((arg1.zone != arg2.zone) && !arg1.anyzone && !arg2.anyzone)
      equal=false;
  if ((arg1.net != arg2.net) && !arg1.anynet && !arg2.anynet)
      equal=false;
  if ((arg1.node != arg2.node) && !arg1.anynode && !arg2.anynode)
      equal=false;
  if ((arg1.point != arg2.point) && !arg1.anypoint && !arg2.anypoint)
      equal=false;
  if ((arg1.domain != arg2.domain) && !arg1.anydomain && !arg2.anydomain)
      equal=false;

  return equal;
}

bool operator==(const CFtnAddr &arg1, const char * arg2)
{
  bool equal=false;
  CFtnAddr addr;

  addr = arg2;
  if (arg1 == addr) equal=true;
  return equal;
}

bool CFtnAddr::isPoint() const
{
	if (point == 0) return false;
	else return true;
}
	
void CFtnAddr::getFromStr(const string& str)
{
   string Z, Ne, No, P, Dom;
	bool ZDone=false;
	bool NeDone=false;
	bool NoDone=false;
	bool PDone=false;
	bool DomDone=false;

	for (unsigned int i=0;i<str.length();i++)
	{
		if (str[i]==':') { ZDone=true; i++; }
		if (str[i]=='/') { NeDone=true; i++; }
		if (str[i]=='.' && PDone==false) { NoDone=true; i++; }
		if (str[i]=='@') { PDone=true; i++; }
		
		if (!ZDone)
			Z+=str[i];
		else
		if (!NeDone)
			Ne+=str[i];
		else
		if (!NoDone)
			No+=str[i];
		else
		if (!PDone)
			P+=str[i];
		else
		if (!DomDone)
			Dom+=str[i];

	}
	zone=atoi(Z.c_str());
	net=atoi(Ne.c_str());
	node=atoi(No.c_str());
	point=atoi(P.c_str());
	domain=Dom;

	anyzone = (Z == "*") || (Z == "");
	anynet = (Ne == "*") || (Ne == "");
	anynode = (No == "*") || (No == "");
	anypoint = (P == "*") || (P == "");
	anydomain = (Dom == "*") || (Dom == "");
	
}

int CFtnAddr::Zone() const
{
	return zone;
};

int CFtnAddr::Net() const
{
	return net;
}

int CFtnAddr::Node() const
{
	return node;
}

int CFtnAddr::Point() const
{
	return point;
}

string CFtnAddr::Domain() const
{
	return domain;
};
										
