//******************************************************************************
// License:     MIT
// Author:      Hoffman
// Create Time: 2018-07-24
// Description: 
//      The header file of class CTeleClientDlg.
//
// Modify Log:
//      2018-11-28    Hoffman
//      Info: a. Add below member variables.
//              a.1. m_CriticalSection;
//              a.2. m_listPauseFilePath;
//            b. Add delcare of below member methods.
//              b.1. CheckPauseFlag();
//              b.2. OnTaskpause();
//
//      2018-12-08    Hoffman
//      Info: a. Add below member variables.
//              a.1. m_WinThreadpool.
//******************************************************************************

#pragma once
#include "ConnectThread.h"
#include "CommunicationIOCP.h"
#include "CmdRecvThread.h"
#include "FileTransportThread.h"
#include "WinThreadpool.h"



// CTeleClientDlg 对话框
class CTeleClientDlg : public CDialog
{
// 构造
public:
    CTeleClientDlg(CWnd* pParent = NULL);    // 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_TELECLIENT_DIALOG };
#endif

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
public:
    // Client context.
    PCLIENTINFO m_pstClientInfo = NULL;
    BOOL m_bBreakEventHasSigal = FALSE;
    BOOL m_bHasDataToSend = FALSE;
    // The flag of cmd quit.
    BOOL m_bCmdQuit = FALSE;

    // The thread had initialized that get file.
    CEvent *m_pevtGetFileThreadInitializeEvent = NULL;
    CCriticalSection m_CriticalSection;
    std::list<CPath *> m_listPauseFilePath;
    CWinThreadpool m_WinThreadpool;

private:
    HICON m_hIcon;
    // Options file.
    CString m_csInitFile = _T(".\\client.ini");;
    CCommunicationIOCP m_IOCP;
    tagProfileInfo m_stGetProfile = { 0 };
    SOCKET m_sctConnect;
    //*****************************************
    //*ALARM* Those points need to free before program had finished.
    //*****************************************
    CConnectThread *m_pthdConnectThread = NULL;
    CFileTransportThread *m_pthdFileTransportThread = NULL;

    HANDLE m_hBreakEvent = NULL;

    IOCPTHREADADDTIONDATA m_stIOCPThreadAddtionData;
    PCONNECTTHREADPARAM m_pstConnectThreadParam = NULL;
    CString m_csUserInput;
    CCmdRecvThread *m_pthdCmdRecvThread = NULL;

    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
    afx_msg LRESULT OnBreakeventsignal(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    void SetConnectSocket(const SOCKET &sctTarget);
    BOOL ReadDataFromCmd();
    BOOL CheckPauseFlag(CPath &pathFilePathWithName);

protected:
    afx_msg LRESULT OnCreatecmdrecvthread(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSendordertocmd(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnGetfile(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnDestroy();
protected:
    afx_msg LRESULT OnTaskpause(WPARAM wParam, LPARAM lParam);
};
