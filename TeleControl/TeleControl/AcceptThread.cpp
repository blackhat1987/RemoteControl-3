#include "stdafx.h"
#include "AcceptThread.h"
#include "StructShare.h"
#include "CommunicationIOCP.h"
#include "ClientManager.h"
#include "HostListView.h"


CAcceptThread::CAcceptThread()
{
}


CAcceptThread::~CAcceptThread()
{
}

bool CAcceptThread::OnThreadEventRun(LPVOID lpParam)
{
    // Analysis parament.
    PACCEPTTHREADPARAM pAcceptThreadParam = (PACCEPTTHREADPARAM)lpParam;
    // IOCP����
    CCommunicationIOCP *pIOCP = pAcceptThreadParam->pIOCP_;
    // �ͻ��˹����߶���
    CClientManager *pClientManager = pAcceptThreadParam->pClientManager_;
    SOCKET sctAcceptSocket = pAcceptThreadParam->sctAcceptSocket_;
    CHostListView *pHostListView = pAcceptThreadParam->pHostListView_;

    // ѭ���ȴ�����
    while (TRUE)
    {
        sockaddr_in stClientAddrInfo = { 0 };
        stClientAddrInfo.sin_family = AF_INET;
        int iSize = sizeof(stClientAddrInfo);
        SOCKET sctClientSocket = SOCKET_ERROR;
        PCLIENTINFO pstClientInfo = NULL;
        BOOL bHasError = FALSE;

        // �ȴ����Ӳ���¼��Ϣ����IOCP
        do
        {
            sctClientSocket = accept(sctAcceptSocket,
                (sockaddr *)&stClientAddrInfo,
                                     &iSize);
            if (sctClientSocket == SOCKET_ERROR)
            {
                // �߳̽�����
                OutputDebugString(_T("Accept�߳��˳�\r\n"));
                return true;
            }

            // ��¼�ͻ�����Ϣ
            // *ע��* ��λ��ΪCLIENTINFO����������ڴ����ClientManager����
            // �����ӶϿ�ʱ�����ͷ�
            pstClientInfo = new CLIENTINFO;
            if (pstClientInfo == NULL)
            {
#ifdef DEBUG
                OutputDebugString(_T("�ͻ�����Ϣ�����ڴ�ʧ��\r\n"));
#endif // DEBUG
                bHasError = TRUE;
                break;
            }

            pstClientInfo->tLastTime_ = time(NULL);
            pstClientInfo->stClientAddrInfo_ = stClientAddrInfo;
            pstClientInfo->sctClientSocket_ = sctClientSocket;
            pstClientInfo->pFileTransferDlg_ = NULL;
            pstClientInfo->pCmdDlg_ = NULL;

            CString csIPAndPort;
            // Change address and Port to wide character.
            USES_CONVERSION;
            csIPAndPort.Format(_T("%s:%d"),
                               A2W(inet_ntoa(stClientAddrInfo.sin_addr)),
                               ntohs(stClientAddrInfo.sin_port));

            // ����Ϣ��ӵ��ͻ��˹�����
            pClientManager->InsertClient(sctClientSocket, pstClientInfo);
            pClientManager->InsertSocket(csIPAndPort, sctClientSocket);

            // ������ʾ�¿ͻ��˵�������Ϣ
            pHostListView->SendMessage(WM_HASINFOTOFLUSH,
                                       (WPARAM)IFT_ACCEPTCLIENT,
                                       (LPARAM)pstClientInfo);

            // ���ͻ���socket��IOCP��
            BOOL bRet =
                pIOCP->Associate((HANDLE)sctClientSocket,
                                 (ULONG_PTR)pstClientInfo);
            if (!bRet)
            {
                OutputDebugString(_T("�ͻ���Socket��IOCP��ʧ��\r\n"));
                bHasError = TRUE;
                break;
            }

            // Ͷ�ݽ�����Ϣ������
            pIOCP->PostRecvRequst(sctClientSocket);
        } while (FALSE); 

        // Continue when no error.
        if (!bHasError)
        {
            continue;
        }

        // Shutdown the socket which has connected if has error. 
        if (sctClientSocket != SOCKET_ERROR)
        {
            shutdown(sctClientSocket, SD_SEND);
            closesocket(sctClientSocket);
            sctClientSocket = SOCKET_ERROR;
        }

        // Free the heap memory.
        if (pstClientInfo != NULL)
        {
            delete pstClientInfo;
            pstClientInfo = NULL;
        }
    } // while ѭ���ȴ����� END

    return false;
} //! CAcceptThread::OnThreadEventRun END
