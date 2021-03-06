
// MainFrm.cpp: CMainFrame 类的实现
//

#include "stdafx.h"
#include "TeleControl.h"

#include "MainFrm.h"
#include "HostListView.h"
#include "LogListView.h"
#include "StructShare.h"
#include "ProcessManagerDlg.h"
#include "FileTransferDlg.h"
#include "ScreenShowerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()
    ON_MESSAGE(WM_HASDLGCLOSE, &CMainFrame::OnHasdlgclose)
    ON_WM_CLOSE()
END_MESSAGE_MAP()

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
    // TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CFrameWnd::PreCreateWindow(cs) )
        return FALSE;
    // TODO: 在此处通过修改
    //  CREATESTRUCT cs 来修改窗口类或样式

    cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
         | WS_MINIMIZEBOX | WS_SYSMENU;

    cs.cx = 600;
    cs.cy = 500;
    cs.hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MAINMENU));

    return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序



BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
    BOOL bRet = FALSE;
    bRet = m_MainWindowsView.CreateStatic(this, 2, 1);

        
    // TODO: 在此添加专用代码和/或调用基类
    bRet = m_MainWindowsView.CreateView(0, 0,
                                        RUNTIME_CLASS(CHostListView),
                                        SIZE{ 200, 300 },
                                        pContext);

    bRet = m_MainWindowsView.CreateView(1, 0,
                                        RUNTIME_CLASS(CLogListView),
                                        SIZE{ 200, 300 },
                                        pContext);

    return bRet;
    //return CFrameWnd::OnCreateClient(lpcs, pContext);
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    // TODO:  在此添加您专用的创建代码
    m_bAutoMenuEnable = FALSE;

    return 0;
}

//Deal with someone dailog had closed.
//Main active is free resource.
afx_msg LRESULT CMainFrame::OnHasdlgclose(WPARAM wParam, LPARAM lParam)
{ 
    PCLIENTINFO pstClientInfo = (PCLIENTINFO)wParam;
    DIALOGTYPE eDailogType = (DIALOGTYPE)lParam;

    // Free resource by type.
    switch (eDailogType)
    {
        case CDT_CMD:
        {
            if (NULL != pstClientInfo->pCmdDlg_)
            {
                delete pstClientInfo->pCmdDlg_;
                pstClientInfo->pCmdDlg_ = NULL;
            }

            break;
        }
        case CDT_FILETRANSFER:
        {
            if (NULL != pstClientInfo->pFileTransferDlg_)
            {
                delete pstClientInfo->pFileTransferDlg_;
                pstClientInfo->pFileTransferDlg_ = NULL;
            }

            break;
        }
        case CDT_PROCESSMANAGER:
        {
            if (NULL != pstClientInfo->pProcessManagerDlg_)
            {
                delete pstClientInfo->pProcessManagerDlg_;
                pstClientInfo->pProcessManagerDlg_ = NULL;
            }

            break;
        }
        case CDT_SCREENSHOWER:
        {
            if (NULL != pstClientInfo->pScreenShowerDlg_)
            {
                delete pstClientInfo->pScreenShowerDlg_;
                pstClientInfo->pScreenShowerDlg_ = NULL;
            }

            break;
        }
        default:
        {
            break;
        }
    } //! switch "Free resource by type" End

    return 0;
} //! CMainFrame::OnHasdlgclose END


void CMainFrame::OnClose()
{

    CFrameWnd::OnClose();
}
