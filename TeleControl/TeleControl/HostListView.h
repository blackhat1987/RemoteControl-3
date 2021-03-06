#pragma once
#include "AcceptThread.h"
#include "CommunicationIOCP.h"
#include "ClientManager.h"
#include "FileTransferDlg.h"
#include "CmdDlg.h"


// CHostLisView 视图

class CHostListView : public CListView
{
    DECLARE_DYNCREATE(CHostListView)

private:
    SOCKET m_sctAccept;
    // The name of initional file.
    CString m_csInitFile = _T(".\\server.ini");
    // The thread work to wait connect request.
    CAcceptThread *m_pthdAcceptThread = NULL;
    // The parament of accept thread.
    PACCEPTTHREADPARAM m_pstAcceptThreadParam;
    // Addtion data for IOCP thread.
    IOCPTHREADADDTIONDATA m_stIOCPThreadAddtionData;
    // Client manager object.
    CClientManager m_ClientManager;
    // IOCP object.
    CCommunicationIOCP m_IOCP;
    CString m_csLogInfo;

protected:
    CHostListView();           // 动态创建所使用的受保护的构造函数
    virtual ~CHostListView();

public:
#ifdef _DEBUG
    virtual void AssertValid() const;
#ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
    DECLARE_MESSAGE_MAP()
public:
    virtual void OnInitialUpdate();
    afx_msg void OnMnStart();
    virtual void PostNcDestroy();
    CView * GetTargetView(const CRuntimeClass * pClass);
protected:
    afx_msg LRESULT OnHasinfotoflush(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnMnClose();
    afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnMnOperationFiletransfer();
    afx_msg void OnMnCmd();
    afx_msg void OnMnProcessmanager();
    afx_msg void OnMnScreenshower();
};


