// CmdEdit.cpp: 实现文件
//

#include "stdafx.h"
#include "TeleControl.h"
#include "CmdEdit.h"


// CCmdEdit

IMPLEMENT_DYNAMIC(CCmdEdit, CEdit)

CCmdEdit::CCmdEdit()
{
}

CCmdEdit::~CCmdEdit()
{
}


BEGIN_MESSAGE_MAP(CCmdEdit, CEdit)
    ON_WM_CHAR()
END_MESSAGE_MAP()



// CCmdEdit 消息处理程序




void CCmdEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    switch (nChar)
    {
        case VK_BACK:
        {
            if (!m_csUserInput.IsEmpty())
            {
                m_csUserInput =
                    m_csUserInput.Left(m_csUserInput.GetLength() - 1);
            }
            else
            {
                return;
            }

            break;
        }
        case VK_RETURN:
        {
            m_csUserInput += _T("\r\n");
            m_bHasDataToWrite = TRUE;

            GetParent()->SendMessage(WM_HASORDERTOSEND, 
                                     (WPARAM)&m_csUserInput,
                                     0);
            m_csUserInput = _T("");
            break;
        }
        default:
        {
            m_csUserInput += (TCHAR)nChar;
            break;
        }
    }

    CEdit::OnChar(nChar, nRepCnt, nFlags);
}
