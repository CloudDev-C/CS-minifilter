#include "Util.h"

PsGetProcessPeb gPsGetProcessPeb = NULL;
ZwQuerySystemInformation gZwQuerySystemInformation = NULL;

NTSTATUS InitImport()
{
	NTSTATUS status = STATUS_SUCCESS;

	UNICODE_STRING FunctionName;

	RtlInitUnicodeString(&FunctionName, L"PsGetProcessPeb");

	gPsGetProcessPeb = (PsGetProcessPeb)MmGetSystemRoutineAddress(&FunctionName);

	if (gPsGetProcessPeb == NULL)
	{
		status = STATUS_UNSUCCESSFUL;
	}

	UNICODE_STRING zwName = RTL_CONSTANT_STRING(L"ZwQuerySystemInformation");

	gZwQuerySystemInformation = (ZwQuerySystemInformation)MmGetSystemRoutineAddress(&zwName);

	if (gZwQuerySystemInformation == NULL)
	{
		status = STATUS_UNSUCCESSFUL;
	}

	return status;
}

NTSTATUS LSFindProcessIdByName(IN PCWSTR imagename, OUT PHANDLE pid)
{
	NTSTATUS durum = STATUS_UNSUCCESSFUL;
	ULONG qmemsize = 0x1024;
	PVOID qmemptr = 0;
	P_SYSTEM_PROCESS_INFO_L spi;
	UNICODE_STRING uimagename;
	RtlInitUnicodeString(&uimagename, imagename);
	*pid = (PHANDLE)-1;
	do
	{
		qmemptr = ExAllocatePoolWithTag(PagedPool, qmemsize, 'CORP');

		if (qmemptr == NULL)
		{
			return STATUS_UNSUCCESSFUL;
		}

		durum = gZwQuerySystemInformation(SystemProcessInformation, qmemptr, qmemsize, NULL);

		if (durum == STATUS_INFO_LENGTH_MISMATCH)
		{
			qmemsize = qmemsize * 2;
			ExFreePool(qmemptr);
		}

	} while (durum == STATUS_INFO_LENGTH_MISMATCH);

	spi = (P_SYSTEM_PROCESS_INFO_L)qmemptr;

	while (1)
	{

		if (RtlEqualUnicodeString(&uimagename, &spi->ImageName, TRUE))
		{
			*pid = spi->ProcessId;
			break;
		}

		if (spi->NextEntryOffset == 0)
			break;

		spi = (P_SYSTEM_PROCESS_INFO_L)((unsigned char*)spi + spi->NextEntryOffset);
	}

	if (!NT_SUCCESS(durum))
	{
		ExFreePool(qmemptr);
		return STATUS_UNSUCCESSFUL;
	}

	ExFreePool(qmemptr);

	return STATUS_SUCCESS;
}

NTSTATUS KeReadProcessMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size)
{
	PEPROCESS SourceProcess = Process;

	PEPROCESS TargetProcess = PsGetCurrentProcess();
	SIZE_T Result;
	if (NT_SUCCESS(MmCopyVirtualMemory(SourceProcess, SourceAddress, TargetProcess, TargetAddress, Size, KernelMode, &Result)))
		return STATUS_SUCCESS;
	else
		return STATUS_ACCESS_DENIED;
}

NTSTATUS KeWriteProcessMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size)
{
	PEPROCESS SourceProcess = PsGetCurrentProcess();

	PEPROCESS TargetProcess = Process;

	SIZE_T Result;

	if (NT_SUCCESS(MmCopyVirtualMemory(SourceProcess, SourceAddress, TargetProcess, TargetAddress, Size, KernelMode, &Result)))
		return STATUS_SUCCESS;
	else
		return STATUS_ACCESS_DENIED;
}




VOID MY_InitUnicodeString(_Inout_ PUNICODE_STRING Dest, _In_ WCHAR* Source, USHORT Len, BOOLEAN IsUppStr)
{
	Dest->MaximumLength = Len * sizeof(WCHAR) + sizeof(UNICODE_NULL);

#if (USE_TAG)
	Dest->Buffer = ExAllocatePoolWithTag(NonPagedPool, Dest->MaximumLength, STRING_TAG);
#else
	Dest->Buffer = ExAllocatePoolWithTag(NonPagedPool, Dest->MaximumLength, 'EEEE');
#endif

	RtlZeroMemory(Dest->Buffer, Dest->MaximumLength);

	wcsncpy(Dest->Buffer, Source, Len);

	if (IsUppStr)
	{
		_wcsupr(Dest->Buffer);
	}

	Dest->Length = (USHORT)wcslen(Dest->Buffer) * sizeof(WCHAR);
}