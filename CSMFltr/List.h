#pragma once

#include <fltKernel.h>

extern LIST_ENTRY g_CSListHead;

typedef VOID(*KELEMENT_FREE_CALLBACK)(PLIST_ENTRY ListEntry);

_Check_return_ NTSTATUS MY_InitList(PLIST_ENTRY ListHead, _In_ PKSPIN_LOCK SpinLock);
VOID MY_PushToList(_In_ PLIST_ENTRY ListEntry, _In_ PLIST_ENTRY ListHead, _In_ PKSPIN_LOCK SpinLock);
VOID ShowAllListProcess(_In_ PLIST_ENTRY ListHead, _In_ PKSPIN_LOCK SpinLock);
