#include "List.h"
#include "globals.h"

_Check_return_ NTSTATUS MY_InitList(PLIST_ENTRY ListHead, _In_ PKSPIN_LOCK SpinLock)
{
	InitializeListHead(ListHead);

	KeInitializeSpinLock(SpinLock);

	return STATUS_SUCCESS;
}

VOID MY_PushToList(_In_ PLIST_ENTRY ListEntry, _In_ PLIST_ENTRY ListHead, _In_ PKSPIN_LOCK SpinLock)
{
	if (!ListEntry)
	{
		return;
	}

	if (!ListHead)
	{
		return;
	}

	ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

	ExInterlockedInsertTailList(ListHead, ListEntry, SpinLock);
}

VOID ShowAllListProcess(_In_ PLIST_ENTRY ListHead, _In_ PKSPIN_LOCK SpinLock)
{
	ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

	int count = 0;

	while (!IsListEmpty(Globals.ProcCollection))
	{
		if (count == Globals.ProcessCount)
		{
			break;
		}

		PLIST_ENTRY pdLink = ExInterlockedRemoveHeadList(ListHead, SpinLock);

		PROCESS_INFO *piElement = CONTAINING_RECORD(pdLink, PROCESS_INFO, link);

		KdPrintEx((0, 0, "ProcessInfo = [%d]\r\n", piElement->ProcessID));

		MY_PushToList(&piElement->link, ListHead, SpinLock);

		count++;
	}	
}

