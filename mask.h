#ifndef _MASK_H_
#define _MASK_H_
#undef min
#undef max
#include <vector>
#include <string>
extern "C"
{
#include <smapi/msgapi.h>
}
#include "ftnaddr.h"
#include "bastypes.h"

using namespace std;

class CMask
{
	public:
		/* methods */
		CMask(string sndr, string recpt, CFtnAddr frm, CFtnAddr to, string subj,
				dword flgs);
		CMask();
		~CMask();
		CMask(const CMask& msk);
		/* operators */
		CMask& operator=(const CMask& msk);
		CMask& operator=(const string& str);
		bool operator==(const CMask& msk);
		/* variables */
		string			s_Sender;
		string			s_Recipient;
		CFtnAddr		F_From;
		CFtnAddr		F_To;
		string			s_Subject;
		dword			d_Flags;
		string			s_Flags;
		vector<int>		i_match;
		/* attributes */
		short    priv;
		short	 crash;
		short	 received;
 		short	 sent;
		short	 attach;
		short	 forward;
		short	 orphan;
		short	 kill;
		short	 local;
		short	 hold;
		short	 file_request;
		short	 scanned;
		short	 direct;
		short	 update_request;
		short	 rrq;
		short	 return_receipt;
		short	 immediate;
		short	 TFS;
		short	 KFS;
		short	 archive_sent;
		short	 is_return_receipt;
		short	 confirm_receipt_request;
		int parseAttr(string s_Flags);
		bool compareAttr(dword attr);
};
#endif
