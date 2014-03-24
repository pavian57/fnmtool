#ifndef _AREA_H_
#define _AREA_H_
#include <vector>
#include <string>
#ifdef IS_NOTGCC
#include <stdio.h>
#else
#include <cstdio>
#endif
#include "mask.h"
#include "op.h"
#include "action.h"
extern "C" 
{
#include <smapi/msgapi.h>
}



int initApi();
int closeApi();

class CArea
{
	public:
		/* methods */
		int 		Open(string Path);
		int		Open();
		int 		Close();
		int		Scan(vector<COperation> M_ScanFor, vector<CAction> Execute, unsigned int start, unsigned int stop);
		HAREA		GetArea();
		CArea();
		CArea(const CArea& ar);
		~CArea();
		/* variables */
		string		s_Path;
		dword		i_msgNum;
		dword		i_type;
		protected:
		HAREA		a_Area;
};
#endif
