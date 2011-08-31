// EImonPluginWindows.cpp : implementation file
//

#include "stdafx.h"
#include "EImonPlugin.h"
#include "EImonPluginWindows.h"
#include "IMAP.h"
#include <psapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MYWM_NOTIFYICON (WM_APP + 1)

CDisplayTestDlg *dlg = 0;
void CALLBACK OnTimer1(HWND, UINT, UINT, DWORD);
void CALLBACK OnTimer2(HWND, UINT, UINT, DWORD);

BOOL TrayMessage (HWND hDlg, DWORD dwMessage, UINT uID, HICON hIcon, LPCWSTR pszTip) 
// systray icon 
{ 
    BOOL res; 

    NOTIFYICONDATA tnd; 

    tnd.cbSize = sizeof(NOTIFYICONDATA); 
    tnd.hWnd = hDlg; 
    tnd.uID = uID; 

    tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; 
    tnd.uCallbackMessage = MYWM_NOTIFYICON; // Сообщение, которое пошлется при всяких там кликах на иконке... 
    tnd.hIcon = hIcon; 

    if (pszTip) 
    { 
        lstrcpyn(tnd.szTip, pszTip, sizeof(tnd.szTip)); 
    } 
    else 
    { 
        tnd.szTip[0] = '\0'; 
    } 

    res = Shell_NotifyIcon(dwMessage, &tnd); 

    return res; 
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CDisplayTestDlg dialog

CDisplayTestDlg::CDisplayTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDisplayTestDlg::IDD, pParent)
{
	m_hIconB = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIconS = AfxGetApp()->LoadIcon(IDR_MAINFRAME1);

	m_strLine1 = L"";
	m_strLine2 = L"";

	m_uEqTimer = 0;
	m_uEqTimer2 = 0;
	m_bVfdConnected = FALSE;
	viewvfd = TIME;
	empClient = new CEMPClient;
	empClient->m_bEMPConnected = FALSE;
	pop3 = new CPop3;
	pop3d = new CPop3;
	pop3->m_bIMAPConnected = FALSE;
	pop3d->m_bIMAPConnected = FALSE;
	dlg = this;

	m_bVisible = FALSE;
}

void CDisplayTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT1, m_strLine1);
	DDX_Text(pDX, IDC_EDIT2, m_strLine2);
}

BEGIN_MESSAGE_MAP(CDisplayTestDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_WINDOWPOSCHANGING()
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButtonInit)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButtonVfdSendText)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButtonVfdRandomEq)

	ON_MESSAGE(WM_DSP_PLUGIN_NOTIFY, OnDisplayPluginNotify)
END_MESSAGE_MAP()


// CDisplayTestDlg message handlers

BOOL CDisplayTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIconB, TRUE);		// Set big icon
	SetIcon(m_hIconS, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_strLine1 = L"Привет котёнок!";
	m_strLine2 = L"iMON Display API";

	UpdateControlUI();

	UpdateData(FALSE);
	
	Email[0] = L"Funcy";
	Email[1] = L"Alis";

	pop3->SetProp("funcy-dcm","20fishka07"); //set user and pass
	pop3d->SetProp("alis-dcm","melnica"); //set user and pass

	SetTimer(102, 250, NULL);
	SetTimer(103, 10000, NULL);
	SetTimer(104, 60000, NULL);
	SetTimer(105, 1000, NULL);
	SetTimer(106, 4000, NULL);
	SetTimer(107, 180000, NULL);
	SetTimer(108, 500, NULL);
	SetTimer(109, 10000, NULL);
	Init();

	TrayMessage(this->GetSafeHwnd(), NIM_ADD, 0, m_hIconS, L"EImonPlugin");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDisplayTestDlg::OnClose()
{
	TrayMessage(this->GetSafeHwnd(), NIM_DELETE, 0, 0, 0);
	CDialog::OnClose();
}

void CDisplayTestDlg::OnWindowPosChanging(WINDOWPOS FAR* pos)
{
	if(!m_bVisible)
        pos->flags &= ~SWP_SHOWWINDOW;

    CDialog::OnWindowPosChanging(pos);
}

void CDisplayTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	} 
	switch (nID & 0xFFF0) {
    case SC_MINIMIZE:
		m_bVisible = false;
		ShowWindow(SW_HIDE);
        break;
	case SC_CLOSE:
		
    default:
        CDialog::OnSysCommand(nID, lParam);
    }
}

LRESULT CDisplayTestDlg::DefWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg) {
	case MYWM_NOTIFYICON:
		switch(lParam) {
		case WM_LBUTTONDBLCLK:
			m_bVisible = true;
			SetActiveWindow();
			ShowWindow(SW_SHOW);
			break;
		case WM_RBUTTONDOWN:
			HandlePopupMenu (this->GetSafeHwnd()); //рисуем меню от координат курсора
			break;
		}
		break;
	}
	return CDialog::DefWindowProc(msg, wParam, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDisplayTestDlg::OnPaint()
{
	if (IsIconic()) {
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIconS);
	} else {
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDisplayTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIconS);
}

void CDisplayTestDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	IMON_Display_Uninit();
}

void CDisplayTestDlg::OnTimer(UINT nIDEvent) 
{
	CString str1;
	CString str2;
	CString str3;
	CString str4;
	int length;
	static BOOL EMPConnectedT = FALSE;
	static int curStr = 0;
	static int tt = 0;

	if(nIDEvent == 101) {
		DSPEQDATA eqData;
		for(int i=0; i<16;i++) {
			eqData.BandData[i] = rand()%100;
		}
		IMON_Display_SetVfdEqData(&eqData);
	} 
	if(nIDEvent == 102) {
		SYSTEMTIME sm;
		GetLocalTime(&sm);
		switch(sm.wDayOfWeek) {
			case 1: str3 = L"Пн"; break;
			case 2: str3 = L"Вт"; break;
			case 3: str3 = L"Ср"; break;
			case 4: str3 = L"Чт"; break;
			case 5: str3 = L"Пт"; break;
			case 6: str3 = L"Сб"; break;
			case 0: str3 = L"Вс"; break;
		}
		str1.Format(L" %02d.%02d.%04d %s", sm.wDay, sm.wMonth, sm.wYear, str3);
		str2.Format(L"    %02d:%02d:%02d", sm.wHour, sm.wMinute, sm.wSecond);
		GetDlgItem(IDC_STATIC_INFO2)->SetWindowText((LPCTSTR)str2);
		if (m_bVfdConnected && (viewvfd == TIME))
			IMON_Display_SetVfdText((LPCTSTR)str1, (LPCTSTR)str2);
	} 
	if(nIDEvent == 103) {
		KillTimer(nIDEvent);
		switch(viewvfd) {
			case TIME:
				viewvfd = MAIL;
				str3 = L"                ";
				str4.Format(L"%s/%s", Email[1], Email[0]); 
				length = (16 - str4.GetLength())/2;
				if ((length>0) && (length<16)) str1 = str3.Left(length);
				str1.AppendFormat(str4);
				str4.Format(L"%d/%d", pop3d->GetNumMsg(), pop3->GetNumMsg());
				length = (16 - str4.GetLength())/2;
				if ((length>0) && (length<16)) str2 = str3.Left(length);
				str2.AppendFormat(str4);
				if (m_bVfdConnected)
					IMON_Display_SetVfdText((LPCTSTR)str1, (LPCTSTR)str2);
				SetTimer(103, 3000, NULL);
				break;
			case MAIL: 
				viewvfd = TIME;
				SetTimer(103, 10000, NULL);
				break;
		}
	}
	if(nIDEvent == 104) {
		pop3->SendCntNewMsg();
		pop3d->SendCntNewMsg();
	}
	if (nIDEvent == 105/* && m_bVfdConnected*/) {
		OnConnEMP();
	}
	if (nIDEvent == 106/* && m_bVfdConnected*/) {
		KillTimer(nIDEvent);
		OnConnImap();
	}
	if (nIDEvent == 107/* && m_bVfdConnected*/) {
		OnConnImap();
	}
	if (nIDEvent == 108) {
		if (EMPConnectedT) {
			WORD sec = (WORD)(empClient->currentTime/1000);
			WORD min = sec/60;
			WORD hour = min/60;
			WORD secT = (WORD)(empClient->totalTime/1000);
			WORD minT = secT/60;
			WORD hourT = minT/60;
			str2.Format(L"%1d:%02d:%02d/%1d:%02d:%02d", hour%60, min%60, sec%60, hourT%60, minT%60, secT%60);
			
			str3 = empClient->lastMsg_t;
			length = str3.GetLength();
			if (length > 16) {
				if (tt >= 2) {
					str3.AppendFormat(L"   ");
					str1 = str3.Right(length+3 - curStr);
					str1.AppendFormat(str3);
					if (curStr < length+2) {
						curStr++;
					} else {
						curStr = 0;
						//tt = 0;
					}
				} else {
					tt++;
					str1 = str3;
				}
			} else str1 = str3;

			if (m_bVfdConnected)
				IMON_Display_SetVfdText((LPCTSTR)str1, (LPCTSTR)str2);
			empClient->SendInfo();
		}
		if (EMPConnectedT != empClient->m_bEMPConnected) {
			EMPConnectedT = empClient->m_bEMPConnected;
			curStr = 0;
			tt = 0;
			if (EMPConnectedT) {
				KillTimer(103);
				viewvfd = EMPINFO;
				empClient->SendInfo();
			} else {
				viewvfd = TIME;
				SetTimer(103, 10000, NULL);
			}	
		}
	}
	if (nIDEvent == 109) {
		// Освобождение памяти при запуске после 10 сек
		KillTimer(nIDEvent);
		SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T)-1, (SIZE_T)-1);
	}
 
	CDialog::OnTimer(nIDEvent);
}

void CALLBACK OnTimer1(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	CString str1;
	str1 = "1";
}

 void CALLBACK OnTimer2(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
 {
	if(nIDEvent == 104) {
		dlg->pop3->SendCntNewMsg();
		dlg->pop3d->SendCntNewMsg();
	}
	if (nIDEvent == 105/* && m_bVfdConnected*/) {
		dlg->OnConnEMP();
	}
	if (nIDEvent == 106/* && m_bVfdConnected*/) {
		KillTimer(hWnd, nIDEvent);
		dlg->OnConnImap();
	}
	if (nIDEvent == 107/* && m_bVfdConnected*/) {
		dlg->OnConnImap();
	}
 }

void CDisplayTestDlg::HandlePopupMenu (HWND hWnd)
{
	HMENU hMenu;
	HMENU hPopup;
	POINT point;

	hMenu = LoadMenu (AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MENU1));
	if (!hMenu) return;
	hPopup = GetSubMenu (hMenu, 0);
	if(!hPopup)	return;
	GetCursorPos(&point);          //вычисляем текущее положение курсора
	TrackPopupMenu (hPopup, 0, point.x, point.y, 0, hWnd, NULL);
	DestroyMenu (hMenu);
}

void CDisplayTestDlg::OnBnClickedButtonInit()
{
	if(IMON_Display_IsInited() != DSP_S_INITED)		Init();
	else											Uninit();
}

void CDisplayTestDlg::OnBnClickedButtonVfdSendText()
{
	UpdateData(TRUE);
	IMON_Display_SetVfdText((LPCTSTR)m_strLine1, (LPCTSTR)m_strLine2);
}

void CDisplayTestDlg::OnBnClickedButtonVfdRandomEq()
{
	if(m_uEqTimer)
	{	
		KillTimer(101);		m_uEqTimer = 0;		
		GetDlgItem(IDC_BUTTON3)->SetWindowText(L"Start Random EQ");
	}
	else
	{
		m_uEqTimer = SetTimer(101, 40, NULL);
		GetDlgItem(IDC_BUTTON3)->SetWindowText(L"Stop Random EQ");
	}
}



LRESULT CDisplayTestDlg::OnDisplayPluginNotify(WPARAM wParam, LPARAM lParam)
{
	if(!GetSafeHwnd() || !IsWindow(GetSafeHwnd()))		return 0;

	switch(wParam)
	{
	case DSPNM_PLUGIN_SUCCEED:
	case DSPNM_IMON_RESTARTED:
	case DSPNM_HW_CONNECTED:
		{
			GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
			m_bVfdConnected = FALSE;
			if((lParam & DSPN_DSP_VFD) == DSPN_DSP_VFD)		m_bVfdConnected = TRUE;
			UpdateControlUI();

			DisplayPluginMessage((UINT)wParam, FALSE);
			
			IMON_Display_SetVfdText((LPCTSTR)L"Строка 1", (LPCTSTR)L"Строка 2");
		}
		break;

	case DSPNM_PLUGIN_FAILED:
	case DSPNM_HW_DISCONNECTED:
	case DSPNM_IMON_CLOSED:
		{
			GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
			m_bVfdConnected = FALSE;
			UpdateControlUI();

			DisplayPluginMessage((UINT)lParam, TRUE);
		}
		break;
	}
	return 0;
}

void CDisplayTestDlg::Init()
{
	Uninit();

	IMON_Display_Init(this->GetSafeHwnd(), WM_DSP_PLUGIN_NOTIFY);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
}

void CDisplayTestDlg::Uninit()
{
	IMON_Display_Uninit();

	m_bVfdConnected = FALSE;
	UpdateControlUI();
}

void CDisplayTestDlg::DisplayPluginMessage(UINT uErrCode, BOOL bError)
{
	CString strErrMsg = L"";

	if(bError)
	{
		switch(uErrCode)
		{
		case DSPN_ERR_IN_USED:			strErrMsg = L"Display Plug-in is Already Used by Other Application.";		break;
		case DSPN_ERR_HW_DISCONNECTED:	strErrMsg = L"iMON HW is Not Connected.";									break;
		case DSPN_ERR_NOT_SUPPORTED_HW:	strErrMsg = L"The Connected iMON HW doesn't Support Display Plug-in.";		break;
		case DSPN_ERR_PLUGIN_DISABLED:	strErrMsg = L"Display Plug-in Mode Option is Disabled.";					break;
		case DSPN_ERR_IMON_NO_REPLY:	strErrMsg = L"The Latest iMON is Not Installed or iMON Not Running.";		break;
		case DSPN_ERR_UNKNOWN:			strErrMsg = L"Unknown Failure.";											break;
		}
	}
	else
	{
		switch(uErrCode)
		{
		case DSPNM_PLUGIN_SUCCEED:		strErrMsg = L"Plug-in Mode Inited Successfully.";		break;
		case DSPNM_IMON_RESTARTED:		strErrMsg = L"iMON Started and Plug-in Mode Inited.";		break;
		case DSPNM_HW_CONNECTED:		strErrMsg = L"iMON HW Connected and Plug-in Mode Inited.";	break;
		}
	}
	GetDlgItem(IDC_STATIC_INFO)->SetWindowText((LPCTSTR)strErrMsg);
}

void CDisplayTestDlg::UpdateControlUI()
{
	GetDlgItem(IDC_STATIC_INFO)->SetWindowText(L"");
	
	if(IMON_Display_IsInited() != DSP_S_INITED)		GetDlgItem(IDC_BUTTON1)->SetWindowText(L"Init Plug-in");
	else											GetDlgItem(IDC_BUTTON1)->SetWindowText(L"Uninit Plug-in");

	for(int i=IDC_EDIT1;i<=IDC_BUTTON3;i++)
	{
		if(GetDlgItem(i))
			GetDlgItem(i)->EnableWindow(m_bVfdConnected);
	}
}

void CDisplayTestDlg::OnConnImap() 
{
	if (!pop3->m_bIMAPConnected) {
		if (pop3->IMAPConnected != pop3->m_bIMAPConnected) {
			pop3->IMAPConnected = pop3->m_bIMAPConnected;
			if (!pop3->IMAPConnected) {
				pop3->Create();
			}
		}
		pop3->Connect((LPCTSTR)L"imap.yandex.com",143); //connect to a server
	}

	if (!pop3d->m_bIMAPConnected) {
		if (pop3d->IMAPConnected != pop3d->m_bIMAPConnected) {
			pop3d->IMAPConnected = pop3d->m_bIMAPConnected;
			if (!pop3d->IMAPConnected) {
				pop3d->Create();
			}
		}
		pop3d->Connect((LPCTSTR)L"imap.yandex.com",143); //connect to a server
	}
}

void CDisplayTestDlg::OnConnEMP() 
{
	static BOOL EMPConnectedT = TRUE;  
	if (!empClient->m_bEMPConnected) {
		if (EMPConnectedT != empClient->m_bEMPConnected) {
			EMPConnectedT = empClient->m_bEMPConnected;
			if (!EMPConnectedT) {
				empClient->Create();
			}
		}
		empClient->Connect((LPCTSTR)L"127.0.0.1",13551); //connect to a server
	} else {
		EMPConnectedT = TRUE;
	}
}

void CDisplayTestDlg::OnAppAbout()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CDisplayTestDlg::OnAppExit()
{
	SendMessage(WM_CLOSE, 0, 0);
}
