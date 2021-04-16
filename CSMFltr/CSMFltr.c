#include <fltKernel.h>
#include <dontuse.h>
#include <wdm.h>
#include "CSMFltr.h"
#include "globals.h"
#include "List.h"
#include "Util.h"
#include "MD5.h"
#include "Constants.h"

PFLT_FILTER gFilterHandle;
ULONG_PTR OperationStatusCtx = 1;

CONST FLT_OPERATION_REGISTRATION Callbacks[] = {

#if 0 // TODO - List all of the requests to filter.

	{ IRP_MJ_CREATE_NAMED_PIPE,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_READ,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_WRITE,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_QUERY_INFORMATION,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_SET_INFORMATION,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_QUERY_EA,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_SET_EA,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_FLUSH_BUFFERS,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_QUERY_VOLUME_INFORMATION,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_SET_VOLUME_INFORMATION,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_DIRECTORY_CONTROL,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_FILE_SYSTEM_CONTROL,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_DEVICE_CONTROL,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_INTERNAL_DEVICE_CONTROL,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_SHUTDOWN,
	  0,
	  CSMFltrPreOperationNoPostOperation,
	  NULL },                               //post operations not supported

	{ IRP_MJ_LOCK_CONTROL,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_CLEANUP,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_CREATE_MAILSLOT,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_QUERY_SECURITY,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_SET_SECURITY,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_QUERY_QUOTA,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_SET_QUOTA,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_PNP,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_ACQUIRE_FOR_MOD_WRITE,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_RELEASE_FOR_MOD_WRITE,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_ACQUIRE_FOR_CC_FLUSH,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_RELEASE_FOR_CC_FLUSH,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_NETWORK_QUERY_OPEN,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_MDL_READ,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_MDL_READ_COMPLETE,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_PREPARE_MDL_WRITE,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_MDL_WRITE_COMPLETE,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_VOLUME_MOUNT,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

	{ IRP_MJ_VOLUME_DISMOUNT,
	  0,
	  CSMFltrPreOperation,
	  CSMFltrPostOperation },

#endif // TODO
	  { IRP_MJ_CREATE,
		0,
		CSMFltrPreOperation,
		NULL },
		//{ IRP_MJ_CLOSE,
		//  0,
		//  CSMFltrPreOperation,
		//  NULL },

	  { IRP_MJ_OPERATION_END }
};

//
//  This defines what we want to filter with FltMgr
//

CONST FLT_REGISTRATION FilterRegistration = {

	sizeof(FLT_REGISTRATION),         //  Size
	FLT_REGISTRATION_VERSION,           //  Version
	0,                                  //  Flags

	NULL,                               //  Context
	Callbacks,                          //  Operation callbacks

	CSMFltrUnload,                           //  MiniFilterUnload

	NULL,                    //  InstanceSetup
	NULL,            //  InstanceQueryTeardown
	NULL,            //  InstanceTeardownStart
	NULL,         //  InstanceTeardownComplete

	NULL,                               //  GenerateFileName
	NULL,                               //  GenerateDestinationFileName
	NULL                                //  NormalizeNameComponent

};


/*************************************************************************
	MiniFilter initialization and unload routines.
*************************************************************************/
VOID CreateProcessNotifyRoutineEx(_Inout_ PEPROCESS Process, _In_ HANDLE ProcessId, _Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo)
{
	UNREFERENCED_PARAMETER(Process);
	UNREFERENCED_PARAMETER(ProcessId);

	UNICODE_STRING tmpCS = { 0 };

	if (CreateInfo)
	{
		if (CreateInfo->CommandLine->Buffer)
		{
			//CS 1.6
			if (wcsstr(CreateInfo->CommandLine->Buffer, L"\\hl.exe\" -steam -game cstrike") ||
				wcsstr(CreateInfo->CommandLine->Buffer, L"\\hl.exe -steam -game cstrike"))
			{
				if (Globals.Directory_CS16.Length <= 0)
				{
					RtlUpcaseUnicodeString(&tmpCS, CreateInfo->CommandLine, TRUE);

					InitGameDirectory(&Globals.Directory_CS16, &tmpCS, CS);

					RtlFreeUnicodeString(&tmpCS);
				}

				AddProcessToList(ProcessId, CS);
			}

			//CSGO

			if (wcsstr(CreateInfo->CommandLine->Buffer, L"\\csgo.exe\" -steam") ||
				wcsstr(CreateInfo->CommandLine->Buffer, L"\\csgo.exe -steam"))
			{
				if (Globals.Directory_CSGO.Length <= 0)
				{
					RtlUpcaseUnicodeString(&tmpCS, CreateInfo->CommandLine, TRUE);

					InitGameDirectory(&Globals.Directory_CSGO, &tmpCS, CSGO);

					RtlFreeUnicodeString(&tmpCS);
				}

				AddProcessToList(ProcessId, CSGO);
			}
		}
	}
	else
	{
		RemoveProcessInList(ProcessId);
	}
}

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;

	UNREFERENCED_PARAMETER(RegistryPath);

	DriverObject->DriverUnload = DriverUnload;

	status = PsSetCreateProcessNotifyRoutineEx(CreateProcessNotifyRoutineEx, FALSE);

	if (!NT_SUCCESS(status))
	{
		return status;
	}

	status = InitImport();

	if (!NT_SUCCESS(status))
	{
		PsSetCreateProcessNotifyRoutineEx(CreateProcessNotifyRoutineEx, TRUE);

		return status;
	}

	status = FltRegisterFilter(DriverObject, &FilterRegistration, &gFilterHandle);

	if (NT_SUCCESS(status))
	{
		status = InitGlobals(DriverObject, gFilterHandle);

		if (!NT_SUCCESS(status))
		{
			PsSetCreateProcessNotifyRoutineEx(CreateProcessNotifyRoutineEx, TRUE);

			FltUnregisterFilter(gFilterHandle);

			return status;
		}

		status = FltStartFiltering(gFilterHandle);

		if (!NT_SUCCESS(status)) {

			PsSetCreateProcessNotifyRoutineEx(CreateProcessNotifyRoutineEx, TRUE);

			FltUnregisterFilter(gFilterHandle);
		}
	}

	return status;
}

NTSTATUS
CSMFltrUnload(_In_ FLT_FILTER_UNLOAD_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(Flags);

	PAGED_CODE();

	FltUnregisterFilter(gFilterHandle);

	DbgPrintEx(0, 0, "\nUnloadFilter\n");

	return STATUS_SUCCESS;
}

NTSTATUS DriverUnload(PDRIVER_OBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);

	PAGED_CODE();

	DbgPrintEx(0, 0, "\nUnloadDriver\n");

	PsSetCreateProcessNotifyRoutineEx(CreateProcessNotifyRoutineEx, TRUE);

	FreeGlobals();

	return STATUS_SUCCESS;
}


/*************************************************************************
	MiniFilter callback routines.
*************************************************************************/
FLT_PREOP_CALLBACK_STATUS
CSMFltrPreOperation(_Inout_ PFLT_CALLBACK_DATA Data, _In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID* CompletionContext)
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);

	NTSTATUS status = STATUS_SUCCESS;

	FLT_PREOP_CALLBACK_STATUS resStatus = FLT_PREOP_SUCCESS_WITH_CALLBACK;

	ACCESS_MASK desiredAccess;

	HANDLE ProcessID = NULL;

	PFLT_FILE_NAME_INFORMATION fileNameInfo = NULL;

	UNICODE_STRING UserModePath = { 0 };

	BOOLEAN isBlock = FALSE;

	PPROCESS_INFO ProcInfo = NULL;

	if (NULL == Data ||
		NULL == Data->Iopb ||
		NULL == Data->Iopb->TargetFileObject)
	{
		return resStatus;
	}

	switch (Data->Iopb->MajorFunction)
	{
	case IRP_MJ_CREATE:
		try
		{
			desiredAccess = Data->Iopb->Parameters.Create.SecurityContext->DesiredAccess;

			if (FlagOn(Data->Iopb->OperationFlags, SL_OPEN_PAGING_FILE) || FlagOn(Data->Iopb->TargetFileObject->Flags, FO_VOLUME_OPEN))
			{
				__leave;
			}

			ProcessID = (HANDLE)FltGetRequestorProcessId(Data);

			if (ProcessID != NULL)
			{
				ProcInfo = GetProcessRecord(ProcessID);

				if (ProcInfo != NULL)
				{
					status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &fileNameInfo);

					if (NT_SUCCESS(status))
					{
						status = FltParseFileNameInformation(fileNameInfo);

						if (NT_SUCCESS(status))
						{
							if (fileNameInfo->Name.Buffer && (fileNameInfo->Name.Length > 1))
							{
								if (FlagOn(desiredAccess, FILE_EXECUTE))
								{
									if (ProcInfo->Type == CS)
									{
										//If need reparse video mode
										UNICODE_STRING FullPathToFile;
										FullPathToFile.Length = MAX_PATH * sizeof(WCHAR);
										FullPathToFile.MaximumLength = FullPathToFile.Length;

										FullPathToFile.Buffer = ExAllocatePoolWithTag(NonPagedPool, FullPathToFile.Length, DRVTAG);

										RtlCopyUnicodeString(&FullPathToFile, &fileNameInfo->Volume);

										status = RtlAppendUnicodeStringToString(&FullPathToFile, &fileNameInfo->ParentDir);

										if (NT_SUCCESS(status))
										{
											if (!RtlCompareUnicodeString(&fileNameInfo->FinalComponent, &bSWDLL, TRUE))
											{
												if (GetProcessStartMode(ProcessID) == UNKNOWN)
												{
													SetProcessStartMode(ProcessID, SOFTWARE);
												}

												status = RtlAppendUnicodeStringToString(&FullPathToFile, &bHWDLL);

												if (NT_SUCCESS(status))
												{
													status = IoReplaceFileObjectName(Data->Iopb->TargetFileObject, FullPathToFile.Buffer, FullPathToFile.Length);

													if (NT_SUCCESS(status))
													{
#if (SHOW_DBG_MESSAGE)
														DbgPrintEx(0, 0, "[%ws] Replace load DLL to [%wZ]\r\n", DRIVERNAME, FullPathToFile);
#endif
														Data->IoStatus.Status = STATUS_REPARSE;
														Data->IoStatus.Information = IO_REPARSE;

														RtlFreeUnicodeString(&FullPathToFile);

														return FLT_PREOP_COMPLETE;
													}
												}
											}

											if (!RtlCompareUnicodeString(&fileNameInfo->FinalComponent, &bHWDLL, TRUE))
											{
												if (GetProcessStartMode(ProcessID) == UNKNOWN)
												{
													SetProcessStartMode(ProcessID, OPENGL);
												}

												SetProcessMode(ProcessID, OPENGL);
											}

											RtlFreeUnicodeString(&FullPathToFile);
										}
									}

									//end need reparse video mode

									status = GetUsermodePath(fileNameInfo, FltObjects, &UserModePath);

									if (!NT_SUCCESS(status))
									{
										Data->IoStatus.Status = STATUS_ACCESS_DENIED;

										IoSetThreadHardErrorMode(FALSE);

										isBlock = TRUE;

										AddModuleToProcess(ProcessID, &UserModePath, TRUE);

										__leave;
									}

									if (fileNameInfo->Extension.Length > 0 && FlagOn(Data->Iopb->Parameters.Create.SecurityContext->DesiredAccess, SYNCHRONIZE))
									{
										if (!IsWhiteModule(&UserModePath, GetGameTypeByPID(ProcessID), &fileNameInfo->FinalComponent))
										{
											Data->IoStatus.Status = STATUS_ACCESS_DENIED;

											IoSetThreadHardErrorMode(FALSE);

											isBlock = TRUE;

											AddModuleToProcess(ProcessID, &UserModePath, TRUE);

											__leave;
										}

										AddModuleToProcess(ProcessID, &UserModePath, FALSE);
									}
								}

								switch (ProcInfo->Type)
								{
								case CS:
									//models
									if (Data->Iopb->Parameters.Create.SecurityContext->DesiredAccess == (FILE_READ_DATA | FILE_READ_EA | FILE_READ_ATTRIBUTES | READ_CONTROL | SYNCHRONIZE))
									{
										PMODEL_MD5_CS pModelData = NULL;

										if (wcsstr(fileNameInfo->Name.Buffer, L"cstrike_hd"))
										{
											for (int i = 0; i < MAX_MODEL_DATA; i++)
											{
												if (RtlEqualUnicodeString(&fileNameInfo->FinalComponent, &CS_MODEL_DATA[i].Name, TRUE) && 
													wcsstr(CS_MODEL_DATA[i].Path.Buffer, L"cstrike_hd\\") )
												{
													pModelData = &CS_MODEL_DATA[i];
												}
											}

											if (pModelData)
											{
												UNICODE_STRING usFileMD5;
												WCHAR usFileMD5Buf[0x200];

												RtlInitEmptyUnicodeString(&usFileMD5, usFileMD5Buf, sizeof(usFileMD5Buf));

												NTSTATUS MD5NTStatus = MD5File(&fileNameInfo->Name, &usFileMD5, gFilterHandle, FltObjects->Instance);

												if (NT_SUCCESS(MD5NTStatus))
												{
													if (RtlEqualUnicodeString(&pModelData->MD5, &usFileMD5, TRUE))
													{
#if (SHOW_DBG_MESSAGE)
														DbgPrintEx(0, 0, "\n[%ws]Good HD Model file [%wZ] MD5[%wZ]\r\n", DRIVERNAME, fileNameInfo->Name, usFileMD5);
#endif
														__leave;
													}
													else
													{
#if (SHOW_DBG_MESSAGE)
														DbgPrintEx(0, 0, "\n[%ws]Incorrect HD Model file [%wZ] MD5[%wZ]\r\n", DRIVERNAME, fileNameInfo->Name, usFileMD5);
#endif

														UNICODE_STRING FullPathToFile;
														FullPathToFile.Length = MAX_PATH * sizeof(WCHAR);
														FullPathToFile.MaximumLength = FullPathToFile.Length;

														FullPathToFile.Buffer = ExAllocatePoolWithTag(NonPagedPool, FullPathToFile.Length, DRVTAG);

														RtlZeroMemory(FullPathToFile.Buffer, FullPathToFile.Length);

														RtlCopyUnicodeString(&FullPathToFile, &fileNameInfo->Name);

														PMODEL_MD5_CS newModule = NULL;

														for (int i = 0; i < MAX_MODEL_DATA; i++)
														{
															if (RtlEqualUnicodeString(&fileNameInfo->FinalComponent, &CS_MODEL_DATA[i].Name, TRUE) &&
																wcsstr(CS_MODEL_DATA[i].Path.Buffer, L"cstrike\\"))
															{
																newModule = &CS_MODEL_DATA[i];
															}
														}

														if (newModule)
														{
															WCHAR * PathPtr = wcsstr(FullPathToFile.Buffer, L"cstrike_hd\\");

															if (PathPtr)
															{
																wcscpy(PathPtr, newModule->Path.Buffer);
																wcscat(FullPathToFile.Buffer, newModule->Name.Buffer);
															}
														}		

														status = IoReplaceFileObjectName(Data->Iopb->TargetFileObject, FullPathToFile.Buffer, FullPathToFile.Length);

														if (NT_SUCCESS(status))
														{
#if (SHOW_DBG_MESSAGE)
															DbgPrintEx(0, 0, "[%ws] Replace load models to [%wZ]\r\n", DRIVERNAME, FullPathToFile);
#endif
															Data->IoStatus.Status = STATUS_REPARSE;
															Data->IoStatus.Information = IO_REPARSE;

															RtlFreeUnicodeString(&FullPathToFile);

															return FLT_PREOP_COMPLETE;
														}

														RtlFreeUnicodeString(&FullPathToFile);
													}
												}
											}
										}
										else
										{
											for (int i = 0; i < MAX_MODEL_DATA; i++)
											{
												if (RtlEqualUnicodeString(&fileNameInfo->FinalComponent, &CS_MODEL_DATA[i].Name, TRUE) &&
													wcsstr(CS_MODEL_DATA[i].Path.Buffer, L"cstrike\\"))
												{
													pModelData = &CS_MODEL_DATA[i];
												}
											}

											if (pModelData)
											{
												UNICODE_STRING usFileMD5;
												WCHAR usFileMD5Buf[0x200];

												RtlInitEmptyUnicodeString(&usFileMD5, usFileMD5Buf, sizeof(usFileMD5Buf));

												NTSTATUS MD5NTStatus = MD5File(&fileNameInfo->Name, &usFileMD5, gFilterHandle, FltObjects->Instance);

												if (NT_SUCCESS(MD5NTStatus))
												{
													if (RtlEqualUnicodeString(&pModelData->MD5, &usFileMD5, TRUE))
													{
#if (SHOW_DBG_MESSAGE)
														DbgPrintEx(0, 0, "\n[%ws]Good normal Model file [%wZ] MD5[%wZ]\r\n", DRIVERNAME, fileNameInfo->Name, usFileMD5);
#endif
														__leave;
													}
													else
													{
#if (SHOW_DBG_MESSAGE)
														DbgPrintEx(0, 0, "\n[%ws]Incorrect normal Model file [%wZ] MD5[%wZ]\r\n", DRIVERNAME, fileNameInfo->Name, usFileMD5);
#endif
														Data->IoStatus.Status = STATUS_ACCESS_DENIED;

														isBlock = TRUE;

														__leave;
													}
												}
											}
										}
									}
									break;
								case CSGO:
									//md5 files CSGO
									if (Data->Iopb->Parameters.Create.SecurityContext->DesiredAccess == (FILE_READ_DATA | FILE_READ_EA | FILE_READ_ATTRIBUTES | READ_CONTROL | SYNCHRONIZE))
									{
										if (RtlEqualUnicodeString(&fileNameInfo->Extension, &shaderExt, TRUE))
										{
											UNICODE_STRING usFileMD5;
											WCHAR usFileMD5Buf[0x200];

											RtlInitEmptyUnicodeString(&usFileMD5, usFileMD5Buf, sizeof(usFileMD5Buf));

											NTSTATUS MD5NTStatus = MD5File(&fileNameInfo->Name, &usFileMD5, gFilterHandle, FltObjects->Instance);

											if (NT_SUCCESS(MD5NTStatus))
											{
												DbgPrintEx(0, 0, "[%ws] Shader file [%wZ] MD5[%wZ]\r\n", DRIVERNAME, fileNameInfo->FinalComponent, usFileMD5);
											}
										}
									}
									break;
								}
							}
						}
					}
				}
			}
		}
		__finally
		{
			if (fileNameInfo != NULL)
			{
				FltReleaseFileNameInformation(fileNameInfo);
			}

			if (UserModePath.Buffer)
			{
				RtlFreeUnicodeString(&UserModePath);
			}

			if (isBlock)
			{
				resStatus = FLT_PREOP_COMPLETE;
			}
		}
		break;
	}

	return resStatus;
}



FLT_POSTOP_CALLBACK_STATUS CSMFltrPostOperation(_Inout_ PFLT_CALLBACK_DATA Data, _In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_opt_ PVOID CompletionContext, _In_ FLT_POST_OPERATION_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(Data);
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);
	UNREFERENCED_PARAMETER(Flags);

	return FLT_POSTOP_FINISHED_PROCESSING;
}