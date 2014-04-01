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
#ifndef _NODE_H
#define _NODE_H

#include <cstring>

using namespace std;



class CFtnAddr
{
	public:				/*------- data ------*/
		int		zone;
		bool            anyzone;
		int		net;
		bool            anynet;
		int		node;
		bool            anynode;
		int		point;
		bool            anypoint;
		string	        domain;
		bool            anydomain;

	public:					/*-- constructors ---*/
		CFtnAddr();
		CFtnAddr(const CFtnAddr &in);
		CFtnAddr(const string &str);

	public:					/*--- destructors ---*/
		~CFtnAddr();
	
	public:					/*---- operators ----*/
		/* output */
		friend ostream&	operator<<(ostream& os, CFtnAddr out);

		/* input */
		friend string&		operator>>(string &str, CFtnAddr &in);
		friend istream&	operator>>(istream &is, CFtnAddr &in);
		
		/* assignment */
		CFtnAddr&			operator=(const CFtnAddr &in);
		CFtnAddr&			operator=(const string &str);
		CFtnAddr&			operator=(const char * str);
		CFtnAddr&			operator=(const NETADDR &addr);

		/* comparison */
		friend bool			operator==(const CFtnAddr &arg1,
							   const CFtnAddr &arg2);
		friend bool			operator==(const CFtnAddr &arg1,
							   const char * arg2);

	public:					/*------- misc -------*/
		bool		isPoint() const;
		int		Zone() const;
		int		Net() const;
		int 		Node() const;
		int		Point() const;
		string          Domain() const;
	
	private:
		void		getFromStr(const string &str);

};
#endif
		
