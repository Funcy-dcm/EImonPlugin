// EMPClient.cpp: implementation of the CEMPClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EMPClient.h"
#include <stdlib.h>

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

}

CEMPClient::~CEMPClient()
{
	
}

void CEMPClient::OnConnect(int err)
{
	if(err==0) {

	} else {
		//CAsyncSocket::Close();
	}
}

void CEMPClient::OnReceive(int err)
{
	if(err==0) {
		char buff[MAX_BUFF];
		int rec=Receive(buff,MAX_BUFF); //receive data
		WORD l0;
		memcpy(&l0, buff, 2);
		rec = htons(l0);
		buff[2+rec]=NULL;
		buff[2+rec+1]=NULL;
		int l1, l2;
		memcpy(&l1, (int*)&buff[2], 4);
		l1 = htonl(l1);
		lastMsg = &buff[6];
		if ((l1+6) < (rec+2)) {
			memcpy(&l2, (int*)&buff[6+l1], 4);
			l2 = htonl(l2);
			char t;
			for (int i = 0; i < l2; i = i + 2) {
				t = buff[6+l1+4+i];
				buff[6+l1+4+i] = buff[6+l1+4+i+1];
				buff[6+l1+4+i+1] = t;
			}
			lastMsg_t.Format(L"%s", &buff[6+l1+4]);
			int l3 = lastMsg_t.GetLength();
			
			memcpy(&currentTime, &buff[6+l1+4+l2], 8);
			currentTime = _byteswap_uint64(currentTime);
			memcpy(&totalTime, &buff[6+l1+4+l2+8], 8);
			totalTime = _byteswap_uint64(totalTime);

			t = 1;
		}
		if (lastMsg == "*OK") {
			m_bEMPConnected = TRUE;
			lastMsg_t = "";
			currentTime = 0;
			totalTime = 0;
			SendInfo();
		}
	} else {

	}
}

void CEMPClient::OnClose(int err)
{
	CAsyncSocket::Close();
	m_bEMPConnected = FALSE;
}

void CEMPClient::SendInfo()
{
	CStringA s;
	s.Format("%c%c%c%c%c%cName", 0, 8, 0, 0, 0, 4);
	Send((LPCSTR)s,s.GetLength());
}