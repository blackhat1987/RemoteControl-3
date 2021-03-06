#pragma once
#ifndef ASSISTFUNC_H_
#define ASSISTFUNC_H_
#include "StructShare.h"
#include "CommunicationIOCP.h"

void OutputDebugStringWithInfo(_In_ const CString csOuput,
                               _In_ const CString csFileNmae,
                               _In_ DWORD dwFileLine);


void GetErrorMessage(_In_ DWORD dwError,
                     _In_ CString &ref_csMessage);

#endif // !ASSISTFUNC_H_
