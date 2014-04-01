#ifndef _RFCMAIL_H_
#define _RFCMAIL_H_

#ifdef __GNUC__
#include <cstdio>
#else
#include <stdio.h>
#endif
#include <string>
#undef min
#undef max
#include <vector>

extern "C"
{
#include <smapi/msgapi.h>
}
#include "ftnaddr.h"
#include "bastypes.h"
#include "area.h"


using namespace std;

class CRfcmail
{
        public:
                CRfcmail();        
                ~CRfcmail();
                
                int	parse(string sender, string recipient, string rawMail);                
                int sendmail();
                string r_From;
                string r_To;
                string s_From;
                string s_To;
             		CFtnAddr	F_From;
                CFtnAddr	F_To;             		
                HAREA 	Area;
                		
                string s_Subject;
                           
                string s_Message;
                string s_Ctrl;
};
#endif
