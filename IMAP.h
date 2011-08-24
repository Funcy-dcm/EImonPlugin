
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_IMAP_H__1957A2DC_027A_43AF_82B6_789B397147F3__INCLUDED_)
#define AFX_IMAP_H__1957A2DC_027A_43AF_82B6_789B397147F3__INCLUDED_

#include <vector>
#include <strstream>
#include <string>

using namespace std;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//enum type describing actual pop3 state
typedef enum {FIRST=0,USER,STAT,GOON} STATE;

#define S_GETNUMMSGS 5 //send when user can obtain number of messages
#define S_GETSIZEMSGS 6 //as above, but size of messages
#define S_ENDRETR 7 //send when done retrieving

// CPop3 socket messages:
#define S_CLOSE 1
#define S_CONNECT 2
#define S_RECEIVE 3

//#define DLG CDisplayTestDlg* //change it to your CDialog-based class

//simple struct that keeps message data, size, and retrieving size
typedef struct 
{
	CString text;
	int msgSize, retrSize;
} MESSAGEPROP;

class CPop3  : public CAsyncSocket
{

public:
	void Set(CDialog * pWnd);
	CDialog * m_pWnd; //pointer to parent

	//gest size of all msgs
	int GetSizeMsg();
	//gets number of msgs that are on server
	int GetNumMsg();
	void SendCntNewMsg();
	//gets string error
	CString GetError();
	//closes and quits from pop3 server
	void Close();
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

	CString lastMsg, error;
	STATE state;
	CStringA user, pass;
	int numMsg, sizeMsg, retrMsg;
	vector<MESSAGEPROP> msgs; //vector for message stuff
};

#endif // !defined(AFX_IMAP_H__1957A2DC_027A_43AF_82B6_789B397147F3__INCLUDED_)
