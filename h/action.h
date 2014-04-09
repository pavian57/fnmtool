#ifndef _ACTION_H_
#define _ACTION_H_
#include "area.h"

extern "C"
{
#include <smapi/msgapi.h>
}

class CAction
{
	public:
		CAction();
		CAction(const CAction & act);
		~CAction();
		HAREA Area;
		string s_Area;
		int msgnum;
		unsigned long seed;             /* MSGID seed        */
		int run();
		string param;
};

class CCopyAction:public CAction
{
	public:
		string destarea;
		int run();
};

class CMoveAction:public CAction
{
	public:
		string destarea;
		int run();
};

class CDeleteAction:public CAction
{
	public:
		int run();
};

class CTwitAction:public CAction
{
	public:
		int run();
};


class CFileAction:public CAction
{
	public:
		string s_Filename;
		int run();
	protected:
		FILE *f_txtFile;
		
};
		
class CHdrFileAction:public CAction
{
	public:
		string s_Filename;
		int run();
	protected:
		FILE *f_txtFile;
};

class CSemaphoreAction:public CAction
{
	public:
		string s_FileName;
		int run();
	protected:
		FILE *f_semFile;
};

class CRewriteAction:public CAction
{
	public:
		CMask M_rewriteTo;
		int run();
};

class CUUCPRewriteAction:public CAction
{
	public:
		CMask M_rewriteTo;
		int run();
};

class CBounceAction:public CAction
{
	public:
		CFtnAddr A_FromAddress;
		string param;
		string s_BounceText;
		int run();
	protected:
		FILE *f_bounceTxt;
		string InsertFields();
		string bounceFname;
};

class CPingPongAction:public CAction
{
	public:
		CFtnAddr A_FromAddress;
		string param;
		string s_BounceText;
		int run();
	protected:
		FILE *f_bounceTxt;
		string InsertFields();
		string bounceFname;
};

class CEmptyBounceAction:public CAction
{
	public:
		CFtnAddr A_FromAddress;
		string s_BounceText;
		int run();
	protected:
		FILE *f_bounceTxt;
		string InsertFields();
};

class CHdrBounceAction:public CAction
{
	public:
		CFtnAddr A_FromAddress;
		string s_BounceText;
		int run();
	protected:
		FILE *f_bounceTxt;
		string InsertFields();
};

class CAddNoteAction:public CAction
{
	public:
		string s_Note;
		int run();
};

class CForwardAction:public CAction
{
	public:
		CMask M_fwdMask;
		int run();
};

class CMakeMsgAction:public CAction
{
	public:
		string s_bodyText;
		CMask M_hdrMask;
		int run();
	protected:
		FILE * f_bodyText;
};

class CChangePathAction:public CAction
{
	public:
		string s_filePath;
		int run();
};

class CChangePathMoveAction:public CAction
{
	public:
		string s_filePath;
		int run();
	protected:
		FILE * f_toMove;
		int MoveFiles(string s_Path);
};

class CEchoCopyAction:public CAction
{
	public:
		CFtnAddr F_From;
		string s_Area;
		string s_Seenby;
		int run();
};

class CEchoMoveAction:public CAction
{
	public:
		CFtnAddr F_From;
		string s_Area;
		string s_Seenby;
		int run();
};

class CIgnoreAction:public CAction
{
	public:
		int run();
};

class CPackmailAction:public CAction
{
	public:
		CFtnAddr F_From;
		CFtnAddr F_To;
		int run();
	protected:
		FILE *f_pkt;
};

class CMovemailAction:public CAction
{
	public:
		CFtnAddr F_From;
		CFtnAddr F_To;
		int run();
	protected:
		FILE *f_pkt;
};

class CEmailAction:public CAction
{
	public:
		int run();
};

	
class CDisplayAction:public CAction
{
	public:
		string s_showstr;
		int run();
};

class CRunExternalAction:public CAction
{
	public:
		string s_program;
		string s_params;
		int run();
};		

#endif
