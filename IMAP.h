
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_IMAP_H__1957A2DC_027A_43AF_82B6_789B397147F3__INCLUDED_)
#define AFX_IMAP_H__1957A2DC_027A_43AF_82B6_789B397147F3__INCLUDED_

#include <strstream>
#include <string>

using namespace std;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//enum type describing actual pop3 state
typedef enum {FIRST=0,USER,STAT,GOON} STATE;

//simple struct that keeps message data, size, and retrieving size
typedef struct 
{
	CString text;
	int msgSize, retrSize;
} MESSAGEPROP;

class CPop3  : public CAsyncSocket
{

public:
	//gest size of all msgs
	int GetSizeMsg();
	//gets number of msgs that are on server
	int GetNumMsg();
	void SendCntNewMsg();
	//sets username and password
	void SetProp(CStringA u, CStringA p);
	//gets last message recievied from server
	void GetLastMsg(CString &);
	//receiving event
	void OnReceive(int err);
	CPop3();
	virtual ~CPop3();

private:
	//main function
	void ParseMsg();
	CString lastMsg;
	STATE state;
	CStringA user, pass;
	int numMsg, sizeMsg;
};

#endif // !defined(AFX_IMAP_H__1957A2DC_027A_43AF_82B6_789B397147F3__INCLUDED_)
