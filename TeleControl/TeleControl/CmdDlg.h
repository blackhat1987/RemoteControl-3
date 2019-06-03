#pragma once
#include "afxwin.h"
#include "CmdEdit.h"
#include "CommunicationIOCP.h"
#include "StructShare.h"

// CCmdDlg �Ի���

class CCmdDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CCmdDlg)
private:
    CString &m_ref_csIPAndPort;
    PCLIENTINFO m_pstClientInfo;
    CCommunicationIOCP &m_ref_IOCP;

    // The event for has command need to send to target host.
    HANDLE m_hSendCommandEvent = NULL;


public:
    CCmdDlg(CString &ref_csIPAndPort,
            PCLIENTINFO pClientInfo,
            CCommunicationIOCP &ref_IOCP,
            CWnd* pParent = NULL);   // ��׼���캯��
    virtual ~CCmdDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_CMD };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()
public:
    // Screen Buffer.
    CString m_csScreen;

    // Edit Control.
    CCmdEdit m_edtCmd;
    virtual BOOL OnInitDialog();
protected:
    afx_msg LRESULT OnHascmdreply(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnHasordertosend(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnClose();
    afx_msg void OnDestroy();
};
