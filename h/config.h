#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef __GNUC__
#include <string>
#include <cstdio>
#include <cstdlib>
#else
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#endif
#include <vector>
#include "area.h"
#include "mask.h"
#include "action.h"
#include "scndr.h"
#include "op.h"
#include "addrlist.h"

typedef struct
{
	string s_Token;
	string s_RestOfLine;
} param;

class CConfig
{
	public:
		CConfig();
		CFtnAddr F_Home;
		int	silent;
		int	debug;
		string s_Home;
		string s_Netmail;
		string s_Outbound;
		string s_Inbound;
		string s_Log;
		string s_CharsetFtn;
		string s_CharsetRfc;
		vector<CScandir> S_Scandir;
		vector<COperation> O_Op;
		vector<CAction> A_Action;
		vector<CAddresslist> A_Addrlist;
	protected:
		bool openConfig(string path);
		string getLine();
		param getParam(string line);
		bool closeConfig();
		FILE * cfgFile;
};
#endif
