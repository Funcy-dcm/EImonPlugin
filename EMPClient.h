
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_EMPClient_H__1957A2DC_027A_43AF_82B6_789B397147F3__INCLUDED_)
#define AFX_EMPClient_H__1957A2DC_027A_43AF_82B6_789B397147F3__INCLUDED_

#include <strstream>
#include <string>

using namespace std;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEMPClient  : public CAsyncSocket
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
	CEMPClient();
	virtual ~CEMPClient();
	BOOL m_bEMPConnected;

private:
	//enum type describing actual EMPClient state
	typedef enum {FIRST1=0,USER1,STAT1,GOON1} STATE1;

	//main function
	void ParseMsg();
	CStringA lastMsg;
	CStringA lastMsg_t;
	STATE1 state;
	CStringA user, pass;
	int numMsg, sizeMsg;
};

#endif // !defined(AFX_EMPClient_H__1957A2DC_027A_43AF_82B6_789B397147F3__INCLUDED_)
