#include <stdio.h>
#ifndef IS_OLDGCC
#include <iostream>
#else
#include <iostream.h>
#endif
#ifdef __GNUC__
#include <cstring>
#else
#include <string.h>
#endif
using namespace std;
#include "msg.h"
#include "area.h"
#include "config.h"
#include "log.h"
#include "global.h"

int initApi()
{
    struct _minf m;
    m.req_version = 0;
    m.def_zone = cfg->F_Home.zone;
    if (MsgOpenApi(&m) != 0)
    {
        exit(1);
    }
    return 0;
}

int closeApi()
{
    MsgCloseApi();
    return 0;
}

CArea::CArea()
{
}

CArea::CArea(const CArea & ar)
{
    s_Path=ar.s_Path;
    i_type=ar.i_type;
}

CArea::~CArea()
{
}

int CArea::Open(string Path)
{
    if (Path[0]=='$')
    {
        i_type=MSGTYPE_SQUISH;
        Path.erase(0,1);
    }
    else
        i_type=MSGTYPE_SDM;
    
    string logstr="opening area ";
    logstr += Path;
    log->add(1, logstr);

    a_Area=MsgOpenArea((unsigned char *)Path.c_str(), MSGAREA_CRIFNEC, i_type);
    if (a_Area!=NULL)
    {
        i_msgNum=MsgGetHighMsg(a_Area);
        return 0;
    }
    else
    {
        a_Area=NULL;
 
        string logstr="opening area " + s_Path + " failed";
        log->add(1, logstr);        
        return -1;
    }
}

int CArea::Open()
{
    a_Area=MsgOpenArea((unsigned char *)s_Path.c_str(), MSGAREA_NORMAL, i_type);
    if (a_Area!=NULL)
    {
        i_msgNum=MsgGetHighMsg(a_Area);
        string logstr="opening area " + s_Path;
        log->add(1, logstr);
        return 0;
    }
    else
    {
        a_Area=NULL;
        cerr << "Could not open area:" << s_Path << "!\n";
        return -1;
    }
}

int CArea::Close()
{
    if (a_Area!=NULL)
    {
        MsgCloseArea(a_Area);
        return 0;
    }
    else
    {
        cerr << "Area not open!\n";
        return -1;
    }
}

HAREA CArea::GetArea()
{
    return a_Area;
}

int CArea::Scan(vector<COperation> M_ScanFor, vector<CAction> A_Execute, unsigned int start, unsigned int stop)
{
    CMsg Message;
    CMask actualMask;
    int result;
    vector<int> matches;
    dword msgnum = 1;
    bool stopwithmsg;

    while (MsgGetCurMsg(a_Area) != MsgGetHighMsg(a_Area))
    {
        result=Message.Open(msgnum, a_Area);
        if (msgnum == 1) msgnum = MSGNUM_NEXT;

        if (result==-1)
        {
            continue;
        }
        stopwithmsg = false;

        for (unsigned int j=start; j<stop+1 && !stopwithmsg; j++)
        {
            actualMask=Message.GetMask();

//	  cerr << "Msgnum=" << Message.s_To  << ": MSGREAD=" << (Message.d_Attr & MSGREAD) << ": MSGLOCAL=" << (Message.d_Attr & MSGLOCAL) << ": MSGRRQ=" << (Message.d_Attr & MSGRRQ) << endl;


            /*------------ scan for matching mask ---------------*/
            if (M_ScanFor[j].M_Mask==actualMask)
            {
                if (0==1)  // FIX: add searching code...
                    continue;
                else
                    matches.push_back(MsgGetCurMsg(a_Area));

                /* TODO: write code to check, if there was already a hit and continue(); if yes */

                /*-------- execute associated actions ------------*/
                for (int k=M_ScanFor[j].firstAction; k<M_ScanFor[j].lastAction+1; k++)
                {
                    /*------ get action type -------*/
                    string type;
                    string RestParam;
                    string temp;

                    /*------ write action data to vars -------*/
                    temp=A_Execute[k].param;
                    while (temp[0]==' ') temp.erase(0,1);
                    if (temp[temp.length()-1]=='\n') temp.erase(temp.length()-1, 1);
                    do
                    {
                        type+=temp[0];
                        temp.erase(0,1);
                    }
                    while (temp.length()>0 && temp[0]!=' ');
                    while (temp[0]==' ') temp.erase(0,1);
                    RestParam=temp;



                    char number[6];
                    sprintf(number, "%lu", MsgGetCurMsg(a_Area));

                    if (cfg->debug)
                    {
                        cerr << "temp=" << temp << ": RestParam=" << RestParam << ": type=" << type << ": number=" << number << endl;
                        cerr << "Msgnum=" << number  << ": MSGREAD=" << (Message.d_Attr & MSGREAD) << ": MSGLOCAL=" << (Message.d_Attr & MSGLOCAL) << ": MSGRRQ=" << (Message.d_Attr & MSGRRQ) << endl;
                    }                        

                    /*----- action file -----*/
                    if (type=="file")
                    {
                        CFileAction TempAction;
                        TempAction.s_Filename=RestParam;
                        if (M_ScanFor[j].M_Mask.i_match.size()>1) TempAction.s_Filename+=number;
                        TempAction.msgnum=MsgGetCurMsg(a_Area);
                        TempAction.Area=a_Area;
                        TempAction.run();
                    }

                    /*----- action headerfile -----*/
                    if (type=="hdrfile")
                    {
                        CHdrFileAction TempAction;
                        TempAction.s_Filename=RestParam;
                        if (M_ScanFor[j].M_Mask.i_match.size()>1) TempAction.s_Filename+=number;
                        TempAction.msgnum=MsgGetCurMsg(a_Area);
                        TempAction.Area=a_Area;
                        TempAction.run();
                    }
                    /*----- action bounce --------*/
                    if (type=="bounce")
                    {
                        CBounceAction TempAction;
                        TempAction.param=RestParam;
                        TempAction.msgnum=MsgGetCurMsg(a_Area);
                        TempAction.Area=a_Area;
                        TempAction.run();
                    }
                    /*----- action ping --------*/
                    if (type=="ping")
                    {
                        if (((Message.d_Attr & MSGREAD) == 0) && ((Message.d_Attr & MSGLOCAL) == 0 ) && ((Message.d_Attr & MSGRRQ) == 0))
                        {
                            CPingPongAction TempAction;
                            TempAction.msgnum=MsgGetCurMsg(a_Area);
                            TempAction.Area=a_Area;
                            TempAction.run();
                        }
                        else if (((Message.d_Attr & MSGREAD) == 0) && ((Message.d_Attr & MSGLOCAL) == 0 ) && ((Message.d_Attr & MSGRRQ) == MSGRRQ ))
                        {
                            CDeleteAction TempAction;
                            TempAction.Area=a_Area;
                            TempAction.msgnum=MsgGetCurMsg(a_Area);
                            TempAction.run();
                            stopwithmsg = true;
                            break;
                        }
                    }
                    /*------- action copy --------*/
                    if (type=="copy")
                    {
                        if (((Message.d_Attr & MSGREAD) == 0) && ((Message.d_Attr & MSGLOCAL) == 0 ))
                        {
                        CCopyAction TempAction;
                        TempAction.param=RestParam;
                        TempAction.Area=a_Area;
                        TempAction.msgnum=MsgGetCurMsg(a_Area);
                        TempAction.run();
                        }
                    }
                    /*------- action move --------*/
                    if (type=="move")
                    {
                        CMoveAction TempAction;
                        TempAction.param=RestParam;
                        TempAction.Area=a_Area;
                        TempAction.msgnum=MsgGetCurMsg(a_Area);
                        TempAction.run();
                        stopwithmsg = true;
                        break;
                    }
                    /*-------- packmail action -------*/
                    if (type=="packmail")
                    {
                        if (((Message.d_Attr & MSGSENT) != MSGSENT) && ((Message.d_Attr & MSGLOCAL) == MSGLOCAL))
                        {
                            CPackmailAction TempAction;
                            TempAction.param=RestParam;
                            TempAction.Area=a_Area;
                            TempAction.msgnum=MsgGetCurMsg(a_Area);
                            TempAction.run();
                        }
                    }
                    /*-------- movemail action -------*/
                    if (type=="movemail")
                    {


                        if (((Message.d_Attr & MSGSENT) != MSGSENT) && ((Message.d_Attr & MSGLOCAL) == MSGLOCAL))
                        {
                            CMovemailAction TempAction;
                            TempAction.param=RestParam;
                            TempAction.Area=a_Area;
                            TempAction.msgnum=MsgGetCurMsg(a_Area);
                            TempAction.run();
                        }
                    }

                    /*-------- email action -------*/
                    if (type=="email")
                    {
                        if ((Message.d_Attr & MSGREAD) == 0 )
                        {
                            CEmailAction TempAction;
                            TempAction.param=RestParam;
                            TempAction.Area=a_Area;
                            TempAction.s_Area=s_Path;
                            TempAction.msgnum=MsgGetCurMsg(a_Area);
                            TempAction.run();
                        }
                    }

                    /*-------- rewrite action -----------*/
                    if (type=="rewrite")
                    {
                        CRewriteAction TempAction;
                        TempAction.param=RestParam;
                        TempAction.Area=a_Area;
                        TempAction.msgnum=MsgGetCurMsg(a_Area);
                        TempAction.run();
                    }
                    /*--------- display action ----------*/
                    if (type=="display")
                    {
                        CDisplayAction TempAction;
                        TempAction.param=RestParam;
                        TempAction.run();
                    }

                    /*------- semaphore action ----------*/
                    if (type=="semaphore")
                    {
                        CSemaphoreAction TempAction;
                        TempAction.param=RestParam;
                        TempAction.run();
                    }
                    if (type=="delete")
                    {
                        CDeleteAction TempAction;
                        TempAction.Area=a_Area;
                        TempAction.msgnum=MsgGetCurMsg(a_Area);
                        TempAction.run();
                        stopwithmsg = true;
                        break;
                    }
                    if (type=="ignore")
                    {
                        stopwithmsg = true;
                        break;
                    }

                }
            }
        }
        Message.Close();
    }
    return 0;
}


