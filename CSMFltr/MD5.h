#pragma once
#include <fltKernel.h>

// MD5.H

#ifndef _MD5_H
#define _MD5_H

/***********************************************************************
* Function Name: MD5File
* Function Description: Calculates the MD5 value of file
* Parameter List:
* PStrFileName: filename
* PStrFileMD5: get the MD5
* Return Value: NTSTATUS
* Note:
* PStrFileName should be the complete path, use symbolic links or device name
* Such as "C: \\ 1.txt" should be "\\ ?? \\ C: \\ 1.txt" or "\\ Device \\ HarddiskVolume1 \\ 1.txt"
* PStrFileMD5 responsible for memory allocation from the outside
***********************************************************************/
NTSTATUS
MD5File(IN PUNICODE_STRING pStrFileName, OUT PUNICODE_STRING pStrFileMD5, PFLT_FILTER HANDLE, PFLT_INSTANCE Instance);

/***********************************************************************
* Function Name: MD5String
* Function Description: Calculates the MD5 string values
* Parameter List:
* PStr: String
* PStrMD5: calculated MD5
* Return Value: NTSTATUS
* PStrFileMD5 responsible for memory allocation from the outside
***********************************************************************/
NTSTATUS
MD5String(IN PUNICODE_STRING pStr, OUT PUNICODE_STRING pStrMD5);

#endif