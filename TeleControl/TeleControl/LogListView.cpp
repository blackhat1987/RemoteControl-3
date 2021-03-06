// LogListView.cpp: 实现文件
//

#include "stdafx.h"
#include "TeleControl.h"
#include "LogListView.h"
#include "StructShare.h"


// LogListView

IMPLEMENT_DYNCREATE(CLogListView, CListView)

CLogListView::CLogListView()
{

}

CLogListView::~CLogListView()
{
}

BEGIN_MESSAGE_MAP(CLogListView, CListView)
END_MESSAGE_MAP()


// LogListView 诊断

#ifdef _DEBUG
void CLogListView::AssertValid() const
{
    CListView::AssertValid();
}

#ifndef _WIN32_WCE
void CLogListView::Dump(CDumpContext& dc) const
{
    CListView::Dump(dc);
}
#endif
#endif //_DEBUG


// LogListView 消息处理程序


void CLogListView::OnInitialUpdate()
{
    CListView::OnInitialUpdate();

    CListCtrl &lstLogList = GetListCtrl();

    // To change the style of list.
    DWORD dwListStyle = lstLogList.GetStyle();
    dwListStyle |= LVS_REPORT | LVS_SHOWSELALWAYS;
    lstLogList.ModifyStyle(0, dwListStyle);

    // Init list control.
    int iColumnIndex = 0;
    int iTargetColumn = 0;
    // Time.
    iTargetColumn = lstLogList.InsertColumn(iColumnIndex++, _T("Time"));
    lstLogList.SetColumnWidth(iTargetColumn, 100);
    // Type.
    iTargetColumn = lstLogList.InsertColumn(iColumnIndex++, _T("Type"));
    lstLogList.SetColumnWidth(iTargetColumn, 100);
    // Info.
    iTargetColumn = lstLogList.InsertColumn(iColumnIndex++, _T("Info"));
    lstLogList.SetColumnWidth(iTargetColumn, 200);
} //! CLogListView::OnInitialUpdate END

// 
BOOL CLogListView::InsertLogInfo(DWORD dwType, const CString csInfo)
{
    CListCtrl &lstLogList = GetListCtrl();

    BOOL bRet = FALSE;
    CString csTime;
    struct tm *pstFormatTime = NULL;
    char acTmpBuffer[100] = { 0 };
    
    do
    {
        // Check index of type.
        if (dwType > LOG_TYPE_NUMBER)
        {
            break;
        }

        // Get time and format.
        time_t tCurrentTime = time(NULL);
        pstFormatTime = gmtime(&tCurrentTime);
        strftime(acTmpBuffer,
                 sizeof(acTmpBuffer),
                 "%Y-%m-%d %H:%M:%S",
                 pstFormatTime);
        DWORD dwRet =
            MultiByteToWideChar(CP_UTF8,
                                MB_ERR_INVALID_CHARS,
                                acTmpBuffer,
                                -1,
                                csTime.GetBufferSetLength(100),
                                100);
        if (0 == dwRet)
        {
            break;
        }

        csTime.ReleaseBuffer();

        // Insert time.
        int iInsertIndex = lstLogList.GetItemCount();
        iInsertIndex = lstLogList.InsertItem(iInsertIndex, csTime);
        if (-1 == iInsertIndex)
        {
            break;
        }

        // Insert type.
        bRet = lstLogList.SetItemText(iInsertIndex, 
                                      LLCT_TYPE,
                                      m_acsType[dwType]);
        if (!bRet)
        {
            break;
        }

        // Insert info.
        bRet = lstLogList.SetItemText(iInsertIndex,
                                      LLCT_INFO,
                                      csInfo);
        if (!bRet)
        {
            break;
        }

    } while (FALSE);

    return bRet;
} //! CLogListView::InsertLogInfo END 