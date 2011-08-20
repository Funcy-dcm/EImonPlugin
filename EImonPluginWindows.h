// EImonPluginWindows.h : header file
//

#pragma once

#define WM_DSP_PLUGIN_NOTIFY	WM_APP + 1001

// CDisplayTestDlg dialog
class CDisplayTestDlg : public CDialog
{
// Construction
public:
	CDisplayTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DISPLAYTEST_DIALOG };
	CString m_strLine1;
	CString m_strLine2;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	UINT_PTR m_uEqTimer;
	UINT_PTR m_uEqTimer2;
	BOOL m_bVfdConnected;

	void Init();
	void Uninit();
	void DisplayPluginMessage(UINT uErrCode, BOOL bError);
	void UpdateControlUI();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonInit();
	afx_msg void OnBnClickedButtonVfdSendText();
	afx_msg void OnBnClickedButtonVfdRandomEq();

	afx_msg LRESULT OnDisplayPluginNotify(WPARAM, LPARAM);
};
