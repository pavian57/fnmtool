#ifndef _MSG_H_
#define _MSG_H_
#include <string>
#include "area.h"
#include "bastypes.h"
#include "time.h"

class CMsg
{
	public:
		/* data */
		string s_From;
		string s_To;
		string s_Subject;
		string s_Charset;
		CFtnAddr F_From;
		CFtnAddr F_To;
		dword d_Attr;
		_stamp t_DateWritten;
		_stamp t_DateArrived;
		// CDate D_Written;     		/* Date functions not implemented yet! */
		// CDate D_Arrvd;			/* Date functions not implemented yet! */
		string s_Ctrl;
		string s_MsgText;
		int 	i_CtrlLen;
		int 	i_TextLen;
		long	i_Size;
		int 	i_number;
		char ftsc_date[20];
		/* methods */
		CMsg();
		CMsg(int i_num);
		int Open(int i_num, HAREA area);
		int New(HAREA area);
		int Write();
		int WriteAttr();
		int Close();
		int Delete(HAREA Area);
		CMask	GetMask();
		bool deleted;
		bool sent;
		bool read;
	protected:
		HMSG	hmsg;
		XMSG	xmsg;
		
		// none yet;
};		
#endif
