// IMAP.cpp: implementation of the CPop3 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IMAP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAX_BUFF 1000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPop3::CPop3()
{
	state=FIRST;
	numMsg = 0;
	IMAPConnected = TRUE;
}

CPop3::~CPop3()
{
	
}

void CPop3::OnReceive(int err)
{
	if(err==0) {
		char buff[MAX_BUFF];
		int rec=Receive(buff,MAX_BUFF); //receive data
		buff[rec]=NULL;
		lastMsg=buff;
		ParseMsg(); //parse data
	} else {

	}
}

void CPop3::SetProp(CStringA u, CStringA p)
{
	user=u;
	pass=p;
}

void CPop3::ParseMsg()
{
	CStringA s;
	strstream str;
	string check;
	CT2A dest( lastMsg );
	str<<dest;
	str>>check;
	if(check=="-ERR") //if there is an error
	{
		return;
	}
	switch(state) {
	case FIRST: //if we are already connected
		s.Format("a001 LOGIN %s %s%c%c",user,pass,13,10);
		Send((LPCSTR)s,s.GetLength()); //send user id
		state=USER;
		break;
	case USER:
		s.Format("a002 STATUS INBOX (UNSEEN)%c%c",13,10); 
		Send((LPCSTR)s,s.GetLength());
		state=STAT;
		break;
	case STAT:
		{
			string s1;
			str.seekg(23);
			str>>s1;
			int l = s1.length() - 1;
			s1.resize(l);
			numMsg = atoi(s1.c_str());
			flush(str);
			state=GOON;
		}
		break;
	case GOON: //default
	default:
		break;
	}
}

void CPop3::OnConnect(int err)
{
	if(err==0) {
		m_bIMAPConnected = TRUE;
		IMAPConnected = TRUE;
		state=FIRST;
	}
}

void CPop3::OnClose(int err)
{
	CAsyncSocket::Close();
	m_bIMAPConnected = FALSE;
	state=FIRST;
}


void CPop3::OnAccept(int err)		  
{															  
	if(err==0) {
	
	} else {
	
	}
}

void CPop3::OnSend(int err)
{
	if(err==0) {
	
	}
}

int CPop3::GetNumMsg()
{
	return numMsg;
}

int CPop3::GetSizeMsg()
{
	return sizeMsg;
}

void CPop3::SendCntNewMsg()
{
	if (m_bIMAPConnected) {
		CStringA s;
		s.Format("a002 STATUS INBOX (UNSEEN)%c%c",13,10); 
		Send((LPCSTR)s,s.GetLength()); //now send password
		state=STAT;
	}
}