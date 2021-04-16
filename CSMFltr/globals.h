#ifndef GLOBALS_H
#define GLOBALS_H
#include <fltKernel.h>
#include "CS_Info.h"

#define MAX_PATH 260
#define SHOW_DBG_MESSAGE 1
#define USE_TAG 1

enum CSENUM
{
	CS,
	CSGO
};

enum CSSTARTMODE
{
	UNKNOWN = -1,
	SOFTWARE,
	OPENGL
};

extern WCHAR* DRIVERNAME;

typedef struct _FLT_GLOBALS
{
	PDRIVER_OBJECT DriverObject;

	PFLT_FILTER Filter;

	__volatile LONGLONG ProcessCount;

	KSPIN_LOCK ProcCollectionLock;

	PLIST_ENTRY ProcCollection;

	//Pathes
	//system

	UNICODE_STRING SteamPath;

	UNICODE_STRING Windows_system32;

	UNICODE_STRING Windows_winsxs;

#if(_WIN64)
	UNICODE_STRING Windows_syswow64;
#endif

	//Game pathes
	
	UNICODE_STRING Directory_CS16;

	UNICODE_STRING Directory_CSGO;

}FLT_GLOBALS, *PFLT_GLOBALS;

//---------------------------
//Process info element for linked list
//---------------------------
typedef struct _PROCESS_INFO
{
	LIST_ENTRY link;
	HANDLE  ProcessID;
	USHORT Type;
	//------------------------------
	SHORT StartMode;
	SHORT CurrentMode;
	__volatile LONGLONG ModulesCount;
	KSPIN_LOCK ModulesSpinLock;
	PLIST_ENTRY ModuleCollection;
	//------------------------------
}PROCESS_INFO, *PPROCESS_INFO;

typedef struct _MODULE_INFO
{
	LIST_ENTRY link;
	UNICODE_STRING ModuleName;
	USHORT IsBlocked;
}MODULE_INFO, * PMODULE_INFO;

extern FLT_GLOBALS Globals;

//---------------------------------
//functions
//---------------------------------

NTSTATUS InitGlobals(PDRIVER_OBJECT DriverObject, _In_ PFLT_FILTER gFilterHandle);
NTSTATUS InitWindowsDirectory(IN PFLT_FILTER gFilterHandle);
static NTSTATUS GetSymbolicLink(IN PUNICODE_STRING SymbolicLinkName, OUT PUNICODE_STRING SymbolicLink, OUT PHANDLE LinkHandle);
NTSTATUS ConstructString(IN PUNICODE_STRING Letter, IN PUNICODE_STRING SourceWindows); //Эта функция зависит от глобальных переменных
VOID FreeGlobals(); //Free dynamic memory (unload driver)
VOID InitGameDirectory(PUNICODE_STRING Dest, PUNICODE_STRING Source, USHORT GameType);
NTSTATUS InitSteamDirectory();
NTSTATUS GetUsermodePath(_In_ PFLT_FILE_NAME_INFORMATION FileNameInfo, _In_ PCFLT_RELATED_OBJECTS FltObjects, _Inout_ PUNICODE_STRING FullPath);


//---------------------------------

#endif // GLOBALS_H







