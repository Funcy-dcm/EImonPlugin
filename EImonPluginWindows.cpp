// EImonPluginWindows.cpp : implementation file
//

#include "stdafx.h"
#include "EImonPlugin.h"
#include "EImonPluginWindows.h"
#include "IMAP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MYWM_NOTIFYICON (WM_APP + 1)

BOOL TrayMessage (HWND hDlg, DWORD dwMessage, UINT uID, HICON hIcon, PSTR pszTip) 
// systray icon 
{ 
    BOOL res; 

    NOTIFYICONDATA tnd; 

    tnd.cbSize = sizeof(NOTIFYICONDATA); 
    tnd.hWnd = hDlg; 
    tnd.uID = uID; 

    tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; 
    tnd.uCallbackMessage = MYWM_NOTIFYICON; // ���������, ������� �������� ��� ������ ��� ������ �� ������... 
    tnd.hIcon = hIcon; 

    if (pszTip) 
    { 
        lstrcpyn(tnd.szTip, (LPCTSTR)pszTip, sizeof(tnd.szTip)); 
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
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	m_strLine1 = L"";
	m_strLine2 = L"";

	m_uEqTimer = 0;
	m_uEqTimer2 = 0;
	m_bVfdConnected = FALSE;
	viewvfd = TIME;
}

void CDisplayTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT1, m_strLine1);
	DDX_Text(pDX, IDC_EDIT2, m_strLine2);
}

BEGIN_MESSAGE_MAP(CDisplayTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_TIMER()
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
 
	/*HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	HWND hWnd = this->GetSafeHwnd();
	BOOL ok = TrayMessage(hWnd, NIM_ADD, 0, hIcon, 0);
	if (ok) ShowWindow(SW_HIDE);*/

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
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_strLine1 = L"������ ������!";
	m_strLine2 = L"iMON Display API";

	UpdateControlUI();

	CSliderCtrl* pSliderCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
	if(pSliderCtrl)
	{
		pSliderCtrl->SetRange(0, 100);
		pSliderCtrl->SetPos(0);
	}

	UpdateData(FALSE);
	
	EMAIL[0] = L"Funcy";
	EMAIL[1] = L"Alis";

	Init();
				//SetTimer(103, 10000, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDisplayTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	HWND hWnd;		// ��������� �� ���� 
	HICON hIcon;	// ��������� �� ������ 
//	char *szText;	// ��������� �� ����� ��� ��������� 
	BOOL ok;

	if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	} 
	switch (nID & 0xFFF0) {
    case SC_MINIMIZE:
		hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
		hWnd = this->GetSafeHwnd();
		ok = TrayMessage(hWnd, NIM_ADD, 0, hIcon, 0);
		if (ok) ShowWindow(SW_HIDE);
        break;
	case MYWM_NOTIFYICON:
		hWnd = this->GetSafeHwnd();
		TrayMessage(hWnd, NIM_DELETE, 0, 0, 0);
		ShowWindow(SW_SHOW);
		break;
    default:
        CDialog::OnSysCommand(nID, lParam);
    }
}

LRESULT CDisplayTestDlg::DefWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND hWnd;
	switch(msg) {
	case MYWM_NOTIFYICON:
		switch(lParam) {
		case WM_LBUTTONDBLCLK:
			hWnd = this->GetSafeHwnd();
			TrayMessage(hWnd, NIM_DELETE, 0, 0, 0);
			ShowWindow(SW_SHOW);
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
	if (IsIconic())
	{
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
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDisplayTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
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
	if(nIDEvent == 101)
	{
		DSPEQDATA eqData;
		for(int i=0; i<16;i++)
		{
			eqData.BandData[i] = rand()%100;
		}
		IMON_Display_SetVfdEqData(&eqData);
	} else if((nIDEvent == 102) && (viewvfd == TIME)) {
		SYSTEMTIME sm;
		GetLocalTime(&sm);
		switch(sm.wDayOfWeek) {
			case 1: str3 = L"��"; break;
			case 2: str3 = L"��"; break;
			case 3: str3 = L"��"; break;
			case 4: str3 = L"��"; break;
			case 5: str3 = L"��"; break;
			case 6: str3 = L"��"; break;
			case 0: str3 = L"��"; break;
		}
		str1.Format(L" %02d.%02d.%04d %s", sm.wDay, sm.wMonth, sm.wYear, str3);
		str2.Format(L"    %02d:%02d:%02d", sm.wHour, sm.wMinute, sm.wSecond);
		IMON_Display_SetVfdText((LPCTSTR)str1, (LPCTSTR)str2);
	} else if(nIDEvent == 103) {
		switch(viewvfd) {
			case TIME: 
				str3 = L"                ";
				length = (16 - EMAIL[0].GetLength())/2;
				if ((length>0) && (length<16)) str1 = str3.Left(length);
				str1.AppendFormat(EMAIL[0]);
				str4.Format(L"%d", pop3.GetNumMsg());
				length = (16 - str4.GetLength())/2;
				if ((length>0) && (length<16)) str2 = str3.Left(length);
				str2.AppendFormat(L"%d", pop3.GetNumMsg());
				IMON_Display_SetVfdText((LPCTSTR)str1, (LPCTSTR)str2);
				viewvfd = EMAIL1;
				break;
			case EMAIL1:
				str1.Format(EMAIL[1]);
				str2.Format(L"%d", 0);
				IMON_Display_SetVfdText((LPCTSTR)str1, (LPCTSTR)str2);
				viewvfd = EMAIL2;
				break;
			case EMAIL2:

				viewvfd = TIME;
				break;
		}
	} else if(nIDEvent == 104) {
		pop3.SendCntNewMsg();
	}
	CDialog::OnTimer(nIDEvent);
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
			
			IMON_Display_SetVfdText((LPCTSTR)L"������ 1", (LPCTSTR)L"������ 2");
			OnConnImap();
			SetTimer(102, 250, NULL);
			SetTimer(103, 10000, NULL);
			SetTimer(104, 30000, NULL);
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

	for(int i=IDC_ORANGE1;i<=IDC_AR_SCR2;i++)
	{
		if(GetDlgItem(i))
			((CButton*)GetDlgItem(i))->SetCheck(FALSE);
	}
	CSliderCtrl* pSliderCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
	if(pSliderCtrl)	pSliderCtrl->SetPos(0);

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
	// TODO: Add your control notification handler code here
	//pop3.Set(this); //set window that would receive messages
	pop3.SetProp("funcy-dcm","20fishka07"); //set user and pass
	pop3.Create();
	pop3.Connect((LPCTSTR)L"imap.yandex.ru",143); //connect to a server
}
