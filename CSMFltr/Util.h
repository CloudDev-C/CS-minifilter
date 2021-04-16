#pragma once
#include <fltKernel.h>

typedef struct _SYSTEM_PROCESS_INFO_L
{
	ULONG                   NextEntryOffset;
	ULONG                   NumberOfThreads;
	LARGE_INTEGER           Reserved[3];
	LARGE_INTEGER           CreateTime;
	LARGE_INTEGER           UserTime;
	LARGE_INTEGER           KernelTime;
	UNICODE_STRING          ImageName;
	ULONG                   BasePriority;
	HANDLE                  ProcessId;
	HANDLE                  InheritedFromProcessId;
}_SYSTEM_PROCESS_INFO_L, * P_SYSTEM_PROCESS_INFO_L;

typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemBasicInformation,
	SystemProcessorInformation,
	SystemPerformanceInformation,
	SystemTimeOfDayInformation,
	SystemPathInformation,
	SystemProcessInformation,
	SystemCallCountInformation,
	SystemDeviceInformation,
	SystemProcessorPerformanceInformation,
	SystemFlagsInformation,
	SystemCallTimeInformation,
	SystemModuleInformation
} SYSTEM_INFORMATION_CLASS,
* PSYSTEM_INFORMATION_CLASS;

typedef NTSTATUS(*ZwQuerySystemInformation)
(SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength);

typedef PPEB(*PsGetProcessPeb)
(IN PEPROCESS Process);

NTSTATUS NTAPI MmCopyVirtualMemory
(
	PEPROCESS SourceProcess,
	PVOID SourceAddress,
	PEPROCESS TargetProcess,
	PVOID TargetAddress,
	SIZE_T BufferSize,
	KPROCESSOR_MODE PreviousMode,
	PSIZE_T ReturnSize
);

EXTERN_C
NTSTATUS
NTAPI
PsAcquireProcessExitSynchronization(
	_In_ PEPROCESS Process
);

EXTERN_C
VOID
NTAPI
PsReleaseProcessExitSynchronization(
	_In_ PEPROCESS Process
);


NTSTATUS InitImport();
NTSTATUS LSFindProcessIdByName(IN PCWSTR imagename, OUT PHANDLE pid);
NTSTATUS KeReadProcessMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size);
NTSTATUS KeWriteProcessMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size);

VOID MY_InitUnicodeString(_Inout_ PUNICODE_STRING Dest, _In_ WCHAR* Source, USHORT Len, BOOLEAN IsUppStr);


extern PsGetProcessPeb gPsGetProcessPeb;
extern ZwQuerySystemInformation gZwQuerySystemInformation;