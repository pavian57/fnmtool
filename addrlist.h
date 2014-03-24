#ifndef _ADDRLIST_H_
#define _ADDRLIST_H_

class CAddresslist
{
        public:
                CAddresslist(string s_AddrStr);
                ~CAddresslist();
                string rfcAddr;
                string fidoName;
                CFtnAddr F_From;
                int firstAddr;
                int lastAddr;
};
#endif
