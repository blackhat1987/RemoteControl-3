#include "stdafx.h"
#include "ConnectThread.h"
#include "StructShare.h"
#include "CommunicationIOCP.h"
#include "TeleClientDlg.h"


CConnectThread::CConnectThread()
{
}


CConnectThread::~CConnectThread()
{
}

bool CConnectThread::OnThreadEventRun(LPVOID lpParam)
{
    // ��������
    PCONNECTTHREADPARAM pConnectThreadParam = (PCONNECTTHREADPARAM)lpParam;
    // ��ȡIOCP�����
    CCommunicationIOCP *pIOCP = pConnectThreadParam->pIOCP_;
    // ��ȡ��������ַ��Ϣ
    sockaddr_in stServerAddrInfo = pConnectThreadParam->stServerAddrInfo_;
    // ��ȡ�ж��¼����
    HANDLE *phBreakEvent = pConnectThreadParam->phBreakEvent_;
    // ��ȡ�ͻ��˴��ڵ�ָ��
    CTeleClientDlg *pTeleClientDlg = pConnectThreadParam->pTeleClientDlg_;

    int iRet = 0;
    SOCKET sctConnectSocket = INVALID_SOCKET;
    BOOL bHasError = FALSE;

    // ѭ����������
    while (TRUE)
    {
        do
        {
            sctConnectSocket = socket(AF_INET,
                                      SOCK_STREAM,
                                      IPPROTO_TCP);

            if (sctConnectSocket == INVALID_SOCKET)
            {
                OutputDebugString(_T("����Socketʧ��\r\n"));
                bHasError = TRUE;
                break;
            }

            // �Ի�������µ�Sokcet
            pTeleClientDlg->SetConnectSocket(sctConnectSocket);

            iRet = connect(sctConnectSocket,
                (sockaddr *)&stServerAddrInfo,
                           sizeof(stServerAddrInfo));

            if (iRet == SOCKET_ERROR)
            {
                WaitForSingleObject(*phBreakEvent, CONNECT_WAIT_TIME);
                bHasError = TRUE;
                break;
            }

            // ����������Ϣ
            PCLIENTINFO pstClientInfo = new CLIENTINFO;
            pstClientInfo->sctClientSocket_ = sctConnectSocket;
            pstClientInfo->tLastTime_ = time(NULL);
            pstClientInfo->hCMD_ = INVALID_HANDLE_VALUE;
            pstClientInfo->hCmdReadPipe_ = INVALID_HANDLE_VALUE;
            pstClientInfo->hCmdWritePipe_ = INVALID_HANDLE_VALUE;
            pstClientInfo->pTeleClientDlg_ = pTeleClientDlg;

            // �����ౣ��Socket��������Ϣ
            pTeleClientDlg->m_pstClientInfo = pstClientInfo;
            // ��socket��IOCP
            BOOL bRet = pIOCP->Associate((HANDLE)sctConnectSocket,
                (ULONG_PTR)pstClientInfo);
            if (!bRet)
            {
                OutputDebugString(_T("��IOCPʧ��\r\n"));
                bHasError = TRUE;
                break;
            }

            // Ͷ��һ����������
            pIOCP->PostRecvRequst(pstClientInfo->sctClientSocket_);

            // ���ü�ʱ��
            bRet = pTeleClientDlg->SetTimer(TIMER_HEATBEAT,
                                            HEATBEAT_ELAPSE,
                                            NULL);

            // �ȴ������ж��¼�����
            DWORD dwRet = WaitForSingleObject(*phBreakEvent, INFINITE);
            pTeleClientDlg->m_bBreakEventHasSigal = FALSE;
            if (dwRet == WAIT_FAILED)
            {
                // �¼���ΪĳЩԭ�򱻹رգ������˳�
#ifdef DEBUG
                OutputDebugString(_T("�ж��¼��ȴ��쳣\r\n"));
#endif // DEBUG
            }
            else if (dwRet == WAIT_OBJECT_0)
            {
#ifdef DEBUG
                OutputDebugString(_T("���������ж��¼�\r\n"));
#endif // DEBUG
            }
        } while (FALSE);

        // ����
        shutdown(sctConnectSocket, SD_SEND);
        closesocket(sctConnectSocket);

        // �رն�ʱ��
        pTeleClientDlg->KillTimer(TIMER_HEATBEAT);

        // �ͷ�Socket��������Ϣ
        if (pTeleClientDlg->m_pstClientInfo != NULL)
        {
            delete pTeleClientDlg->m_pstClientInfo;
            pTeleClientDlg->m_pstClientInfo = NULL;
        }
    } //! while ѭ���������� END

    return true;
} //! CConnectThread::OnThreadEventRun END