#include "globals.h"
#include "List.h"
#include "Util.h"
#include "Structures.h"
#include <ntstrsafe.h>
#include "Constants.h"

WCHAR* DRIVERNAME = L"CSMFltr";

UNICODE_STRING _system32 = RTL_CONSTANT_STRING(L"\\SYSTEM32");
UNICODE_STRING _winsxs = RTL_CONSTANT_STRING(L"\\WINSXS");
UNICODE_STRING _syswow64 = RTL_CONSTANT_STRING(L"\\SYSWOW64");

FLT_GLOBALS Globals;

NTSTATUS InitGlobals(PDRIVER_OBJECT DriverObject, _In_ PFLT_FILTER gFilterHandle)
{
	NTSTATUS status = STATUS_SUCCESS;

	PAGED_CODE();

	if (DriverObject == NULL)
	{
		return STATUS_INVALID_PARAMETER;
	}

	status = InitWindowsDirectory(gFilterHandle);

	if (!NT_SUCCESS(status))
	{
#if(SHOW_DBG_MESSAGE)
		DbgPrintEx(0, 0, "Error init system dirrectories!\r\n");
#endif

		return STATUS_UNSUCCESSFUL;
	}

	status = InitSteamDirectory();

	if (!NT_SUCCESS(status))
	{
#if(SHOW_DBG_MESSAGE)
		DbgPrintEx(0, 0, "Error init steam dirrectory!\r\n");
#endif
		return STATUS_UNSUCCESSFUL;
	}

	Globals.DriverObject = DriverObject;

	Globals.ProcessCount = 0;

	//Globals.Filter = gFilterHandle; //Need check it, write adress pre reg

	Globals.ProcCollection = (PLIST_ENTRY)ExAllocatePoolWithTag(NonPagedPool, sizeof(LIST_ENTRY), DRVTAG);

	if (!Globals.ProcCollection)
	{
		status = STATUS_UNSUCCESSFUL;
	}

	RtlZeroMemory(Globals.ProcCollection, sizeof(LIST_ENTRY));

	MY_InitList(Globals.ProcCollection, &Globals.ProcCollectionLock);

	return status;
}

NTSTATUS InitWindowsDirectory(IN PFLT_FILTER gFilterHandle)
{
	NTSTATUS status;

	UNICODE_STRING SystemRoot;

	RtlInitUnicodeString(&SystemRoot, L"\\SystemRoot");

	PFLT_VOLUME Volume;

	PDEVICE_OBJECT pDiskDeviceObject = NULL;

	UNICODE_STRING VolumeName = { 0 };

	UNICODE_STRING		systemRootSymbolicLink1 = { 0 };
	HANDLE	linkHandle;

	__try
	{
		status = GetSymbolicLink(&SystemRoot, &systemRootSymbolicLink1, &linkHandle);

		if (NT_SUCCESS(status))
		{
			status = FltGetVolumeFromName(gFilterHandle, &SystemRoot, &Volume);
			
			if (!NT_SUCCESS(status))
			{
				return status;
			}
			
			status = FltGetDiskDeviceObject(Volume, &pDiskDeviceObject);
			
			if (!NT_SUCCESS(status))
			{
				return status;
			}
			
			status = RtlVolumeDeviceToDosName(pDiskDeviceObject, &VolumeName);
			
			if (!NT_SUCCESS(status))
			{
				return status;
			}

			
			status = ConstructString(&VolumeName, &systemRootSymbolicLink1);
			
			if (!NT_SUCCESS(status))
			{
				return status;
			}
		}

		return status;
	}

	__finally
	{
		if (systemRootSymbolicLink1.Buffer)
		{
			ExFreePool(systemRootSymbolicLink1.Buffer);
		}
	}
}

static NTSTATUS GetSymbolicLink(IN PUNICODE_STRING SymbolicLinkName, OUT PUNICODE_STRING SymbolicLink, OUT PHANDLE LinkHandle)
{
	NTSTATUS			status;
	NTSTATUS			returnStatus;
	OBJECT_ATTRIBUTES	oa;
	UNICODE_STRING		tmpSymbolicLink;
	HANDLE				tmpLinkHandle;
	ULONG				symbolicLinkLength;

	returnStatus = STATUS_UNSUCCESSFUL;

	//
	// Open and query the symbolic link
	//
	InitializeObjectAttributes
	(
		&oa,
		SymbolicLinkName,
		OBJ_KERNEL_HANDLE,
		NULL,
		NULL
	);

	status = ZwOpenSymbolicLinkObject
	(
		&tmpLinkHandle,
		GENERIC_READ,
		&oa
	);

	if (STATUS_SUCCESS == status)
	{
		//
		// Get the size of the symbolic link string
		//
		symbolicLinkLength = 0;
		tmpSymbolicLink.Length = 0;
		tmpSymbolicLink.MaximumLength = 0;

		status = ZwQuerySymbolicLinkObject
		(
			tmpLinkHandle,
			&tmpSymbolicLink,
			&symbolicLinkLength
		);


		if (STATUS_BUFFER_TOO_SMALL == status && symbolicLinkLength > 0)
		{
			//
			// Allocate the memory and get the ZwQuerySymbolicLinkObject
			//
			tmpSymbolicLink.Buffer = ExAllocatePoolWithTag(NonPagedPool, symbolicLinkLength, DRVTAG);
			tmpSymbolicLink.Length = 0;
			tmpSymbolicLink.MaximumLength = (USHORT)symbolicLinkLength;

			status = ZwQuerySymbolicLinkObject
			(
				tmpLinkHandle,
				&tmpSymbolicLink,
				&symbolicLinkLength
			);

			if (STATUS_SUCCESS == status)
			{
				SymbolicLink->Buffer = tmpSymbolicLink.Buffer;
				SymbolicLink->Length = tmpSymbolicLink.Length;
				*LinkHandle = tmpLinkHandle;
				SymbolicLink->MaximumLength = tmpSymbolicLink.MaximumLength;
				returnStatus = STATUS_SUCCESS;
			}
			else
			{
				ExFreePool(tmpSymbolicLink.Buffer);
			}
		}
	}


	return returnStatus;
}

NTSTATUS ConstructString(IN PUNICODE_STRING Letter, IN PUNICODE_STRING SourceWindows)
{
	NTSTATUS status = STATUS_SUCCESS;

	int sCount = 0;
	int sCount1 = 0;

	WCHAR WIN[20] = { 0 };
	USHORT WinLen = 0;

	UNICODE_STRING tempString = { 0 };

	status = RtlUpcaseUnicodeString(&tempString, SourceWindows, TRUE);

	if (!NT_SUCCESS(status))
	{
		return status;
	}

	__try
	{
		for (int i = 0; i < SourceWindows->Length / 2; i++)
		{
			if (SourceWindows->Buffer[i] == L'\\')
			{
				sCount++;
			}
		}

		for (USHORT i = 0; i < SourceWindows->Length / 2; i++)
		{
			if (SourceWindows->Buffer[i] == L'\\')
			{
				sCount1++;

				if (sCount1 == sCount)
				{
					WinLen = (SourceWindows->Length / 2) - i;
					wcsncpy(WIN, &SourceWindows->Buffer[i], WinLen);
					break;
				}
			}
		}

		WCHAR Buff[MAX_PATH] = { 0 };

		//Start init system32 directory

		swprintf_s(Buff, Letter->Length + WinLen * sizeof(WCHAR), L"%ws%ws%ws", Letter->Buffer, WIN, _system32.Buffer);

		USHORT Len = (Letter->Length + _system32.Length) / (USHORT)sizeof(WCHAR) + WinLen;

		MY_InitUnicodeString(&Globals.Windows_system32, Buff, Len, TRUE);
#if(SHOW_DBG_MESSAGE)
		DbgPrintEx(0, 0, "-----------------------------------\r\n");

		DbgPrintEx(0, 0, "System32 Buffer: %ws\r\n", Buff);

		DbgPrintEx(0, 0, "System32 directory: %wZ\r\n", Globals.Windows_system32);

		DbgPrintEx(0, 0, "-----------------------------------\r\n");
#endif
		//end init system32 directory

		//------

		//start init winsxs directory

		RtlZeroMemory(Buff, sizeof(Buff));

		swprintf_s(Buff, Letter->Length + WinLen * sizeof(WCHAR), L"%ws%ws%ws", Letter->Buffer, WIN, _winsxs.Buffer);

		Len = (Letter->Length + _winsxs.Length) / (USHORT)sizeof(WCHAR) + WinLen;

		MY_InitUnicodeString(&Globals.Windows_winsxs, Buff, Len, TRUE);
#if(SHOW_DBG_MESSAGE)
		DbgPrintEx(0, 0, "-----------------------------------\r\n");

		DbgPrintEx(0, 0, "winsxs Buffer: %ws\r\n", Buff);

		DbgPrintEx(0, 0, "winsxs directory: %wZ\r\n", Globals.Windows_winsxs);

		DbgPrintEx(0, 0, "-----------------------------------\r\n");
#endif
		//end init winsxs directory

#if (_WIN64)
		//start init syswow64 directory
		RtlZeroMemory(Buff, sizeof(Buff));

		swprintf_s(Buff, Letter->Length + WinLen * sizeof(WCHAR), L"%ws%ws%ws", Letter->Buffer, WIN, _syswow64.Buffer);

		Len = (Letter->Length + _syswow64.Length) / (USHORT)sizeof(WCHAR) + WinLen;

		MY_InitUnicodeString(&Globals.Windows_syswow64, Buff, Len, TRUE);
#if(SHOW_DBG_MESSAGE)
		DbgPrintEx(0, 0, "-----------------------------------\r\n");

		DbgPrintEx(0, 0, "syswow64 Buffer: %ws\r\n", Buff);

		DbgPrintEx(0, 0, "syswow64 directory: %wZ\r\n", Globals.Windows_syswow64);

		DbgPrintEx(0, 0, "-----------------------------------\r\n");
#endif
		//end init syswow64 directory
#endif
	}
	__finally
	{
		RtlFreeUnicodeString(&tempString);	
	}

	return status;
}

VOID FreeGlobals()
{
	FreeProcessList();

	if (Globals.Windows_system32.Buffer)
	{
		ExFreePool(Globals.Windows_system32.Buffer);
	}

#if (_WIN64)
	if (Globals.Windows_syswow64.Buffer)
	{
		ExFreePool(Globals.Windows_syswow64.Buffer);
	}
#endif

	if (Globals.Windows_winsxs.Buffer)
	{
		ExFreePool(Globals.Windows_winsxs.Buffer);
	}

	if (Globals.Directory_CS16.Buffer)
	{
		ExFreePool(Globals.Directory_CS16.Buffer);
	}

	if (Globals.Directory_CSGO.Buffer)
	{
		ExFreePool(Globals.Directory_CSGO.Buffer);
	}

	if (Globals.SteamPath.Buffer)
	{
		ExFreePool(Globals.SteamPath.Buffer);
	}
}

VOID InitGameDirectory(PUNICODE_STRING Dest, PUNICODE_STRING Source, USHORT GameType)
{
	WCHAR* searchPtr = NULL;

	USHORT offset = 0;

	if (GameType == CS)
	{
		searchPtr = wcsstr(Source->Buffer, CSMName);
	}
	else if (GameType == CSGO)
	{
		searchPtr = wcsstr(Source->Buffer, CSGOMName);
	}

	USHORT copyCount = 0;

	if (searchPtr)
	{
		for (; copyCount < Source->Length / sizeof(WCHAR); copyCount++)
		{
			if (&Source->Buffer[copyCount] == searchPtr)
			{
				break;
			}
		}

		if (Source->Buffer[0] == '\"')
		{
			offset = 1;
			copyCount -= offset;
		}

		if (GameType == CS)
		{
			MY_InitUnicodeString(Dest, Source->Buffer + offset, copyCount, TRUE);
#if(SHOW_DBG_MESSAGE)
			DbgPrintEx(0, 0, "CS16 Dirrectory is: %wZ\r\n", Dest);
#endif
		}

		else if (GameType == CSGO)
		{
			MY_InitUnicodeString(Dest, Source->Buffer + offset, copyCount, TRUE);
#if(SHOW_DBG_MESSAGE)
			DbgPrintEx(0, 0, "CSGO Dirrectory is: %wZ\r\n", Dest);
#endif
		}
	}
}

NTSTATUS GetUsermodePath(_In_ PFLT_FILE_NAME_INFORMATION FileNameInfo, _In_ PCFLT_RELATED_OBJECTS FltObjects, _Inout_ PUNICODE_STRING FullPath)
{
	NTSTATUS status;

	PDEVICE_OBJECT pDiskDeviceObject = NULL;
	UNICODE_STRING VolumeName;


	status = FltGetDiskDeviceObject(FltObjects->Volume, &pDiskDeviceObject);

	if (!NT_SUCCESS(status))
	{
		return status;
	}

	status = RtlVolumeDeviceToDosName(pDiskDeviceObject, &VolumeName);

	if (!NT_SUCCESS(status))
	{
		return status;
	}

	__try
	{
		RtlInitUnicodeString(FullPath, (PWSTR)NULL);

		FullPath->MaximumLength = (USHORT)(VolumeName.Length + FileNameInfo->Name.Length + sizeof(UNICODE_NULL));

#if (!USE_TAG)
		FullPath->Buffer = (PWSTR)ExAllocatePool(PagedPool, FullPath->MaximumLength);
#else
		FullPath->Buffer = (PWSTR)ExAllocatePoolWithTag(PagedPool, FullPath->MaximumLength, DRVTAG);
#endif

		if (FullPath->Buffer != NULL)
		{
			status = RtlAppendUnicodeStringToString(FullPath, &VolumeName);

			if (!NT_SUCCESS(status))
			{
				return status;
			}

			status = RtlAppendUnicodeStringToString(FullPath, &FileNameInfo->ParentDir);

			if (!NT_SUCCESS(status))
			{
				return status;
			}

			if (!NT_SUCCESS(status))
			{
				return status;
			}

			status = RtlAppendUnicodeStringToString(FullPath, &FileNameInfo->FinalComponent);

			if (!NT_SUCCESS(status))
			{
				return status;
			}
		}
	}
	__finally
	{
		if (pDiskDeviceObject)
		{
			ObDereferenceObject(pDiskDeviceObject);
		}

		if (VolumeName.Buffer)
		{
			ExFreePool(VolumeName.Buffer);
		}
	}

	return status;
}

NTSTATUS InitSteamDirectory()
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	HANDLE pid;

	PWCH searchPtr = NULL;

	UNICODE_STRING tmpSteam = { 0 };

	USHORT copyCount = 0;

	status = LSFindProcessIdByName(STEAMMName, &pid);

	if (NT_SUCCESS(status))
	{
		PEPROCESS hProcess = NULL;

		KAPC_STATE kapc;

		BOOLEAN BIsAttached = FALSE;

		status = PsLookupProcessByProcessId(pid, &hProcess);

		if (NT_SUCCESS(status))
		{
			PPEB pPeb = gPsGetProcessPeb(hProcess);

			if (pPeb == NULL)
			{
				return STATUS_UNSUCCESSFUL;
			}

			KeStackAttachProcess(hProcess, &kapc);

			BIsAttached = TRUE;

			status = RtlUpcaseUnicodeString(&tmpSteam, &pPeb->ProcessParameters->ImagePathName, TRUE);

			if (!NT_SUCCESS(status))
			{
				return STATUS_UNSUCCESSFUL;
			}

			__try
			{
				searchPtr = wcsstr(tmpSteam.Buffer, STEAMMName_);

				if (!searchPtr)
				{
					return STATUS_UNSUCCESSFUL;
				}

				for (; copyCount < tmpSteam.Length / sizeof(WCHAR); copyCount++)
				{
					if (&tmpSteam.Buffer[copyCount] == searchPtr)
					{
						break;
					}
				}

				MY_InitUnicodeString(&Globals.SteamPath, tmpSteam.Buffer, copyCount, TRUE);
#if(SHOW_DBG_MESSAGE)
				DbgPrintEx(0, 0, "Steam Directory is: %wZ\r\n", Globals.SteamPath);
#endif
				status = STATUS_SUCCESS;
			}
			__finally
			{
				RtlFreeUnicodeString(&tmpSteam);
			}

			if (BIsAttached != FALSE)
			{
				KeUnstackDetachProcess(&kapc);
			}

			if (hProcess != NULL)
			{
				ObDereferenceObject(hProcess);

				hProcess = NULL;
			}
		}
		else
		{
			status = STATUS_UNSUCCESSFUL;
		}
	}

	return status;
}
