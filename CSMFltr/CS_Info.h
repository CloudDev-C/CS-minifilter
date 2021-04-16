#ifndef CS_INFO_H
#define CS_INFO_H
#include <fltKernel.h>
#include "globals.h"

enum COLLECTIONS_ID
{
	STEAM_COLLECTION,
	GAME_CS_COLLECTION,
	GAME_CSGO_COLLECTION,
	SYSTEM32_COLLECTION,
	SYSTEM_COLLECTION_CS,
	SYSTEM_COLLECTION_CSGO
};

VOID AddProcessToList(_In_ HANDLE ProcessId, USHORT Type);
VOID RemoveProcessInList(_In_ HANDLE ProcessId);
VOID FreeProcessList();
HANDLE IsExistProcessInList(_In_ HANDLE ProcessId);
VOID* GetProcessRecord(_In_ HANDLE ProcessId);
VOID AddModuleToProcess(_In_ HANDLE ProcessID, _In_ PUNICODE_STRING ModulePath, BOOLEAN IsBlocked);
VOID FreeModuleList(_In_ PLIST_ENTRY ListHead, _In_ PKSPIN_LOCK SpinLock, _Inout_ __volatile LONGLONG* Count);

BOOLEAN IsWhiteModule(_In_ PUNICODE_STRING FullModulePath, USHORT GameType, PUNICODE_STRING FinalComponent);
BOOLEAN IsValidModule(PUNICODE_STRING FullPath, INT CollectionID);
SHORT GetGameTypeByPID(_In_ HANDLE ProcessID);
SHORT GetProcessStartMode(_In_ HANDLE ProcessID);
SHORT GetProcessMode(_In_ HANDLE ProcessID);
VOID SetProcessStartMode(_In_ HANDLE ProcessID, SHORT Mode);
VOID SetProcessMode(_In_ HANDLE ProcessID, SHORT Mode);

#endif // !CS_INFO_H

