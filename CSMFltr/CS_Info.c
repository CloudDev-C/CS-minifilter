#include "CS_Info.h"
#include "globals.h"
#include "List.h"
#include "Util.h"
#include "Constants.h"


VOID AddProcessToList(_In_ HANDLE ProcessId, USHORT Type)
{
    if (IsExistProcessInList(ProcessId) != (HANDLE)-1)
    {
#if(SHOW_DBG_MESSAGE)
        //DbgPrintEx(0,0, "Error! Process alredy exist!");
#endif
        return;
    }

    PPROCESS_INFO Proc = ExAllocatePoolWithTag(NonPagedPool, sizeof(PROCESS_INFO), DRVTAG);

    Proc->ProcessID = ProcessId;
    Proc->Type = Type;
    Proc->StartMode = UNKNOWN;
    Proc->CurrentMode = UNKNOWN;

    Proc->ModuleCollection = (PLIST_ENTRY)ExAllocatePoolWithTag(NonPagedPool, sizeof(LIST_ENTRY), DRVTAG);

    Proc->ModulesCount = 0;
    
    MY_InitList(Proc->ModuleCollection, &Proc->ModulesSpinLock);

    InterlockedIncrement64(&Globals.ProcessCount);

    MY_PushToList(&Proc->link, Globals.ProcCollection, &Globals.ProcCollectionLock); 

#if(SHOW_DBG_MESSAGE)
    ShowAllListProcess(Globals.ProcCollection, &Globals.ProcCollectionLock);
#endif
}

SHORT GetProcessStartMode(_In_ HANDLE ProcessID)
{
    PPROCESS_INFO ProcInfo = GetProcessRecord(ProcessID);

    if (ProcInfo != NULL)
    {
        return ProcInfo->StartMode;
    }

    return UNKNOWN;
}


SHORT GetProcessMode(_In_ HANDLE ProcessID)
{
    PPROCESS_INFO ProcInfo = GetProcessRecord(ProcessID);

    if (ProcInfo != NULL)
    {
        return ProcInfo->CurrentMode;
    }

    return UNKNOWN;
}

VOID SetProcessStartMode(_In_ HANDLE ProcessID, SHORT Mode)
{
    PPROCESS_INFO ProcInfo = GetProcessRecord(ProcessID);

    if (ProcInfo != NULL)
    {
        ProcInfo->StartMode = Mode;
    }
}


VOID SetProcessMode(_In_ HANDLE ProcessID, SHORT Mode)
{
    PPROCESS_INFO ProcInfo = GetProcessRecord(ProcessID);

    if (ProcInfo != NULL)
    {
        ProcInfo->CurrentMode = Mode;
    }
}

INT IsExistModuleInList(PUNICODE_STRING Module, _In_ LONGLONG Count, _In_ PLIST_ENTRY ListHead, _In_ PKSPIN_LOCK SpinLock)
{
    INT Result = FALSE;

    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    KIRQL oldIrql;

    PLIST_ENTRY current;

    KeAcquireSpinLock(SpinLock, &oldIrql);

    LONGLONG Iter = 0;

    while (!IsListEmpty(ListHead))
    {
        if (Iter == Count)
        {
            break;
        }

        current = RemoveHeadList(ListHead);

        PMODULE_INFO piElement = CONTAINING_RECORD(current, MODULE_INFO, link);

        InsertTailList(ListHead, current);

        if (RtlEqualUnicodeString(Module, &piElement->ModuleName, TRUE))
        {
#if(SHOW_DBG_MESSAGE)
            //DbgPrintEx(0, 0, "Equal %wZ %wZ\r\n", Module, piElement->ModuleName);
#endif
            Result = TRUE;
            break;
        }

        Iter++;
    }

    KeReleaseSpinLock(SpinLock, oldIrql);

    return Result;
}


VOID AddModuleToProcess(_In_ HANDLE ProcessID, _In_ PUNICODE_STRING ModulePath, BOOLEAN IsBlocked)
{
    PPROCESS_INFO CurrentProcess = GetProcessRecord(ProcessID);

    if (CurrentProcess == NULL)
    {
#if(SHOW_DBG_MESSAGE)
       // DbgPrintEx(0, 0, "No exist process in list!");
#endif
        return;
    }

    if (IsExistModuleInList(ModulePath, CurrentProcess->ModulesCount, CurrentProcess->ModuleCollection, &CurrentProcess->ModulesSpinLock))
    {
#if(SHOW_DBG_MESSAGE)
        DbgPrintEx(0, 0, "Module alredy exist! [%wZ]\r\n", ModulePath);
#endif
        return;
    }

    PMODULE_INFO CurrentModule = (PMODULE_INFO)ExAllocatePoolWithTag(NonPagedPool, sizeof(MODULE_INFO), DRVTAG);

    if (CurrentModule != NULL)
    {
        MY_InitUnicodeString(&CurrentModule->ModuleName, ModulePath->Buffer, ModulePath->Length, TRUE);

        CurrentModule->IsBlocked = IsBlocked;

        MY_PushToList(&CurrentModule->link, CurrentProcess->ModuleCollection, &CurrentProcess->ModulesSpinLock);

        InterlockedIncrement64(&CurrentProcess->ModulesCount);

#if(SHOW_DBG_MESSAGE)
        DbgPrintEx(0,0, "[%ws] Add module to list PID[%d] Type[%ws] IsBlocked[%d] Module: %wZ\r\n",
            DRIVERNAME, CurrentProcess->ProcessID, !CurrentProcess->Type ? L"CS" : L"CSGO", CurrentModule->IsBlocked, CurrentModule->ModuleName);
#endif
    } 
}

VOID FreeModuleList(_In_ PLIST_ENTRY ListHead, _In_ PKSPIN_LOCK SpinLock, _Inout_ __volatile LONGLONG* Count)
{
    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    KIRQL oldIrql;

    PLIST_ENTRY current;

    KeAcquireSpinLock(SpinLock, &oldIrql);

    while (!IsListEmpty(ListHead))
    {
        current = RemoveHeadList(ListHead);

        KeReleaseSpinLock(SpinLock, oldIrql);

        PMODULE_INFO piElement = CONTAINING_RECORD(current, MODULE_INFO, link);

        RtlFreeUnicodeString(&piElement->ModuleName);

        InterlockedDecrement64(Count);

        ExFreePoolWithTag(piElement, DRVTAG);

        KeAcquireSpinLock(SpinLock, &oldIrql);
    }

    ExFreePoolWithTag(ListHead, DRVTAG);

    KeReleaseSpinLock(SpinLock, oldIrql);
}

VOID RemoveProcessInList(HANDLE ProcessId)
{
    KIRQL oldIrql;

    PLIST_ENTRY current;

    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    KeAcquireSpinLock(&Globals.ProcCollectionLock, &oldIrql);

    LONGLONG Iter = 0;

    while (!IsListEmpty(Globals.ProcCollection))
    {
        if (Iter == Globals.ProcessCount)
        {
            break;
        }

        current = RemoveHeadList(Globals.ProcCollection);

        PROCESS_INFO* piElement = CONTAINING_RECORD(current, PROCESS_INFO, link);

        if (ProcessId != piElement->ProcessID)
        {
            InsertTailList(Globals.ProcCollection, current);
        }
        else
        {
#if(SHOW_DBG_MESSAGE)
            DbgPrintEx(0, 0, "Deleted procID = [%d] Type = [%ws]\n", piElement->ProcessID, !piElement->Type ? L"CS" : L"CSGO");
#endif

            KeReleaseSpinLock(&Globals.ProcCollectionLock, oldIrql);

            FreeModuleList(piElement->ModuleCollection, &piElement->ModulesSpinLock, &piElement->ModulesCount);

            ExFreePoolWithTag(piElement, DRVTAG);

            InterlockedDecrement64(&Globals.ProcessCount);

            return;
        }   

        Iter++;
    }

    KeReleaseSpinLock(&Globals.ProcCollectionLock, oldIrql);
}

VOID FreeProcessList()
{
    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    KIRQL oldIrql;

    PLIST_ENTRY current;

    KeAcquireSpinLock(&Globals.ProcCollectionLock, &oldIrql);

    while (!IsListEmpty(Globals.ProcCollection))
    {
        current = RemoveHeadList(Globals.ProcCollection);

        KeReleaseSpinLock(&Globals.ProcCollectionLock, oldIrql);

        PROCESS_INFO* piElement = CONTAINING_RECORD(current, PROCESS_INFO, link);

        FreeModuleList(piElement->ModuleCollection, &piElement->ModulesSpinLock, &piElement->ModulesCount);

        ExFreePoolWithTag(piElement, DRVTAG);

        KeAcquireSpinLock(&Globals.ProcCollectionLock, &oldIrql);

        InterlockedDecrement64(&Globals.ProcessCount);
    }

    ExFreePoolWithTag(Globals.ProcCollection, DRVTAG);

    KeReleaseSpinLock(&Globals.ProcCollectionLock, oldIrql);
}

HANDLE IsExistProcessInList(_In_ HANDLE ProcessId)
{
    HANDLE Result = (HANDLE)-1;

    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    KIRQL oldIrql;

    PLIST_ENTRY current;

    KeAcquireSpinLock(&Globals.ProcCollectionLock , &oldIrql);

    LONGLONG Iter = 0;

    while (!IsListEmpty(Globals.ProcCollection))
    {
        if (Iter == Globals.ProcessCount)
        {
            break;
        }

        current = RemoveHeadList(Globals.ProcCollection);

        PROCESS_INFO* piElement = CONTAINING_RECORD(current, PROCESS_INFO, link);

        InsertTailList(Globals.ProcCollection, current);

        if (piElement->ProcessID == ProcessId)
        {
            Result = ProcessId;
            break;
        }

        Iter++;
    }

    KeReleaseSpinLock(&Globals.ProcCollectionLock, oldIrql);

    return Result;
}

VOID* GetProcessRecord(_In_ HANDLE ProcessId)
{
    VOID* Result = NULL;

    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    KIRQL oldIrql;

    PLIST_ENTRY current;

    KeAcquireSpinLock(&Globals.ProcCollectionLock, &oldIrql);

    LONGLONG Iter = 0;

    while (!IsListEmpty(Globals.ProcCollection))
    {
        if (Iter == Globals.ProcessCount)
        {
            break;
        }

        current = RemoveHeadList(Globals.ProcCollection);

        PROCESS_INFO* piElement = CONTAINING_RECORD(current, PROCESS_INFO, link);

        InsertTailList(Globals.ProcCollection, current);

        if (piElement->ProcessID == ProcessId)
        {
            Result = (VOID*)piElement;
            break;
        }

        Iter++;
    }

    KeReleaseSpinLock(&Globals.ProcCollectionLock, oldIrql);

    return Result;
}


SHORT GetGameTypeByPID(_In_ HANDLE ProcessID)
{
    SHORT Result = -1;

    ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    KIRQL oldIrql;

    PLIST_ENTRY current;

    KeAcquireSpinLock(&Globals.ProcCollectionLock, &oldIrql);

    LONGLONG Iter = 0;

    while (!IsListEmpty(Globals.ProcCollection))
    {
        if (Iter == Globals.ProcessCount)
        {
            break;
        }

        current = RemoveHeadList(Globals.ProcCollection);

        PROCESS_INFO* piElement = CONTAINING_RECORD(current, PROCESS_INFO, link);

        InsertTailList(Globals.ProcCollection, current);

        if (piElement->ProcessID == ProcessID)
        {
            Result = piElement->Type;
            break;
        }

        Iter++;
    }

    KeReleaseSpinLock(&Globals.ProcCollectionLock, oldIrql);

    return Result;
}

BOOLEAN IsWhiteModule(_In_ PUNICODE_STRING FullModulePath, USHORT GameType, PUNICODE_STRING FinalComponent)
{
    BOOLEAN result = FALSE;

    UNREFERENCED_PARAMETER(FinalComponent);

    //for (size_t i = 0; i < WHITE_MODLES_SOFT_COUNT; i++)
    //{
    //    if (RtlEqualUnicodeString(FinalComponent, &WHITE_MODULES_SOFT[i], TRUE))
    //    {
    //        return TRUE;
    //    }
    //}

    UNICODE_STRING MPath = {0};

    __try
    {
        NTSTATUS status = RtlUpcaseUnicodeString(&MPath, FullModulePath, TRUE);

        if (!NT_SUCCESS(status))
        {
            __leave;
        }

        if (!wcsncmp(MPath.Buffer, Globals.Windows_winsxs.Buffer, Globals.Windows_winsxs.Length / 2))
        {
            return TRUE;
        }

#if (_WIN64)
        if (!wcsncmp(MPath.Buffer, Globals.Windows_syswow64.Buffer, Globals.Windows_syswow64.Length / 2))
        {
            return TRUE;
        }
#endif
//#else
        if (!wcsncmp(MPath.Buffer, Globals.Windows_system32.Buffer, Globals.Windows_system32.Length / 2))
        {
            if (GameType == CS)
            {
                result = TRUE;
                __leave;
            }

            if (GameType == CSGO)
            {
                result = TRUE;
                __leave;
            }
        }
//#endif
        if ((GameType == CS && !wcsncmp(MPath.Buffer, Globals.Directory_CS16.Buffer, Globals.Directory_CS16.Length / 2)))
        {
            result = IsValidModule(FullModulePath, GAME_CS_COLLECTION);

            __leave;
        }

        if (GameType == CSGO && MPath.Length > Globals.Directory_CSGO.Length)
        {
            if (!wcsncmp(MPath.Buffer, Globals.Directory_CSGO.Buffer, Globals.Directory_CSGO.Length / 2))
            {
                result = IsValidModule(FullModulePath, GAME_CSGO_COLLECTION);

                __leave;
            }
        }

        if (!wcsncmp(Globals.SteamPath.Buffer, MPath.Buffer, Globals.SteamPath.Length / 2))
        {
            result = IsValidModule(FullModulePath, STEAM_COLLECTION);

            __leave;
        }
    }
    __finally
    {
        if (MPath.Buffer)
        {
            RtlFreeUnicodeString(&MPath);
        }     
    }

    return result;
}

BOOLEAN IsValidModule(PUNICODE_STRING FullPath, INT CollectionID)
{
    PUNICODE_STRING path = NULL;
    PUNICODE_STRING colletion = NULL;
    INT collection_count = 0;

    switch (CollectionID)
    {
    case STEAM_COLLECTION:
        path = &Globals.SteamPath;
        colletion = WHITE_STEAM_MODULES;
        collection_count = STEAM_MODULES_COUNT;
        break;
    case GAME_CS_COLLECTION:
        path = &Globals.Directory_CS16;
        colletion = WHITE_MODULES_ARRAY_GAME_CS;
        collection_count = CS_CLIENT_MODULES_COUNT;
        break;
    case GAME_CSGO_COLLECTION:
        path = &Globals.Directory_CSGO;
        colletion = WHITE_MODULES_ARRAY_GAME_CSGO;
        collection_count = CSGO_CLIENT_MODULES_COUNT;
        break;
    }

    for (int i = 0; i < collection_count; i++)
    {
        UNICODE_STRING directory;
        directory.MaximumLength = MAX_PATH * sizeof(WCHAR);
        directory.Buffer = ExAllocatePoolWithTag(NonPagedPool, directory.MaximumLength, DRVTAG);

        wcscpy(directory.Buffer, path->Buffer);

        wcscat(directory.Buffer, colletion[i].Buffer);

        directory.Length = (USHORT)wcslen(directory.Buffer) * (USHORT)sizeof(UNICODE_NULL);

        if (RtlEqualUnicodeString(&directory, FullPath, TRUE))
        {
            RtlFreeUnicodeString(&directory);

            return TRUE;
        }

        RtlFreeUnicodeString(&directory);
    }

    return FALSE;
}