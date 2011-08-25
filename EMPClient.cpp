// EMPClient.cpp: implementation of the CEMPClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EMPClient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAX_BUFF 20000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEMPClient::CEMPClient()
{
	state=FIRST1;
	numMsg = 0;
}

CEMPClient::~CEMPClient()
{
	
}

void CEMPClient::OnReceive(int err)
{
	if(err==0) {
		char buff[MAX_BUFF];
		int rec=Receive(buff,MAX_BUFF); //receive data
		buff[rec]=NULL;
		int l1, l2;
		memcpy(&l1, (int*)&buff[2], 4);
		l1 = htonl(l1);
		lastMsg = &buff[6];
		if ((l1+6) < rec) {
			memcpy(&l2, (int*)&buff[6+l1], 4);
			l2 = htonl(l2);
			lastMsg_t = &buff[6+l1+4];
		}
		ParseMsg(); //parse data
	} else {

	}
}

void CEMPClient::ParseMsg()
{
	CStringA s;
	strstream str;
	string check;
	str<<lastMsg;
	str>>check;

	if (check == "*OK") {
		m_bEMPConnected = TRUE;
		s.Format("%c%c%c%c%c%cName", 0, 8, 0, 0, 0, 4);
		char buff[20];
		memcpy(buff, s, s.GetLength());
		Send((LPCSTR)s,s.GetLength());
	} else if (check == "Name") {

	}
}

int CEMPClient::GetNumMsg()
{
	return numMsg;
}

int CEMPClient::GetSizeMsg()
{
	return sizeMsg;
}

void CEMPClient::SendCntNewMsg()
{
	CStringA s;
	s.Format("a002 STATUS INBOX (UNSEEN)%c%c",13,10); 
	Send((LPCSTR)s,s.GetLength()); //now send password
	state=STAT1;
}